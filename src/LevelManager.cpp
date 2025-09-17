#include "LevelManager.h"

LevelManager::LevelManager() : currentLevel(0) {
    initializeLevels();
}

LevelManager::~LevelManager() {}

void LevelManager::initializeLevels() {
    levels.clear();
    createLevel1();
    createLevel2();
    createLevel3();
}

void LevelManager::createLevel1() {
    // NIVEL 1 - FÁCIL
    LevelData level1;
    level1.levelNumber = 1;
    level1.name = "Nivel Fácil";
    level1.description = "¡Tu primer desafío! Solo unos pocos cerdos te esperan.";
    level1.targetScore = 15000;
    
    // Pájaros disponibles (3 pájaros rojos)
    level1.birds = {
        {cv::Point2f(150, 600), BirdType::RED},
        {cv::Point2f(100, 600), BirdType::RED},
        {cv::Point2f(50, 600), BirdType::RED}
    };
    
    // Cerdos (2 cerdos pequeños)
    level1.pigs = {
        {cv::Point2f(800, 650), PigType::SMALL},
        {cv::Point2f(900, 650), PigType::SMALL}
    };
    
    // Bloques (estructura simple)
    level1.blocks = {
        // Plataforma base
        {std::make_tuple(cv::Point2f(850, 680), BlockType::WOOD, cv::Size2f(150, 20))},
        // Columnas laterales
        {std::make_tuple(cv::Point2f(780, 620), BlockType::WOOD, cv::Size2f(20, 80))},
        {std::make_tuple(cv::Point2f(920, 620), BlockType::WOOD, cv::Size2f(20, 80))},
        // Techo
        {std::make_tuple(cv::Point2f(850, 560), BlockType::WOOD, cv::Size2f(120, 20))}
    };
    
    levels.push_back(level1);
}

void LevelManager::createLevel2() {
    // NIVEL 2 - INTERMEDIO
    LevelData level2;
    level2.levelNumber = 2;
    level2.name = "Nivel Intermedio";
    level2.description = "¡Las cosas se complican! Más cerdos y estructuras más resistentes.";
    level2.targetScore = 35000;
    
    // Pájaros disponibles (mix de tipos)
    level2.birds = {
        {cv::Point2f(150, 600), BirdType::RED},
        {cv::Point2f(100, 600), BirdType::BLUE},
        {cv::Point2f(50, 600), BirdType::YELLOW},
        {cv::Point2f(0, 600), BirdType::RED}
    };
    
    // Cerdos (mix de tamaños)
    level2.pigs = {
        {cv::Point2f(750, 650), PigType::SMALL},
        {cv::Point2f(850, 600), PigType::MEDIUM},
        {cv::Point2f(950, 650), PigType::SMALL},
        {cv::Point2f(1050, 620), PigType::MEDIUM}
    };
    
    // Bloques (estructura más compleja)
    level2.blocks = {
        // Primera torre (izquierda)
        {std::make_tuple(cv::Point2f(750, 680), BlockType::WOOD, cv::Size2f(60, 20))},
        {std::make_tuple(cv::Point2f(730, 620), BlockType::STONE, cv::Size2f(20, 80))},
        {std::make_tuple(cv::Point2f(770, 620), BlockType::STONE, cv::Size2f(20, 80))},
        {std::make_tuple(cv::Point2f(750, 560), BlockType::WOOD, cv::Size2f(80, 20))},
        
        // Torre central
        {std::make_tuple(cv::Point2f(850, 680), BlockType::STONE, cv::Size2f(80, 20))},
        {std::make_tuple(cv::Point2f(820, 620), BlockType::ICE, cv::Size2f(20, 80))},
        {std::make_tuple(cv::Point2f(880, 620), BlockType::ICE, cv::Size2f(20, 80))},
        {std::make_tuple(cv::Point2f(850, 560), BlockType::STONE, cv::Size2f(100, 20))},
        {std::make_tuple(cv::Point2f(850, 540), BlockType::WOOD, cv::Size2f(40, 40))},
        
        // Torre derecha
        {std::make_tuple(cv::Point2f(950, 680), BlockType::WOOD, cv::Size2f(60, 20))},
        {std::make_tuple(cv::Point2f(930, 620), BlockType::WOOD, cv::Size2f(20, 80))},
        {std::make_tuple(cv::Point2f(970, 620), BlockType::WOOD, cv::Size2f(20, 80))},
        
        // Torre lejana
        {std::make_tuple(cv::Point2f(1050, 680), BlockType::ICE, cv::Size2f(60, 20))},
        {std::make_tuple(cv::Point2f(1050, 640), BlockType::ICE, cv::Size2f(60, 60))}
    };
    
    levels.push_back(level2);
}

