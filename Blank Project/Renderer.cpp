#include "Renderer.h"
#include "SceneNode.h"

std::vector<RainData> rainData(5000); //LOW SETTINGS
//std::vector<RainData> rainData(100000);
std::vector<std::unique_ptr<SceneNode>> sceneNodes;


Renderer::Renderer(Window& parent) : OGLRenderer(parent), controls(waterAttributes, light, metallic, smoothness, texScale, wireFrame, fogEnabled, postProcessEnabled, nightTime,freecam) {

    camera = new Camera(-15.0f, 180.0f, Vector3(0,0,0));

    light = new Light(Vector3(25000.0f + 70, 1000.0f + 410, 25000.0f + 30), Vector4(1, 0.91, 0.74, 1), 50000, lightRad, Vector3(0, 0, 0));
    light2 = new Light(Vector3(20000.0f + 70, 10000, (25000.0f + 30)*2), Vector4(1, 0.91, 0.74, 1), 0.1f, 0, Vector3(-100,0.1,-100));

    waterQuad = Mesh::GenerateQuad(quadScale, Vector3(0, 0, 0), GL_PATCHES);
    postProcessQuad = Mesh::GenerateQuad2();
    lightHouse = Mesh::LoadFromMeshFile("LightHouse.msh");
    heightMap = new HeightMapRaw(TEXTUREDIR "terrain.raw", 1025, 1025);
    rainQuad = Mesh::GenerateQuad4();
    skyQuad = Mesh::GenerateQuad3();

    waterShader = new Shader("WaterTessVertex.glsl", "WaterTessFragment.glsl", "", "WaterTessControl.glsl", "WaterTessEval.glsl");
    lightShader = new Shader("LighthouseVertex.glsl", "LighthouseFragment.glsl");
    islandShader = new Shader("IslandVertex.glsl", "IslandFragment.glsl");
    processShader = new Shader("PostProcessVertex.glsl", "PostProcessFragment.glsl");
    sceneShader = new Shader("TexturedVertex.glsl", "TexturedFragment.glsl");
    rainShader = new Shader("RainVertex.glsl", "RainFragment.glsl");
    skyboxShader = new Shader("skyboxVertex.glsl", "skyboxFragment.glsl");

    setTexture(lighthouseTex, "brick.tga");
    setTexture(lighthouseNormal, "brickDOT3.tga");
    setTexture(terrainTex, "Water 2.jpg");
    setTexture(bumpmap, "Water 2_normal.jpg");
    std::vector<GLuint> islandDiffuseTextures(4);
    std::vector<GLuint> islandNormalTextures(4);
    setTexture(islandDiffuseTextures[0], "Sand.png");
    setTexture(islandDiffuseTextures[1], "Gravel.png");
    setTexture(islandDiffuseTextures[2], "Rock.png");
    setTexture(islandDiffuseTextures[3], "Grass.png");
    setTexture(islandNormalTextures[0], "Sand_Normals.png");
    setTexture(islandNormalTextures[1], "Gravel_Normals.png");
    setTexture(islandNormalTextures[2], "Rock_Normals.png");
    setTexture(islandNormalTextures[3], "Grass_Normals.png");

    sceneNodes.emplace_back(std::make_unique<IslandRender>(islandShader, islandDiffuseTextures, islandNormalTextures, this));
    sceneNodes.emplace_back(std::make_unique<LightHouseRender>(lightShader, std::vector<GLuint>{lighthouseTex}, std::vector<GLuint>{lighthouseNormal}, this));
    sceneNodes.emplace_back(std::make_unique<WaterRender>(waterShader, std::vector<GLuint>{terrainTex}, std::vector<GLuint>{bumpmap}, this));

    cubeMap = SOIL_load_OGL_cubemap(TEXTUREDIR "px (Custom).jpg", TEXTUREDIR "nx (Custom).jpg", TEXTUREDIR "py (Custom).jpg",
        TEXTUREDIR "ny (Custom).jpg", TEXTUREDIR "pz (Custom).jpg", TEXTUREDIR "nz (Custom).jpg",
        SOIL_LOAD_RGB, SOIL_CREATE_NEW_ID, 0);
    

    manShader = new Shader("SkinningVertex.glsl", "texturedFragment.glsl");
    manMesh = Mesh::LoadFromMeshFile("Role_T.msh");
    anim = new MeshAnimation("Role_T.anm");
    material = new MeshMaterial("Role_T.mat");


    for (int i = 0; i < manMesh->GetSubMeshCount(); ++i) {
        const MeshMaterialEntry* matEntry = material->GetMaterialForLayer(i);
        const string* filename = nullptr;
        matEntry->GetEntry("Diffuse", &filename);

        string path = TEXTUREDIR + *filename;
        GLuint texID = SOIL_load_OGL_texture(path.c_str(), SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y);
        matTextures.emplace_back(texID);
    }

    currentFrame = 0;
    frameTime = 0.0f;

    InitPostProcess();
    initLighthouse();

    if (!Shader::CheckShaders()) {
        std::cout << "NOT LOADED SHADER";
        std::string s;
        std::cin >> s;
        return;
    }
    for (RainData& d : rainData) {
        d.position = GenerateRandomRainPosition(camera->GetPosition(),true);
    }
    init = true;
}


