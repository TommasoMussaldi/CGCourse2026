#version 410
layout(location = 0) out vec4 color;
in vec3 vColor;

uniform vec3 uCol;
void main(void)
{
    if(uCol.x<0)
        color = vec4(vColor, 1.0);
    else
        color = vec4(uCol, 1.0);

}