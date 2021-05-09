#version 460

//Input Variables
layout (location = 0) in vec3 VertexPosition; 
layout (location = 1) in vec3 VertexNormal;

out vec3 VNormal;
out vec3 VPosition;

//Uniform variables, data passed through from the SetMatrices() method
uniform mat4 ModelViewMatrix; 			
uniform mat3 NormalMatix; 			
uniform mat4 MVP; 			

void main() 
{ 
    VNormal = normalize( NormalMatix * VertexNormal);
    VPosition = vec3(ModelViewMatrix * vec4(VertexPosition, 1.0));
    gl_Position = MVP * vec4(VertexPosition, 1.0);
} 