void Renderer::InitPostProcess() {
    glGenTextures(1, &bufferDepthTex);
    glBindTexture(GL_TEXTURE_2D, bufferDepthTex);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, width, height, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, NULL);
    for (int i = 0; i < 2; ++i) {
        glGenTextures(1, &bufferColourTex[i]);
        glBindTexture(GL_TEXTURE_2D, bufferColourTex[i]);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    }
    glGenFramebuffers(1, &bufferFBO);
    glGenFramebuffers(1, &processFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, bufferFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, bufferDepthTex, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_TEXTURE_2D, bufferDepthTex, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, bufferColourTex[0], 0);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE || !bufferDepthTex || !bufferColourTex[0]) {
        return;
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glEnable(GL_DEPTH_TEST);
    glPatchParameteri(GL_PATCH_VERTICES, 4);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
}


Renderer::~Renderer() {
    delete waterQuad;
    delete tessShader;
    delete camera;
    delete heightMap;
    delete waterShader;
    delete light;
    delete lightHouse;
    delete lightShader;
    delete processShader;
    delete sceneShader;
    glDeleteTextures(2, bufferColourTex);
    glDeleteTextures(1, &bufferDepthTex);
    glDeleteFramebuffers(1, &bufferFBO);
    glDeleteFramebuffers(1, &processFBO);
}


void Renderer::UpdateScene(float dt) {
    camera->UpdateCamera(dt);
    viewMatrix = camera->BuildViewMatrix();
    frameTime -= dt/2;
    while (frameTime < 0.0f) {
        currentFrame = (currentFrame + 1) % anim->GetFrameCount();
        frameTime += 1.0f / anim->GetFrameRate();
    }
    RunControls();
    if(!freecam) runCamMove(dt);
    waterAttributes.Update(dt);

    if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_SPACE)) {
        nightTime = !nightTime;
        TransitionTime(dt,true);
    }
    if (reflectionsLevel !=1) {
        for (RainData& d : rainData) {
            d.position.y -= dt * 70.0f * tiltY;
            d.position.x -= dt * 70.0f * tiltX;
            if (d.position.y < -500.0f && raining) {
                d.position = GenerateRandomRainPosition(camera->GetPosition(), false);
            }
        }
    }
    TransitionTime(dt, false);
}


Vector3 Renderer::GenerateRandomRainPosition(const Vector3& cameraPos, bool start) {
    static std::random_device dev;             
    static std::mt19937 rng(dev());            
    std::uniform_real_distribution<float> angleDist(0.0f, 2.0f * PI); 
    std::uniform_real_distribution<float> distanceDist(0.0f, rainRadius);                   
    std::uniform_real_distribution<float> yDist(start?1000.0f:500.0f, start ? 10000.0f : 1000.0f);     
    float angle = angleDist(rng);                    
    float distance = distanceDist(rng);                                       
    return Vector3(cameraPos.x / 7 + distance * cos(angle), yDist(rng), cameraPos.z / 7 + distance * sin(angle));
}


