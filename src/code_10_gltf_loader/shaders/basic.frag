#version 430 core  
out vec4 color; 
in vec3 vColor;
in vec3 vPosVS;
in vec3 vNormalVS;
in vec3 vLDirVS;

uniform vec3 uLDir;
uniform vec3 uColor;

uniform int	 uShadingMode;
uniform vec3 uDiffuseColor;
uniform vec3 uAmbientColor;
uniform vec3 uSpecularColor;
uniform vec3 uEmissiveColor;
uniform vec3 uLightColor;
uniform float uShininess;

uniform float uCT_eta;
uniform float uCT_m;

uniform float uON_ro;
uniform float uON_sigma;

/* phong lighting */
vec3 phong ( vec3 L, vec3 V, vec3 N){
	float LN = max(0.0,dot(L,N));

	vec3 R = -L+2*dot(L,N)*N;

	float spec = ((LN>0.f)?1.f:0.f) * max(0.0,pow(dot(V,R),uShininess));

	return (uAmbientColor+LN*uDiffuseColor + spec * uSpecularColor)*uLightColor;
}

void main(void) 
{    
	if(uShadingMode == 1){
		vec3 N = normalize(cross(dFdx(vPosVS),dFdy(vPosVS)));
		color = vec4(phong(vLDirVS,normalize(-vPosVS),N),1.0);
	}
 	else
	if(uShadingMode == 2){
		color = vec4(vColor,1.0);
	}
 	else
	if(uShadingMode == 3){
		color = vec4(phong(vLDirVS,normalize(-vPosVS),normalize(vNormalVS)),1.0);
	}
	else
	if(uShadingMode == 4){
		color = vec4(phong(vLDirVS,normalize(-vPosVS),normalize(vNormalVS)),1.0);
		vec3 H = normalize((vLDirVS + normalize(-vPosVS)) * 0.5);

		float NH = dot(normalize(vNormalVS),H);

		float alpha = acos(NH);
		float m_2 = uCT_m * uCT_m;
		float tang_alpha = tan(alpha);
		float tang_alpha_2 = tang_alpha * tang_alpha;

		float cosalpha_4 = NH*NH;
		cosalpha_4 = cosalpha_4*cosalpha_4;

		float NL = dot(normalize(vNormalVS),vLDirVS);
		float NV = dot(normalize(vNormalVS),normalize(-vPosVS));
		float VH = dot(normalize(-vPosVS),H);

		float D = 1.f / (m_2*cosalpha_4) * exp( - tang_alpha_2 / m_2);

		float G1 = NH*NL/VH;
		float G2 = NH*NV/VH;
		float G = min(1.f, min(G1,G2));

		float R0 = (1-uCT_eta) / (1+uCT_eta);
		R0 = R0*R0;

		float schlick = R0 + (1.f - R0) * pow( 1.f - NV ,5.f);

		float spec = D*G*schlick / (4.f*NL*NV);

		if( NL < 0.f || NV < 0.f )
			spec = 0.f;

		color = vec4(spec * uSpecularColor * uLightColor,1.0);

		//  color
		//color = vec4(  D*G/(4.0*NV*NL)*vec3(0.95, 0.64, 0.54) *uSpecularColor * uLightColor,1.0);
		
		

//		color = vec4(  spec  *uSpecularColor * uLightColor,1.0);
		 

	}
	else
	if(uShadingMode == 5){
		float NL = dot(normalize(vNormalVS),vLDirVS);
		float NV = dot(normalize(vNormalVS),normalize(-vPosVS));

		float sin_alpha = sqrt(max(1.0 - NL*NL, 0.0));
		float sin_beta  = sqrt(max(1.0 - NV*NV, 0.0));
		float tan_beta  = (NV > 0.01) ? (sin_beta / NV) : 0.00;


		float sigma_2 = uON_sigma*uON_sigma;

		float A = 1.f-0.5*sigma_2/(sigma_2+0.33f);
		float B = 0.45f*sigma_2/(sigma_2+0.09f);

		vec3 Lp = normalize(vLDirVS-NL*vNormalVS);
		vec3 Vp = normalize(normalize(-vPosVS)-NV*vNormalVS);
		float C = max(0.f,dot(Lp,Vp));

		float diff = uON_ro *NL*(A+B*C*sin_alpha*tan_beta);

		if( NL < 0.f || NV < 0.f )
			diff = 0.f;

		color = vec4(diff * uDiffuseColor * uLightColor,1.0);

		//color = vec4(A * uDiffuseColor * uLightColor,1.0);

	}
	else
	/* just output the interpolated vertex normal as color		*/
	/* Note: normal is a vector with values in [-1,-1,-1][1,1,1]*/
	/* and  must be remapped in  in [0,0,0][1,1,1]				*/ 
	color = vec4(normalize(vNormalVS)*0.5+0.5,1.0);
} 