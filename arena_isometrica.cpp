#include <opencv2/opencv.hpp>
#include <cmath>
#include <vector>
#include <algorithm>
#include <random>

using namespace cv;
using namespace std;

struct V3 { double x, y, z; };

// Proyección isométrica
Point isoProject(V3 p, int w, int h, double zoom) {
    // apply global pan offsets (set via setIsoPan)
    extern double g_isoPanX, g_isoPanY;
    double px = p.x - g_isoPanX;
    double py = p.y - g_isoPanY;
    double screenX = (px - py) * zoom;
    double screenY = ((px + py) * 0.5 - p.z) * zoom;
    return Point(w/2 + (int)screenX, h/2 + (int)screenY);
}

// Global pan offsets used by isoProject
double g_isoPanX = 0.0;
double g_isoPanY = 0.0;
void setIsoPan(double x, double y) { g_isoPanX = x; g_isoPanY = y; }
// Global zoom used by mouse callback and main
double zoom = 25.0;

// Estructura para pelotas que caen
struct Ball {
    V3 pos;
    V3 vel;
    double radius;
    bool alive;
    Scalar color;
    bool golpeoReciente;  // Para evitar múltiples golpes seguidos
    int framesSinGolpe;   // Contador de frames sin golpear
    
    Ball(double x, double y, double z, double r) {
        pos = {x, y, z};
        // Velocidad inicial aleatoria
        vel = {(rand() % 40 - 20) / 100.0, (rand() % 40 - 20) / 100.0, -0.2};
        radius = r;
        alive = true;
        golpeoReciente = false;
        framesSinGolpe = 0;
        // Color aleatorio brillante
        color = Scalar(80 + rand()%176, 80 + rand()%176, 80 + rand()%176);
    }
};

// Cara de un cubo 3D
struct Face3D {
    Point pts[4];
    Scalar color;
    double depth;
    bool drawFace;
};

// Personaje Roblox
struct RobloxPlayer {
    V3 pos;
    double velZ;         // Velocidad vertical para salto
    bool enSuelo;        // Si está tocando el suelo
    double speed;        // Velocidad de movimiento
    int vida;            // Puntos de vida (0-100)
    int playerNum;       // Número de jugador
    bool muerto;         // Si el jugador está muerto
    int framesDesdeRespawn;  // Frames desde que murió
    V3 posicionMuerte;   // Posición donde murió
    Scalar colorCabeza, colorTorso, colorBrazo, colorPierna;
    
    RobloxPlayer(double x, double y, int playerNum) {
        pos = {x, y, 0};
        velZ = 0;
        enSuelo = true;
        speed = 0.50;  // Velocidad aumentada para movimiento más rápido
        vida = 100;  // Vida inicial
        this->playerNum = playerNum;
        muerto = false;
        framesDesdeRespawn = 0;
        
        if(playerNum == 1) {
            colorCabeza = Scalar(80, 220, 255);
            colorTorso = Scalar(220, 180, 80);
            colorBrazo = Scalar(80, 220, 255);
            colorPierna = Scalar(80, 200, 80);
        } else {
            colorCabeza = Scalar(80, 120, 255);
            colorTorso = Scalar(180, 180, 180);
            colorBrazo = Scalar(80, 160, 255);
            colorPierna = Scalar(150, 80, 220);
        }
    }
    
    void recibirDanio(int cantidad) {
        vida -= cantidad;
        if(vida < 0) vida = 0;
        if(vida == 0 && !muerto) {
            muerto = true;
            framesDesdeRespawn = 0;
            posicionMuerte = pos;
        }
    }
    
    bool estaVivo() const {
        return vida > 0 && !muerto;
    }
    
    void saltar() {
        if(enSuelo) {
            velZ = 0.3;  // Impulso inicial del salto
            enSuelo = false;
        }
    }
    
    void actualizarFisica() {
        // Aplicar gravedad
        velZ -= 0.02;  // Gravedad
        pos.z += velZ;
        
        // Colisión con el suelo
        if(pos.z <= 0) {
            pos.z = 0;
            velZ = 0;
            enSuelo = true;
        }
    }
    
    void mover(double dx, double dy, double limite) {
        pos.x += dx * speed;
        pos.y += dy * speed;
        
        // Limitar dentro del campo
        if(pos.x < -limite) pos.x = -limite;
        if(pos.x > limite) pos.x = limite;
        if(pos.y < -limite) pos.y = -limite;
        if(pos.y > limite) pos.y = limite;
    }
};

