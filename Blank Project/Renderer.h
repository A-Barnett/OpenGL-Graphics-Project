#pragma once
#include "../nclgl/OGLRenderer.h"
#include "../nclgl/Mesh.h"
#include "../nclgl/Camera.h"
#include "../nclgl/HeightMap.h"
#include "../nclgl/HeightMapRaw.h"
#include "../nclgl/Light.h"
#include "../nclgl/MeshAnimation.h"
#include "../nclgl/MeshMaterial.h"
#include "../nclgl/window.h"
#include "Controls.h"
#include "WaterAttributes.h"
#include "CameraMove.h"
#include <sstream>
#include <iomanip>
#include <list> 
#include <array>
#include <chrono>
#include <thread>
#include <vector>
#include <iostream>
#include <algorithm>
#include <cmath>
#include <random>


struct SubMesh {
    int index;
    std::array<float, 5> properties;
};

struct RainData {
    Vector3 position;
};

struct MeshData {
    float posX, posZ;
    float depth;
    int indexX, indexZ;
    float tessLevelT, tessLevelB, tessLevelL, tessLevelR;
    float tessLevelInner;
};


class Renderer : public OGLRenderer {
public:
    Renderer(Window& parent);
    ~Renderer(void);
    void RenderScene() override;
    void UpdateScene(float dt) override;
    void DrawSkybox();
    void RenderRain();
    void RenderMan();
    void RenderIsland();
    void RenderWater();
    void RenderLighthouse();
    void InitPostProcess();
    Vector3 GenerateRandomRainPosition(const Vector3& cameraPos,bool start);
    void RunControls() { controls.ProcessInputs();}
    inline void setTime(float t) { time = t; }

    void setTexture(GLuint &tex, const char* str) { 
        std::string filepath = std::string(TEXTUREDIR) + str;
        const char* path = filepath.c_str();
        tex = SOIL_load_OGL_texture(path, SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);
        SetTextureRepeating(tex, true);
        if (!tex) {
            std::cout << "NOT LOADED TEX";
            std::string s;
            std::cin >> s;
            return;
        }
    }

    void initLighthouse() {
        int i = 0;
        for (const std::array<float, 5>&var : lightHouseVars) {
            SubMesh subMesh = { i, var };
            var[4] == 1.0f ? transparentSubMeshes.push_back(subMesh) : opaqueSubMeshes.push_back(subMesh);
            i++;
        }
    }

    void TransitionTime(float dt, bool enable) {
        if (!transitioning && enable) {
            if (nightTime) {
                controls.setNight();
                raining = true;
                transitioning = true;
            }
            else {
                controls.setDay();
                raining = false;
                transitioning = true;
            }
            transitionProgress = 0.0f;
        }
        if (transitioning) {
            transitionProgress += dt / transitionTime;
            if (transitionProgress > 1.0f) transitionProgress = 1.0f; 
            float t = transitionProgress;

            light->SetRadius(nightTime ? (1.0f - t) * 0 + t * 50000 : (1.0f - t) * 50000 + t * 0);
            light2->SetRadius(nightTime ? (1.0f - t) * 1.0f + t * 0.1f : (1.0f - t) * 0.1f + t * 1.0f);
            waterColour = nightTime
                ? (Vector4(0.35f, 0.42f, 1.0f, 0.95f) * (1.0f - t)) + (Vector4(0.5f, 0.6f, 1.0f, 1.0f) * t)
                : Vector4(0.5f, 0.6f, 1.0f, 1.0f) * (1.0f - t) + Vector4(0.35f, 0.42f, 1.0f, 0.95f) * t;
            islandMetallic = nightTime ? (1.0f - t) * 0.5f + t * 3.0f : (1.0f - t) * 3.0f + t * 0.5f;
            fogLevel = nightTime ? (1.0f - t) * 1.0f + t * 0.9994f : (1.0f - t) * 0.9994f + t * 1.0f;
            reflectionsLevel = nightTime ? (1.0f - t) * 1 + t * 0 : (1.0f - t) * 0 + t * 1;
            if (transitionProgress >= 1.0f) {
                transitioning = false; 
            }
        }
    }

