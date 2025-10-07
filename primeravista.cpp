#include <opencv2/opencv.hpp>
#include <iostream>
#include <string>

using namespace cv;
using namespace std;

// Rutas
#define BG_IMG_PATH "sources/squid.jpg"

// Botones y estado global
struct Button {
    Rect rect;
    std::string label;
};

Button btnJugar, btnSalir;
int hovered = 0; // 1: JUGAR, 2: SALIR
int clicked = 0; // 1: JUGAR, 2: SALIR

// Dibuja un botón con estilo
void drawButton(Mat &img, const Button &btn, bool hovered) {
    Scalar color = hovered ? Scalar(60, 180, 255) : Scalar(30, 30, 30);
    rectangle(img, btn.rect, color, FILLED, LINE_AA);
    rectangle(img, btn.rect, Scalar(255,255,255), 2, LINE_AA);
    int baseline = 0;
    Size textSize = getTextSize(btn.label, FONT_HERSHEY_SIMPLEX, 1.0, 2, &baseline);
    Point textOrg(
        btn.rect.x + (btn.rect.width - textSize.width) / 2,
        btn.rect.y + (btn.rect.height + textSize.height) / 2
    );
    putText(img, btn.label, textOrg, FONT_HERSHEY_SIMPLEX, 1.0, Scalar(255,255,255), 2, LINE_AA);
}

// Callback global de mouse
void menuMouseCallback(int event, int x, int y, int, void*) {
    if(event == EVENT_MOUSEMOVE) {
        if(btnJugar.rect.contains(Point(x,y))) hovered = 1;
        else if(btnSalir.rect.contains(Point(x,y))) hovered = 2;
        else hovered = 0;
    }
    if(event == EVENT_LBUTTONDOWN) {
        if(btnJugar.rect.contains(Point(x,y))) clicked = 1;
        else if(btnSalir.rect.contains(Point(x,y))) clicked = 2;
    }
}

int main() {
    // Cargar fondo
    Mat bg = imread(BG_IMG_PATH);
    if(bg.empty()) {
        cerr << "No se pudo cargar el fondo: " << BG_IMG_PATH << endl;
        return 1;
    }
    int W = 900, H = 600;
    resize(bg, bg, Size(W, H));

    // Definir botones
    int btnW = 250, btnH = 60;
    btnJugar = {Rect((W-btnW)/2, H/2-40, btnW, btnH), "JUGAR"};
    btnSalir = {Rect((W-btnW)/2, H/2+40, btnW, btnH), "SALIR"};

    // *** CRÍTICO: Crear la ventana PRIMERO ***
    namedWindow("MENU", WINDOW_AUTOSIZE);
    
    // *** LUEGO configurar el callback ***
    setMouseCallback("MENU", menuMouseCallback, nullptr);

    bool running = true;
    while(running) {
        Mat frame = bg.clone();
        
        // Título
        string title = "SQUIDGAMES BALL";
        int baseline = 0;
        Size tsize = getTextSize(title, FONT_HERSHEY_COMPLEX, 2.0, 4, &baseline);
        Point torg((W-tsize.width)/2, 80);
        putText(frame, title, torg, FONT_HERSHEY_COMPLEX, 2.0, Scalar(0,0,255), 4, LINE_AA);

        // Botones
        drawButton(frame, btnJugar, hovered==1);
        drawButton(frame, btnSalir, hovered==2);

        imshow("MENU", frame);
        int key = waitKey(10);
        if(key == 27) running = false;

        // Acción de click
        if(clicked == 1) {
            // Mostrar mensaje de carga
            Mat loading = frame.clone();
            string cargando = "Cargando...";
            Size csize = getTextSize(cargando, FONT_HERSHEY_SIMPLEX, 1.2, 3, nullptr);
            Point corg((frame.cols-csize.width)/2, frame.rows-60);
            putText(loading, cargando, corg, FONT_HERSHEY_SIMPLEX, 1.2, Scalar(0,255,255), 3, LINE_AA);
            imshow("MENU", loading);
            waitKey(300);
            destroyAllWindows();
            system("./arena");
            return 0;
        } else if(clicked == 2) {
            running = false;
        }
        clicked = 0;
    }
    destroyAllWindows();
    return 0;
}