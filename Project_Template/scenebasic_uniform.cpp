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
float fov = 60.0f;

//Texturing
GLuint forestTex;
GLuint pisaTex;
GLuint LavaTex;

GLuint Brick;
GLuint Cement;
GLuint R2Diffuse;
GLuint R2Bump;
GLuint GunTex;
GLuint CarTex;
GLuint Wood;
GLuint LavaAnimation;
GLuint Water;

//GUI Settings
bool rotate;
float rotationSpeed = 1;
float LiquidIndex;
float skyboxIndex = 0;

//ImGUI
static int shaderIndex = 0;
static int modelIndex = 0;
const char* ShaderTitle = "title_text";
const char* ShaderDescription = "description_text";


SceneBasic_Uniform::SceneBasic_Uniform() : rotationAmount(0.0f), time(0), plane(30.0f, 30.0f, 100, 100)
{
    Triangle = ObjMesh::load("../Project_Template/media/Trinagle.obj", true);
    Car = ObjMesh::load("../Project_Template/media/Car.obj", true);
    R2D2 = ObjMesh::load("../Project_Template/media/R2D2.obj", true);
    Gun = ObjMesh::load("../Project_Template/media/1911.obj", true);
    Ground = ObjMesh::load("../Project_Template/media/Platform.obj", true);
}

void SceneBasic_Uniform::initScene()
{
    //Load Shaders into Program and Enable Depth Testing
    compile();
    glEnable(GL_DEPTH_TEST);

    //Create View and Projection Matrix's
    view = glm::lookAt(vec3(0.0f, 0.0f, 4.5f), vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 2.0f, 0.0f));
    projection = mat4(1.0f);

    getTextures();
}

void SceneBasic_Uniform::getTextures() 
{
    //Texturing
    Cement = Texture::loadTexture("media/texture/cement.jpg");
    R2Diffuse = Texture::loadTexture("media/texture/R2_diffuse.png");
    R2Bump = Texture::loadTexture("media/texture/R2_bump.png");
    GunTex = Texture::loadTexture("media/texture/1911tex_2.png");
    CarTex = Texture::loadTexture("media/texture/CarTex.png");
    Wood = Texture::loadTexture("media/texture/Wood.png");
    LavaAnimation = Texture::loadTexture("media/texture/LavaAnimationTex.jpg");
    Water = Texture::loadTexture("media/texture/WaterTex.png");

    //Skybox
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
		
        skyShader.compileShader("shader/Sky_Uniform.vert", GLSLShader::VERTEX);
        skyShader.compileShader("shader/Sky_Uniform.frag", GLSLShader::FRAGMENT);
        skyShader.link();

        edgeShader.compileShader("shader/edgeShader.vert", GLSLShader::VERTEX);
        edgeShader.compileShader("shader/edgeShader.frag", GLSLShader::FRAGMENT);
        edgeShader.link();

        wireframeShader.compileShader("shader/Wireframe.vert");
        wireframeShader.compileShader("shader/Wireframe.frag");
        wireframeShader.compileShader("shader/Wireframe.geom");
        wireframeShader.link();

        waterShader.compileShader("shader/waterShader.vert");
        waterShader.compileShader("shader/waterShader.frag");
        waterShader.link();

	} catch (GLSLProgramException &e) {
		cerr << e.what() << endl;
		exit(EXIT_FAILURE);
	}
}
void SceneBasic_Uniform::update( float t )
{
    renderGUI();

    if (rotate == true)
    {
        rotationAmount = t;
    }

    else 
    {
        rotationAmount = 0;
    }

    time = t;
}

