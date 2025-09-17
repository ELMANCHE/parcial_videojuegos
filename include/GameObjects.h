#pragma once
#include "Physics.h"
#include <opencv2/opencv.hpp>

enum class BirdType {
    RED,
    BLUE,
    YELLOW
};

class Bird : public PhysicsObject {
private:
    BirdType type;
    cv::Scalar color;
    bool hasBeenLaunched;
    bool specialAbilityUsed;
    int damage;
    
public:
    Bird(cv::Point2f pos, BirdType birdType);
    
    void render(cv::Mat& canvas);
    void launch(cv::Point2f velocity);
    void useSpecialAbility();
    
    bool isLaunched() const { return hasBeenLaunched; }
    bool isOffScreen(cv::Size windowSize) const;
    int getDamage() const { return damage; }
    BirdType getType() const { return type; }
};

enum class PigType {
    SMALL,
    MEDIUM,
    LARGE
};

class Pig : public PhysicsObject {
private:
    PigType type;
    int health;
    int maxHealth;
    cv::Scalar color;
    bool isDestroyed;
    
public:
    Pig(cv::Point2f pos, PigType pigType);
    
    void render(cv::Mat& canvas);
    void takeDamage(int damage);
    bool checkDestroyed() const { return isDestroyed; }
    int getScore() const;
};

enum class BlockType {
    WOOD,
    STONE,
    ICE
};

class Block : public PhysicsObject {
private:
    BlockType type;
    int health;
    int maxHealth;
    cv::Scalar color;
    cv::Size2f size;  // For rectangular blocks
    bool isDestroyed;
    
public:
    Block(cv::Point2f pos, BlockType blockType, cv::Size2f blockSize = cv::Size2f(40, 80));
    
    void render(cv::Mat& canvas);
    void takeDamage(int damage);
    bool checkDestroyed() const { return isDestroyed; }
    cv::Size2f getSize() const { return size; }
};

class Slingshot {
private:
    cv::Point2f basePosition;
    cv::Point2f pullPosition;
    bool isPulling;
    float maxPullDistance;
    
    Bird* currentBird;
    
public:
    Slingshot(cv::Point2f pos);
    
    void render(cv::Mat& canvas);
    void startPull(cv::Point2f mousePos);
    void updatePull(cv::Point2f mousePos);
    cv::Point2f releaseBird();
    
    void setBird(Bird* bird);
    Bird* getCurrentBird() const { return currentBird; }
    
    bool getIsPulling() const { return isPulling; }
    cv::Point2f getBasePosition() const { return basePosition; }
    cv::Point2f getPullPosition() const { return pullPosition; }
    float getPullStrength() const;
};