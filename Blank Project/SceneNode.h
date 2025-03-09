#include <vector>
#include <iostream>
#include <string>
#include <memory>
#include "../nclgl/Shader.h"
#include "Renderer.h"

class SceneNode {
public:
    Shader* shader;
    std::vector<GLuint> diffuseTexts;
    std::vector<GLuint> normalTexts;
    Renderer* renderer;

    SceneNode(Shader* shader, const std::vector<GLuint>& diffuseTexts, const std::vector<GLuint>& normalTexts, Renderer* renderer)
        : shader(shader), diffuseTexts(diffuseTexts), normalTexts(normalTexts), renderer(renderer) {}

    virtual void Initialise() const = 0;
};

class IslandRender : public SceneNode {
public:
    IslandRender(Shader* shader, const std::vector<GLuint>& diffuseTexts, const std::vector<GLuint>& normalTexts, Renderer* renderer)
        : SceneNode(shader, diffuseTexts, normalTexts, renderer) {}
    void Initialise() const override;
};

class LightHouseRender : public SceneNode {
public:
    LightHouseRender(Shader* shader, const std::vector<GLuint>& diffuseTexts, const std::vector<GLuint>& normalTexts, Renderer* renderer)
        : SceneNode(shader, diffuseTexts, normalTexts, renderer) {}
    void Initialise() const override;
};

class WaterRender : public SceneNode {
public:
    WaterRender(Shader* shader, const std::vector<GLuint>& diffuseTexts, const std::vector<GLuint>& normalTexts, Renderer* renderer)
        : SceneNode(shader, diffuseTexts, normalTexts, renderer) {}
    void Initialise() const override;
};
