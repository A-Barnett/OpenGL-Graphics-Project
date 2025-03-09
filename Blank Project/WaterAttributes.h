#pragma once
#include <sstream>
#include <iomanip>
#include <iostream>

class WaterAttributes {
public:
    float amplitude;
    float frequency;
    float waveSpeed;
    float amplitudeScale;
    float uniformity;
    int waveCount;
    float targetAmplitude;
    float targetFrequency;
    float targetWaveSpeed;
    float targetAmplitudeScale;
    float targetUniformity;
    int targetWaveCount;
    float transitionTimer = 0.0f;
    float transitionDuration = 60.0f;
    bool isTransitioning = false;
    //waterAttributes.StartTransition(1.20f, 0.000059f, 0.72f, 1.30f, 16.10f, 15); // low settings
    WaterAttributes()
        : amplitude(0.40f), frequency(0.000130), waveSpeed(0.72f), amplitudeScale(1.01f), uniformity(18.6f), waveCount(84) {}


    void StartTransition(float amp, float freq, float wave, float ampScale, float uni, int wav) {
        targetAmplitude = amp;
        targetFrequency = freq;
        targetWaveSpeed = wave;
        targetAmplitudeScale = ampScale;
        targetUniformity = uni;
        targetWaveCount = wav;

        transitionTimer = 0.0f;
        isTransitioning = true;
    }
    void Update(float dt) {
        if (isTransitioning) {
            transitionTimer += dt;
            float t = transitionTimer / transitionDuration;
            if (t >= 1.0f) {
                amplitude = targetAmplitude;
                frequency = targetFrequency;
                waveSpeed = targetWaveSpeed;
                amplitudeScale = targetAmplitudeScale;
                uniformity = targetUniformity;
                waveCount = targetWaveCount;
                isTransitioning = false;
            }
            else {
                amplitude = Lerp(amplitude, targetAmplitude, t);
                frequency = Lerp(frequency, targetFrequency, t);
                waveSpeed = Lerp(waveSpeed, targetWaveSpeed, t);
                amplitudeScale = Lerp(amplitudeScale, targetAmplitudeScale, t);
                uniformity = Lerp(uniformity, targetUniformity, t);
   

                float waveCountTransitionFactor = std::min(t / 0.4f, 1.0f);
                waveCount = static_cast<int>(Lerp(waveCount, targetWaveCount, waveCountTransitionFactor));
            }
        }
    }

    std::string GetWaveParameters() const {
        std::ostringstream oss;
        oss << std::fixed << std::setprecision(2);
        oss << "Amplitude: " << amplitude
            << ", Frequency: ";

        // Increase precision for frequency
        oss << std::setprecision(6) << frequency; // Change to higher precision
        oss << std::setprecision(2); // Reset precision back to 2 for the other values

        oss << ", Wave Speed: " << waveSpeed
            << ", Amplitude Scale: " << amplitudeScale
            << ", Uniformity: " << uniformity
            << ", Wave Count: " << waveCount;

        return oss.str();
    }

private:
    float Lerp(float start, float end, float t) const {
        return start + t * (end - start);
    }
};
