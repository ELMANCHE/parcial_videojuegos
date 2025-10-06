#include <opencv2/opencv.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>

#include <vector>
#include <cmath>
#include <random>
#include <string>

using namespace cv;
using namespace std;

struct Vec3 { double x,y,z; };

// Rotaciones (misma convención usada en otros archivos)
Vec3 rotX(const Vec3 &p, double a) { return {p.x, cos(a)*p.y - sin(a)*p.z, sin(a)*p.y + cos(a)*p.z}; }
Vec3 rotY(const Vec3 &p, double a) { return {cos(a)*p.x + sin(a)*p.z, p.y, -sin(a)*p.x + cos(a)*p.z}; }
Vec3 rotZ(const Vec3 &p, double a) { return {cos(a)*p.x - sin(a)*p.y, sin(a)*p.x + cos(a)*p.y, p.z}; }

Point projectPoint(const Vec3 &p, int width, int height, double fov, double camDist) {
    double f = (width/2) / tan(fov * 0.5);
    double z = p.z + camDist;
    if (z <= 0.0001) z = 0.0001;
    double x = (p.x * f) / z + width/2.0;
    double y = (-p.y * f) / z + height/2.0;
    return Point(static_cast<int>(x+0.5), static_cast<int>(y+0.5));
}

// Estructura para una parte del cuerpo (cubo con posición y tamaño)
struct BodyPart {
    Vec3 offset;  // Offset relativo al centro del personaje
    Vec3 size;    // Ancho, Alto, Profundidad
    Scalar color; // Color de esta parte
};

// Personaje tipo Roblox compuesto de cubos
struct RobloxCharacter {
    Vec3 position;     // Posición central del personaje en el mundo
    double rotation;   // Rotación en Y (yaw)
    double speed;      // Velocidad de movimiento
    int playerNumber;  // 1 o 2
    
    // Partes del cuerpo (offsets relativos)
    BodyPart head;
    BodyPart torso;
    BodyPart leftArm;
    BodyPart rightArm;
    BodyPart leftLeg;
    BodyPart rightLeg;
    
    // Colores del personaje
    Scalar headColor;
    Scalar torsoColor;
    Scalar armColor;
    Scalar legColor;
    
    RobloxCharacter(Vec3 pos, int playerNum) : position(pos), rotation(0), speed(0.15), playerNumber(playerNum) {
        // Colores diferentes por jugador
        if(playerNum == 1) {
            // Jugador 1: Amarillo, Azul, Verde (colores cálidos)
            headColor = Scalar(50, 200, 255);   // Amarillo
            torsoColor = Scalar(200, 150, 50);  // Azul
            armColor = Scalar(50, 200, 255);    // Amarillo
            legColor = Scalar(50, 180, 50);     // Verde
        } else {
            // Jugador 2: Rojo, Gris, Naranja (colores diferentes)
            headColor = Scalar(50, 100, 255);   // Rojo
            torsoColor = Scalar(150, 150, 150); // Gris
            armColor = Scalar(50, 150, 255);    // Naranja
            legColor = Scalar(100, 50, 200);    // Morado
        }
        
        // Cabeza (arriba del torso)
        head = {{0, 0, 2.6}, {1.0, 1.0, 1.0}, headColor};
        
        // Torso (centro) - más alto y rectangular
        torso = {{0, 0, 1.5}, {1.4, 0.8, 1.6}, torsoColor};
        
        // Brazos (pegados a los lados del torso)
        leftArm = {{-1.0, 0, 1.5}, {0.5, 0.5, 1.5}, armColor};
        rightArm = {{1.0, 0, 1.5}, {0.5, 0.5, 1.5}, armColor};
        
        // Piernas (conectadas debajo del torso)
        leftLeg = {{-0.4, 0, 0.4}, {0.5, 0.5, 1.3}, legColor};
        rightLeg = {{0.4, 0, 0.4}, {0.5, 0.5, 1.3}, legColor};
    }
    
    void move(double dx, double dy) {
        position.x += dx * speed;
        position.y += dy * speed;
    }
};

