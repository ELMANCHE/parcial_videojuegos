#include <opencv2/opencv.hpp>
#include <iostream>
#include <string>
#include <SFML/Audio.hpp>
#include <memory>

using namespace cv;
using namespace std;

// Ruta de fondo (reutilizamos la misma imagen del menú)
#define BG_IMG_PATH "sources/squid.jpg"

// Botones y estado global
struct Button {
    Rect rect;
    string label;
};

Button btnReintentar, btnSalir;
int hovered = 0; // 1: REINTENTAR, 2: SALIR
int clicked = 0; // 1: REINTENTAR, 2: SALIR

// Dibuja un botón con estilo
void drawButton(Mat &img, const Button &btn, bool hovered) {
    Scalar color = hovered ? Scalar(60, 180, 255) : Scalar(30, 30, 30);
    rectangle(img, btn.rect, color, FILLED, LINE_AA);
    rectangle(img, btn.rect, Scalar(255,255,255), 2, LINE_AA);
    int baseline = 0;
    Size textSize = getTextSize(btn.label, FONT_HERSHEY_SIMPLEX, 0.9, 2, &baseline);
    Point textOrg(
        btn.rect.x + (btn.rect.width - textSize.width) / 2,
        btn.rect.y + (btn.rect.height + textSize.height) / 2
    );
    putText(img, btn.label, textOrg, FONT_HERSHEY_SIMPLEX, 0.9, Scalar(255,255,255), 2, LINE_AA);
}

// Callback global de mouse
void endMouseCallback(int event, int x, int y, int, void*) {
    if(event == EVENT_MOUSEMOVE) {
        if(btnReintentar.rect.contains(Point(x,y))) hovered = 1;
        else if(btnSalir.rect.contains(Point(x,y))) hovered = 2;
        else hovered = 0;
    }
    if(event == EVENT_LBUTTONDOWN) {
        if(btnReintentar.rect.contains(Point(x,y))) clicked = 1;
        else if(btnSalir.rect.contains(Point(x,y))) clicked = 2;
    }
}

int main() {
    // Cargar y reproducir musica con SFML
    sf::SoundBuffer musicBuffer;
    std::unique_ptr<sf::Sound> music;
    if(!musicBuffer.loadFromFile("sources/free.wav")) {
        cerr << "Advertencia: No se pudo abrir sources/squiddy.wav" << endl;
    } else {
        music = std::make_unique<sf::Sound>(musicBuffer);
        music->setLooping(true);
        music->play();
        cerr << "[END] Reproduciendo música" << endl;
    }

    // Cargar fondo
    Mat bg = imread(BG_IMG_PATH);
    if(bg.empty()) {
        cerr << "No se pudo cargar el fondo: " << BG_IMG_PATH << endl;
        return 1;
    }
    int W = 900, H = 600;
    resize(bg, bg, Size(W, H));

    // Definir botones
    int btnW = 280, btnH = 60;
    btnReintentar = {Rect((W-btnW)/2, H/2-40, btnW, btnH), "JUGAR DE NUEVO"};
    btnSalir = {Rect((W-btnW)/2, H/2+40, btnW, btnH), "SALIR"};

    // Crear ventana y configurar callback
    namedWindow("GAME OVER", WINDOW_AUTOSIZE);
    setMouseCallback("GAME OVER", endMouseCallback, nullptr);

    bool running = true;
    while(running) {
        Mat frame = bg.clone();
        
        // Overlay oscuro para mejor legibilidad
        Mat overlay = frame.clone();
        rectangle(overlay, Point(0, 0), Point(W, H), Scalar(0,0,0), FILLED);
        addWeighted(frame, 0.4, overlay, 0.6, 0, frame);
        
        // Título "GAME OVER"
        string title = "GAME OVER";
        int baseline = 0;
        Size tsize = getTextSize(title, FONT_HERSHEY_COMPLEX, 2.5, 5, &baseline);
        Point torg((W-tsize.width)/2, 100);
        putText(frame, title, torg, FONT_HERSHEY_COMPLEX, 2.5, Scalar(0,0,255), 5, LINE_AA);
        
        // Subtítulo
        string subtitle = "Ambos jugadores han sido eliminados";
        Size ssize = getTextSize(subtitle, FONT_HERSHEY_SIMPLEX, 0.8, 2, nullptr);
        Point sorg((W-ssize.width)/2, 160);
        putText(frame, subtitle, sorg, FONT_HERSHEY_SIMPLEX, 0.8, Scalar(200,200,200), 2, LINE_AA);

        // Botones
        drawButton(frame, btnReintentar, hovered==1);
        drawButton(frame, btnSalir, hovered==2);

        imshow("GAME OVER", frame);
        int key = waitKey(10);
        if(key == 27) running = false; // ESC para salir

        // Acción de click
        if(clicked == 1) {
            // Reiniciar juego
            Mat loading = frame.clone();
            string cargando = "Reiniciando...";
            Size csize = getTextSize(cargando, FONT_HERSHEY_SIMPLEX, 1.2, 3, nullptr);
            Point corg((frame.cols-csize.width)/2, frame.rows-60);
            putText(loading, cargando, corg, FONT_HERSHEY_SIMPLEX, 1.2, Scalar(0,255,255), 3, LINE_AA);
            imshow("GAME OVER", loading);
            waitKey(500);
            if(music) { music->stop(); music.reset(); }
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
