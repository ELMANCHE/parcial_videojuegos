#include "GameObjects.h"
#include <algorithm>
#include <cmath>

// Bird Implementation
Bird::Bird(cv::Point2f pos, BirdType birdType) 
    : PhysicsObject(pos, 15.0f, 1.0f), type(birdType), hasBeenLaunched(false), specialAbilityUsed(false) {
    
    switch(type) {
        case BirdType::RED:
            color = cv::Scalar(0, 0, 255);  // Red
            damage = 50;
            break;
        case BirdType::BLUE:
            color = cv::Scalar(255, 0, 0);  // Blue
            damage = 30;
            break;
        case BirdType::YELLOW:
            color = cv::Scalar(0, 255, 255);  // Yellow
            damage = 40;
            break;
    }
}

void Bird::render(cv::Mat& canvas) {
    if (!isActive) return;
    
    // Draw bird body
    cv::circle(canvas, cv::Point(position.x, position.y), radius, color, -1);
    
    // Draw bird eyes
    cv::circle(canvas, cv::Point(position.x - 5, position.y - 3), 3, cv::Scalar(255, 255, 255), -1);
    cv::circle(canvas, cv::Point(position.x + 5, position.y - 3), 3, cv::Scalar(255, 255, 255), -1);
    cv::circle(canvas, cv::Point(position.x - 5, position.y - 3), 1, cv::Scalar(0, 0, 0), -1);
    cv::circle(canvas, cv::Point(position.x + 5, position.y - 3), 1, cv::Scalar(0, 0, 0), -1);
    
    // Draw beak
    std::vector<cv::Point> beak = {
        cv::Point(position.x + 15, position.y),
        cv::Point(position.x + 8, position.y - 3),
        cv::Point(position.x + 8, position.y + 3)
    };
    cv::fillPoly(canvas, beak, cv::Scalar(0, 165, 255));  // Orange beak
}

void Bird::launch(cv::Point2f vel) {
    velocity = vel;
    hasBeenLaunched = true;
}

void Bird::useSpecialAbility() {
    if (specialAbilityUsed || !hasBeenLaunched) return;
    
    switch(type) {
        case BirdType::RED:
            // Red bird: Extra damage on impact
            damage *= 1.5f;
            break;
        case BirdType::BLUE:
            // Blue bird: Split into 3 smaller birds (simplified: just boost)
            velocity.x *= 1.2f;
            velocity.y *= 1.2f;
            break;
        case BirdType::YELLOW:
            // Yellow bird: Speed boost
            velocity.x *= 1.5f;
            velocity.y *= 1.5f;
            break;
    }
    specialAbilityUsed = true;
}

bool Bird::isOffScreen(cv::Size windowSize) const {
    return position.x < -50 || position.x > windowSize.width + 50 || 
           position.y > windowSize.height + 50;
}

// Pig Implementation
Pig::Pig(cv::Point2f pos, PigType pigType) 
    : PhysicsObject(pos, 0, 2.0f), type(pigType), isDestroyed(false) {
    
    isStatic = true;  // Pigs don't move initially
    
    switch(type) {
        case PigType::SMALL:
            radius = 15.0f;
            health = maxHealth = 25;
            color = cv::Scalar(0, 255, 0);  // Green
            break;
        case PigType::MEDIUM:
            radius = 20.0f;
            health = maxHealth = 50;
            color = cv::Scalar(0, 200, 0);  // Darker green
            break;
        case PigType::LARGE:
            radius = 25.0f;
            health = maxHealth = 100;
            color = cv::Scalar(0, 150, 0);  // Even darker green
            break;
    }
}

void Pig::render(cv::Mat& canvas) {
    if (isDestroyed || !isActive) return;
    
    // Health-based color intensity
    float healthRatio = (float)health / maxHealth;
    cv::Scalar currentColor = color * healthRatio;
    
    // Draw pig body
    cv::circle(canvas, cv::Point(position.x, position.y), radius, currentColor, -1);
    
    // Draw pig features
    // Eyes
    cv::circle(canvas, cv::Point(position.x - radius/3, position.y - radius/3), 3, cv::Scalar(255, 255, 255), -1);
    cv::circle(canvas, cv::Point(position.x + radius/3, position.y - radius/3), 3, cv::Scalar(255, 255, 255), -1);
    cv::circle(canvas, cv::Point(position.x - radius/3, position.y - radius/3), 1, cv::Scalar(0, 0, 0), -1);
    cv::circle(canvas, cv::Point(position.x + radius/3, position.y - radius/3), 1, cv::Scalar(0, 0, 0), -1);
    
    // Snout
    cv::ellipse(canvas, cv::Point(position.x, position.y + radius/4), cv::Size(8, 6), 0, 0, 360, cv::Scalar(0, 100, 0), -1);
    
    // Nostrils
    cv::circle(canvas, cv::Point(position.x - 2, position.y + radius/4), 1, cv::Scalar(0, 0, 0), -1);
    cv::circle(canvas, cv::Point(position.x + 2, position.y + radius/4), 1, cv::Scalar(0, 0, 0), -1);
}

void Pig::takeDamage(int damage) {
    health -= damage;
    if (health <= 0) {
        health = 0;
        isDestroyed = true;
        isActive = false;
    } else {
        // Pig starts moving when hit
        isStatic = false;
    }
}

