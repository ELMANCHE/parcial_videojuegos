#include "GameEngine.h"
#include "Physics.h"
#include "GameObjects.h"
#include "LevelManager.h"
#include <chrono>
#include <sstream>
#include <iomanip>

GameEngine::GameEngine(int width, int height) 
    : windowSize(width, height), currentState(GameState::MENU), 
      mousePressed(false), score(0), currentLevel(0), birdsRemaining(0) {
    
    canvas = cv::Mat::zeros(windowSize, CV_8UC3);
}

GameEngine::~GameEngine() {}

void GameEngine::initialize() {
    // Initialize game systems
    physics = std::make_unique<Physics>();
    levelManager = std::make_unique<LevelManager>();
    slingshot = std::make_unique<Slingshot>(cv::Point2f(150, 600));
    
    // Load first level
    loadLevel(0);
    
    cv::namedWindow("Angry Birds OpenCV", cv::WINDOW_AUTOSIZE);
    cv::setMouseCallback("Angry Birds OpenCV", onMouse, this);
}

void GameEngine::onMouse(int event, int x, int y, int flags, void* userdata) {
    GameEngine* engine = static_cast<GameEngine*>(userdata);
    engine->handleMouse(event, x, y, flags);
}

void GameEngine::run() {
    auto lastTime = std::chrono::steady_clock::now();
    
    while (true) {
        auto currentTime = std::chrono::steady_clock::now();
        float deltaTime = std::chrono::duration<float>(currentTime - lastTime).count();
        lastTime = currentTime;
        
        // Cap delta time to prevent large jumps
        deltaTime = std::min(deltaTime, 0.016f);  // Max 60 FPS
        
        update(deltaTime);
        render();
        
        int key = cv::waitKey(1) & 0xFF;
        if (key == 27) break;  // ESC key
        
        handleInput(key);
    }
    
    cv::destroyAllWindows();
}

void GameEngine::update(float deltaTime) {
    switch (currentState) {
        case GameState::MENU:
            // Menu logic here
            break;
            
        case GameState::PLAYING:
        case GameState::AIMING:
        case GameState::BIRD_FLYING: {
            // Update physics for all objects
            std::vector<std::shared_ptr<PhysicsObject>> allObjects;
            
            // Add birds to physics
            for (auto& bird : birds) {
                allObjects.push_back(std::static_pointer_cast<PhysicsObject>(bird));
            }
            
            // Add pigs to physics  
            for (auto& pig : pigs) {
                allObjects.push_back(std::static_pointer_cast<PhysicsObject>(pig));
            }
            
            // Add blocks to physics
            for (auto& block : blocks) {
                allObjects.push_back(std::static_pointer_cast<PhysicsObject>(block));
            }
            
            physics->update(allObjects, deltaTime);
            
            // Check collisions and damage
            checkCollisions();
            
            // Remove destroyed objects
            pigs.erase(std::remove_if(pigs.begin(), pigs.end(),
                [](const std::shared_ptr<Pig>& pig) { return pig->checkDestroyed(); }), pigs.end());
                
            blocks.erase(std::remove_if(blocks.begin(), blocks.end(),
                [](const std::shared_ptr<Block>& block) { return block->checkDestroyed(); }), blocks.end());
            
            // Remove off-screen birds
            birds.erase(std::remove_if(birds.begin(), birds.end(),
                [this](const std::shared_ptr<Bird>& bird) { 
                    return !bird->isActive || bird->isOffScreen(windowSize); 
                }), birds.end());
            
            // Check if current bird has stopped moving (landed)
            if (currentState == GameState::BIRD_FLYING) {
                bool birdStopped = true;
                for (auto& bird : birds) {
                    if (bird->isLaunched() && bird->isActive) {
                        float speed = std::sqrt(bird->velocity.x * bird->velocity.x + bird->velocity.y * bird->velocity.y);
                        if (speed > 5.0f) {  // Reduced from 10.0f to 5.0f for better control
                            birdStopped = false;
                            break;
                        }
                    }
                }
                
                if (birdStopped) {
                    // Check if there are birds remaining
                    bool hasActiveBirds = false;
                    for (auto& bird : birds) {
                        if (!bird->isLaunched()) {
                            hasActiveBirds = true;
                            break;
                        }
                    }
                    
                    if (hasActiveBirds) {
                        // Load next bird
                        for (auto& bird : birds) {
                            if (!bird->isLaunched()) {
                                slingshot->setBird(bird.get());
                                currentState = GameState::AIMING;
                                break;
                            }
                        }
                    } else {
                        // No more birds
                        checkLevelComplete();
                    }
                }
            }
            
            checkLevelComplete();
            break;
        }
        
        case GameState::LEVEL_COMPLETE:
        case GameState::GAME_OVER:
            // Handle completion states
            break;
    }
}