void Renderer::RenderScene() {
    if (postProcessEnabled) glBindFramebuffer(GL_FRAMEBUFFER, bufferFBO);
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    projMatrix = Matrix4::Perspective(1.0f, 100000.0f, (float)width / (float)height, 60.0f);
    DrawSkybox();
    DrawScene();
    if (postProcessEnabled) {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        DrawPostProcess();
        PresentScene();
     }
}


void Renderer::DrawSkybox() {
    glDepthMask(GL_FALSE);
    BindShader(skyboxShader);
    UpdateShaderMatrices();
    skyQuad->Draw(GL_TRIANGLE_STRIP);
    glDepthMask(GL_TRUE);
}


void Renderer::DrawScene() {
    glPolygonMode(GL_FRONT_AND_BACK, wireFrame ? GL_LINE : GL_FILL);
    for (int i = 0; i < sceneNodes.size();i++) {
        BindShader(sceneNodes[i]->shader);
        sceneNodes[i]->Initialise();
    }
    modelMatrix = Matrix4::Translation(Vector3(25000.0f + 70, 1022.0f + 410, 25000.0f + 30)) * Matrix4::Scale(Vector3(15.0f, 15.0f, 15.0f));
    RenderMan();
    modelMatrix = Matrix4::Scale(Vector3(1.0f, 1.0f, 1.0f));
    if (reflectionsLevel != 1) RenderRain();
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void Renderer::RenderMan() {
    BindShader(manShader);
    glUniform1i(glGetUniformLocation(manShader->GetProgram(), "diffuseTex"), 0);

    UpdateShaderMatrices();

    vector<Matrix4> frameMatrices;
    const Matrix4* invBindPose = manMesh->GetInverseBindPose();
    const Matrix4* frameData = anim->GetJointData(currentFrame);

    for (unsigned int i = 0; i < manMesh->GetJointCount(); ++i) {
        frameMatrices.emplace_back(frameData[i] * invBindPose[i]);
    }

    int j = glGetUniformLocation(manShader->GetProgram(), "joints");
    glUniformMatrix4fv(j, frameMatrices.size(), false, (float*)frameMatrices.data());

    for (int i = 0; i < manMesh->GetSubMeshCount(); ++i) {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, matTextures[i]);
        manMesh->DrawSubMesh(i);
    }
}


void Renderer::RenderRain() {
    BindShader(rainShader);
    modelMatrix = Matrix4::Scale(Vector3(7.0f, 7.0f, 7.0f));
    UpdateShaderMatrices();
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    GLuint instanceVBO;
    glGenBuffers(1, &instanceVBO);
    glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
    glBufferData(GL_ARRAY_BUFFER, rainData.size() * sizeof(RainData), rainData.data(), GL_DYNAMIC_DRAW);
    glBindVertexArray(rainQuad->GetVAO());
    glEnableVertexAttribArray(2); 
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(RainData), (void*)offsetof(RainData, position));
    glVertexAttribDivisor(2, 3);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    glBindVertexArray(rainQuad->GetVAO());
    glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, rainQuad->GetVertexCount(), rainData.size());
    glBindVertexArray(0);
    glDeleteBuffers(1, &instanceVBO);
}


