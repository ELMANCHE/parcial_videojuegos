#pragma once
#include "GameObjects.h"
#include <vector>
#include <memory>

struct LevelData {
    int levelNumber;
    std::string name;
    std::string description;
    int targetScore;
    std::vector<std::pair<cv::Point2f, BirdType>> birds;
    std::vector<std::pair<cv::Point2f, PigType>> pigs;
    std::vector<std::tuple<cv::Point2f, BlockType, cv::Size2f>> blocks;
};

class LevelManager {
private:
    std::vector<LevelData> levels;
    int currentLevel;
    
    void createLevel1();  // Easy
    void createLevel2();  // Medium
    void createLevel3();  // Hard
    
public:
    LevelManager();
    ~LevelManager();
    
    void initializeLevels();
    LevelData getCurrentLevel() const;
    LevelData getLevel(int levelNum) const;
    
    bool hasNextLevel() const;
    void nextLevel();
    void resetToLevel(int levelNum);
    
    int getCurrentLevelNumber() const { return currentLevel; }
    int getTotalLevels() const { return levels.size(); }
};