#include "SceneNode.h"

void IslandRender::Initialise() const {
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, diffuseTexts[0]);
    glUniform1i(glGetUniformLocation(shader->GetProgram(), "sandTex"), 0);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, diffuseTexts[1]);
    glUniform1i(glGetUniformLocation(shader->GetProgram(), "gravelTex"), 1);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, diffuseTexts[2]);
    glUniform1i(glGetUniformLocation(shader->GetProgram(), "cliffTex"), 2);
    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, diffuseTexts[3]);
    glUniform1i(glGetUniformLocation(shader->GetProgram(), "grassTex"), 3);
    glActiveTexture(GL_TEXTURE4);
    glBindTexture(GL_TEXTURE_2D, normalTexts[0]);
    glUniform1i(glGetUniformLocation(shader->GetProgram(), "sandNormal"), 4);
    glActiveTexture(GL_TEXTURE5);
    glBindTexture(GL_TEXTURE_2D, normalTexts[1]);
    glUniform1i(glGetUniformLocation(shader->GetProgram(), "gravelNormal"), 5);
    glActiveTexture(GL_TEXTURE6);
    glBindTexture(GL_TEXTURE_2D, normalTexts[2]);
    glUniform1i(glGetUniformLocation(shader->GetProgram(), "cliffNormal"), 6);
    glActiveTexture(GL_TEXTURE7);
    glBindTexture(GL_TEXTURE_2D, normalTexts[3]);
    glUniform1i(glGetUniformLocation(shader->GetProgram(), "grassNormal"), 7);
    renderer->RenderIsland();
}

void LightHouseRender::Initialise() const {
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, diffuseTexts[0]);
    glUniform1i(glGetUniformLocation(shader->GetProgram(), "diffuseTex"), 0);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, normalTexts[0]);
    glUniform1i(glGetUniformLocation(shader->GetProgram(), "normalTex"), 1);
    renderer->RenderLighthouse(); 
}

void WaterRender::Initialise() const {
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, diffuseTexts[0]);
    glUniform1i(glGetUniformLocation(shader->GetProgram(), "diffusetex"), 1);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, normalTexts[0]);
    glUniform1i(glGetUniformLocation(shader->GetProgram(), "bumpTex"), 2);
    renderer->RenderWater();
}
