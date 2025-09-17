#pragma once
#include <opencv2/opencv.hpp>
#include <vector>
#include <memory>

// Forward declarations
class Physics;
class Bird;
class Pig;
class Block;
class Slingshot;
class LevelManager;

enum class GameState {
    MENU,
    PLAYING,
    AIMING,
    BIRD_FLYING,
    LEVEL_COMPLETE,
    GAME_OVER
};

class GameEngine {
private:
    cv::Mat canvas;
    cv::Size windowSize;
    GameState currentState;
    
    std::unique_ptr<Physics> physics;
    std::unique_ptr<LevelManager> levelManager;
    std::unique_ptr<Slingshot> slingshot;
    
    std::vector<std::shared_ptr<Bird>> birds;
    std::vector<std::shared_ptr<Pig>> pigs;
    std::vector<std::shared_ptr<Block>> blocks;
    
    cv::Point2f mousePos;
    bool mousePressed;
    
    int score;
    int currentLevel;
    int birdsRemaining;
    
    // Mouse callback
    static void onMouse(int event, int x, int y, int flags, void* userdata);
    
public:
    GameEngine(int width = 1200, int height = 800);
    ~GameEngine();
    
    void initialize();
    void run();
    void update(float deltaTime);
    void render();
    
    void handleInput(int key);
    void handleMouse(int event, int x, int y, int flags);
    
    void loadLevel(int levelNum);
    void nextLevel();
    void resetLevel();
    
    void addBird(std::shared_ptr<Bird> bird);
    void addPig(std::shared_ptr<Pig> pig);
    void addBlock(std::shared_ptr<Block> block);
    
    void checkCollisions();
    void checkLevelComplete();
    
    // Getters
    cv::Size getWindowSize() const { return windowSize; }
    GameState getState() const { return currentState; }
    void setState(GameState state) { currentState = state; }
};