// CÓDIGO ANTIGUO COMENTADO (pelotas y cuadrado)
// struct Ball { Vec3 pos; Vec3 vel; double radius; bool alive=true; };

// Variables globales para control de mouse
int mouseX = 0, mouseY = 0;
bool mousePressed = false;
int lastMouseX = 0, lastMouseY = 0;

// Callback para eventos del mouse
void onMouse(int event, int x, int y, int flags, void* userdata) {
    double* angles = (double*)userdata;
    
    if(event == EVENT_LBUTTONDOWN) {
        mousePressed = true;
        lastMouseX = x;
        lastMouseY = y;
    }
    else if(event == EVENT_LBUTTONUP) {
        mousePressed = false;
    }
    else if(event == EVENT_MOUSEMOVE && mousePressed) {
        int dx = x - lastMouseX;
        int dy = y - lastMouseY;
        
        // angles[0] = yaw, angles[1] = pitch
        angles[0] += dx * 0.005;  // Rotar horizontalmente
        angles[1] += dy * 0.005;  // Rotar verticalmente
        
        // Limitar pitch para evitar gimbal lock
        if(angles[1] > M_PI/2.0 - 0.1) angles[1] = M_PI/2.0 - 0.1;
        if(angles[1] < -M_PI/2.0 + 0.1) angles[1] = -M_PI/2.0 + 0.1;
        
        lastMouseX = x;
        lastMouseY = y;
    }
    else if(event == EVENT_MOUSEWHEEL) {
        // Zoom con la rueda del mouse
        double* camDist = &angles[2];
        int delta = getMouseWheelDelta(flags);
        *camDist -= delta * 0.5;
        if(*camDist < 5.0) *camDist = 5.0;
        if(*camDist > 30.0) *camDist = 30.0;
    }
}