// Simple global mouse camera control (single view)
bool g_mousePressed = false;
int g_lastMouseX = 0, g_lastMouseY = 0;
// Mouse callback: dragging pans camera (global g_isoPanX/Y), wheel zooms (global zoom)
void mouseCamCallback(int event, int x, int y, int flags, void* userdata) {
    (void)userdata;
    if(event == EVENT_LBUTTONDOWN) {
        g_mousePressed = true;
        g_lastMouseX = x; g_lastMouseY = y;
    } else if(event == EVENT_LBUTTONUP) {
        g_mousePressed = false;
    } else if(event == EVENT_MOUSEMOVE && g_mousePressed) {
        int dx = x - g_lastMouseX;
        int dy = y - g_lastMouseY;
        // Convert pixel delta to world units via current zoom
        double sensitivity = 1.0 / max(1.0, zoom);
        g_isoPanX += -dx * sensitivity;
        g_isoPanY += dy * sensitivity;
        g_lastMouseX = x; g_lastMouseY = y;
    } else if(event == EVENT_MOUSEWHEEL) {
        int delta = getMouseWheelDelta(flags);
        zoom += -delta * 1.0;
        if(zoom < 5.0) zoom = 5.0;
        if(zoom > 200.0) zoom = 200.0;
    }
}

// Dibujar un cubo 3D isométrico
void drawCube3D(vector<Face3D>& faces, V3 center, V3 size, Scalar color, int w, int h, double zoom, bool isFrontFace = false) {
    // 8 vértices del cubo
    V3 v[8];
    for(int i = 0; i < 8; i++) {
        v[i].x = center.x + ((i & 1) ? size.x/2 : -size.x/2);
        v[i].y = center.y + ((i & 2) ? size.y/2 : -size.y/2);
        v[i].z = center.z + ((i & 4) ? size.z/2 : -size.z/2);
    }
    
    // Proyectar los 8 vértices
    Point proj[8];
    for(int i = 0; i < 8; i++) {
        proj[i] = isoProject(v[i], w, h, zoom);
    }
    
    // Las 3 caras visibles desde vista isométrica
    int visibleFaces[3][4] = {
        {4, 5, 7, 6},  // Cara superior
        {2, 3, 7, 6},  // Cara frontal (Y+)
        {1, 5, 7, 3}   // Cara derecha
    };
    
    // Colores con sombreado para efecto 3D
    Scalar faceColors[3];
    faceColors[0] = Scalar(min(255.0, color[0]*1.3), min(255.0, color[1]*1.3), min(255.0, color[2]*1.3));
    faceColors[1] = color;
    faceColors[2] = Scalar(color[0]*0.6, color[1]*0.6, color[2]*0.6);
    
    for(int f = 0; f < 3; f++) {
        Face3D face;
        for(int i = 0; i < 4; i++) {
            face.pts[i] = proj[visibleFaces[f][i]];
        }
        
        // Profundidad promedio
        double totalZ = 0;
        for(int i = 0; i < 4; i++) {
            totalZ += v[visibleFaces[f][i]].z;
        }
        face.depth = totalZ / 4.0;
        face.color = faceColors[f];
        face.drawFace = (isFrontFace && f == 1);
        
        faces.push_back(face);
    }
}

// Renderizar personaje Roblox
void renderRobloxPlayer(vector<Face3D>& faces, const RobloxPlayer& player, int w, int h, double zoom) {
    V3 base = player.pos;
    
    // PIERNAS
    drawCube3D(faces, {base.x - 0.3, base.y, base.z + 0.5}, {0.4, 0.4, 1.0}, player.colorPierna, w, h, zoom);
    drawCube3D(faces, {base.x + 0.3, base.y, base.z + 0.5}, {0.4, 0.4, 1.0}, player.colorPierna, w, h, zoom);
    
    // TORSO
    drawCube3D(faces, {base.x, base.y, base.z + 1.6}, {1.2, 0.6, 1.2}, player.colorTorso, w, h, zoom);
    
    // BRAZOS
    drawCube3D(faces, {base.x - 0.8, base.y, base.z + 1.6}, {0.4, 0.4, 1.0}, player.colorBrazo, w, h, zoom);
    drawCube3D(faces, {base.x + 0.8, base.y, base.z + 1.6}, {0.4, 0.4, 1.0}, player.colorBrazo, w, h, zoom);
    
    // CABEZA
    drawCube3D(faces, {base.x, base.y, base.z + 2.7}, {0.8, 0.8, 0.8}, player.colorCabeza, w, h, zoom, true);
}