void GameEngine::render() {
    // Clear canvas
    canvas = cv::Scalar(135, 206, 235);  // Sky blue background
    
    // Draw ground
    cv::rectangle(canvas, cv::Point(0, 700), cv::Point(windowSize.width, windowSize.height), 
                  cv::Scalar(34, 139, 34), -1);  // Forest green ground
    
    // Draw game objects
    switch (currentState) {
        case GameState::MENU: {
            // Draw menu
            cv::putText(canvas, "ANGRY BIRDS OPENCV", cv::Point(windowSize.width/2 - 200, 200), 
                       cv::FONT_HERSHEY_COMPLEX, 1.5, cv::Scalar(255, 255, 255), 3);
            cv::putText(canvas, "Presiona SPACE para jugar", cv::Point(windowSize.width/2 - 150, 300), 
                       cv::FONT_HERSHEY_SIMPLEX, 0.8, cv::Scalar(255, 255, 255), 2);
            cv::putText(canvas, "ESC para salir", cv::Point(windowSize.width/2 - 80, 350), 
                       cv::FONT_HERSHEY_SIMPLEX, 0.6, cv::Scalar(255, 255, 255), 2);
            break;
        }
        
        case GameState::PLAYING:
        case GameState::AIMING:
        case GameState::BIRD_FLYING: {
            // Draw trajectory preview when aiming
            if (currentState == GameState::AIMING && slingshot->getIsPulling()) {
                cv::Point2f startPos = slingshot->getPullPosition();
                cv::Point2f velocity = (slingshot->getBasePosition() - startPos) * 2.0f;  // Reduced from 4.0f to 2.0f to match new physics
                
                auto trajectoryPoints = physics->getTrajectoryPoints(startPos, velocity, 20);  // More points for better preview
                for (size_t i = 1; i < trajectoryPoints.size(); ++i) {
                    cv::circle(canvas, cv::Point(trajectoryPoints[i].x, trajectoryPoints[i].y), 
                              3, cv::Scalar(255, 255, 0), -1);  // Yellow dots
                }
            }
            
            // Draw slingshot
            slingshot->render(canvas);
            
            // Draw all game objects
            for (auto& block : blocks) {
                block->render(canvas);
            }
            
            for (auto& pig : pigs) {
                pig->render(canvas);
            }
            
            for (auto& bird : birds) {
                bird->render(canvas);
            }
            
            // Draw UI
            std::stringstream scoreText;
            scoreText << "Puntuacion: " << score;
            cv::putText(canvas, scoreText.str(), cv::Point(10, 30), 
                       cv::FONT_HERSHEY_SIMPLEX, 0.7, cv::Scalar(255, 255, 255), 2);
            
            std::stringstream levelText;
            levelText << "Nivel: " << (currentLevel + 1) << "/3";
            cv::putText(canvas, levelText.str(), cv::Point(10, 60), 
                       cv::FONT_HERSHEY_SIMPLEX, 0.7, cv::Scalar(255, 255, 255), 2);
            
            std::stringstream birdsText;
            int remainingBirds = 0;
            for (auto& bird : birds) {
                if (!bird->isLaunched()) remainingBirds++;
            }
            birdsText << "Pajaros: " << remainingBirds;
            cv::putText(canvas, birdsText.str(), cv::Point(10, 90), 
                       cv::FONT_HERSHEY_SIMPLEX, 0.7, cv::Scalar(255, 255, 255), 2);
            
            std::stringstream pigsText;
            pigsText << "Cerdos restantes: " << pigs.size();
            cv::putText(canvas, pigsText.str(), cv::Point(10, 120), 
                       cv::FONT_HERSHEY_SIMPLEX, 0.7, cv::Scalar(255, 255, 255), 2);
            
            // Draw controls help
            cv::putText(canvas, "Mouse: Apuntar y disparar | SPACE: Reiniciar | N: Siguiente nivel", 
                       cv::Point(10, windowSize.height - 20), 
                       cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(255, 255, 255), 1);
            break;
        }
        
        case GameState::LEVEL_COMPLETE: {
            // Draw level complete screen
            cv::rectangle(canvas, cv::Point(windowSize.width/2 - 200, 200), 
                         cv::Point(windowSize.width/2 + 200, 400), 
                         cv::Scalar(0, 0, 0), -1);
            cv::rectangle(canvas, cv::Point(windowSize.width/2 - 200, 200), 
                         cv::Point(windowSize.width/2 + 200, 400), 
                         cv::Scalar(255, 255, 255), 3);
            
            cv::putText(canvas, "NIVEL COMPLETADO!", cv::Point(windowSize.width/2 - 140, 250), 
                       cv::FONT_HERSHEY_COMPLEX, 1, cv::Scalar(0, 255, 0), 2);
            
            std::stringstream finalScore;
            finalScore << "Puntuacion: " << score;
            cv::putText(canvas, finalScore.str(), cv::Point(windowSize.width/2 - 80, 300), 
                       cv::FONT_HERSHEY_SIMPLEX, 0.7, cv::Scalar(255, 255, 255), 2);
            
            if (levelManager->hasNextLevel()) {
                cv::putText(canvas, "Presiona N para siguiente nivel", cv::Point(windowSize.width/2 - 130, 350), 
                           cv::FONT_HERSHEY_SIMPLEX, 0.6, cv::Scalar(255, 255, 255), 1);
            } else {
                cv::putText(canvas, "JUEGO COMPLETADO!", cv::Point(windowSize.width/2 - 100, 350), 
                           cv::FONT_HERSHEY_SIMPLEX, 0.6, cv::Scalar(255, 255, 0), 2);
            }
            break;
        }
        
        case GameState::GAME_OVER: {
            // Draw game over screen
            cv::rectangle(canvas, cv::Point(windowSize.width/2 - 150, 200), 
                         cv::Point(windowSize.width/2 + 150, 350), 
                         cv::Scalar(0, 0, 0), -1);
            cv::rectangle(canvas, cv::Point(windowSize.width/2 - 150, 200), 
                         cv::Point(windowSize.width/2 + 150, 350), 
                         cv::Scalar(255, 255, 255), 3);
            
            cv::putText(canvas, "GAME OVER", cv::Point(windowSize.width/2 - 100, 250), 
                       cv::FONT_HERSHEY_COMPLEX, 1, cv::Scalar(0, 0, 255), 2);
            cv::putText(canvas, "Presiona SPACE para reiniciar", cv::Point(windowSize.width/2 - 120, 300), 
                       cv::FONT_HERSHEY_SIMPLEX, 0.6, cv::Scalar(255, 255, 255), 1);
            break;
        }
    }
    
    cv::imshow("Angry Birds OpenCV", canvas);
}