int Pig::getScore() const {
    switch(type) {
        case PigType::SMALL: return 5000;
        case PigType::MEDIUM: return 10000;
        case PigType::LARGE: return 15000;
        default: return 1000;
    }
}

// Block Implementation
Block::Block(cv::Point2f pos, BlockType blockType, cv::Size2f blockSize) 
    : PhysicsObject(pos, std::max(blockSize.width, blockSize.height) / 2, 3.0f), 
      type(blockType), size(blockSize), isDestroyed(false) {
    
    isStatic = true;  // Blocks don't move initially
    
    switch(type) {
        case BlockType::WOOD:
            health = maxHealth = 30;
            color = cv::Scalar(0, 100, 200);  // Brown
            break;
        case BlockType::STONE:
            health = maxHealth = 80;
            color = cv::Scalar(128, 128, 128);  // Gray
            break;
        case BlockType::ICE:
            health = maxHealth = 15;
            color = cv::Scalar(255, 200, 200);  // Light blue
            break;
    }
}

void Block::render(cv::Mat& canvas) {
    if (isDestroyed || !isActive) return;
    
    // Health-based transparency
    float healthRatio = (float)health / maxHealth;
    cv::Scalar currentColor = color * healthRatio;
    
    // Draw block as rectangle
    cv::Point2f topLeft(position.x - size.width/2, position.y - size.height/2);
    cv::Point2f bottomRight(position.x + size.width/2, position.y + size.height/2);
    
    cv::rectangle(canvas, cv::Point(topLeft.x, topLeft.y), cv::Point(bottomRight.x, bottomRight.y), currentColor, -1);
    
    // Draw border
    cv::rectangle(canvas, cv::Point(topLeft.x, topLeft.y), cv::Point(bottomRight.x, bottomRight.y), cv::Scalar(0, 0, 0), 2);
}

void Block::takeDamage(int damage) {
    health -= damage;
    if (health <= 0) {
        health = 0;
        isDestroyed = true;
        isActive = false;
    } else {
        // Block starts moving when hit
        isStatic = false;
    }
}

// Slingshot Implementation
Slingshot::Slingshot(cv::Point2f pos) 
    : basePosition(pos), pullPosition(pos), isPulling(false), maxPullDistance(200.0f), currentBird(nullptr) {}

void Slingshot::render(cv::Mat& canvas) {
    // Draw slingshot base
    cv::circle(canvas, cv::Point(basePosition.x, basePosition.y + 30), 8, cv::Scalar(0, 50, 100), -1);
    
    // Draw slingshot arms
    cv::line(canvas, cv::Point(basePosition.x - 15, basePosition.y), 
             cv::Point(basePosition.x - 5, basePosition.y + 30), cv::Scalar(0, 50, 100), 4);
    cv::line(canvas, cv::Point(basePosition.x + 15, basePosition.y), 
             cv::Point(basePosition.x + 5, basePosition.y + 30), cv::Scalar(0, 50, 100), 4);
    
    // Draw elastic band if pulling
    if (isPulling && currentBird) {
        cv::line(canvas, cv::Point(basePosition.x - 15, basePosition.y), 
                 cv::Point(pullPosition.x, pullPosition.y), cv::Scalar(100, 50, 0), 3);
        cv::line(canvas, cv::Point(basePosition.x + 15, basePosition.y), 
                 cv::Point(pullPosition.x, pullPosition.y), cv::Scalar(100, 50, 0), 3);
    }
}

void Slingshot::startPull(cv::Point2f mousePos) {
    if (currentBird) {
        isPulling = true;
        updatePull(mousePos);
    }
}

void Slingshot::updatePull(cv::Point2f mousePos) {
    if (!isPulling || !currentBird) return;
    
    // Calculate pull direction and distance
    cv::Point2f pullVector = mousePos - basePosition;
    float distance = std::sqrt(pullVector.x * pullVector.x + pullVector.y * pullVector.y);
    
    // Limit pull distance
    if (distance > maxPullDistance) {
        pullVector = pullVector * (maxPullDistance / distance);
    }
    
    pullPosition = basePosition + pullVector;
    
    // Update bird position
    currentBird->position = pullPosition;
}

cv::Point2f Slingshot::releaseBird() {
    if (!isPulling || !currentBird) return cv::Point2f(0, 0);
    
    // Calculate launch velocity
    cv::Point2f launchVector = basePosition - pullPosition;
    float strength = std::sqrt(launchVector.x * launchVector.x + launchVector.y * launchVector.y);
    
    // Scale velocity based on pull strength (reduced from 400 to 200 for better control)
    cv::Point2f velocity = launchVector * (strength / maxPullDistance) * 200.0f;  // Max velocity of 200 pixels/s
    
    currentBird->launch(velocity);
    currentBird = nullptr;
    isPulling = false;
    pullPosition = basePosition;
    
    return velocity;
}

void Slingshot::setBird(Bird* bird) {
    currentBird = bird;
    if (bird) {
        bird->position = basePosition;
        bird->velocity = cv::Point2f(0, 0);
    }
}

float Slingshot::getPullStrength() const {
    if (!isPulling) return 0.0f;
    
    cv::Point2f pullVector = pullPosition - basePosition;
    float distance = std::sqrt(pullVector.x * pullVector.x + pullVector.y * pullVector.y);
    return std::min(distance / maxPullDistance, 1.0f);
}