void Renderer::RenderLighthouse() {

    modelMatrix = Matrix4::Scale(Vector3(10, 10, 10));
    UpdateShaderMatrices();
    SetShaderLight(*light);
    SetShaderLight2(*light2);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glUniform3f(glGetUniformLocation(lightShader->GetProgram(), "startingPos"), (5000.0f - 12) / 2, 91.0f, (5000.0f - 2) / 2);
    glUniform3fv(glGetUniformLocation(lightShader->GetProgram(), "cameraPos"), 1, (float*)&camera->GetPosition());

    for (const SubMesh& subMesh : opaqueSubMeshes) {
        const std::array<float, 5>& var = subMesh.properties;
        glUniform1f(glGetUniformLocation(lightShader->GetProgram(), "textureScale"), var[0]);
        glUniform1f(glGetUniformLocation(lightShader->GetProgram(), "textureRotation"), var[1]);
        glUniform1f(glGetUniformLocation(lightShader->GetProgram(), "metallic"), var[2]);
        glUniform1f(glGetUniformLocation(lightShader->GetProgram(), "smoothness"), var[3]);
        glUniform1i(glGetUniformLocation(lightShader->GetProgram(), "transparent"), static_cast<int>(var[4]));
        glUniform4f(glGetUniformLocation(lightShader->GetProgram(), "nodeColour"), 1, 1, 1, 1.0f);
        lightHouse->DrawSubMesh(subMesh.index);
    }
    for (const SubMesh& subMesh : transparentSubMeshes) {
        const std::array<float, 5>& var = subMesh.properties;
        glUniform1f(glGetUniformLocation(lightShader->GetProgram(), "textureScale"), var[0]);
        glUniform1f(glGetUniformLocation(lightShader->GetProgram(), "textureRotation"), var[1]);
        glUniform1f(glGetUniformLocation(lightShader->GetProgram(), "metallic"), var[2]);
        glUniform1f(glGetUniformLocation(lightShader->GetProgram(), "smoothness"), var[3]);
        glUniform1i(glGetUniformLocation(lightShader->GetProgram(), "transparent"), static_cast<int>(var[4]));

        glUniform4f(glGetUniformLocation(lightShader->GetProgram(), "nodeColour"), 1, 1, 1, 1.0f);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, lighthouseTex);
        lightHouse->DrawSubMesh(subMesh.index);
    }
}

void Renderer::RenderIsland() {
    modelMatrix = Matrix4::Scale(Vector3(0.5f, 0.5f, 0.5f));
    SetShaderLight(*light);
    SetShaderLight2(*light2);
    glUniform3fv(glGetUniformLocation(islandShader->GetProgram(), "cameraPos"), 1, (float*)&camera->GetPosition());
    glUniform1f(glGetUniformLocation(islandShader->GetProgram(), "maxSandHeight"), 100);
    glUniform1f(glGetUniformLocation(islandShader->GetProgram(), "maxGravelHeight"), 300);
    glUniform1f(glGetUniformLocation(islandShader->GetProgram(), "maxCliffHeight"), 800);
    glUniform1f(glGetUniformLocation(islandShader->GetProgram(), "smoothness"), 3.0f);
    glUniform1f(glGetUniformLocation(islandShader->GetProgram(), "metallic"), islandMetallic);
    glUniform3f(glGetUniformLocation(islandShader->GetProgram(), "startingPos"), 42000, -50.0f, 41000);
    UpdateShaderMatrices();
    heightMap->Draw(GL_TRIANGLES);
}

