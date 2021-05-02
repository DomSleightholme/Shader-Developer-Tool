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

//Cam coords
float camX;
float camY;
float camZ;

//Texturing
GLuint forestTex;
GLuint pisaTex;
GLuint LavaTex;

bool rotate;

//ImGUI
static int shaderIndex = 0;
const char* ShaderTitle = "text";
const char* ShaderDescription = "text";


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

    forestTex = Texture::loadCubeMap("media/texture/cube/forest/forest");
    pisaTex = Texture::loadCubeMap("media/texture/cube/pisa/pisa");
    LavaTex = Texture::loadCubeMap("media/texture/cube/LavaWorld/LavaWorld");
}

void SceneBasic_Uniform::compile()
{
	try {
		prog.compileShader("shader/basic_uniform.vert");
		prog.compileShader("shader/basic_uniform.frag");
		prog.link();
		prog.use();

        skyShader.compileShader("shader/Sky_Uniform.vert", GLSLShader::VERTEX);
        skyShader.compileShader("shader/Sky_Uniform.frag", GLSLShader::FRAGMENT);
        skyShader.link();

	} catch (GLSLProgramException &e) {
		cerr << e.what() << endl;
		exit(EXIT_FAILURE);
	}
}
void SceneBasic_Uniform::update( float t )
{
    if (rotate) 
    {
        //Constantly rotate the cam on the Y axis
        view = glm::rotate(view, glm::radians(0.5f), vec3(0.0f, 1.0f, 0.0f));
    }
}

void SceneBasic_Uniform::render()
{
    renderGUI();

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

    setSkyBox();

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void SceneBasic_Uniform::renderGUI() 
{
    //GUI
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    //Frame One
    {
        ImGuiWindowFlags window_flags = 0;
        window_flags |= ImGuiWindowFlags_NoScrollbar;
        window_flags |= ImGuiWindowFlags_MenuBar;
        window_flags |= ImGuiWindowFlags_NoMove;
        window_flags |= ImGuiWindowFlags_NoResize;
        window_flags |= ImGuiWindowFlags_NoNav;
        bool* p_open = NULL;

        ImGui::Begin("Shader Settings", p_open, window_flags);

        //Camera Settings
        ImGui::Text("Camera Coordinates:");
        static float vec4f[3] = { 0.10f, 0.20f, 0.30f};
        ImGui::InputFloat3(": Transform", vec4f);
        ImGui::InputFloat3(": Rotation", vec4f);
        ImGui::Separator();

        ImGui::Columns(2, "", true);
        ImGui::Checkbox("Camera Rotate", &rotate); ImGui::NextColumn();
        if (ImGui::Button("Reset Camera"))
        {
            rotate = false;
            view = glm::lookAt(vec3(0.0f, 0.0f, 4.5f), vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 2.0f, 0.0f));
        }
        ImGui::Columns(1);
        ImGui::Separator();

        //Skybox
        static int item_current_2 = 0;
        ImGui::Combo(": Skybox", &item_current_2, "Wild Forest\0Lava World\0City Landscape\0dddd\0eeee");        
             
        if (item_current_2 == 0) 
        {
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_CUBE_MAP, forestTex);
        }
        if(item_current_2 == 1)
        {
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_CUBE_MAP, LavaTex);
        }
        if (item_current_2 == 2)
        {
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_CUBE_MAP, pisaTex);
        }

        ImGui::End();
    }

    //Frame Two
    {
        ImGuiWindowFlags window_flags = 0;
        window_flags |= ImGuiWindowFlags_NoTitleBar;
        window_flags |= ImGuiWindowFlags_NoScrollbar;
        window_flags |= ImGuiWindowFlags_MenuBar;
        window_flags |= ImGuiWindowFlags_NoMove;
        window_flags |= ImGuiWindowFlags_NoResize;
        window_flags |= ImGuiWindowFlags_NoCollapse;
        window_flags |= ImGuiWindowFlags_NoNav;
        bool* p_open = NULL;

        ImGui::Begin("Shader Information", p_open, window_flags);
        ImGui::Text(ShaderTitle);
        ImGui::Separator();
        ImGui::Text(ShaderDescription);

        ImGui::End();
    }

    //Frame Three
    {
        ShaderInfo(shaderIndex);

        ImGuiWindowFlags window_flags = 0;
        window_flags |= ImGuiWindowFlags_NoTitleBar;
        window_flags |= ImGuiWindowFlags_NoScrollbar;
        window_flags |= ImGuiWindowFlags_MenuBar;
        window_flags |= ImGuiWindowFlags_NoMove;
        window_flags |= ImGuiWindowFlags_NoResize;
        window_flags |= ImGuiWindowFlags_NoCollapse;
        window_flags |= ImGuiWindowFlags_NoNav;
        bool* p_open = NULL;
        ImGui::Begin("Shader Selection", p_open, window_flags);

        ImGui::Text("Shader Type:");
        ImGui::Separator();          
        ImGui::Combo("", &shaderIndex, "Blinn-Phong\0HDR\0HDR with Bloom\0Night Vision\0Edge Dectection\0Guassian Blur\0Spotlight");
        ImGui::Separator();

        ImGui::End();
    }
}

void SceneBasic_Uniform::setSkyBox()
{
    //Start using the Shaders for the Skybox
    skyShader.use();

    //Create a model to calculate the view matrixs, send the results to the Skybox Shader
    model = mat4(1.0f);
    mat4 mv = view * model;
    skyShader.setUniform("MVP", projection * mv);

    //Render Skybox
    sky.render();
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

void SceneBasic_Uniform::ShaderInfo(int shaderType)
{
    if (shaderType == 0) 
    {
        ShaderTitle = "Blinn-Phong Model";
        ShaderDescription = "this is it";
    }
    if (shaderType == 1)
    {
        ShaderTitle = "Phong Model";
        ShaderDescription = "this is it2";
    }
}
