#version 430 core  
out vec4 color; 

in vec2 p;

uniform vec2 uPos;
uniform float uR;
uniform float uIso;

float field_p(vec2 center){
	float r = distance(p, center);

	float f = 2*r*r*r/(uR*uR*uR)-3*r*r/(uR*uR)+1;	

	if(r > uR) f = 0;

	return f;
}

float field(){
	return field_p(vec2(0,0)) + field_p(uPos);
}
void main(void) 
{		 
	float f = field();	
	
	if(f < uIso+0.005 && f > uIso-0.005 )
		color = vec4(1.0-abs(f-uIso)/0.01, 0, 0, 1);
		else
	if(f < uIso*2+0.005 && f > uIso*2-0.005 )
		color = vec4(0,0,1.0-abs(f-uIso*2)/0.01, 1);
		else
	if(f < uIso*4+0.005 && f > uIso*4-0.005 )
		color = vec4(0,1.0-abs(f-uIso*4)/0.01,0, 1);
	else
		color = vec4(f, f, f, 1);
} 