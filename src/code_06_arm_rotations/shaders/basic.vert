#version 460 core 
layout (location = 0) in vec3 aPosition; 
layout (location = 1) in vec3 aColor; 
 
out vec3 vPos;
out vec3 vColor;

uniform mat4 uProj;
uniform mat4 uView;
uniform mat4 uModel;
uniform mat4 uRot;

void main(void) 
{ 
	vColor = aColor;
	vPos = (uView*uRot*uModel*vec4(aPosition, 1.0)).xyz; 
    gl_Position = uProj*uView*uRot*uModel*vec4(aPosition, 1.0); 
}