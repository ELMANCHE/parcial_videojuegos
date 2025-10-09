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

// Estructura del puente de vidrio (2 columnas × 6 filas)
struct TileGrid {
    static const int COLS = 2;
    static const int ROWS = 6;
    bool isSafe[ROWS][COLS];  // true = seguro, false = trampa
    bool isRevealed[ROWS][COLS];  // Si ya se reveló como trampa
    
    TileGrid() {
        // Inicializar todo como no revelado
        for(int r = 0; r < ROWS; r++) {
            for(int c = 0; c < COLS; c++) {
                isRevealed[r][c] = false;
            }
        }
        
        // Para cada fila, elegir aleatoriamente cuál columna es segura
        srand(time(0));
        for(int r = 0; r < ROWS; r++) {
            int safeCol = rand() % COLS;  // 0 o 1
            isSafe[r][0] = (safeCol == 0);
            isSafe[r][1] = (safeCol == 1);
        }
    }
    
    bool checkTile(int row, int col) {
        if(row < 0 || row >= ROWS || col < 0 || col >= COLS) return false;
        
        // Revelar el cuadro
        isRevealed[row][col] = true;
        
        return isSafe[row][col];
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
    int gridRow;         // Fila actual en el grid (0-5)
    int gridCol;         // Columna actual en el grid (0-1)
    bool enMovimiento;   // Para animar el movimiento
    V3 targetPos;        // Posición objetivo del movimiento
    int framesMovimiento;
    Scalar colorCabeza, colorTorso, colorBrazo, colorPierna;
    
    RobloxPlayer(double x, double y, int playerNum, int startCol) {
        double tileHeight = 0.5;  // Altura de las tiles (actualizado)
        pos = {x, y, tileHeight + 0.4};  // Jugador SOBRE la tile desde el inicio
        velZ = 0;
        enSuelo = true;
        speed = 0.50;
        vida = 100;
        this->playerNum = playerNum;
        muerto = false;
        framesDesdeRespawn = 0;
        gridRow = 0;  // Empiezan en la fila 0
        gridCol = startCol;  // Cada jugador en una columna diferente
        enMovimiento = false;
        framesMovimiento = 0;
        targetPos = pos;
        
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
    
    // Intentar moverse a una casilla del grid
    bool intentarMovimiento(int nuevaFila, int nuevaCol, TileGrid& grid) {
        // Validar que el movimiento es válido
        if(muerto || enMovimiento) return false;
        if(nuevaCol < 0 || nuevaCol >= TileGrid::COLS) return false;
        if(nuevaFila < 0 || nuevaFila >= TileGrid::ROWS) return false;
        
        // Solo permitir avanzar hacia adelante (no retroceder)
        if(nuevaFila <= gridRow) return false;
        
        // Solo permitir movimiento a fila adyacente
        if(nuevaFila > gridRow + 1) return false;
        
        // Iniciar movimiento
        gridRow = nuevaFila;
        gridCol = nuevaCol;
        
        // Convertir posición del grid a coordenadas del mundo
        // Grid centrado en Y=0, cada tile es 3 unidades
        double gridSpacing = 3.0;
        double offsetX = -1.5; // Centrar el grid de 2 columnas
        double tileHeight = 0.5; // Altura de las tiles sobre el piso negro (actualizado)
        targetPos.x = offsetX + gridCol * gridSpacing;
        targetPos.y = -9.0 + gridRow * gridSpacing; // Grid empieza en Y=-9
        targetPos.z = tileHeight + 0.4;  // Jugador SOBRE la tile
        
        enMovimiento = true;
        framesMovimiento = 0;
        
        // Verificar si es una trampa
        bool esSafe = grid.checkTile(gridRow, gridCol);
        if(!esSafe) {
            vida = 0;
            muerto = true;
            posicionMuerte = targetPos;
        }
        
        return true;
    }
    
    void actualizarMovimiento() {
        if(!enMovimiento) return;
        
        framesMovimiento++;
        float t = framesMovimiento / 20.0f; // 20 frames para completar el movimiento
        if(t > 1.0f) t = 1.0f;
        
        // Interpolación suave
        pos.x = pos.x + (targetPos.x - pos.x) * 0.2;
        pos.y = pos.y + (targetPos.y - pos.y) * 0.2;
        pos.z = pos.z + (targetPos.z - pos.z) * 0.2;  // Interpolar también Z
        
        if(framesMovimiento >= 20) {
            pos = targetPos;
            enMovimiento = false;
        }
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
    
    // Cargar textura de vidrio para los tiles del puente
    Mat texturaVidrio = imread("sources/vidrio.jpg");
    if(texturaVidrio.empty()) {
        cerr << "Error: No se pudo cargar sources/vidrio.jpg" << endl;
    }
    
    // CARGAR TEXTURAS PARA LAS PAREDES
    Mat texturaPared1 = imread("sources/glass1.jpg");      // Pared derecha
    Mat texturaPared2 = imread("sources/glass2.jpeg");     // Pared izquierda
    
    if(texturaPared1.empty()) {
        cerr << "Error: No se pudo cargar sources/glass1.jpg" << endl;
    }
    if(texturaPared2.empty()) {
        cerr << "Error: No se pudo cargar sources/glass2.jpeg" << endl;
    }
    
    // Crear el grid de trampas del puente de vidrio
    TileGrid grid;
    
    // CREAR 2 JUGADORES (empiezan en columna 0 y 1, fila 0)
    RobloxPlayer player1(-1.5, -9.0, 1, 0);  // Columna izquierda
    RobloxPlayer player2(1.5, -9.0, 2, 1);   // Columna derecha
    
    namedWindow("SQUID GAMES - Glass Bridge", WINDOW_AUTOSIZE);
    // setup mouse callback for single-view camera control
    setMouseCallback("SQUID GAMES - Glass Bridge", mouseCamCallback, nullptr);
    
    while(true) {
    img.setTo(Scalar(25, 25, 35));

    // Single-view rendering: all drawing happens into `img` below
        
        // ===== ACTUALIZAR MOVIMIENTOS DE JUGADORES =====
        player1.actualizarMovimiento();
        player2.actualizarMovimiento();
        
        // Actualizar contador de frames desde muerte
        if(player1.muerto) player1.framesDesdeRespawn++;
        if(player2.muerto) player2.framesDesdeRespawn++;
        
        // ===== LEER TECLAS =====
        int k = waitKey(30);
        if(k == 27) break;  // ESC para salir
        else if(k == '+' || k == '=') zoom += 2;
        else if(k == '-' || k == '_') zoom = max(10.0, zoom - 2);
        
        // ===== CONTROLES JUGADOR 1 (WASD - movimiento discreto en grid) =====
        if(player1.estaVivo() && !player1.enMovimiento) {
            if(k == 'w' || k == 'W') {  // Avanzar recto
                player1.intentarMovimiento(player1.gridRow + 1, player1.gridCol, grid);
            } else if(k == 'a' || k == 'A') {  // Diagonal izquierda-adelante
                player1.intentarMovimiento(player1.gridRow + 1, player1.gridCol - 1, grid);
            } else if(k == 'd' || k == 'D') {  // Diagonal derecha-adelante
                player1.intentarMovimiento(player1.gridRow + 1, player1.gridCol + 1, grid);
            }
        }
        
        // ===== CONTROLES JUGADOR 2 (IJKL - movimiento discreto en grid) =====
        if(player2.estaVivo() && !player2.enMovimiento) {
            if(k == 'i' || k == 'I') {  // Avanzar recto
                player2.intentarMovimiento(player2.gridRow + 1, player2.gridCol, grid);
            } else if(k == 'j' || k == 'J') {  // Diagonal izquierda-adelante
                player2.intentarMovimiento(player2.gridRow + 1, player2.gridCol - 1, grid);
            } else if(k == 'l' || k == 'L') {  // Diagonal derecha-adelante
                player2.intentarMovimiento(player2.gridRow + 1, player2.gridCol + 1, grid);
            }
        }
        
        // ===== VERIFICAR VICTORIA =====
        if(player1.gridRow >= TileGrid::ROWS - 1 && player1.estaVivo()) {
            putText(img, "JUGADOR 1 GANA!", Point(W/2 - 200, H/2), 
                    FONT_HERSHEY_COMPLEX, 2, Scalar(0, 255, 0), 3);
            putText(img, "Has completado el desafio!", Point(W/2 - 200, H/2 + 60), 
                    FONT_HERSHEY_SIMPLEX, 1, Scalar(255, 255, 255), 2);
            imshow("SQUID GAMES - Glass Bridge", img);
            waitKey(3000);
            destroyAllWindows();
            system("./end");
            return 0;
        }
        if(player2.gridRow >= TileGrid::ROWS - 1 && player2.estaVivo()) {
            putText(img, "JUGADOR 2 GANA!", Point(W/2 - 200, H/2), 
                    FONT_HERSHEY_COMPLEX, 2, Scalar(0, 255, 0), 3);
            putText(img, "Has completado el desafio!", Point(W/2 - 200, H/2 + 60), 
                    FONT_HERSHEY_SIMPLEX, 1, Scalar(255, 255, 255), 2);
            imshow("SQUID GAMES - Glass Bridge", img);
            waitKey(3000);
            destroyAllWindows();
            system("./end");
            return 0;
        }
        
        // Si ambos mueren, game over
        if(player1.muerto && player2.muerto) {
            putText(img, "GAME OVER", Point(W/2 - 150, H/2), 
                    FONT_HERSHEY_COMPLEX, 2, Scalar(0, 0, 255), 3);
            imshow("SQUID GAMES - Glass Bridge", img);
            waitKey(2000);
            destroyAllWindows();
            system("./end");
            return 0;
        }
        
        // ===== PAREDES DEL FONDO CON TEXTURA =====
        double alturaParedes = 8.0;  // Altura de las paredes
        
        // PARED FONDO (en Y = -campoSize, de X=-campoSize a X=campoSize)
        V3 paredFondo[4] = {
            {-campoSize, -campoSize, 0},           // Esquina inferior izquierda
            {campoSize, -campoSize, 0},            // Esquina inferior derecha
            {campoSize, -campoSize, alturaParedes},  // Esquina superior derecha
            {-campoSize, -campoSize, alturaParedes}  // Esquina superior izquierda
        };
        Point paredFondoPts[4];
        for(int i = 0; i < 4; i++) {
            paredFondoPts[i] = isoProject(paredFondo[i], W, H, zoom);
        }
        
        // Aplicar textura a pared del fondo (glass2)
        if(!texturaPared2.empty()) {
            applyTextureToWall(img, paredFondoPts, 4, texturaPared2);
        } else {
            fillConvexPoly(img, paredFondoPts, 4, Scalar(120, 100, 80));  // Color ladrillo fallback
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
        
        // Aplicar textura a pared lateral izquierda (glass2)
        if(!texturaPared2.empty()) {
            applyTextureToWall(img, paredLatIzqPts, 4, texturaPared2);
        } else {
            fillConvexPoly(img, paredLatIzqPts, 4, Scalar(110, 95, 75));  // Color ladrillo fallback
        }
        
        // ===== PLANO DEL PISO NEGRO =====
        V3 pisoCorners[4] = {
            {-campoSize, -campoSize, 0}, 
            {campoSize, -campoSize, 0},
            {campoSize, campoSize, 0}, 
            {-campoSize, campoSize, 0}
        };
        Point pisoPts[4];
        for(int i = 0; i < 4; i++) {
            pisoPts[i] = isoProject(pisoCorners[i], W, H, zoom);
        }
        
        // Renderizar piso negro
        fillConvexPoly(img, pisoPts, 4, Scalar(0, 0, 0));  // Negro
        
        // ===== RENDERIZAR GRID DEL PUENTE DE VIDRIO =====
        double gridSpacing = 3.0;
        double offsetX = -1.5;
        double offsetY = -9.0;
        double tileSize = 2.8;  // Más grande para ser más visible
        double tileHeight = 0.5;  // MUCHO MÁS ALTO para que se vea claramente
        
        for(int row = 0; row < TileGrid::ROWS; row++) {
            for(int col = 0; col < TileGrid::COLS; col++) {
                double tileX = offsetX + col * gridSpacing;
                double tileY = offsetY + row * gridSpacing;
                
                Scalar tileColor;
                bool usarTextura = false;
                
                if(grid.isRevealed[row][col]) {
                    if(grid.isSafe[row][col]) {
                        tileColor = Scalar(50, 255, 50);  // Verde brillante para segura
                    } else {
                        tileColor = Scalar(50, 50, 255);  // Rojo brillante para trampa rota
                    }
                } else {
                    // Usar textura de vidrio para tiles no reveladas
                    usarTextura = !texturaVidrio.empty();
                    tileColor = Scalar(255, 255, 255);  // Blanco brillante para que la textura se vea bien
                }
                
                // Dibujar tile como cubo delgado BIEN ELEVADO del piso negro
                vector<Face3D> tileFaces;
                drawCube3D(tileFaces, {tileX, tileY, tileHeight}, {tileSize, tileSize, 0.4}, tileColor, W, H, zoom);
                
                // Ordenar y renderizar
                sort(tileFaces.begin(), tileFaces.end(), [](const Face3D& a, const Face3D& b) {
                    return a.depth < b.depth;
                });
                
                int faceIndex = 0;
                for(auto& face : tileFaces) {
                    if(face.drawFace || true) {  // Renderizar todas las caras
                        // Si debe usar textura, aplicarla con brillo ajustado según la cara
                        if(usarTextura) {
                            // Aplicar textura con ajuste de brillo según la cara (superior, frontal, lateral)
                            Mat texturaAjustada = texturaVidrio.clone();
                            
                            // Ajustar brillo según la cara para mantener efecto 3D
                            double factorBrillo = 1.0;
                            if(faceIndex == 0) factorBrillo = 1.2;      // Cara superior más brillante
                            else if(faceIndex == 1) factorBrillo = 1.0; // Cara frontal normal
                            else if(faceIndex == 2) factorBrillo = 0.7; // Cara lateral más oscura
                            
                            if(factorBrillo != 1.0) {
                                texturaAjustada.convertTo(texturaAjustada, -1, factorBrillo, 0);
                            }
                            
                            applyTextureToWall(img, face.pts, 4, texturaAjustada);
                        } else {
                            fillConvexPoly(img, face.pts, 4, face.color);
                        }
                    }
                    faceIndex++;
                }
                
                // Dibujar contorno negro de la cuadrícula (más grueso)
                // Obtener las 4 esquinas superiores del tile
                V3 corners[4] = {
                    {tileX - tileSize/2, tileY - tileSize/2, tileHeight + 0.4},
                    {tileX + tileSize/2, tileY - tileSize/2, tileHeight + 0.4},
                    {tileX + tileSize/2, tileY + tileSize/2, tileHeight + 0.4},
                    {tileX - tileSize/2, tileY + tileSize/2, tileHeight + 0.4}
                };
                Point cornerPts[4];
                for(int i = 0; i < 4; i++) {
                    cornerPts[i] = isoProject(corners[i], W, H, zoom);
                }
                
                // Dibujar líneas del contorno negro MUY gruesas
                for(int i = 0; i < 4; i++) {
                    line(img, cornerPts[i], cornerPts[(i+1)%4], Scalar(0, 0, 0), 5);
                }
            }
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
        putText(img, "GLASS BRIDGE CHALLENGE", Point(20, 40), 
                FONT_HERSHEY_COMPLEX, 1.2, Scalar(255, 255, 255), 2);
        rectangle(img, Point(15, 60), Point(480, 135), Scalar(0, 0, 0), -1);
        putText(img, "P1: W=Adelante | A/D=Diagonales", Point(20, 80), 
                FONT_HERSHEY_SIMPLEX, 0.5, Scalar(80, 220, 255), 1);
        putText(img, "P2: I=Adelante | J/L=Diagonales", Point(20, 100), 
                FONT_HERSHEY_SIMPLEX, 0.5, Scalar(80, 120, 255), 1);
        putText(img, "+/- Zoom", Point(20, 125), 
                FONT_HERSHEY_SIMPLEX, 0.5, Scalar(200, 200, 200), 1);
        
        putText(img, "ESC para Salir", Point(20, H-20), 
                FONT_HERSHEY_SIMPLEX, 0.5, Scalar(200, 200, 200), 1);
        
    // Single view (we removed split rendering). Display img directly.
    imshow("SQUID GAMES - Glass Bridge", img);
    }
    
    return 0;
}

