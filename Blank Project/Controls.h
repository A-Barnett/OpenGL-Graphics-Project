#pragma once
#include "../nclgl/Window.h"
#include "WaterAttributes.h"
#include "../nclgl/Light.h"

class Controls {
public:
    Controls(WaterAttributes& waterAttr, Light* light, float& metallic, float& smoothness, float& texScale, bool& wireFrame, bool& fogEnabled, bool& postProcessEnabled, bool& nightTime,bool& freecam)
        : waterAttributes(waterAttr), light(light), metallic(metallic), smoothness(smoothness), texScale(texScale), wireFrame(wireFrame), fogEnabled(fogEnabled) , postProcessEnabled(postProcessEnabled), nightTime(nightTime),freecam(freecam){}

    void ProcessInputs() {
        if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_V)) {
            wireFrame = !wireFrame;
        }
        if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_B)) {
            postProcessEnabled = !postProcessEnabled;
        }
        if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_N)) {
            fogEnabled = !fogEnabled;
        }
        if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_C)) {
            freecam = !freecam;
        }
        if (Window::GetKeyboard()->KeyDown(KEYBOARD_I)) {
            light->SetRadius(light->GetRadius() + 10);
        }
        if (Window::GetKeyboard()->KeyDown(KEYBOARD_K)) {
            light->SetRadius(light->GetRadius() - 10);
        }
        if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_M)) {
            std::cout << waterAttributes.GetWaveParameters() << std::endl;
        }
        HandleWaterAttributes();
        AdjustValue(metallic, KEYBOARD_Y, KEYBOARD_H, 0.1f);
        AdjustValue(smoothness, KEYBOARD_T, KEYBOARD_G, 0.1f);
        AdjustValue(texScale, KEYBOARD_R, KEYBOARD_F, 1.0f);
    }

    void setDay() {
        waterAttributes.StartTransition(0.50f, 0.000167f, 0.54f, 0.99f, 1.40f, 78);
       // waterAttributes.StartTransition(2.5f, 0.000167f, 0.54f, 0.99f, 1.40f, 10); //low settings
    }

    void setNight() {
        waterAttributes.StartTransition(0.40f, 0.000130f, 0.72f, 1.01f, 18.6f, 84);
       // waterAttributes.StartTransition(1.20f, 0.000059f, 0.72f, 1.30f, 16.10f, 15); // low settings
    }

private:
    WaterAttributes& waterAttributes;
    Light* light;
    float& metallic;
    float& smoothness;
    float& texScale;
    bool& wireFrame;
    bool& postProcessEnabled;
    bool& fogEnabled;
    bool& nightTime;
    bool& freecam;


    void HandleWaterAttributes() {
        AdjustIntValue(waterAttributes.waveCount, KEYBOARD_UP, KEYBOARD_DOWN, 1, 1);
        AdjustValue(waterAttributes.amplitude, KEYBOARD_NUMPAD9, KEYBOARD_NUMPAD6, 0.001f, 0);
        AdjustValue(waterAttributes.frequency, KEYBOARD_NUMPAD8, KEYBOARD_NUMPAD5, 0.000001f, 0);
        AdjustValue(waterAttributes.waveSpeed, KEYBOARD_NUMPAD7, KEYBOARD_NUMPAD4, 0.01f, 0);
        AdjustValue(waterAttributes.amplitudeScale, KEYBOARD_U, KEYBOARD_J, 0.001f);
        AdjustValue(waterAttributes.uniformity, KEYBOARD_Q, KEYBOARD_E, 0.1f, 0);

        if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_1)) {
            waterAttributes.StartTransition(0.40f, 0.000130f, 0.72f, 1.01f, 18.6f, 85);
        }
        if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_2)) {
            waterAttributes.StartTransition(5.40f, 0.000220f, 2.55f, 0.99f, 15.90f, 14);
        }
        if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_3)) {
            waterAttributes.StartTransition(13.20f, 0.000195f, 1.92f, 0.91f, 18.60f, 25);
        }
        if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_4)) {
            waterAttributes.StartTransition(25.10f, 0.000127f, 1.27f, 0.91f, 7.90f, 166);
        }
        if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_5)) {
            waterAttributes.StartTransition(1.70f, 0.000094f, 0.54f, 0.94f, 1.40f, 78);
        }
    }

    void AdjustValue(float& value, KeyboardKeys keyIncrease, KeyboardKeys keyDecrease, float step, float minValue = -FLT_MAX) {
        if (Window::GetKeyboard()->KeyDown(keyIncrease)) {
            value += step;
        }
        if (Window::GetKeyboard()->KeyDown(keyDecrease)) {
            value = std::max(value - step, minValue);
        }
    }
    void AdjustIntValue(int& value, KeyboardKeys keyIncrease, KeyboardKeys keyDecrease, int step, int minValue = 0) {
        if (Window::GetKeyboard()->KeyDown(keyIncrease)) {
            value += step;
        }
        if (Window::GetKeyboard()->KeyDown(keyDecrease)) {
            value = std::max(value - step, minValue);
        }
    }
};