void SceneBasic_Uniform::render()
{
    view = glm::lookAt(vec3(0.0f, 0.0f, 4.5f), vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 2.0f, 0.0f));
    view = glm::rotate(view, glm::radians(30.0f * rotationAmount), vec3(0.0f, 1.0f, 0.0f));

    //Clear Color and Depth, use the initial shader program
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //Mesh Render
    if (shaderIndex == 0) 
    {
        //Send Shader Index to Frag Shader
        prog.use();
        prog.setUniform("ShaderIndex", 0);

        //Lighthing
        prog.setUniform("light.L", vec3(0.8f, 0.8f, 0.8f));
        prog.setUniform("light.La", vec3(0.1f, 0.1f, 0.1f));
        prog.setUniform("light.Position", view * glm::vec4(0.0f, 1.2f, 0.0f + 1.0f, 1.0f));

        if (modelIndex == 0)
        {
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, Wood);

            prog.setUniform("Material.Kd", 0.1f, 0.1f, 0.1f);
            prog.setUniform("Material.Ks", 0.9f, 0.9f, 0.9f);
            prog.setUniform("Material.Ka", 0.5f, 0.5f, 0.5f);
            prog.setUniform("Material.Shininess", 100.0f);
            model = mat4(1.0f);
            setMatrices(prog);
            Triangle->render();
        }
        if (modelIndex == 1)
        {
            //Texturing
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, CarTex);

            //Rendering
            prog.setUniform("Material.Kd", 0.1f, 0.1f, 0.1f);
            prog.setUniform("Material.Ks", 0.9f, 0.9f, 0.9f);
            prog.setUniform("Material.Ka", 0.5f, 0.5f, 0.5f);
            prog.setUniform("Material.Shininess", 30.0f);
            model = mat4(1.0f);
            model = glm::scale(model, vec3(0.25f, 0.25f, 0.25f));
            model = glm::translate(model, vec3(0.0f, -1.0f, 0.0f));
            setMatrices(prog);
            Car->render();
        }
        if (modelIndex == 2)
        {
            //Texturing
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, R2Diffuse);
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, R2Bump);

            //Rendering
            prog.setUniform("Material.Kd", 0.1f, 0.1f, 0.1f);
            prog.setUniform("Material.Ks", 0.9f, 0.9f, 0.9f);
            prog.setUniform("Material.Ka", 0.5f, 0.5f, 0.5f);
            prog.setUniform("Material.Shininess", 30.0f);
            model = mat4(1.0f);
            model = glm::scale(model, vec3(0.3f, 0.3f, 0.3f));
            setMatrices(prog);
            R2D2->render();
        }
        if (modelIndex == 3)
        {
            //Texturing
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, GunTex);

            //Rendering
            prog.setUniform("Material.Kd", 0.1f, 0.1f, 0.1f);
            prog.setUniform("Material.Ks", 0.9f, 0.9f, 0.9f);
            prog.setUniform("Material.Ka", 0.5f, 0.5f, 0.5f);
            prog.setUniform("Material.Shininess", 30.0f);
            model = mat4(1.0f);
            model = glm::scale(model, vec3(0.75f, 0.75f, 0.75f));
            setMatrices(prog);
            Gun->render();
        }

        liquidAnimation();
    }
    if (shaderIndex == 1)
    {
        //Send Shader Index to Frag Shader
        prog.use();
        prog.setUniform("ShaderIndex", 1);

        //Lighthing
        prog.setUniform("Spot.L", vec3(0.9f));
        prog.setUniform("Spot.La", vec3(0.6f));
        prog.setUniform("Spot.Exponent", 10.0f);
        prog.setUniform("Spot.Cutoff", glm::radians(20.5f));

        //Assigning the Spotlight position
        mat4 spotLightPos = glm::lookAt(vec3(1.0f, -0.1f, 0.0f), vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f));
        vec4 lightPos = vec4(0.0f, 0.0f, 1.0f, 0.0f);
        prog.setUniform("Spot.Position", vec3(spotLightPos * lightPos));
        mat3 normalMatrix = mat3(vec3(spotLightPos[0]), vec3(spotLightPos[0]), vec3(spotLightPos[0]));
        prog.setUniform("Spot.Direction", normalMatrix * vec3(-lightPos));

        if (modelIndex == 0)
        {
            //Texturing
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, Wood);

            //Rendering
            prog.setUniform("Material.Kd", 0.4f, 0.4f, 0.4f);
            prog.setUniform("Material.Ks", 0.9f, 0.9f, 0.9f);
            prog.setUniform("Material.Ka", 0.5f, 0.5f, 0.5f);
            prog.setUniform("Material.Shininess", 30.0f);
            model = mat4(1.0f);
            setMatrices(prog);
            Triangle->render();
        }
        if (modelIndex == 1)
        {
            //Texturing
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, CarTex);

            //Rendering
            prog.setUniform("Material.Kd", 0.4f, 0.4f, 0.4f);
            prog.setUniform("Material.Ks", 0.9f, 0.9f, 0.9f);
            prog.setUniform("Material.Ka", 0.5f, 0.5f, 0.5f);
            prog.setUniform("Material.Shininess", 30.0f);
            model = mat4(1.0f);
            model = glm::scale(model, vec3(0.25f, 0.25f, 0.25f));
            model = glm::translate(model, vec3(0.0f, -1.0f, 0.0f));
            setMatrices(prog);
            Car->render();
        }
        if (modelIndex == 2)
        {
            //Texture
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, R2Diffuse);
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, R2Bump);

            //Render
            prog.setUniform("Material.Kd", 0.4f, 0.4f, 0.4f);
            prog.setUniform("Material.Ks", 0.9f, 0.9f, 0.9f);
            prog.setUniform("Material.Ka", 0.5f, 0.5f, 0.5f);
            prog.setUniform("Material.Shininess", 30.0f);
            model = mat4(1.0f);
            model = glm::scale(model, vec3(0.3f, 0.3f, 0.3f));
            setMatrices(prog);
            R2D2->render();
        }
        if (modelIndex == 3)
        {
            //Texturing
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, GunTex);

            //Rendering
            prog.setUniform("Material.Kd", 0.4f, 0.4f, 0.4f);
            prog.setUniform("Material.Ks", 0.9f, 0.9f, 0.9f);
            prog.setUniform("Material.Ka", 0.5f, 0.5f, 0.5f);
            prog.setUniform("Material.Shininess", 30.0f);
            model = mat4(1.0f);
            model = glm::scale(model, vec3(0.75f, 0.75f, 0.75f));
            setMatrices(prog);
            Gun->render();
        }

        //Ground Render
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, Cement);

        prog.setUniform("Material.Kd", 0.1f, 0.1f, 0.1f);
        prog.setUniform("Material.Ks", 0.9f, 0.9f, 0.9f);
        prog.setUniform("Material.Ka", 0.1f, 0.1f, 0.1f);
        prog.setUniform("Material.Shininess", 180.0f);
        model = mat4(1.0f);
        model = glm::translate(model, vec3(0.0f, -3.0f, 0.0f));
        model = glm::scale(model, vec3(0.5f, 0.5f, 0.5f));
        setMatrices(prog);
        Ground->render();

        liquidAnimation();
    }
    if (shaderIndex == 4)
    {
        edgeShader.use();

        initEdgeDetection();
        pass1();
        glFlush();
        pass2();
    }
    if (shaderIndex == 5) 
    {
        wireframeShader.use();
        setupWireframe();

        wireframeShader.setUniform("ViewportMatrix", viewport);

        if (modelIndex == 0)
        {
            //Rendering
            model = mat4(1.0f);
            setMatrices(wireframeShader);
            Triangle->render();
        }
        if (modelIndex == 1)
        {
            //Rendering
            model = mat4(1.0f);
            model = glm::scale(model, vec3(0.25f, 0.25f, 0.25f));
            model = glm::translate(model, vec3(0.0f, -1.0f, 0.0f));
            setMatrices(wireframeShader);
            Car->render();
        }
        if (modelIndex == 2)
        {
            //Render
            model = mat4(1.0f);
            model = glm::scale(model, vec3(0.3f, 0.3f, 0.3f));
            setMatrices(wireframeShader);
            R2D2->render();
        }
        if (modelIndex == 3)
        {
            //Rendering
            model = mat4(1.0f);
            model = glm::scale(model, vec3(0.75f, 0.75f, 0.75f));
            setMatrices(wireframeShader);
            Gun->render();
        }
    }

    setSkyBox();

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void SceneBasic_Uniform::liquidAnimation() 
{
    if (LiquidIndex == 1)
    {
        //Water Render
        waterShader.use();
        waterShader.setUniform("Time", time);
       
        if (skyboxIndex == 1) 
        {
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, LavaAnimation);
        }
        else 
        {
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, Water);
        }

        waterShader.setUniform("light.L", vec3(0.8f, 0.8f, 0.8f));
        waterShader.setUniform("light.La", vec3(0.1f, 0.1f, 0.1f));
        waterShader.setUniform("light.Position", view * glm::vec4(0.0f, 1.2f, 0.0f + 1.0f, 1.0f));

        waterShader.setUniform("Material.Kd", 0.4f, 0.4f, 0.4f);
        waterShader.setUniform("Material.Ks", 0.8f, 0.8f, 0.8f);
        waterShader.setUniform("Material.Ka", 0.2f, 0.5f, 0.9f);
        waterShader.setUniform("Material.Shininess", 10.0f);
        model = mat4(1.0f);
        model = glm::translate(model, vec3(0.0f, -2.0f, 0.0f));
        setMatrices(waterShader);
        plane.render();
   }
}

