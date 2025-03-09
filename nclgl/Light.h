#pragma once

#include "Vector4.h"
#include "Vector3.h"

class Light {
public:
    Light() {} // Default constructor
    Light(const Vector3& position, const Vector4& colour, float radius, float angle, const Vector3& direction) {
        this -> position = position;
        this -> colour = colour;
        this -> radius = radius;
        this -> angle = angle;
        this->direction = direction;
    }
   

    ~Light(void) {}

    Vector3 GetPosition() const { return position; }
    void SetPosition(const Vector3& val) { position = val; }

    float GetRadius() const { return radius; }
    void SetRadius(float val) { radius = val; }

    Vector4 GetColour() const { return colour; }
    void SetColour(const Vector4& val) { colour = val; }

    float GetAngle() const { return angle; }
    void SetAngle(float val) { angle = val; }

    Vector3 GetDirection() const { return direction; }
    void SetDirection(const Vector3& val) { direction = val; }



protected:
    Vector3 position;
    float radius;
    Vector4 colour;
    float angle;
    Vector3 direction;
};
