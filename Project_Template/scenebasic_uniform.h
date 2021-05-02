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

    //Mesh Loader
    Plane plane;
    std::unique_ptr<ObjMesh> mesh;

    //Skybox
    SkyBox sky;

    void compile();

public:
    SceneBasic_Uniform();

    void initScene();
    void update(float t);
    void render();
    void resize(int, int);

    void setMatrices();
    void renderGUI();
    void setSkyBox();
    void ShaderInfo(int shaderType);
};

#endif // SCENEBASIC_UNIFORM_H