    void runCamMove(float dt) {
        cameraMove.setCam(camera);
        int result = cameraMove.CameraMovement(dt);
        if (result != 0) {
            nightTime = result==2;
            TransitionTime(dt,true);
        };
    }


private:
    float transitionTime = 20.0f; 
    float transitionProgress = 0.0f; 
    bool transitioning = false;
    Controls controls;
    void DrawPostProcess();
    void DrawScene();
    void PresentScene();
    HeightMapRaw* heightMap;
    Vector3 dimensions;
    Shader* waterShader;
    Shader* lightShader;
    Shader* islandShader;
    Shader* rainShader;
    Mesh* waterQuad;
    Mesh* postProcessQuad;
    Mesh* lightHouse;
    Mesh* island;
    Mesh* skyQuad;
    Mesh* rainQuad;
    Shader* tessShader;
    float tessLevel = 64.0f; // Number of subdivisions, can be adjusted
    Camera* camera;
    Vector3 position = { 0,-300,-1000.0f };
    Shader* skyboxShader;
    GLuint cubeMap;
    GLuint terrainTex;
    GLuint islandTex;
    GLuint bumpmap;
    GLuint lighthouseTex;
    GLuint lighthouseNormal;
    Light* light;
    Light* light2;
    Shader* sceneShader;
    float rotation = 25.0f;
    float time;
    bool nightTime = true;
    bool wireFrame = false;
    bool fogEnabled = true;
    float reflectionsLevel=0;
    bool postProcessEnabled = true;
    bool raining = true;
    bool freecam = false;
    float fogLevel= 0.9994f;
    float quadScale = 500.0f;
    float lastFrameTime = -10.0f;
    float metallic = 5.27f;
    float smoothness = 2.5f;
    float texScale = 0.0f;
    float lightDirectionX = 0.0f;
    float lightDirectionY = -0.5f;
    float lightDirectionZ = 0.0f;
    float lightRad = 60.0f;
    float spotlightIntensity = 2.0f;
    float transitionDuration = 10.0f;
    float transitionTimer = 0.0f;
    bool isTransitioning = false;
    float lightRotationAngle = 0.0f;
    float tessLevelMin = 16.0f; // FPS SETTINGS HIGH
    float tessLevelMax = 64.0f; //
    float minDistance = 2000.0f;
    float maxDistance = 20000.0f;
    //float tessLevelMin = 3.0f; // FPS SETTINGS LOW
    //float tessLevelMax = 10.0f; //
    //float minDistance = 100.0f;
    //float maxDistance = 500.0f;
    Vector4 waterColour = Vector4(0.5f, 0.6f, 1.0f, 1.0f);
    int grid = 50;
    Shader* processShader;
    GLuint bufferFBO;
    GLuint processFBO;
    GLuint bufferColourTex[2];
    GLuint bufferDepthTex;
    GLuint heightTexture;
    GLuint rainPositionBuffer;
    GLuint rainQuadVAO;
    float islandMetallic = 3.0f;
    float lighthouseBrickMetallic = 0.1;
    float lighthouseBrickSmooth = 1;
    float lighthouseGlassMetallic = 1000000;
    float lighthouseGlassSmooth = 0.1;
    float lighthouseMetalMetallic = 5;
    float lighthouseMetalSmooth = 0.5;
    std::list<std::array<float, 5>> lightHouseVars = {
        {0,0,lighthouseMetalMetallic,lighthouseMetalSmooth,0 },
        {14.5,0,lighthouseBrickMetallic,lighthouseBrickSmooth,0},
        {14.5,0,lighthouseBrickMetallic,lighthouseBrickSmooth,0},
        {0,0,lighthouseMetalMetallic,lighthouseMetalSmooth,0},
        {14.5,0,lighthouseBrickMetallic,lighthouseBrickSmooth,0},
        {0,0,lighthouseMetalMetallic,lighthouseMetalSmooth,0},
        {0,0,lighthouseGlassMetallic,lighthouseGlassSmooth,1},
        {0,0,lighthouseGlassMetallic,lighthouseGlassSmooth,1},
        {0,0,lighthouseGlassMetallic,lighthouseGlassSmooth,1},
        {0,0,lighthouseGlassMetallic,lighthouseGlassSmooth,1},
        {0,0,lighthouseGlassMetallic,lighthouseGlassSmooth,0}
    };
    std::vector<SubMesh> opaqueSubMeshes;
    std::vector<SubMesh> transparentSubMeshes;
    WaterAttributes waterAttributes;
    CameraMove cameraMove;

    float tiltAngle = 7.0 * (PI / 180);;
    float tiltX = sin(tiltAngle);
    float tiltY = cos(tiltAngle);
    float rainRadius = 1000;

    Mesh* manMesh;
    Shader* manShader;
    MeshAnimation* anim;
    MeshMaterial* material;
    vector<GLuint> matTextures;

    int currentFrame;
    float frameTime;

};