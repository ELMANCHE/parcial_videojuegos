#pragma once
#include <opencv2/opencv.hpp>
#include <vector>
#include <memory>

struct PhysicsObject {
    cv::Point2f position;
    cv::Point2f velocity;
    cv::Point2f acceleration;
    float mass;
    float radius;
    bool isStatic;
    bool isActive;
    
    PhysicsObject(cv::Point2f pos, float r, float m = 1.0f) 
        : position(pos), velocity(0, 0), acceleration(0, 0), 
          mass(m), radius(r), isStatic(false), isActive(true) {}
};

class Physics {
private:
    static constexpr float GRAVITY = 200.0f;  // pixels/s^2 (reduced from 400 for slower, more visible flight)
    static constexpr float AIR_RESISTANCE = 0.99f; // Reduced air resistance for smoother flight
    static constexpr float GROUND_Y = 700.0f;
    static constexpr float BOUNCE_DAMPING = 0.6f;
    
public:
    Physics();
    ~Physics();
    
    void update(std::vector<std::shared_ptr<PhysicsObject>>& objects, float deltaTime);
    
    bool checkCollision(const PhysicsObject& obj1, const PhysicsObject& obj2);
    void resolveCollision(PhysicsObject& obj1, PhysicsObject& obj2);
    
    void applyGravity(PhysicsObject& obj, float deltaTime);
    void applyVelocity(PhysicsObject& obj, float deltaTime);
    void checkBounds(PhysicsObject& obj, cv::Size bounds);
    
    cv::Point2f calculateTrajectory(cv::Point2f start, cv::Point2f velocity, float time);
    std::vector<cv::Point2f> getTrajectoryPoints(cv::Point2f start, cv::Point2f velocity, int numPoints = 20);
};