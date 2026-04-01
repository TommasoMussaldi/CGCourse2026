#version 430 core 
layout (location = 0) in vec3 aPosition; 

out vec2 p;

void main(void) 
{ 
    gl_Position = vec4(aPosition, 1.0); 
    p = aPosition.xy;
}