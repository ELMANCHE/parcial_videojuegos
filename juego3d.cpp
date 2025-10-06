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

struct Ball { Vec3 pos; Vec3 vel; double radius; bool alive=true; };

int main(){
    int width=1000, height=700;
    Mat frame(height, width, CV_8UC3);

    // Mundo
    double worldHalf = 8.0; // cubo desde -worldHalf..worldHalf en x,y y z desde 0..2*worldHalf
    double minZ = 0.5, maxZ = 2*worldHalf;

    // Cámara
    double yaw = -M_PI/6.0, pitch = M_PI/8.0, roll = 0.0;
    double camDist = 14.0;
    double fov = M_PI/3.0;
    // Flip/voltear escena (rotación extra alrededor de Y)
    double extraFlip = 0.0;

    // Jugador: cuadrado atrapador (centro en x,y,z)
    Vec3 sq{0.0, 0.0, 1.0}; // posición central
    double sqSize = 2.0; // tamaño en unidades del mundo (lado)
    double sqSpeed = 0.3; // movimiento por frame

    // Pelotas
    vector<Ball> balls;
    int initialBalls = 12;
    std::mt19937 rng(12345);
    uniform_real_distribution<double> ux(-worldHalf+0.5, worldHalf-0.5);
    uniform_real_distribution<double> uy(-worldHalf+0.5, worldHalf-0.5);
    uniform_real_distribution<double> uz(minZ+0.5, maxZ-0.5);
    uniform_real_distribution<double> uv(-0.2,0.2);
    for(int i=0;i<initialBalls;i++){
        Ball b; b.pos = {ux(rng), uy(rng), uz(rng)};
        b.vel = {uv(rng), uv(rng), uv(rng)};
        b.radius = 0.25 + (i%3)*0.08;
        b.alive = true;
        balls.push_back(b);
    }

    int caught = 0;

    namedWindow("Juego3D", WINDOW_AUTOSIZE);

    while(true){
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

        // Update balls
        for(auto &b: balls){
            if(!b.alive) continue;
            b.pos.x += b.vel.x;
            b.pos.y += b.vel.y;
            b.pos.z += b.vel.z;
            // Rebotes en cubo
            double minX = -worldHalf + b.radius, maxX = worldHalf - b.radius;
            double minY = -worldHalf + b.radius, maxY = worldHalf - b.radius;
            if(b.pos.x < minX){ b.pos.x=minX; b.vel.x*=-1; }
            if(b.pos.x > maxX){ b.pos.x=maxX; b.vel.x*=-1; }
            if(b.pos.y < minY){ b.pos.y=minY; b.vel.y*=-1; }
            if(b.pos.y > maxY){ b.pos.y=maxY; b.vel.y*=-1; }
            if(b.pos.z < minZ + b.radius){ b.pos.z = minZ + b.radius; b.vel.z *= -1; }
            if(b.pos.z > maxZ - b.radius){ b.pos.z = maxZ - b.radius; b.vel.z *= -1; }
        }

    // (Se unifica la lectura de teclas más abajo con waitKey(16))

        // Limitar cuadrado dentro del cubo
        sq.x = min(max(sq.x, -worldHalf + sqSize/2.0), worldHalf - sqSize/2.0);
        sq.y = min(max(sq.y, -worldHalf + sqSize/2.0), worldHalf - sqSize/2.0);
        sq.z = min(max(sq.z, minZ + 0.2), maxZ - 0.2);

        // Detección de colisiones (si la pelota toca el cuadrado, desaparece)
        for(auto &b: balls){
            if(!b.alive) continue;
            // Proyección del centro de la bola en las coordenadas del cuadrado (axis aligned)
            double dx = fabs(b.pos.x - sq.x);
            double dy = fabs(b.pos.y - sq.y);
            double dz = fabs(b.pos.z - sq.z);
            if(dx <= (sqSize/2.0 + b.radius) && dy <= (sqSize/2.0 + b.radius) && dz <= (b.radius + 0.3)){
                b.alive = false;
                caught++;
            }
        }

        // Dibujar bolas (orden simple: dibujarlas según z para mejorar z-order)
        vector<int> indices;
        for(size_t i=0;i<balls.size();++i) if(balls[i].alive) indices.push_back(i);
        sort(indices.begin(), indices.end(), [&](int a,int b){ return balls[a].pos.z < balls[b].pos.z; });
        for(int idx: indices){
            Ball &b = balls[idx];
            // proyectar
            Vec3 bflip = rotY(b.pos, extraFlip);
            Vec3 bc = rotX(rotY(rotZ(bflip, -roll), -yaw), -pitch);
            Point pb = projectPoint(bc, width, height, fov, camDist);
            double f = (width/2) / tan(fov * 0.5);
            double zcam = bc.z + camDist; if(zcam<=0.0001) zcam=0.0001;
            int rscreen = max(2, static_cast<int>(b.radius * (f / zcam)));
            circle(frame, pb, rscreen, Scalar(220,180,80), FILLED);
            // sombra sobre z=0
            Vec3 shw{b.pos.x, b.pos.y, 0.0}; Vec3 shw_flip = rotY(shw, extraFlip); Vec3 shc = rotX(rotY(rotZ(shw_flip, -roll), -yaw), -pitch);
            Point psh = projectPoint(shc, width, height, fov, camDist);
            ellipse(frame, psh, Size(max(2, (int)(rscreen*1.2)), max(2, (int)(rscreen*0.5))), 0,0,360, Scalar(30,30,30), FILLED);
        }

        // Dibujar atrapador como un cubo 3D (vértices y caras), con depth-sort
        double half = sqSize/2.0;
        double halfD = sqSize/2.0; // profundidad del cubo
        // 8 vértices del cubo (local offsets)
        vector<Vec3> cubeLocal = {
            {-half, -half, -halfD}, { half, -half, -halfD}, { half,  half, -halfD}, {-half,  half, -halfD},
            {-half, -half,  halfD}, { half, -half,  halfD}, { half,  half,  halfD}, {-half,  half,  halfD}
        };
        vector<Vec3> cubeWorld(8);
    for(int i=0;i<8;i++) cubeWorld[i] = {sq.x + cubeLocal[i].x, sq.y + cubeLocal[i].y, sq.z + cubeLocal[i].z};

        // Caras (listas de índices) - cada cara como polígono convexo
        vector<vector<int>> faces = {
            {0,1,2,3}, // back
            {4,5,6,7}, // front
            {0,1,5,4}, // bottom
            {2,3,7,6}, // top
            {1,2,6,5}, // right
            {0,3,7,4}  // left
        };

        // Para cada cara, proyectar y calcular profundidad media para ordenar
        struct FaceDraw { double depth; vector<Point> pts; Scalar color; };
        vector<FaceDraw> toDraw;
        for(auto &f: faces){
            double avgZ = 0.0;
            vector<Point> pts2;
            for(int idx: f){
                Vec3 wc = cubeWorld[idx];
                Vec3 wc_flip = rotY(wc, extraFlip);
                Vec3 tc = rotX(rotY(rotZ(wc_flip, -roll), -yaw), -pitch);
                Point pp = projectPoint(tc, width, height, fov, camDist);
                pts2.push_back(pp);
                avgZ += tc.z;
            }
            avgZ /= f.size();
            // Color simple según orientación (más cercano = más brillante)
            double shade = 0.6 + 0.4 * (1.0 - (avgZ / (maxZ + camDist)));
            Scalar base(100*shade, 180*shade, 240*shade);
            toDraw.push_back({avgZ, pts2, base});
        }
        sort(toDraw.begin(), toDraw.end(), [](const FaceDraw &a, const FaceDraw &b){ return a.depth > b.depth; }); // dibujar de atrás hacia delante (mayor z primero)
        for(auto &fd: toDraw){
            const Point *pp = &fd.pts[0]; int n = (int)fd.pts.size();
            fillConvexPoly(frame, pp, n, fd.color);
            polylines(frame, vector<vector<Point>>{fd.pts}, true, Scalar(20,60,110), 2);
        }

        // HUD
        string info = format("Pelotas totales: %d  Atrapadas: %d  Pos (%.2f,%.2f,%.2f)", (int)balls.size(), caught, sq.x, sq.y, sq.z);
        putText(frame, info, Point(10,25), FONT_HERSHEY_SIMPLEX, 0.6, Scalar(230,230,230), 2);
        putText(frame, "Controles: flechas/WASD mover, q/e subir/bajar, ESC salir", Point(10,50), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(200,200,200),1);

    imshow("Juego3D", frame);

    // Leer tecla una vez por frame y procesar movimientos (WASD y flechas)
    int kk = waitKey(16);
    if(kk==27) break;
    // flechas (OpenCV devuelve códigos especiales en algunos entornos):
    if(kk==81) sq.x -= sqSpeed; // left
    else if(kk==83) sq.x += sqSpeed; // right
    else if(kk==82) sq.y += sqSpeed; // up
    else if(kk==84) sq.y -= sqSpeed; // down
    // teclas ASCII
    else if(kk=='a' || kk=='A') sq.x -= sqSpeed;
    else if(kk=='d' || kk=='D') sq.x += sqSpeed;
    else if(kk=='w' || kk=='W') sq.y += sqSpeed;
    else if(kk=='s' || kk=='S') sq.y -= sqSpeed;
    else if(kk=='q' || kk=='Q') sq.z = max(minZ+0.2, sq.z - 0.2);
    else if(kk=='e' || kk=='E') sq.z = min(maxZ-0.2, sq.z + 0.2);
    else if(kk=='+' || kk=='=') extraFlip += 0.1;
    else if(kk=='-' || kk=='_') extraFlip -= 0.1;
    }

    return 0;
}
