#include "Physics.h"
#include <algorithm>
#include <cmath>

Physics::Physics() {}

Physics::~Physics() {}

void Physics::update(std::vector<std::shared_ptr<PhysicsObject>>& objects, float deltaTime) {
    for (auto& obj : objects) {
        if (!obj->isActive || obj->isStatic) continue;
        
        // Apply gravity
        applyGravity(*obj, deltaTime);
        
        // Apply velocity
        applyVelocity(*obj, deltaTime);
        
        // Apply air resistance
        obj->velocity.x *= AIR_RESISTANCE;
        obj->velocity.y *= AIR_RESISTANCE;
    }
    
    // Check collisions between objects
    for (size_t i = 0; i < objects.size(); ++i) {
        for (size_t j = i + 1; j < objects.size(); ++j) {
            if (objects[i]->isActive && objects[j]->isActive &&
                checkCollision(*objects[i], *objects[j])) {
                resolveCollision(*objects[i], *objects[j]);
            }
        }
    }
}

bool Physics::checkCollision(const PhysicsObject& obj1, const PhysicsObject& obj2) {
    float dx = obj1.position.x - obj2.position.x;
    float dy = obj1.position.y - obj2.position.y;
    float distance = std::sqrt(dx * dx + dy * dy);
    return distance < (obj1.radius + obj2.radius);
}

void Physics::resolveCollision(PhysicsObject& obj1, PhysicsObject& obj2) {
    // Calculate collision normal
    float dx = obj1.position.x - obj2.position.x;
    float dy = obj1.position.y - obj2.position.y;
    float distance = std::sqrt(dx * dx + dy * dy);
    
    if (distance == 0) return; // Avoid division by zero
    
    // Normalize
    dx /= distance;
    dy /= distance;
    
    // Separate objects
    float overlap = (obj1.radius + obj2.radius) - distance;
    obj1.position.x += dx * overlap * 0.5f;
    obj1.position.y += dy * overlap * 0.5f;
    obj2.position.x -= dx * overlap * 0.5f;
    obj2.position.y -= dy * overlap * 0.5f;
    
    // Calculate relative velocity
    float relVelX = obj1.velocity.x - obj2.velocity.x;
    float relVelY = obj1.velocity.y - obj2.velocity.y;
    
    // Calculate relative velocity in collision normal direction
    float velAlongNormal = relVelX * dx + relVelY * dy;
    
    // Do not resolve if velocities are separating
    if (velAlongNormal > 0) return;
    
    // Calculate restitution
    float restitution = 0.6f;
    
    // Calculate impulse scalar
    float j = -(1 + restitution) * velAlongNormal;
    j /= 1/obj1.mass + 1/obj2.mass;
    
    // Apply impulse
    cv::Point2f impulse(j * dx, j * dy);
    
    if (!obj1.isStatic) {
        obj1.velocity.x += impulse.x / obj1.mass;
        obj1.velocity.y += impulse.y / obj1.mass;
    }
    
    if (!obj2.isStatic) {
        obj2.velocity.x -= impulse.x / obj2.mass;
        obj2.velocity.y -= impulse.y / obj2.mass;
    }
}

void Physics::applyGravity(PhysicsObject& obj, float deltaTime) {
    obj.acceleration.y = GRAVITY;
}

void Physics::applyVelocity(PhysicsObject& obj, float deltaTime) {
    // Update velocity with acceleration
    obj.velocity.x += obj.acceleration.x * deltaTime;
    obj.velocity.y += obj.acceleration.y * deltaTime;
    
    // Update position with velocity
    obj.position.x += obj.velocity.x * deltaTime;
    obj.position.y += obj.velocity.y * deltaTime;
    
    // Ground collision
    if (obj.position.y + obj.radius > GROUND_Y) {
        obj.position.y = GROUND_Y - obj.radius;
        obj.velocity.y *= -BOUNCE_DAMPING;
        
        // Stop very small bounces
        if (std::abs(obj.velocity.y) < 10.0f) {
            obj.velocity.y = 0;
        }
    }
}

void Physics::checkBounds(PhysicsObject& obj, cv::Size bounds) {
    // Left and right bounds
    if (obj.position.x - obj.radius < 0) {
        obj.position.x = obj.radius;
        obj.velocity.x *= -0.5f;
    }
    if (obj.position.x + obj.radius > bounds.width) {
        obj.position.x = bounds.width - obj.radius;
        obj.velocity.x *= -0.5f;
    }
    
    // Deactivate if object falls too far
    if (obj.position.y > bounds.height + 100) {
        obj.isActive = false;
    }
}

cv::Point2f Physics::calculateTrajectory(cv::Point2f start, cv::Point2f velocity, float time) {
    cv::Point2f result;
    result.x = start.x + velocity.x * time;
    result.y = start.y + velocity.y * time + 0.5f * GRAVITY * time * time;
    return result;
}

std::vector<cv::Point2f> Physics::getTrajectoryPoints(cv::Point2f start, cv::Point2f velocity, int numPoints) {
    std::vector<cv::Point2f> points;
    float timeStep = 0.1f;
    
    for (int i = 0; i < numPoints; ++i) {
        float time = i * timeStep;
        points.push_back(calculateTrajectory(start, velocity, time));
        
        // Stop if trajectory hits ground
        if (points.back().y >= GROUND_Y) break;
    }
    
    return points;
}