void Renderer::RenderWater() {
    modelMatrix = Matrix4::Scale(Vector3(1.0f, 1.0f, 1.0f));
    UpdateShaderMatrices();
    float currentTime = time;
    float deltaTime = currentTime - lastFrameTime;
    lastFrameTime = currentTime;
    lightRotationAngle += deltaTime * 0.5f;
    float lightDirectionX = sin(lightRotationAngle);             
    float lightDirectionZ = cos(lightRotationAngle); 
    light->SetDirection(Vector3(lightDirectionX, lightDirectionY, lightDirectionZ));

    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMap);
    glUniform1i(glGetUniformLocation(waterShader->GetProgram(), "cubeTex"), 3);
    glUniform3fv(glGetUniformLocation(waterShader->GetProgram(), "cameraPos"), 1, (float*)&camera->GetPosition());

    SetShaderLight(*light);
    SetShaderLight2(*light2);

    std::vector<MeshData> meshes;

    float camX = camera->GetPosition().x;
    float camZ = camera->GetPosition().z;

    float maxDepth = 0;
    for (int x = 0; x < grid; x++) {
        for (int z = 0; z < grid; z++) {
            float posX = x * (quadScale * 2);
            float posZ = z * (quadScale * 2);
            float depth = std::sqrt(std::pow(posX - camX, 2) + std::pow(posZ - camZ, 2));
            if (depth > maxDepth) {
                maxDepth = depth;
            }
            meshes.push_back({ posX, posZ, depth,x,z,1,1,1,1,0 });
        }
    }

    glUniform4f(glGetUniformLocation(waterShader->GetProgram(), "colour"), waterColour.x, waterColour.y, waterColour.z, waterColour.w);
    glUniform1f(glGetUniformLocation(waterShader->GetProgram(), "time"), time);
    glUniform1f(glGetUniformLocation(waterShader->GetProgram(), "frequency"), waterAttributes.frequency);
    glUniform1f(glGetUniformLocation(waterShader->GetProgram(), "amplitude"), waterAttributes.amplitude);
    glUniform1f(glGetUniformLocation(waterShader->GetProgram(), "waveSpeed"), waterAttributes.waveSpeed);
    glUniform1f(glGetUniformLocation(waterShader->GetProgram(), "amplitudeScale"), waterAttributes.amplitudeScale);
    glUniform1f(glGetUniformLocation(waterShader->GetProgram(), "smoothness"), smoothness);
    glUniform1f(glGetUniformLocation(waterShader->GetProgram(), "metallic"), metallic);
    glUniform1f(glGetUniformLocation(waterShader->GetProgram(), "textureScale"), texScale);
    glUniform1f(glGetUniformLocation(waterShader->GetProgram(), "spotlightIntensity"), spotlightIntensity);
    glUniform1f(glGetUniformLocation(waterShader->GetProgram(), "directionBiasStrength"), waterAttributes.uniformity);
    glUniform1i(glGetUniformLocation(waterShader->GetProgram(), "waveCount"), waterAttributes.waveCount);
    glUniform1f(glGetUniformLocation(waterShader->GetProgram(), "reflections"), reflectionsLevel);

    for (int i = 0; i < meshes.size(); i++) {
        MeshData mesh = meshes[i];
        float depthRatio = (mesh.depth - minDistance) / (maxDistance - minDistance);
        if (depthRatio < 0.0f) {depthRatio = 0.0f;}
        else if (depthRatio > 1.0f) { depthRatio = 1.0f;}


        float tessLevelInner = tessLevelMax - depthRatio * (tessLevelMax - tessLevelMin);
        meshes[i].tessLevelInner = tessLevelInner;
        meshes[i].tessLevelInner = tessLevelInner;

        if (i % grid != 0) { // not on left side
            meshes[i - 1].tessLevelL = tessLevelInner;
            meshes[i].tessLevelR = tessLevelInner;
        }
        if ((i + 1) % grid != 0) { //not on right side
            meshes[i + 1].tessLevelR = tessLevelInner;
            meshes[i].tessLevelL = tessLevelInner;
        }
        if (i >= grid) { // not on top side
            meshes[i - grid].tessLevelT = tessLevelInner;
            meshes[i].tessLevelB = tessLevelInner;
        }
        if (i < grid * (grid - 1)) { // not on bottom side
            meshes[i + grid].tessLevelB = tessLevelInner;
            meshes[i].tessLevelT = tessLevelInner;
        }
        if (mesh.tessLevelL == 1) {
            meshes[i].tessLevelL = tessLevelInner;
        }
        if (mesh.tessLevelR == 1) {
            meshes[i].tessLevelR = tessLevelInner;
        }
        if (mesh.tessLevelT == 1) {
            meshes[i].tessLevelT = tessLevelInner;
        }
        if (mesh.tessLevelB == 1) {
            meshes[i].tessLevelB = tessLevelInner;
        }
    }

    GLuint instanceVBO;
    glGenBuffers(1, &instanceVBO);
    glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
    glBufferData(GL_ARRAY_BUFFER, meshes.size() * sizeof(MeshData), meshes.data(), GL_DYNAMIC_DRAW);
    glBindVertexArray(waterQuad->GetVAO());

    glEnableVertexAttribArray(2); 
    glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, sizeof(MeshData), (void*)offsetof(MeshData, tessLevelInner));
    glVertexAttribDivisor(2, 1);

    glEnableVertexAttribArray(3); 
    glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, sizeof(MeshData), (void*)offsetof(MeshData, tessLevelL));
    glVertexAttribDivisor(3, 1);

    glEnableVertexAttribArray(4); 
    glVertexAttribPointer(4, 1, GL_FLOAT, GL_FALSE, sizeof(MeshData), (void*)offsetof(MeshData, tessLevelR));
    glVertexAttribDivisor(4, 1);

    glEnableVertexAttribArray(5); 
    glVertexAttribPointer(5, 1, GL_FLOAT, GL_FALSE, sizeof(MeshData), (void*)offsetof(MeshData, tessLevelT));
    glVertexAttribDivisor(5, 1);

    glEnableVertexAttribArray(6); 
    glVertexAttribPointer(6, 1, GL_FLOAT, GL_FALSE, sizeof(MeshData), (void*)offsetof(MeshData, tessLevelB));
    glVertexAttribDivisor(6, 1);

    glEnableVertexAttribArray(7);
    glVertexAttribPointer(7, 1, GL_FLOAT, GL_FALSE, sizeof(MeshData), (void*)offsetof(MeshData, posX));
    glVertexAttribDivisor(7, 1);

    glEnableVertexAttribArray(8);
    glVertexAttribPointer(8, 1, GL_FLOAT, GL_FALSE, sizeof(MeshData), (void*)offsetof(MeshData, posZ));
    glVertexAttribDivisor(8, 1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    glBindVertexArray(waterQuad->GetVAO());
    glDrawArraysInstanced(GL_PATCHES, 0, waterQuad->GetVertexCount(), meshes.size());
    glBindVertexArray(0);
    glDeleteBuffers(1, &instanceVBO);
}

