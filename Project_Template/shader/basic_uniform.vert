#version 460

//Input Variables
layout (location = 0) in vec3 VertexPosition; 
layout (location = 1) in vec3 VertexNormal; 
layout (location = 2) in vec2 VertexTexCoord;

//Send the data in these variables to the fragment shader
out vec3 Position;
out vec3 Normal;
out vec2 TexCoord;

//Uniform variables, data passed through from the SetMatrices() method
uniform mat4 ModelViewMatrix;
uniform mat3 NormalMatrix;
uniform mat4 MVP; 			


void main() 
{ 
    Normal = normalize( NormalMatrix * VertexNormal);
    //Assigning the position using the data passed into the uniforms
    Position = (ModelViewMatrix * vec4(VertexPosition,1.0)).xyz;

    //Used to calculate the texture coordinates
    TexCoord = VertexTexCoord;

    //Assign the position using the MVP data passed in
    gl_Position = MVP * vec4(VertexPosition,1.0);
} 