//GUI
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
        ImGui::Text("Camera Zoom:");
       
        projection = glm::perspective(glm::radians(fov), (float)width / (float)height, 0.1f, 100.0f);
        ImGui::SliderFloat(" ", &fov, 10.0f, 70.0f);   
  
        ImGui::Separator();

        ImGui::Columns(2, "", true);
        ImGui::Checkbox("Camera Rotate", &rotate); ImGui::NextColumn();
        if (ImGui::Button("Reset Camera"))
        {
            rotate = false;
            fov = 60.0f;
            view = glm::lookAt(vec3(0.0f, 0.0f, 4.5f), vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 2.0f, 0.0f));
        }
        ImGui::Columns(1);
        ImGui::Separator();

        //Mesh Selection
        ImGui::Text("Model Selection:");
        ImGui::Combo("", &modelIndex, "Triangle\0Car\0R2-D2\0Gun");       

        //Skybox
        static int item_current_2 = 0;
        ImGui::Text("Skybox Selection:");
        ImGui::Combo("  ", &item_current_2, "Wild Forest\0Lava World\0City Landscape");        
             
        if (item_current_2 == 0) 
        {
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_CUBE_MAP, forestTex);
            skyboxIndex = 0;
        }
        if(item_current_2 == 1)
        {
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_CUBE_MAP, LavaTex);
            skyboxIndex = 1;
        }
        if (item_current_2 == 2)
        {
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_CUBE_MAP, pisaTex);
            skyboxIndex = 2;
        }

        if (shaderIndex == 0 || shaderIndex == 1) 
        {
            static int liquidIndex = 0;
            ImGui::Text("Surface Animation:");
            ImGui::Combo("     ", &liquidIndex, "None\0Water");

            if (liquidIndex == 0)
            {
                LiquidIndex = 0;
            }
            if (liquidIndex == 1)
            {
                LiquidIndex = 1;
            }
        }

        ImGui::Separator();

        ImGui::TextWrapped("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
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
        ImGui::Begin("Shader Selection", p_open, window_flags);

        ImGui::Text("Shader Type:");
        ImGui::Separator();          
        ImGui::Combo("", &shaderIndex, "Blinn-Phong\0Spotlight\0HDR\0Night Vision\0Edge Dectection\0Wireframe");
        ImGui::Separator();
        ShaderInfo(shaderIndex);

        ImGui::End();
    }

    //Frame Three
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
        ImGui::TextWrapped(ShaderDescription);

        ImGui::End();
    }

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
        ShaderTitle = "Spotlight";
        ShaderDescription = "this is it2";
    }
    if (shaderType == 2)
    {
        ShaderTitle = "HDR";
        ShaderDescription = "this is it2";
    }
    if (shaderType == 3)
    {
        ShaderTitle = "Night Vision";
        ShaderDescription = "Using Perlin noise, I have developed a night-vision effect that simulates the idea of looking through night-vision goggles, this effect is rendered in the second pass, our first pass being where I rendered the objects and textures.";
    }
    if (shaderType == 4)
    {
        ShaderTitle = "Edge Detection";
        ShaderDescription = "This effect is an image processing technique that identifies regions of most significant change in the brightness of the image to create a 2D pencil sketch effect on the camera, this effect provides a way to detect the boundaries of objects or changes in topology.";
    }
    if (shaderType == 5)
    {
        ShaderTitle = "Wireframe";
        ShaderDescription = " ";
    }
}