// Dibujar pelota en isométrico
void drawBall(Mat& img, const Ball& ball, int w, int h, double zoom) {
    // Proyectar centro de la pelota
    Point center = isoProject(ball.pos, w, h, zoom);
    
    // Radio en pantalla (aproximado)
    int screenRadius = (int)(ball.radius * zoom);
    if(screenRadius < 2) screenRadius = 2;
    
    // Dibujar pelota con degradado simple
    circle(img, center, screenRadius, ball.color, -1);
    circle(img, center, screenRadius, Scalar(0, 0, 0), 2);  // Borde negro
    
    // Highlight para efecto 3D
    Point highlight(center.x - screenRadius/3, center.y - screenRadius/3);
    circle(img, highlight, screenRadius/3, Scalar(255, 255, 255), -1);
}

// Dibujar barra de vida sobre la cabeza del personaje
void drawHealthBar(Mat& img, const RobloxPlayer& player, int w, int h, double zoom) {
    if(!player.estaVivo()) return;
    
    // Posición de la barra (sobre la cabeza)
    V3 barPos = {player.pos.x, player.pos.y, player.pos.z + 3.8};
    Point barCenter = isoProject(barPos, w, h, zoom);
    
    int barWidth = 40;
    int barHeight = 6;
    int barX = barCenter.x - barWidth/2;
    int barY = barCenter.y;
    
    // Fondo de la barra (negro)
    rectangle(img, Point(barX-1, barY-1), Point(barX + barWidth+1, barY + barHeight+1), 
              Scalar(0, 0, 0), -1);
    
    // Barra de vida vacía (rojo oscuro)
    rectangle(img, Point(barX, barY), Point(barX + barWidth, barY + barHeight), 
              Scalar(40, 40, 100), -1);
    
    // Barra de vida actual
    int vidaWidth = (int)((player.vida / 100.0) * barWidth);
    Scalar colorVida;
    if(player.vida > 60) {
        colorVida = Scalar(80, 220, 80);  // Verde
    } else if(player.vida > 30) {
        colorVida = Scalar(80, 200, 255); // Amarillo
    } else {
        colorVida = Scalar(80, 80, 255);  // Rojo
    }
    
    if(vidaWidth > 0) {
        rectangle(img, Point(barX, barY), Point(barX + vidaWidth, barY + barHeight), 
                  colorVida, -1);
    }
    
    // Texto de vida
    string vidaText = to_string(player.vida);
    putText(img, vidaText, Point(barX + barWidth + 5, barY + barHeight), 
            FONT_HERSHEY_SIMPLEX, 0.4, Scalar(255, 255, 255), 1);
}

// Dibujar destello de muerte
void drawDeathFlash(Mat& img, const RobloxPlayer& player, int w, int h, double zoom) {
    int frames = player.framesDesdeRespawn;
    
    // El destello dura 60 frames (2 segundos a 30 fps)
    if(frames < 60) {
        // Calcular intensidad del destello (fade out)
        double intensity = 1.0 - (frames / 60.0);
        intensity = intensity * intensity; // Curva cuadrática para fade más dramático
        
        // Posición del destello
        Point center = isoProject(player.posicionMuerte, w, h, zoom);
        
        // Dibujar múltiples círculos con transparencia simulada
        int maxRadius = 80;
        int numCircles = 5;
        
        for(int i = numCircles; i >= 0; i--) {
            int radius = (int)(maxRadius * intensity * (i + 1) / numCircles);
            int alpha = (int)(255 * intensity * (numCircles - i) / numCircles);
            
            // Color rojo brillante con fade
            Scalar color(0, 0, 255 * alpha / 255);
            circle(img, center, radius, color, -1, LINE_AA);
        }
        
        // Círculo blanco brillante en el centro
        int centerRadius = (int)(15 * intensity);
        if(centerRadius > 0) {
            circle(img, center, centerRadius, Scalar(200, 200, 255), -1, LINE_AA);
        }
    }
}