void LevelManager::createLevel3() {
    // NIVEL 3 - DIFÍCIL
    LevelData level3;
    level3.levelNumber = 3;
    level3.name = "Nivel Difícil";
    level3.description = "¡El desafío final! Fortaleza impenetrable con cerdos grandes.";
    level3.targetScore = 60000;
    
    // Pájaros disponibles (variedad completa pero limitada)
    level3.birds = {
        {cv::Point2f(150, 600), BirdType::YELLOW},
        {cv::Point2f(100, 600), BirdType::RED},
        {cv::Point2f(50, 600), BirdType::BLUE},
        {cv::Point2f(0, 600), BirdType::RED},
        {cv::Point2f(-50, 600), BirdType::YELLOW}
    };
    
    // Cerdos (muchos y grandes)
    level3.pigs = {
        {cv::Point2f(700, 650), PigType::SMALL},
        {cv::Point2f(800, 580), PigType::LARGE},
        {cv::Point2f(900, 520), PigType::MEDIUM},
        {cv::Point2f(1000, 580), PigType::LARGE},
        {cv::Point2f(1100, 650), PigType::SMALL},
        {cv::Point2f(850, 450), PigType::MEDIUM}
    };
    
    // Bloques (fortaleza compleja y resistente)
    level3.blocks = {
        // Base de la fortaleza
        {std::make_tuple(cv::Point2f(850, 690), BlockType::STONE, cv::Size2f(300, 30))},
        
        // Torres laterales (izquierda)
        {std::make_tuple(cv::Point2f(720, 650), BlockType::STONE, cv::Size2f(40, 40))},
        {std::make_tuple(cv::Point2f(720, 610), BlockType::STONE, cv::Size2f(40, 40))},
        {std::make_tuple(cv::Point2f(720, 570), BlockType::STONE, cv::Size2f(40, 40))},
        
        // Torres laterales (derecha)
        {std::make_tuple(cv::Point2f(980, 650), BlockType::STONE, cv::Size2f(40, 40))},
        {std::make_tuple(cv::Point2f(980, 610), BlockType::STONE, cv::Size2f(40, 40))},
        {std::make_tuple(cv::Point2f(980, 570), BlockType::STONE, cv::Size2f(40, 40))},
        
        // Estructura central - nivel 1
        {std::make_tuple(cv::Point2f(780, 650), BlockType::WOOD, cv::Size2f(20, 40))},
        {std::make_tuple(cv::Point2f(820, 650), BlockType::STONE, cv::Size2f(20, 40))},
        {std::make_tuple(cv::Point2f(880, 650), BlockType::STONE, cv::Size2f(20, 40))},
        {std::make_tuple(cv::Point2f(920, 650), BlockType::WOOD, cv::Size2f(20, 40))},
        
        // Estructura central - nivel 2
        {std::make_tuple(cv::Point2f(850, 620), BlockType::STONE, cv::Size2f(120, 20))},
        {std::make_tuple(cv::Point2f(790, 600), BlockType::ICE, cv::Size2f(20, 40))},
        {std::make_tuple(cv::Point2f(850, 600), BlockType::STONE, cv::Size2f(20, 40))},
        {std::make_tuple(cv::Point2f(910, 600), BlockType::ICE, cv::Size2f(20, 40))},
        
        // Estructura central - nivel 3
        {std::make_tuple(cv::Point2f(850, 570), BlockType::STONE, cv::Size2f(100, 20))},
        {std::make_tuple(cv::Point2f(820, 550), BlockType::WOOD, cv::Size2f(20, 40))},
        {std::make_tuple(cv::Point2f(880, 550), BlockType::WOOD, cv::Size2f(20, 40))},
        
        // Torre central superior
        {std::make_tuple(cv::Point2f(850, 520), BlockType::STONE, cv::Size2f(80, 20))},
        {std::make_tuple(cv::Point2f(830, 500), BlockType::ICE, cv::Size2f(20, 40))},
        {std::make_tuple(cv::Point2f(870, 500), BlockType::ICE, cv::Size2f(20, 40))},
        {std::make_tuple(cv::Point2f(850, 470), BlockType::STONE, cv::Size2f(60, 20))},
        
        // Cúpula superior
        {std::make_tuple(cv::Point2f(850, 440), BlockType::STONE, cv::Size2f(40, 40))},
        
        // Muros exteriores
        {std::make_tuple(cv::Point2f(650, 650), BlockType::STONE, cv::Size2f(20, 80))},
        {std::make_tuple(cv::Point2f(1050, 650), BlockType::STONE, cv::Size2f(20, 80))},
        
        // Obstáculos adicionales
        {std::make_tuple(cv::Point2f(600, 670), BlockType::WOOD, cv::Size2f(60, 20))},
        {std::make_tuple(cv::Point2f(1100, 670), BlockType::WOOD, cv::Size2f(60, 20))}
    };
    
    levels.push_back(level3);
}

LevelData LevelManager::getCurrentLevel() const {
    if (currentLevel < 0 || currentLevel >= levels.size()) {
        return LevelData();  // Return empty level data
    }
    return levels[currentLevel];
}

LevelData LevelManager::getLevel(int levelNum) const {
    if (levelNum < 0 || levelNum >= levels.size()) {
        return LevelData();
    }
    return levels[levelNum];
}

bool LevelManager::hasNextLevel() const {
    return currentLevel < (levels.size() - 1);
}

void LevelManager::nextLevel() {
    if (hasNextLevel()) {
        currentLevel++;
    }
}

void LevelManager::resetToLevel(int levelNum) {
    if (levelNum >= 0 && levelNum < levels.size()) {
        currentLevel = levelNum;
    }
}