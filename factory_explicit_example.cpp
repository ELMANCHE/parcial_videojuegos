// IMPLEMENTACIÓN EXPLÍCITA DEL FACTORY METHOD 
// (Esta es la versión que mostré conceptualmente, NO está en tu código actual)

#include <opencv2/opencv.hpp>
#include <string>

using namespace cv;
using namespace std;

// Forward declaration
class Personaje;
class RobloxPlayer;

// ABSTRACT FACTORY (no existe en tu código actual)
class GameFactory {
public:
    virtual ~GameFactory() = default;
    virtual RobloxPlayer* crearJugador(double x, double y, int playerNum, int startCol) = 0;
    virtual string cargarMusica() = 0;
    virtual string cargarTexturaPared1() = 0;
    virtual string cargarTexturaPared2() = 0;
};

// CONCRETE FACTORY 1 
class NormalFactory : public GameFactory {
public:
    RobloxPlayer* crearJugador(double x, double y, int playerNum, int startCol) override {
        return new RobloxPlayer(x, y, playerNum, startCol, false); // Halloween = false
    }
    
    string cargarMusica() override { 
        return "./sources/squiddy.wav"; 
    }
    
    string cargarTexturaPared1() override { 
        return "sources/glass1.jpg"; 
    }
    
    string cargarTexturaPared2() override { 
        return "sources/glass2.jpeg"; 
    }
};

// CONCRETE FACTORY 2 
class HalloweenFactory : public GameFactory {
public:
    RobloxPlayer* crearJugador(double x, double y, int playerNum, int startCol) override {
        return new RobloxPlayer(x, y, playerNum, startCol, true); // Halloween = true
    }
    
    string cargarMusica() override { 
        return "./sources/Boo! Bitch! - Kim Petras.mp3"; 
    }
    
    string cargarTexturaPared1() override { 
        return "sources/hallowen2.jpeg"; 
    }
    
    string cargarTexturaPared2() override { 
        return "sources/hallowen2.jpeg"; 
    }
};

// COMO SERÍA EL MAIN CON FACTORY EXPLÍCITO (no está en tu código actual)
int main(int argc, char** argv) {
    // Factory selection
    GameFactory* factory;
    bool halloweenMode = false;
    
    for(int i = 1; i < argc; ++i) {
        if(string(argv[i]) == "--halloween") {
            halloweenMode = true;
        }
    }
    
    // Create appropriate factory
    if(halloweenMode) {
        factory = new HalloweenFactory();
    } else {
        factory = new NormalFactory();
    }
    
    // Use factory to create objects
    string musicPath = factory->cargarMusica();
    string texturaPared1Path = factory->cargarTexturaPared1();
    string texturaPared2Path = factory->cargarTexturaPared2();
    
    RobloxPlayer* player1 = factory->crearJugador(-1.5, -9.0, 1, 0);
    RobloxPlayer* player2 = factory->crearJugador(1.5, -9.0, 2, 1);
    
    // ... resto del juego ...
    
    delete factory;
    delete player1;
    delete player2;
    return 0;
}