// Función para aplicar textura a un polígono (pared)
void applyTextureToWall(Mat& img, Point* pts, int npts, const Mat& texture) {
    if(texture.empty()) return;
    
    // Obtener el bounding box del polígono
    Rect bbox = boundingRect(vector<Point>(pts, pts + npts));
    
    // Crear máscara para el polígono
    Mat mask = Mat::zeros(img.size(), CV_8UC1);
    fillConvexPoly(mask, pts, npts, Scalar(255));
    
    // Redimensionar textura al tamaño del bbox
    Mat resizedTexture;
    resize(texture, resizedTexture, bbox.size());
    
    // Aplicar la textura solo en el área del polígono
    for(int y = 0; y < bbox.height; y++) {
        for(int x = 0; x < bbox.width; x++) {
            int imgX = bbox.x + x;
            int imgY = bbox.y + y;
            
            if(imgX >= 0 && imgX < img.cols && imgY >= 0 && imgY < img.rows) {
                if(mask.at<uchar>(imgY, imgX) > 0) {
                    img.at<Vec3b>(imgY, imgX) = resizedTexture.at<Vec3b>(y, x);
                }
            }
        }
    }
}

int main() {
    int W = 1200, H = 800;
    Mat img(H, W, CV_8UC3);
    double zoom = 25.0;

    double campoSize = 12.0;  // Tamaño del campo
    
    // CARGAR TEXTURAS PARA LAS PAREDES Y PISO
    Mat texturaPared1 = imread("sources/images-5.jpeg");
    Mat texturaPared2 = imread("sources/images-6.jpeg");
    Mat texturaPiso = imread("sources/piso.jpeg");
    
    if(texturaPared1.empty()) {
        cout << "Error: No se pudo cargar sources/images-5.jpeg" << endl;
    }
    if(texturaPared2.empty()) {
        cout << "Error: No se pudo cargar sources/images-6.jpeg" << endl;
    }
    if(texturaPiso.empty()) {
        cout << "Error: No se pudo cargar sources/piso.jpeg" << endl;
    }
    
    // CREAR 2 JUGADORES
    RobloxPlayer player1(-4.0, 0.0, 1);
    RobloxPlayer player2(4.0, 0.0, 2);
    
    // CREAR 30 PELOTAS cayendo del cielo
    vector<Ball> balls;
    srand(time(0));
    for(int i = 0; i < 30; i++) {
        double x = -10.0 + (rand() % 200) / 10.0;  // Entre -10 y 10
        double y = -10.0 + (rand() % 200) / 10.0;
        double z = 20.0 + (rand() % 150) / 10.0;  // Caen desde arriba (20-35)
        double r = 0.25 + (rand() % 4) * 0.08;    // Radios variados
        balls.push_back(Ball(x, y, z, r));
    }
    
    namedWindow("SQUID GAMES", WINDOW_AUTOSIZE);
    // setup mouse callback for single-view camera control
    setMouseCallback("SQUID GAMES", mouseCamCallback, nullptr);
    
    while(true) {
    img.setTo(Scalar(25, 25, 35));

    // Single-view rendering: all drawing happens into `img` below
        
        // ===== ACTUALIZAR FÍSICAS DE JUGADORES =====
        player1.actualizarFisica();
        player2.actualizarFisica();
        
        // Actualizar contador de frames desde muerte
        if(player1.muerto) player1.framesDesdeRespawn++;
        if(player2.muerto) player2.framesDesdeRespawn++;
        
        // ===== ACTUALIZAR FÍSICAS DE PELOTAS =====
        for(auto& ball : balls) {
            if(!ball.alive) continue;
            
            // Actualizar contador de frames sin golpe
            if(ball.golpeoReciente) {
                ball.framesSinGolpe++;
                if(ball.framesSinGolpe > 30) {  // 30 frames = ~1 segundo
                    ball.golpeoReciente = false;
                    ball.framesSinGolpe = 0;
                }
            }
            
            // Actualizar posición
            ball.pos.x += ball.vel.x;
            ball.pos.y += ball.vel.y;
            ball.pos.z += ball.vel.z;
            
            // Aplicar gravedad
            ball.vel.z -= 0.018;  // Gravedad más fuerte
            
            // Rebotes en los límites del campo con más energía
            double minX = -campoSize + ball.radius;
            double maxX = campoSize - ball.radius;
            double minY = -campoSize + ball.radius;
            double maxY = campoSize - ball.radius;
            
            if(ball.pos.x < minX) { 
                ball.pos.x = minX; 
                ball.vel.x *= -0.9;  // Rebote con más energía
                ball.vel.y += (rand() % 20 - 10) / 100.0;  // Variación aleatoria
            }
            if(ball.pos.x > maxX) { 
                ball.pos.x = maxX; 
                ball.vel.x *= -0.9; 
                ball.vel.y += (rand() % 20 - 10) / 100.0;
            }
            if(ball.pos.y < minY) { 
                ball.pos.y = minY; 
                ball.vel.y *= -0.9; 
                ball.vel.x += (rand() % 20 - 10) / 100.0;
            }
            if(ball.pos.y > maxY) { 
                ball.pos.y = maxY; 
                ball.vel.y *= -0.9; 
                ball.vel.x += (rand() % 20 - 10) / 100.0;
            }
            
            // Rebote en el suelo con más energía
            if(ball.pos.z <= ball.radius) {
                ball.pos.z = ball.radius;
                ball.vel.z *= -0.85;  // Rebote con más energía
                ball.vel.x *= 0.95;   // Menos fricción
                ball.vel.y *= 0.95;
                
                // Agregar variación aleatoria al rebotar
                ball.vel.x += (rand() % 20 - 10) / 100.0;
                ball.vel.y += (rand() % 20 - 10) / 100.0;
                
                // Si está casi quieta, darle un impulso
                if(abs(ball.vel.z) < 0.08 && abs(ball.vel.x) < 0.05 && abs(ball.vel.y) < 0.05) {
                    ball.vel.z = 0.3 + (rand() % 20) / 100.0;  // Impulso aleatorio
                    ball.vel.x = (rand() % 40 - 20) / 50.0;
                    ball.vel.y = (rand() % 40 - 20) / 50.0;
                }
            }
            
            // Límite de altura para que no salgan volando
            if(ball.pos.z > 30.0) {
                ball.pos.z = 30.0;
                ball.vel.z *= -0.5;
            }
        }
        
        // ===== DETECTAR COLISIONES PELOTA-JUGADOR =====
        for(auto& ball : balls) {
            if(!ball.alive || ball.golpeoReciente) continue;
            
            // Colisión con jugador 1
            if(player1.estaVivo()) {
                double dx = ball.pos.x - player1.pos.x;
                double dy = ball.pos.y - player1.pos.y;
                double dz = ball.pos.z - (player1.pos.z + 1.5);  // Centro del personaje
                double distancia = sqrt(dx*dx + dy*dy + dz*dz);
                
                if(distancia < (ball.radius + 1.2)) {  // Radio de colisión del personaje
                    player1.recibirDanio(10);
                    ball.golpeoReciente = true;
                    // Rebotar pelota en dirección opuesta
                    ball.vel.x = -dx * 0.3;
                    ball.vel.y = -dy * 0.3;
                    ball.vel.z = abs(ball.vel.z) * 0.5 + 0.2;
                }
            }
            
            // Colisión con jugador 2
            if(player2.estaVivo()) {
                double dx = ball.pos.x - player2.pos.x;
                double dy = ball.pos.y - player2.pos.y;
                double dz = ball.pos.z - (player2.pos.z + 1.5);
                double distancia = sqrt(dx*dx + dy*dy + dz*dz);
                
                if(distancia < (ball.radius + 1.2)) {
                    player2.recibirDanio(10);
                    ball.golpeoReciente = true;
                    ball.vel.x = -dx * 0.3;
                    ball.vel.y = -dy * 0.3;
                    ball.vel.z = abs(ball.vel.z) * 0.5 + 0.2;
                }
            }
        }
        
        // ===== LEER TECLAS =====
        int k = waitKey(30);
        if(k == 27) break;  // ESC para salir
        else if(k == '+' || k == '=') zoom += 2;
        else if(k == '-' || k == '_') zoom = max(10.0, zoom - 2);
        
        // ===== CONTROLES JUGADOR 1 (WASD + Q para saltar) =====
        if(player1.estaVivo()) {
            double dx1 = 0, dy1 = 0;
            if(k == 'q' || k == 'Q') player1.saltar();  // Q para saltar
            if(k == 'w' || k == 'W') { dy1 -= 1.0; }    // Arriba
            if(k == 's' || k == 'S') { dy1 += 1.0; }    // Abajo
            if(k == 'a' || k == 'A') { dx1 -= 1.0; }    // Izquierda
            if(k == 'd' || k == 'D') { dx1 += 1.0; }    // Derecha
            
            // Normalizar movimiento diagonal
            double mag1 = sqrt(dx1*dx1 + dy1*dy1);
            if(mag1 > 0.001) {
                dx1 /= mag1;
                dy1 /= mag1;
            }
            player1.mover(dx1, dy1, campoSize - 1.0);
        }
        
        // ===== CONTROLES JUGADOR 2 (IJKL + U para saltar) =====
        if(player2.estaVivo()) {
            double dx2 = 0, dy2 = 0;
            if(k == 'u' || k == 'U') player2.saltar();  // U para saltar
            if(k == 'i' || k == 'I') { dy2 -= 1.0; }    // Arriba
            if(k == 'k' || k == 'K') { dy2 += 1.0; }    // Abajo
            if(k == 'j' || k == 'J') { dx2 -= 1.0; }    // Izquierda
            if(k == 'l' || k == 'L') { dx2 += 1.0; }    // Derecha
            
            // Normalizar movimiento diagonal
            double mag2 = sqrt(dx2*dx2 + dy2*dy2);
            if(mag2 > 0.001) {
                dx2 /= mag2;
                dy2 /= mag2;
            }
            player2.mover(dx2, dy2, campoSize - 1.0);
        }
        
        // ===== PAREDES DEL FONDO CON TEXTURA =====
        double alturaParedes = 8.0;  // Altura de las paredes
        
        // PARED FONDO IZQUIERDA (en Y = -campoSize, de X=-campoSize a X=campoSize)
        V3 paredIzq[4] = {
            {-campoSize, -campoSize, 0},           // Esquina inferior izquierda
            {campoSize, -campoSize, 0},            // Esquina inferior derecha
            {campoSize, -campoSize, alturaParedes},  // Esquina superior derecha
            {-campoSize, -campoSize, alturaParedes}  // Esquina superior izquierda
        };
        Point paredIzqPts[4];
        for(int i = 0; i < 4; i++) {
            paredIzqPts[i] = isoProject(paredIzq[i], W, H, zoom);
        }
        
        // Aplicar textura a pared izquierda
        if(!texturaPared1.empty()) {
            applyTextureToWall(img, paredIzqPts, 4, texturaPared1);
        } else {
            fillConvexPoly(img, paredIzqPts, 4, Scalar(120, 100, 80));  // Color ladrillo fallback
        }
        
        // PARED FONDO DERECHA (en X = campoSize, de Y=-campoSize a Y=campoSize)
        V3 paredDer[4] = {
            {campoSize, -campoSize, 0},            // Esquina inferior frontal
            {campoSize, campoSize, 0},             // Esquina inferior trasera
            {campoSize, campoSize, alturaParedes},   // Esquina superior trasera
            {campoSize, -campoSize, alturaParedes}   // Esquina superior frontal
        };
        Point paredDerPts[4];
        for(int i = 0; i < 4; i++) {
            paredDerPts[i] = isoProject(paredDer[i], W, H, zoom);
        }
        
        // Aplicar textura a pared derecha
        if(!texturaPared2.empty()) {
            applyTextureToWall(img, paredDerPts, 4, texturaPared2);
        } else {
            fillConvexPoly(img, paredDerPts, 4, Scalar(100, 90, 70));  // Color ladrillo fallback
        }
        
        // PARED LATERAL IZQUIERDA (en X = -campoSize, de Y=-campoSize a Y=campoSize)
        V3 paredLatIzq[4] = {
            {-campoSize, campoSize, 0},             // Esquina inferior trasera
            {-campoSize, -campoSize, 0},            // Esquina inferior frontal
            {-campoSize, -campoSize, alturaParedes},  // Esquina superior frontal
            {-campoSize, campoSize, alturaParedes}    // Esquina superior trasera
        };
        Point paredLatIzqPts[4];
        for(int i = 0; i < 4; i++) {
            paredLatIzqPts[i] = isoProject(paredLatIzq[i], W, H, zoom);
        }
        
        // Aplicar textura a pared lateral izquierda
        if(!texturaPared2.empty()) {
            applyTextureToWall(img, paredLatIzqPts, 4, texturaPared2);
        } else {
            fillConvexPoly(img, paredLatIzqPts, 4, Scalar(110, 95, 75));  // Color ladrillo fallback
        }
        
        // PLANO DEL SUELO CON TEXTURA
        V3 corners[4] = {
            {-campoSize, -campoSize, 0}, {campoSize, -campoSize, 0},
            {campoSize, campoSize, 0}, {-campoSize, campoSize, 0}
        };
        Point planePts[4];
        for(int i = 0; i < 4; i++) {
            planePts[i] = isoProject(corners[i], W, H, zoom);
        }
        
        // Aplicar textura al piso
        if(!texturaPiso.empty()) {
            applyTextureToWall(img, planePts, 4, texturaPiso);
        } else {
            fillConvexPoly(img, planePts, 4, Scalar(40, 160, 70));  // Verde fallback
        }
        
        // RENDERIZAR PERSONAJES 3D
        vector<Face3D> allFaces;
        if(player1.estaVivo()) {
            renderRobloxPlayer(allFaces, player1, W, H, zoom);
        }
        if(player2.estaVivo()) {
            renderRobloxPlayer(allFaces, player2, W, H, zoom);
        }
        
        // Ordenar por profundidad
        sort(allFaces.begin(), allFaces.end(), [](const Face3D& a, const Face3D& b) {
            return a.depth < b.depth;
        });
        
        // Dibujar todas las caras
        for(auto& face : allFaces) {
            fillConvexPoly(img, face.pts, 4, face.color);
            polylines(img, vector<vector<Point>>{vector<Point>(face.pts, face.pts+4)}, 
                     true, Scalar(0, 0, 0), 2);
            
            // Dibujar rostro
            if(face.drawFace) {
                Point center(0, 0);
                for(int i = 0; i < 4; i++) {
                    center.x += face.pts[i].x;
                    center.y += face.pts[i].y;
                }
                center.x /= 4;
                center.y /= 4;
                
                // Ojos
                circle(img, Point(center.x - 8, center.y - 5), 3, Scalar(0, 0, 0), -1);
                circle(img, Point(center.x + 8, center.y - 5), 3, Scalar(0, 0, 0), -1);
                // Boca
                ellipse(img, Point(center.x, center.y + 5), Size(8, 4), 0, 0, 180, Scalar(0, 0, 0), 2);
            }
        }
        
        // ===== DIBUJAR PELOTAS =====
        // Ordenar pelotas por profundidad (Z) para dibujar correctamente
        vector<int> ballIndices;
        for(size_t i = 0; i < balls.size(); i++) {
            if(balls[i].alive) ballIndices.push_back(i);
        }
        sort(ballIndices.begin(), ballIndices.end(), [&](int a, int b) {
            return balls[a].pos.z < balls[b].pos.z;
        });
        
        for(int idx : ballIndices) {
            drawBall(img, balls[idx], W, H, zoom);
        }
        
        // ===== DESTELLOS DE MUERTE =====
        if(player1.muerto) {
            drawDeathFlash(img, player1, W, H, zoom);
        }
        if(player2.muerto) {
            drawDeathFlash(img, player2, W, H, zoom);
        }
        
        // ===== BARRAS DE VIDA =====
        if(player1.estaVivo()) {
            drawHealthBar(img, player1, W, H, zoom);
        }
        if(player2.estaVivo()) {
            drawHealthBar(img, player2, W, H, zoom);
        }
        
        // HUD
        putText(img, "SQUID GAMES", Point(20, 40), 
                FONT_HERSHEY_COMPLEX, 1.2, Scalar(255, 255, 255), 2);
        rectangle(img, Point(15, 60), Point(480, 135), Scalar(0, 0, 0), -1);
        putText(img, "P1: WASD=Mover | Q=Saltar", Point(20, 80), 
                FONT_HERSHEY_SIMPLEX, 0.5, Scalar(80, 220, 255), 1);
        putText(img, "P2: IJKL=Mover | U=Saltar", Point(20, 100), 
                FONT_HERSHEY_SIMPLEX, 0.5, Scalar(80, 120, 255), 1);
        putText(img, "+/- Zoom", Point(20, 125), 
                FONT_HERSHEY_SIMPLEX, 0.5, Scalar(200, 200, 200), 1);
        
        putText(img, "ESC para Salir", Point(20, H-20), 
                FONT_HERSHEY_SIMPLEX, 0.5, Scalar(200, 200, 200), 1);
        
    // Single view (we removed split rendering). Display img directly.
    imshow("SQUID GAMES", img);
    }
    
    return 0;
}