//View, Projection Matrix Set Up
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
void SceneBasic_Uniform::setMatrices(GLSLProgram& p)
{
    //Create a model view matrix
    mat4 mv = view * model;

    //Send the Model Matrix's to the Shaders
    p.setUniform("ModelViewMatrix", mv);
    p.setUniform("NormalMatrix", glm::mat3(vec3(mv[0]), vec3(mv[1]), vec3(mv[2])));

    //Calculate the MVP using the ModelViewMatrix and the Projection
    p.setUniform("MVP", projection * mv);
}

//Edge Detection
void SceneBasic_Uniform::initEdgeDetection()
{
    setupFBO();

    // Array for full-screen quad
    GLfloat verts[] = {
    -1.0f, -1.0f, 0.0f, 1.0f, -1.0f, 0.0f, 1.0f, 1.0f, 0.0f,
    -1.0f, -1.0f, 0.0f, 1.0f, 1.0f, 0.0f, -1.0f, 1.0f, 0.0f
    };
    GLfloat tc[] = {
    0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f,
    0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f
    };

    // Set up the buffers
    unsigned int handle[2];
    glGenBuffers(2, handle);

    glBindBuffer(GL_ARRAY_BUFFER, handle[0]);
    glBufferData(GL_ARRAY_BUFFER, 6 * 3 * sizeof(float), verts, GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, handle[1]);
    glBufferData(GL_ARRAY_BUFFER, 6 * 2 * sizeof(float), tc, GL_STATIC_DRAW);

    // Set up the vertex array object
    glGenVertexArrays(1, &fsQuad);
    glBindVertexArray(fsQuad);

    glBindBuffer(GL_ARRAY_BUFFER, handle[0]);
    glVertexAttribPointer((GLuint)0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0); // Vertex position

    glBindBuffer(GL_ARRAY_BUFFER, handle[1]);
    glVertexAttribPointer((GLuint)2, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(2); // Texture coordinates

    glBindVertexArray(0);

    edgeShader.setUniform("EdgeThreshold", 0.05f);
    edgeShader.setUniform("Light.L", vec3(1.0f));
    edgeShader.setUniform("Light.La", vec3(0.2f));
}
void SceneBasic_Uniform::setupFBO() 
{
    // Generate and bind the framebuffer
    glGenFramebuffers(1, &fboHandle);
    glBindFramebuffer(GL_FRAMEBUFFER, fboHandle);

    // Create the texture object
    glGenTextures(1, &renderTex);
    glBindTexture(GL_TEXTURE_2D, renderTex);
    glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, width, height);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);

    // Bind the texture to the FBO
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
        renderTex, 0);

    // Create the depth buffer
    GLuint depthBuf;
    glGenRenderbuffers(1, &depthBuf);
    glBindRenderbuffer(GL_RENDERBUFFER, depthBuf);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);

    // Bind the depth buffer to the FBO
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
        GL_RENDERBUFFER, depthBuf);

    // Set the targets for the fragment output variables
    GLenum drawBuffers[] = { GL_COLOR_ATTACHMENT0 };
    glDrawBuffers(1, drawBuffers);

    // Unbind the framebuffer, and revert to default framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
