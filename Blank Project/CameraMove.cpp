#include "CameraMove.h"

float transitionTimeStart = 20.0f; 
float transitionTime = 6.0f;      
float currentTime = 0.0f;
int currentSegment = 0;           
bool initialWaitComplete = false;  
bool reverse = false;             

int CameraMove::CameraMovement(float dt) {
    if (!reverse && currentSegment >= directions.size() - 1) {
        reverse = true;        
        currentSegment--;         
        return 1;
    }
    if (reverse && currentSegment <= 0) {
        reverse = false;            
        initialWaitComplete = false; 
        currentTime = 0.0f;       
        return 2;
    }
    if (!initialWaitComplete) {
        currentTime += dt;
        if (currentTime >= transitionTimeStart) {
            initialWaitComplete = true;
            currentTime = 0.0f;     
        }
        else {
            camera->SetPosition(directions[0].first);
            camera->SetYaw(directions[0].second.first);
            camera->SetPitch(directions[0].second.second);
            return 0;
        }
    }

    currentTime += dt;
    float t = currentTime / transitionTime;
    if (t < 0.0f){t = 0.0f;}
    else if (t > 1.0f){t = 1.0f;}
    auto& start = directions[currentSegment];
    auto& end = directions[reverse ? currentSegment - 1 : currentSegment + 1];
    Vector3 newPosition = start.first * (1.0f - t) + end.first * t;
    float startYaw = start.second.first;
    float endYaw = end.second.first;
    float yawDifference = endYaw - startYaw;
    if (yawDifference > 180.0f) {endYaw -= 360.0f;}
    else if (yawDifference < -180.0f) {endYaw += 360.0f;}
    float newYaw = startYaw * (1.0f - t) + endYaw * t;
    float newPitch = start.second.second * (1.0f - t) + end.second.second * t;

    camera->SetPosition(newPosition);
    camera->SetYaw(newYaw);
    camera->SetPitch(newPitch);

    if (t >= 1.0f) {
        currentSegment += reverse ? -1 : 1;
        currentTime = 0.0f; 
    }

    return 0;
}
