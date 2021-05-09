#version 460

//Input Variables
layout (location = 0) in vec3 VertexPosition; 
layout (location = 1) in vec3 VertexNormal; 
layout (location = 2) in vec2 VertexTexCoord;

//Send the data in these variables to the fragment shader
out vec4 Position;
out vec3 Normal;
out vec2 TexCoord;

//Animation Time
uniform float Time;

//Wave Parameters
uniform float Freq = 0.5;
uniform float Velocity = 0.5;
uniform float Amp = 0.6;

//Uniform variables, data passed through from the SetMatrices() method
uniform mat4 ModelViewMatrix;
uniform mat3 NormalMatrix;
uniform mat4 MVP; 			


void main() 
{ 
    vec4 pos = vec4(VertexPosition,1.0);

    //translate verteces on y coordinates
    float u = Freq * pos.x - Velocity * Time;
    pos.y = Amp * sin(u);

    //Compute the normal Vector
    vec3 n = vec3(0.0);
    n.xy = normalize(vec2(cos( u ), 1.0));

    //Assigning the position using the data passed into the uniforms
    Position = ModelViewMatrix * pos;
    Normal = NormalMatrix * n;

    //Used to calculate the texture coordinates
    TexCoord = VertexTexCoord;

    //Assign the position using the MVP data passed in
    gl_Position = MVP * pos;
} 