void GameEngine::handleInput(int key) {
    switch (key) {
        case ' ':  // Spacebar
            if (currentState == GameState::MENU) {
                currentState = GameState::AIMING;
            } else if (currentState == GameState::GAME_OVER) {
                resetLevel();
            } else {
                resetLevel();
            }
            break;
            
        case 'n':
        case 'N':
            if (currentState == GameState::LEVEL_COMPLETE) {
                nextLevel();
            }
            break;
            
        case 'c':
        case 'C':
            // Special ability for current bird
            if (currentState == GameState::BIRD_FLYING) {
                for (auto& bird : birds) {
                    if (bird->isLaunched() && bird->isActive) {
                        bird->useSpecialAbility();
                        break;
                    }
                }
            }
            break;
    }
}

void GameEngine::handleMouse(int event, int x, int y, int flags) {
    mousePos = cv::Point2f(x, y);
    
    if (currentState == GameState::AIMING) {
        switch (event) {
            case cv::EVENT_LBUTTONDOWN:
                mousePressed = true;
                slingshot->startPull(mousePos);
                break;
                
            case cv::EVENT_MOUSEMOVE:
                if (mousePressed) {
                    slingshot->updatePull(mousePos);
                }
                break;
                
            case cv::EVENT_LBUTTONUP:
                if (mousePressed) {
                    mousePressed = false;
                    cv::Point2f velocity = slingshot->releaseBird();
                    if (velocity.x != 0 || velocity.y != 0) {
                        currentState = GameState::BIRD_FLYING;
                    }
                }
                break;
        }
    }
}

