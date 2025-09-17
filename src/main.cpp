#include "GameEngine.h"
#include <iostream>
#include <opencv2/opencv.hpp>

int main() {
    std::cout << "==================================" << std::endl;
    std::cout << "   ANGRY BIRDS OPENCV - v1.0     " << std::endl;
    std::cout << "==================================" << std::endl;
    std::cout << "Desarrollado en C++ con OpenCV" << std::endl;
    std::cout << "Parcial de Videojuegos" << std::endl;
    std::cout << std::endl;
    
    // Verificar que OpenCV esté disponible
    std::cout << "OpenCV Version: " << CV_VERSION << std::endl;
    std::cout << std::endl;
    
    try {
        // Crear y configurar el motor del juego
        std::cout << "Inicializando motor del juego..." << std::endl;
        GameEngine game(1200, 800);  // 1200x800 resolution
        
        std::cout << "Cargando recursos del juego..." << std::endl;
        game.initialize();
        
        std::cout << "¡Juego iniciado!" << std::endl;
        std::cout << std::endl;
        std::cout << "CONTROLES:" << std::endl;
        std::cout << "- Mouse: Apuntar y disparar pájaros" << std::endl;
        std::cout << "- SPACE: Reiniciar nivel actual" << std::endl;
        std::cout << "- N: Ir al siguiente nivel (cuando esté disponible)" << std::endl;
        std::cout << "- C: Usar habilidad especial del pájaro (durante el vuelo)" << std::endl;
        std::cout << "- ESC: Salir del juego" << std::endl;
        std::cout << std::endl;
        std::cout << "NIVELES DISPONIBLES:" << std::endl;
        std::cout << "1. Fácil: Estructura simple con pocos cerdos" << std::endl;
        std::cout << "2. Intermedio: Torres múltiples y cerdos medianos" << std::endl;
        std::cout << "3. Difícil: Fortaleza compleja con cerdos grandes" << std::endl;
        std::cout << std::endl;
        std::cout << "¡Que comience la diversión!" << std::endl;
        std::cout << "==================================" << std::endl;
        
        // Ejecutar el bucle principal del juego
        game.run();
        
    } catch (const cv::Exception& e) {
        std::cerr << "Error de OpenCV: " << e.what() << std::endl;
        return -1;
    } catch (const std::exception& e) {
        std::cerr << "Error del juego: " << e.what() << std::endl;
        return -1;
    } catch (...) {
        std::cerr << "Error desconocido occurred!" << std::endl;
        return -1;
    }
    
    std::cout << std::endl;
    std::cout << "¡Gracias por jugar Angry Birds OpenCV!" << std::endl;
    std::cout << "==================================" << std::endl;
    
    return 0;
}