int main(){
    int width=1000, height=700;
    Mat frame(height, width, CV_8UC3);

    // Mundo
    double worldHalf = 8.0; // cubo desde -worldHalf..worldHalf en x,y y z desde 0..2*worldHalf
    double minZ = 0.5, maxZ = 2*worldHalf;

    // Cámara ISOMÉTRICA (vista desde arriba, típica de juegos isométricos)
    // Vista más cenital para ver el campo desde arriba
    double yaw = M_PI/4.0;          // 45 grados para vista diagonal
    double pitch = M_PI/3.5;        // ~51° hacia abajo (vista más cenital)
    double roll = 0.0;
    double camDist = 25.0;          // Distancia mayor para ver más campo
    double fov = M_PI/6.0;          // FOV pequeño para efecto ortográfico
    // Flip/voltear escena (rotación extra alrededor de Y)
    double extraFlip = 0.0;
    
    // Array para pasar ángulos al callback del mouse: [yaw, pitch, camDist]
    double cameraAngles[3] = {yaw, pitch, camDist};

    // ========== CÓDIGO ANTIGUO COMENTADO ==========
    // Jugador: cuadrado atrapador (centro en x,y,z)
    // Vec3 sq{0.0, 0.0, 1.0}; // posición central
    // double sqSize = 2.0; // tamaño en unidades del mundo (lado)
    // double sqSpeed = 0.3; // movimiento por frame

    // Pelotas
    // vector<Ball> balls;
    // int initialBalls = 12;
    // std::mt19937 rng(12345);
    // uniform_real_distribution<double> ux(-worldHalf+0.5, worldHalf-0.5);
    // uniform_real_distribution<double> uy(-worldHalf+0.5, worldHalf-0.5);
    // uniform_real_distribution<double> uz(minZ+0.5, maxZ-0.5);
    // uniform_real_distribution<double> uv(-0.2,0.2);
    // for(int i=0;i<initialBalls;i++){
    //     Ball b; b.pos = {ux(rng), uy(rng), uz(rng)};
    //     b.vel = {uv(rng), uv(rng), uv(rng)};
    //     b.radius = 0.25 + (i%3)*0.08;
    //     b.alive = true;
    //     balls.push_back(b);
    // }
    // int caught = 0;
    // ============================================

    // CREAR 2 PERSONAJES TIPO ROBLOX con colores diferentes
    RobloxCharacter player1({-3.0, 0.0, 1.5}, 1);  // Personaje 1 (colores cálidos)
    RobloxCharacter player2({3.0, 0.0, 1.5}, 2);   // Personaje 2 (colores diferentes)

    namedWindow("Juego3D", WINDOW_AUTOSIZE);
    
    // Configurar callback del mouse
    setMouseCallback("Juego3D", onMouse, cameraAngles);

    while(true){
        // Actualizar ángulos desde el array modificado por el mouse
        yaw = cameraAngles[0];
        pitch = cameraAngles[1];
        camDist = cameraAngles[2];
        
        frame.setTo(Scalar(18,18,26));

        // Dibujar rejilla (plano z=0)
        double step = 1.0;
        int gridHalf = static_cast<int>(worldHalf / step);
        vector<vector<Point>> gridPts(2*gridHalf+1, vector<Point>(2*gridHalf+1));
        for(int i=-gridHalf;i<=gridHalf;i++) for(int j=-gridHalf;j<=gridHalf;j++){
            Vec3 p{i*step, j*step, 0.0};
            Vec3 pflip = rotY(p, extraFlip);
            Vec3 pt = rotX(rotY(rotZ(pflip, -roll), -yaw), -pitch);
            gridPts[i+gridHalf][j+gridHalf]=projectPoint(pt, width, height, fov, camDist);
        }
        for(int j=0;j<=2*gridHalf;j++) for(int i=0;i<2*gridHalf;i++) line(frame, gridPts[i][j], gridPts[i+1][j], Scalar(60,60,60),1);
        for(int i=0;i<=2*gridHalf;i++) for(int j=0;j<2*gridHalf;j++) line(frame, gridPts[i][j], gridPts[i][j+1], Scalar(60,60,60),1);

        // ========== CÓDIGO ANTIGUO COMENTADO (update balls, colisiones, etc) ==========
        // Update balls
        // for(auto &b: balls){
        //     if(!b.alive) continue;
        //     b.pos.x += b.vel.x;
        //     b.pos.y += b.vel.y;
        //     b.pos.z += b.vel.z;
        //     // Rebotes en cubo
        //     double minX = -worldHalf + b.radius, maxX = worldHalf - b.radius;
        //     double minY = -worldHalf + b.radius, maxY = worldHalf - b.radius;
        //     if(b.pos.x < minX){ b.pos.x=minX; b.vel.x*=-1; }
        //     if(b.pos.x > maxX){ b.pos.x=maxX; b.vel.x*=-1; }
        //     if(b.pos.y < minY){ b.pos.y=minY; b.vel.y*=-1; }
        //     if(b.pos.y > maxY){ b.pos.y=maxY; b.vel.y*=-1; }
        //     if(b.pos.z < minZ + b.radius){ b.pos.z = minZ + b.radius; b.vel.z *= -1; }
        //     if(b.pos.z > maxZ - b.radius){ b.pos.z = maxZ - b.radius; b.vel.z *= -1; }
        // }

        // Limitar cuadrado dentro del cubo
        // sq.x = min(max(sq.x, -worldHalf + sqSize/2.0), worldHalf - sqSize/2.0);
        // sq.y = min(max(sq.y, -worldHalf + sqSize/2.0), worldHalf - sqSize/2.0);
        // sq.z = min(max(sq.z, minZ + 0.2), maxZ - 0.2);

        // Detección de colisiones (si la pelota toca el cuadrado, desaparece)
        // for(auto &b: balls){
        //     if(!b.alive) continue;
        //     double dx = fabs(b.pos.x - sq.x);
        //     double dy = fabs(b.pos.y - sq.y);
        //     double dz = fabs(b.pos.z - sq.z);
        //     if(dx <= (sqSize/2.0 + b.radius) && dy <= (sqSize/2.0 + b.radius) && dz <= (b.radius + 0.3)){
        //         b.alive = false;
        //         caught++;
        //     }
        // }

        // Dibujar bolas
        // vector<int> indices;
        // for(size_t i=0;i<balls.size();++i) if(balls[i].alive) indices.push_back(i);
        // sort(indices.begin(), indices.end(), [&](int a,int b){ return balls[a].pos.z < balls[b].pos.z; });
        // for(int idx: indices){
        //     Ball &b = balls[idx];
        //     Vec3 bflip = rotY(b.pos, extraFlip);
        //     Vec3 bc = rotX(rotY(rotZ(bflip, -roll), -yaw), -pitch);
        //     Point pb = projectPoint(bc, width, height, fov, camDist);
        //     double f = (width/2) / tan(fov * 0.5);
        //     double zcam = bc.z + camDist; if(zcam<=0.0001) zcam=0.0001;
        //     int rscreen = max(2, static_cast<int>(b.radius * (f / zcam)));
        //     circle(frame, pb, rscreen, Scalar(220,180,80), FILLED);
        //     Vec3 shw{b.pos.x, b.pos.y, 0.0}; Vec3 shw_flip = rotY(shw, extraFlip); Vec3 shc = rotX(rotY(rotZ(shw_flip, -roll), -yaw), -pitch);
        //     Point psh = projectPoint(shc, width, height, fov, camDist);
        //     ellipse(frame, psh, Size(max(2, (int)(rscreen*1.2)), max(2, (int)(rscreen*0.5))), 0,0,360, Scalar(30,30,30), FILLED);
        // }
        // ============================================

        // FUNCIÓN PARA RENDERIZAR UN PERSONAJE ROBLOX
        auto renderRobloxCharacter = [&](const RobloxCharacter& character) {
            // Lista de partes del cuerpo
            vector<BodyPart> parts = {character.head, character.torso, character.leftArm, 
                                     character.rightArm, character.leftLeg, character.rightLeg};
            
            // Estructura para almacenar todas las caras de todas las partes
            struct FaceDraw { 
                double depth; 
                vector<Point> pts; 
                Scalar color;
                Vec3 normal; // Para backface culling
            };
            vector<FaceDraw> allFaces;
            
            // Para cada parte, crear un cubo
            for(const auto& part : parts) {
                // Calcular posición mundial de la parte
                Vec3 partPos = {
                    character.position.x + part.offset.x,
                    character.position.y + part.offset.y,
                    character.position.z + part.offset.z
                };
                
                // Crear los 8 vértices del cubo de esta parte
                double hx = part.size.x / 2.0;
                double hy = part.size.y / 2.0;
                double hz = part.size.z / 2.0;
                
                vector<Vec3> cubeLocal = {
                    {-hx, -hy, -hz}, { hx, -hy, -hz}, { hx,  hy, -hz}, {-hx,  hy, -hz},
                    {-hx, -hy,  hz}, { hx, -hy,  hz}, { hx,  hy,  hz}, {-hx,  hy,  hz}
                };
                
                vector<Vec3> cubeWorld(8);
                vector<Vec3> cubeTransformed(8);
                for(int i=0; i<8; i++) {
                    cubeWorld[i] = {
                        partPos.x + cubeLocal[i].x,
                        partPos.y + cubeLocal[i].y,
                        partPos.z + cubeLocal[i].z
                    };
                    // Transformar con rotaciones de cámara
                    Vec3 wc_flip = rotY(cubeWorld[i], extraFlip);
                    cubeTransformed[i] = rotX(rotY(rotZ(wc_flip, -roll), -yaw), -pitch);
                }
                
                // Caras del cubo con sus normales
                struct FaceInfo {
                    vector<int> indices;
                    Vec3 normalDir; // Normal en espacio local
                };
                
                vector<FaceInfo> faces = {
                    {{0,1,2,3}, {0,0,-1}},  // back
                    {{4,5,6,7}, {0,0,1}},   // front
                    {{0,1,5,4}, {0,-1,0}},  // bottom
                    {{2,3,7,6}, {0,1,0}},   // top
                    {{1,2,6,5}, {1,0,0}},   // right
                    {{0,3,7,4}, {-1,0,0}}   // left
                };
                
                for(auto &faceInfo: faces) {
                    auto &f = faceInfo.indices;
                    
                    // Calcular profundidad promedio y normal transformada
                    double avgZ = 0.0;
                    vector<Point> pts2;
                    
                    for(int idx: f) {
                        Point pp = projectPoint(cubeTransformed[idx], width, height, fov, camDist);
                        pts2.push_back(pp);
                        avgZ += cubeTransformed[idx].z;
                    }
                    avgZ /= f.size();
                    
                    // Backface culling: calcular si la cara apunta hacia la cámara
                    Vec3 v1 = {
                        cubeTransformed[f[1]].x - cubeTransformed[f[0]].x,
                        cubeTransformed[f[1]].y - cubeTransformed[f[0]].y,
                        cubeTransformed[f[1]].z - cubeTransformed[f[0]].z
                    };
                    Vec3 v2 = {
                        cubeTransformed[f[2]].x - cubeTransformed[f[0]].x,
                        cubeTransformed[f[2]].y - cubeTransformed[f[0]].y,
                        cubeTransformed[f[2]].z - cubeTransformed[f[0]].z
                    };
                    
                    // Producto cruz para obtener normal
                    Vec3 normal = {
                        v1.y * v2.z - v1.z * v2.y,
                        v1.z * v2.x - v1.x * v2.z,
                        v1.x * v2.y - v1.y * v2.x
                    };
                    
                    // Si la normal apunta hacia la cámara (z > 0), no dibujar esta cara
                    if(normal.z > 0) continue;
                    
                    // Iluminación simple: caras más alejadas = más oscuras
                    double lightIntensity = 0.85 + 0.15 * abs(normal.z) / sqrt(normal.x*normal.x + normal.y*normal.y + normal.z*normal.z + 0.001);
                    
                    Scalar shadedColor(
                        min(255.0, part.color[0] * lightIntensity),
                        min(255.0, part.color[1] * lightIntensity),
                        min(255.0, part.color[2] * lightIntensity)
                    );
                    
                    allFaces.push_back({avgZ, pts2, shadedColor, normal});
                }
            }
            
            // Ordenar todas las caras por profundidad (más lejanas primero)
            sort(allFaces.begin(), allFaces.end(), [](const FaceDraw &a, const FaceDraw &b){ 
                return a.depth > b.depth; 
            });
            
            // Dibujar todas las caras
            for(auto &fd: allFaces) {
                const Point *pp = &fd.pts[0]; 
                int n = (int)fd.pts.size();
                fillConvexPoly(frame, pp, n, fd.color);
                polylines(frame, vector<vector<Point>>{fd.pts}, true, Scalar(0,0,0), 2);
            }
        };

        // RENDERIZAR LOS 2 PERSONAJES
        renderRobloxCharacter(player1);
        renderRobloxCharacter(player2);
        
        // DIBUJAR ROSTROS en las cabezas (solo en cara frontal)
        auto drawFace = [&](const RobloxCharacter& character) {
            // Posición de la cabeza en el mundo
            Vec3 headPos = {
                character.position.x + character.head.offset.x,
                character.position.y + character.head.offset.y,
                character.position.z + character.head.offset.z
            };
            
            // Calcular la cara frontal de la cabeza (cara que mira hacia +Y)
            double halfSize = character.head.size.y / 2.0;
            Vec3 frontFaceCenter = {
                headPos.x,
                headPos.y + halfSize, // Cara frontal en +Y
                headPos.z
            };
            
            // Transformar el centro de la cara frontal
            Vec3 faceFlip = rotY(frontFaceCenter, extraFlip);
            Vec3 faceTrans = rotX(rotY(rotZ(faceFlip, -roll), -yaw), -pitch);
            
            // Calcular la normal de la cara frontal después de transformación
            Vec3 normalStart = headPos;
            Vec3 normalEnd = {headPos.x, headPos.y + 1.0, headPos.z};
            Vec3 normStartFlip = rotY(normalStart, extraFlip);
            Vec3 normEndFlip = rotY(normalEnd, extraFlip);
            Vec3 normStartTrans = rotX(rotY(rotZ(normStartFlip, -roll), -yaw), -pitch);
            Vec3 normEndTrans = rotX(rotY(rotZ(normEndFlip, -roll), -yaw), -pitch);
            
            Vec3 faceNormal = {
                normEndTrans.x - normStartTrans.x,
                normEndTrans.y - normStartTrans.y,
                normEndTrans.z - normStartTrans.z
            };
            
            // Solo dibujar si la cara frontal está visible (mirando hacia la cámara)
            // En espacio de cámara, la cámara está en -Z, así que queremos normal.z < 0
            if(faceNormal.z >= 0) return; // No dibujar si la cara no es visible
            
            Point faceCenter = projectPoint(faceTrans, width, height, fov, camDist);
            
            // Calcular tamaño de la cabeza en pantalla
            double f = (width/2) / tan(fov * 0.5);
            double zcam = faceTrans.z + camDist;
            if(zcam <= 0.0001) zcam = 0.0001;
            double headSize = character.head.size.x * (f / zcam);
            
            // Dibujar ojos (dos círculos negros)
            int eyeRadius = max(2, (int)(headSize * 0.15));
            int eyeOffsetX = (int)(headSize * 0.25);
            int eyeOffsetY = (int)(headSize * 0.15);
            
            Point leftEye(faceCenter.x - eyeOffsetX, faceCenter.y - eyeOffsetY);
            Point rightEye(faceCenter.x + eyeOffsetX, faceCenter.y - eyeOffsetY);
            
            circle(frame, leftEye, eyeRadius, Scalar(0, 0, 0), FILLED);
            circle(frame, rightEye, eyeRadius, Scalar(0, 0, 0), FILLED);
            
            // Dibujar boca (línea curva sonriente)
            int mouthWidth = (int)(headSize * 0.35);
            int mouthY = faceCenter.y + (int)(headSize * 0.2);
            
            // Sonrisa simple (arco)
            ellipse(frame, Point(faceCenter.x, mouthY - eyeRadius/2), 
                   Size(mouthWidth, eyeRadius * 2), 0, 0, 180, 
                   Scalar(0, 0, 0), 2);
        };
        
        drawFace(player1);
        drawFace(player2);

        // ========== CÓDIGO ANTIGUO COMENTADO (dibujar cuadrado atrapador) ==========
        // Dibujar atrapador como un cubo 3D (vértices y caras), con depth-sort
        // double half = sqSize/2.0;
        // double halfD = sqSize/2.0;
        // vector<Vec3> cubeLocal = {
        //     {-half, -half, -halfD}, { half, -half, -halfD}, { half,  half, -halfD}, {-half,  half, -halfD},
        //     {-half, -half,  halfD}, { half, -half,  halfD}, { half,  half,  halfD}, {-half,  half,  halfD}
        // };
        // vector<Vec3> cubeWorld(8);
        // for(int i=0;i<8;i++) cubeWorld[i] = {sq.x + cubeLocal[i].x, sq.y + cubeLocal[i].y, sq.z + cubeLocal[i].z};
        // vector<vector<int>> faces = {
        //     {0,1,2,3}, {4,5,6,7}, {0,1,5,4}, {2,3,7,6}, {1,2,6,5}, {0,3,7,4}
        // };
        // struct FaceDraw { double depth; vector<Point> pts; Scalar color; };
        // vector<FaceDraw> toDraw;
        // for(auto &f: faces){
        //     double avgZ = 0.0;
        //     vector<Point> pts2;
        //     for(int idx: f){
        //         Vec3 wc = cubeWorld[idx];
        //         Vec3 wc_flip = rotY(wc, extraFlip);
        //         Vec3 tc = rotX(rotY(rotZ(wc_flip, -roll), -yaw), -pitch);
        //         Point pp = projectPoint(tc, width, height, fov, camDist);
        //         pts2.push_back(pp);
        //         avgZ += tc.z;
        //     }
        //     avgZ /= f.size();
        //     double shade = 0.6 + 0.4 * (1.0 - (avgZ / (maxZ + camDist)));
        //     Scalar base(100*shade, 180*shade, 240*shade);
        //     toDraw.push_back({avgZ, pts2, base});
        // }
        // sort(toDraw.begin(), toDraw.end(), [](const FaceDraw &a, const FaceDraw &b){ return a.depth > b.depth; });
        // for(auto &fd: toDraw){
        //     const Point *pp = &fd.pts[0]; int n = (int)fd.pts.size();
        //     fillConvexPoly(frame, pp, n, fd.color);
        //     polylines(frame, vector<vector<Point>>{fd.pts}, true, Scalar(20,60,110), 2);
        // }
        // ============================================

        // HUD
        putText(frame, "ROBLOX ARENA - 2 JUGADORES", Point(10,25), FONT_HERSHEY_SIMPLEX, 0.8, Scalar(230,230,230), 2);
        
        // Controles Jugador 1
        putText(frame, "JUGADOR 1 (Izquierda)", Point(10,55), FONT_HERSHEY_SIMPLEX, 0.6, player1.headColor, 2);
        putText(frame, "W/A/S/D = Mover", Point(10,80), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(200,200,200), 1);
        
        // Controles Jugador 2
        putText(frame, "JUGADOR 2 (Derecha)", Point(width-250,55), FONT_HERSHEY_SIMPLEX, 0.6, player2.headColor, 2);
        putText(frame, "I/J/K/L = Mover", Point(width-250,80), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(200,200,200), 1);
        
        // Info adicional
        putText(frame, "Vista: Isometrica | Mouse: Rotar camara | Scroll: Zoom", Point(10,height-40), FONT_HERSHEY_SIMPLEX, 0.45, Scalar(150,150,150),1);
        putText(frame, "ESC = Salir", Point(10,height-20), FONT_HERSHEY_SIMPLEX, 0.45, Scalar(150,150,150),1);
        
        // Mostrar posiciones
        string pos1 = format("P1: (%.1f, %.1f)", player1.position.x, player1.position.y);
        string pos2 = format("P2: (%.1f, %.1f)", player2.position.x, player2.position.y);
        putText(frame, pos1, Point(10, 105), FONT_HERSHEY_SIMPLEX, 0.4, Scalar(180,180,180), 1);
        putText(frame, pos2, Point(width-250, 105), FONT_HERSHEY_SIMPLEX, 0.4, Scalar(180,180,180), 1);

    imshow("Juego3D", frame);

    // Leer tecla una vez por frame
    int kk = waitKey(16);
    if(kk==27) break;
    
    // ========== CONTROLES JUGADOR 1 (WASD) - Corregidos para isométrica ==========
    // En vista isométrica, los ejes están rotados 45°, así que ajustamos:
    if(kk=='w' || kk=='W') player1.move(-0.707, 0.707);      // Arriba-diagonal
    else if(kk=='s' || kk=='S') player1.move(0.707, -0.707); // Abajo-diagonal
    else if(kk=='a' || kk=='A') player1.move(-0.707, -0.707); // Izquierda-diagonal
    else if(kk=='d' || kk=='D') player1.move(0.707, 0.707);  // Derecha-diagonal
    
    // ========== CONTROLES JUGADOR 2 (IJKL) - Corregidos para isométrica ==========
    else if(kk=='i' || kk=='I') player2.move(-0.707, 0.707);  // Arriba-diagonal
    else if(kk=='k' || kk=='K') player2.move(0.707, -0.707); // Abajo-diagonal
    else if(kk=='j' || kk=='J') player2.move(-0.707, -0.707); // Izquierda-diagonal
    else if(kk=='l' || kk=='L') player2.move(0.707, 0.707);  // Derecha-diagonal
    
    // ========== CONTROLES DE CÁMARA (OPCIONAL) ==========
    else if(kk=='+' || kk=='=') extraFlip += 0.1;
    else if(kk=='-' || kk=='_') extraFlip -= 0.1;
    
    // Limitar personajes dentro del campo
    double limit = worldHalf - 1.0;
    player1.position.x = min(max(player1.position.x, -limit), limit);
    player1.position.y = min(max(player1.position.y, -limit), limit);
    player2.position.x = min(max(player2.position.x, -limit), limit);
    player2.position.y = min(max(player2.position.y, -limit), limit);
    }

    return 0;
}