void GameEngine::loadLevel(int levelNum) {
    currentLevel = levelNum;
    levelManager->resetToLevel(levelNum);
    LevelData levelData = levelManager->getCurrentLevel();
    
    // Clear existing objects
    birds.clear();
    pigs.clear();
    blocks.clear();
    
    // Load birds
    for (auto& birdData : levelData.birds) {
        birds.push_back(std::make_shared<Bird>(birdData.first, birdData.second));
    }
    
    // Load pigs
    for (auto& pigData : levelData.pigs) {
        pigs.push_back(std::make_shared<Pig>(pigData.first, pigData.second));
    }
    
    // Load blocks
    for (auto& blockData : levelData.blocks) {
        blocks.push_back(std::make_shared<Block>(std::get<0>(blockData), std::get<1>(blockData), std::get<2>(blockData)));
    }
    
    // Set up first bird in slingshot
    if (!birds.empty()) {
        slingshot->setBird(birds[0].get());
        currentState = GameState::AIMING;
    }
    
    score = 0;  // Reset score for new level (or keep cumulative)
}

void GameEngine::nextLevel() {
    if (levelManager->hasNextLevel()) {
        levelManager->nextLevel();
        loadLevel(levelManager->getCurrentLevelNumber());
    }
}

void GameEngine::resetLevel() {
    loadLevel(currentLevel);
}

void GameEngine::checkCollisions() {
    // Check bird-pig collisions
    for (auto& bird : birds) {
        if (!bird->isActive || !bird->isLaunched()) continue;
        
        for (auto& pig : pigs) {
            if (!pig->isActive) continue;
            
            if (physics->checkCollision(*bird, *pig)) {
                pig->takeDamage(bird->getDamage());
                if (pig->checkDestroyed()) {
                    score += pig->getScore();
                }
            }
        }
        
        // Check bird-block collisions
        for (auto& block : blocks) {
            if (!block->isActive) continue;
            
            if (physics->checkCollision(*bird, *block)) {
                block->takeDamage(bird->getDamage() / 2);  // Blocks take less damage from birds
            }
        }
    }
    
    // Check block-pig collisions (falling blocks can hit pigs)
    for (auto& block : blocks) {
        if (!block->isActive || block->isStatic) continue;
        
        for (auto& pig : pigs) {
            if (!pig->isActive) continue;
            
            if (physics->checkCollision(*block, *pig)) {
                pig->takeDamage(25);  // Moderate damage from falling blocks
                if (pig->checkDestroyed()) {
                    score += pig->getScore();
                }
            }
        }
    }
}

void GameEngine::checkLevelComplete() {
    // Check win condition (all pigs destroyed)
    if (pigs.empty()) {
        // Bonus points for remaining birds
        int remainingBirds = 0;
        for (auto& bird : birds) {
            if (!bird->isLaunched()) remainingBirds++;
        }
        score += remainingBirds * 10000;  // 10k bonus per remaining bird
        
        currentState = GameState::LEVEL_COMPLETE;
        return;
    }
    
    // Check lose condition (no birds left and pigs remain)
    bool hasBirdsLeft = false;
    bool hasActiveBirds = false;
    
    for (auto& bird : birds) {
        if (!bird->isLaunched()) {
            hasBirdsLeft = true;
        }
        if (bird->isActive && bird->isLaunched()) {
            float speed = std::sqrt(bird->velocity.x * bird->velocity.x + bird->velocity.y * bird->velocity.y);
            if (speed > 5.0f) {  // Bird is still moving
                hasActiveBirds = true;
            }
        }
    }
    
    if (!hasBirdsLeft && !hasActiveBirds && !pigs.empty()) {
        currentState = GameState::GAME_OVER;
    }
}