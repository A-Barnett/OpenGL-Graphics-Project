/******************************************************************************

CSC8502 - Advanced Graphics for Games 
Author:Alex Barnett (200960975)
Graphical Showcase: Scene of a lighthouse in an ocean, transitioning from daytime with calm waters into a stormy night.


References:

Island Mesh and Textures - Terrain Sample Asset Pack by Unity Technologies 
https://assetstore.unity.com/packages/3d/environments/landscapes/terrain-sample-asset-pack-145808?srsltid=AfmBOooU5XuAN0V7BZsOj8fw_XYcKhzd5SykKQScQHXhhc4FbGh00eUs

Daytime Skybox - Partly Cloudy (Pure Sky) Image by Greg Zaal
https://polyhaven.com/a/kloofendal_48d_partly_cloudy_puresky

Lighthouse Model - Red and white lighthouse by Ummoon
https://www.turbosquid.com/3d-models/red-and-white-lighthouse-1888547

Water Textures - CADhatch Seamless Water Textures
https://www.cadhatch.com/seamless-water-textures

*//////////////////////////////////////////////////////////////////////////////


#include "Renderer.h"

int main() {
    Window w("Alex Barnett - Lighthouse", 1920, 1080, false);

    if (!w.HasInitialised()) {
        std::cout << "NOT INITIALISED" << std::endl;
        std::string temp;
        std::cin >> temp;
        return -1;
    }
    w.LockMouseToWindow(false);
    w.ShowOSPointer(true);
    Renderer renderer(w);
    if (!renderer.HasInitialised()) {
        std::cout << "RENDERER NOT INITIALISED" << std::endl;
        std::string temp;
        std::cin >> temp;
        return -1;
    }
    float elapsedTime = 0.0f;
    int frameCount = 0;
    std::cout << "WORKS" << std::endl;
    int currentCount = 0;

    auto startTime = std::chrono::high_resolution_clock::now();
    auto lastFrameTime = startTime;

    while (w.UpdateWindow() && !Window::GetKeyboard()->KeyDown(KEYBOARD_ESCAPE)) {
        auto frameStartTime = std::chrono::high_resolution_clock::now();

        if (Window::GetKeyboard()->KeyDown(KEYBOARD_F5)) {
            Shader::ReloadAllShaders();
        }

        renderer.setTime(w.GetTimer()->GetTotalTimeSeconds());
        renderer.RenderScene();
        renderer.SwapBuffers();
        renderer.UpdateScene(w.GetTimer()->GetTimeDeltaSeconds() * 5);

        auto frameEndTime = std::chrono::high_resolution_clock::now();
        float frameDuration = std::chrono::duration<float, std::milli>(frameEndTime - frameStartTime).count();

        elapsedTime += frameDuration;
        frameCount++;
        currentCount++;

        float totalElapsedTime = std::chrono::duration<float, std::milli>(frameEndTime - startTime).count();

        if (currentCount >= 500) {
            float averageFPS = frameCount / (totalElapsedTime / 1000.0f);
            std::cout << "Average FPS: " << averageFPS << std::endl;
            currentCount = 0;
        }
        lastFrameTime = frameEndTime;
    }
    return 0;
}
