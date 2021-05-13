#version 460

//Input Variables
layout (location = 0) in vec3 VertexPosition; 
layout (location = 1) in vec3 VertexNormal; 

//Send the data in these variables to the fragment shader
out vec3 Position;
out vec3 Normal;

//Uniform variables, data passed through from the SetMatrices() method
uniform mat4 ModelViewMatrix;
uniform mat3 NormalMatrix;
uniform mat4 MVP; 			


void main() 
{ 
    Normal = normalize( NormalMatrix * VertexNormal);
    //Assigning the position using the data passed into the uniforms
    Position = (ModelViewMatrix * vec4(VertexPosition,1.0)).xyz;

    //Assign the position using the MVP data passed in
    gl_Position = MVP * vec4(VertexPosition,1.0);
} 
