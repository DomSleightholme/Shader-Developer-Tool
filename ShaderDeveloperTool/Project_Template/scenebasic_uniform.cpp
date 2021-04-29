#include "scenebasic_uniform.h"
#include "helper/texture.h"

//ImGUI
#include "imGUI/imgui.h"
#include "imGUI/imgui_impl_opengl3.h"
#include "imGUI/imgui_impl_glfw.h"

#include <sstream>
#include <iostream>
using std::cerr;
using std::endl;

#include <glm/gtc/matrix_transform.hpp>
using glm::vec3;
using glm::mat4;
using glm::vec4;
using glm::mat3;


SceneBasic_Uniform::SceneBasic_Uniform() : plane(30.0f, 30.0f, 100, 100) 
{
    mesh = ObjMesh::load("../Project_Template/media/Trinagle.obj", true);
}

void SceneBasic_Uniform::initScene()
{
    //Load Shaders into Program and Enable Depth Testing
    compile();
    glEnable(GL_DEPTH_TEST);

    //Create View and Projection Matrix's
    view = glm::lookAt(vec3(0.0f, 0.0f, 4.5f), vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 2.0f, 0.0f));
    projection = mat4(1.0f);

    //Lighthing
    prog.setUniform("light.L", vec3(0.8f, 0.8f, 0.8f));
    prog.setUniform("light.La", vec3(0.1f, 0.1f, 0.1f));
    prog.setUniform("light.Position", view * glm::vec4(0.0f, 1.2f, 0.0f +  1.0f, 1.0f));


    //Texturing
    GLuint texID1 = Texture::loadTexture("media/texture/brick1.jpg");
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texID1);
}

void SceneBasic_Uniform::compile()
{
	try {
		prog.compileShader("shader/basic_uniform.vert");
		prog.compileShader("shader/basic_uniform.frag");
		prog.link();
		prog.use();
	} catch (GLSLProgramException &e) {
		cerr << e.what() << endl;
		exit(EXIT_FAILURE);
	}
}
void SceneBasic_Uniform::update( float t )
{
    //Constantly rotate the cam on the Y axis
    view = glm::rotate(view, glm::radians(0.5f), vec3(0.0f, 1.0f, 0.0f));
}

void SceneBasic_Uniform::render()
{
    //GUI
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();


    {
        ImGui::Begin("hi there");
        ImGui::Text("sadasdfasdf");
        ImGui::End();
    }


    //Clear Color and Depth, use the initial shader program
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    prog.use();

    //Mesh Render
    prog.setUniform("Material.Kd", 0.4f, 0.4f, 0.4f);
    prog.setUniform("Material.Ks", 0.9f, 0.9f, 0.9f);
    prog.setUniform("Material.Ka", 0.5f, 0.5f, 0.5f);
    prog.setUniform("Material.Shininess", 30.0f);
    model = mat4(1.0f);
    setMatrices();
    mesh->render();

    //Ground Render
    prog.setUniform("Material.Kd", 0.1f, 0.1f, 0.1f);
    prog.setUniform("Material.Ks", 0.9f, 0.9f, 0.9f);
    prog.setUniform("Material.Ka", 0.1f, 0.1f, 0.1f);
    prog.setUniform("Material.Shininess", 180.0f);
    model = mat4(1.0f);
    model = glm::translate(model, vec3(0.0f, -1.1f, 0.0f));
    setMatrices();
    plane.render();

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void SceneBasic_Uniform::setMatrices()
{
    //Create a model view matrix
    mat4 mv = view * model;

    //Send the Model Matrix's to the Shaders
    prog.setUniform("ModelViewMatrix", mv);
    prog.setUniform("NormalMatrix", glm::mat3(vec3(mv[0]), vec3(mv[1]), vec3(mv[2])));

    //Calculate the MVP using the ModelViewMatrix and the Projection
    prog.setUniform("MVP", projection * mv);
}
void SceneBasic_Uniform::resize(int w, int h)
{
    glViewport(0, 0, w, h);
    width = w;
    height = h;
    projection = glm::perspective(glm::radians(70.0f), (float)w / h, 0.3f, 100.0f);
}
