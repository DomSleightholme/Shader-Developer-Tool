#ifndef SCENEBASIC_UNIFORM_H
#define SCENEBASIC_UNIFORM_H

#include "helper/scene.h"

#include <glad/glad.h>
#include "helper/glslprogram.h"

#include "helper/plane.h"
#include "helper/objmesh.h"
#include "helper/skybox.h"

#include <glm/glm.hpp>

class SceneBasic_Uniform : public Scene
{
private:
    //Shaders
    GLSLProgram prog;
    GLSLProgram skyShader;
    GLSLProgram edgeShader;
    GLSLProgram wireframeShader;
    GLSLProgram waterShader;

    //Mesh Loader
    std::unique_ptr<ObjMesh> Triangle;
    std::unique_ptr<ObjMesh> Ground;
    std::unique_ptr<ObjMesh> Car;
    std::unique_ptr<ObjMesh> Gun;
    std::unique_ptr<ObjMesh> R2D2;

    //Skybox
    SkyBox sky;

    //Edge Detection
    float rotationAmount;
    void initEdgeDetection();
    void setupFBO();
    void pass1();
    void pass2();
    GLuint fsQuad, fboHandle, renderTex;

    //Wireframe
    void setupWireframe();
    glm::mat4 viewport;

    //Water / Lava
    void liquidAnimation();
    Plane plane;
    float time;

    void compile();

public:
    SceneBasic_Uniform();

    void initScene();
    void update(float t);
    void render();
    void resize(int, int);

    void setMatrices(GLSLProgram& p);
    void setSkyBox();

    //Texturing
    void getTextures();

    //GUI
    void renderGUI();
    void ShaderInfo(int shaderType);

    //Image Processing

};

#endif // SCENEBASIC_UNIFORM_H