void SceneBasic_Uniform::pass1() 
{
    edgeShader.setUniform("Pass", 1);

    glBindFramebuffer(GL_FRAMEBUFFER, fboHandle);
    glEnable(GL_DEPTH_TEST);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    view = glm::lookAt(vec3(0.0f, 0.0f, 4.5f), vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 2.0f, 0.0f));
    view = glm::rotate(view, glm::radians(30.0f * rotationAmount), vec3(0.0f, 1.0f, 0.0f));

    edgeShader.setUniform("Light.Position", vec4(0.0f, 0.0f, 0.0f, 1.0f));
 

    //Texturing
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, GunTex);

    //Rendering
    if (modelIndex == 0)
    {
        //Texturing
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, Wood);

        //Rendering
        edgeShader.setUniform("Material.Kd", 0.4f, 0.4f, 0.4f);
        edgeShader.setUniform("Material.Ks", 0.9f, 0.9f, 0.9f);
        edgeShader.setUniform("Material.Ka", 0.5f, 0.5f, 0.5f);
        edgeShader.setUniform("Material.Shininess", 30.0f);
        model = mat4(1.0f);
        setMatrices(edgeShader);
        Triangle->render();
    }
    if (modelIndex == 1)
    {
        //Texturing
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, CarTex);

        //Rendering
        edgeShader.setUniform("Material.Kd", 0.4f, 0.4f, 0.4f);
        edgeShader.setUniform("Material.Ks", 0.9f, 0.9f, 0.9f);
        edgeShader.setUniform("Material.Ka", 0.5f, 0.5f, 0.5f);
        edgeShader.setUniform("Material.Shininess", 30.0f);
        model = mat4(1.0f);
        model = glm::scale(model, vec3(0.25f, 0.25f, 0.25f));
        model = glm::translate(model, vec3(0.0f, -1.0f, 0.0f));
        setMatrices(edgeShader);
        Car->render();
    }
    if (modelIndex == 2)
    {
        //Texture
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, R2Diffuse);

        //Render
        edgeShader.setUniform("Material.Kd", 0.4f, 0.4f, 0.4f);
        edgeShader.setUniform("Material.Ks", 0.9f, 0.9f, 0.9f);
        edgeShader.setUniform("Material.Ka", 0.5f, 0.5f, 0.5f);
        edgeShader.setUniform("Material.Shininess", 30.0f);
        model = mat4(1.0f);
        model = glm::scale(model, vec3(0.3f, 0.3f, 0.3f));
        setMatrices(edgeShader);
        R2D2->render();
    }
    if (modelIndex == 3)
    {
        //Texturing
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, GunTex);

        //Rendering
        edgeShader.setUniform("Material.Kd", 0.4f, 0.4f, 0.4f);
        edgeShader.setUniform("Material.Ks", 0.9f, 0.9f, 0.9f);
        edgeShader.setUniform("Material.Ka", 0.5f, 0.5f, 0.5f);
        edgeShader.setUniform("Material.Shininess", 30.0f);
        model = mat4(1.0f);
        model = glm::scale(model, vec3(0.75f, 0.75f, 0.75f));
        setMatrices(edgeShader);
        Gun->render();
    }
}
void SceneBasic_Uniform::pass2() 
{
    edgeShader.setUniform("Pass", 2);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, renderTex);

    glDisable(GL_DEPTH_TEST);
    glClear(GL_COLOR_BUFFER_BIT);

    model = mat4(1.0f);
    view = mat4(1.0f);
    projection = mat4(1.0f);
    setMatrices(edgeShader);

    // Render the full-screen quad
    glBindVertexArray(fsQuad);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}