void Renderer::DrawPostProcess() {
    glBindFramebuffer(GL_FRAMEBUFFER, processFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, bufferColourTex[1], 0);
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

    BindShader(processShader);
    modelMatrix.ToIdentity();
    viewMatrix.ToIdentity();
    projMatrix.ToIdentity();
    UpdateShaderMatrices();

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, bufferColourTex[0]);
    glUniform1i(glGetUniformLocation(processShader->GetProgram(), "sceneTex"), 0);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, bufferDepthTex);
    glUniform1i(glGetUniformLocation(processShader->GetProgram(), "depthTex"), 1);

    glUniform1i(glGetUniformLocation(processShader->GetProgram(), "fogEnabled"), fogEnabled); // Adjust based on scene
    glUniform1f(glGetUniformLocation(processShader->GetProgram(), "fogStart"), fogLevel); // Adjust based on scene
    glUniform1f(glGetUniformLocation(processShader->GetProgram(), "fogEnd"), 0.99998f); // Adjust based on scene
    glUniform3fv(glGetUniformLocation(processShader->GetProgram(), "cameraDirection"), 1, (float*)&camera->getRotation());
    glUniform3fv(glGetUniformLocation(processShader->GetProgram(), "cameraPos"), 1, (float*)&camera->GetPosition());
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, bufferColourTex[0], 0);
    SetShaderLight(*light);
    postProcessQuad->Draw(GL_TRIANGLE_STRIP);
}

void Renderer::PresentScene() {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
    BindShader(sceneShader);
    modelMatrix.ToIdentity();
    viewMatrix.ToIdentity();
    projMatrix.ToIdentity();
    UpdateShaderMatrices();
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, bufferColourTex[0]);
    glUniform1i(glGetUniformLocation(sceneShader->GetProgram(), "diffuseTex"), 0);
    postProcessQuad->Draw(GL_TRIANGLE_STRIP);
}