//Wireframe
void SceneBasic_Uniform::setupWireframe() 
{
    //Uniforms
    wireframeShader.setUniform("Line.Width", 2.75f);
    wireframeShader.setUniform("Line.Color", vec4(0.05f, 0.05f, 0.05f, 1.0f));
    wireframeShader.setUniform("Material.Kd", 0.7f, 0.7f, 0.7f);
    wireframeShader.setUniform("Light.Position", vec4(0.0f, 0.0f, 0.0f, 0.0f));
    wireframeShader.setUniform("Material.Ka", 0.2f, 0.2f, 0.2f);
    wireframeShader.setUniform("Light.Intensity", 0.7f, 0.7f, 0.7f);
    wireframeShader.setUniform("Matieral.Ks", 0.8f, 0.0f, 0.8f);
    wireframeShader.setUniform("Material.Shininess", 100.0f);
}

void SceneBasic_Uniform::resize(int w, int h)
{
    glViewport(0, 0, w, h);

    width = w;
    height = h;

    projection = glm::perspective(glm::radians(70.0f), (float)w / h, 0.3f, 100.0f);

    //Wireframe
    float w2 = w / 2.0f;
    float h2 = h / 2.0f;
    viewport = mat4(vec4(w2, 0.0f, 0.0f, 0.0f), vec4(0.0f, h2, 0.0f, 0.0f), vec4(0.0f, 0.0f, 1.0f, 0.0f), vec4(w2 + 0, h2 + 0, 0.0f, 1.0f));
}


