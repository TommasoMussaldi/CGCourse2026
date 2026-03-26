#version 430 core  
out vec4 color; 
in vec3 vColor;
in vec3 vPosVS;
in vec3 vNormalVS;
in vec3 vLDirVS;

uniform vec3 uLDir;
uniform vec3 uColor;

uniform int  uShadingMode;
uniform vec3 uDiffuseColor;
uniform vec3 uAmbientColor;
uniform vec3 uSpecularColor;
uniform vec3 uEmissiveColor;
uniform vec3 uLightColor;
uniform float uShininess; 
uniform float uRoughness; // Aggiunta uniform mancante

/* phong lighting */
vec3 phong ( vec3 L, vec3 V, vec3 N){
    float LN = max(0.0, dot(L, N));
    vec3 R = reflect(-L, N); // Più efficiente della formula manuale

    float spec = ((LN > 0.0) ? 1.0 : 0.0) * max(0.0, pow(dot(V, R), uShininess));

    return (uAmbientColor + LN * uDiffuseColor + spec * uSpecularColor) * uLightColor;
}

/* Cook-Torrance: Termine D (Beckmann) */
float D_torrance(vec3 N, vec3 H, float m) {
    float NdotH = max(dot(N, H), 0.0001); // Evitiamo 0
    float m2 = m * m;
    float NdotH2 = NdotH * NdotH;
    float NdotH4 = NdotH2 * NdotH2;
    
    float exponent = -(1.0 - NdotH2) / (NdotH2 * m2);
    return exp(exponent) / (3.14159265 * m2 * NdotH4);
}

/* Cook-Torrance: Termine G (Geometric Attenuation) */
float G_torrance(vec3 N, vec3 H, vec3 V, vec3 L){
    float NdotH = max(dot(N, H), 0.0);
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float VdotH = max(dot(V, H), 0.0001);

    float G1 = (2.0 * NdotH * NdotV) / VdotH;
    float G2 = (2.0 * NdotH * NdotL) / VdotH;
    
    return min(1.0, min(G1, G2)); // Corretta sintassi min
}

/* Cook-Torrance: Termine F (Fresnel Schlick) */
vec3 F_torrance(vec3 V, vec3 H, vec3 F0) {
    float VdotH = max(dot(V, H), 0.0);
    return F0 + (1.0 - F0) * pow(1.0 - VdotH, 5.0);
}

vec3 cook_torrance(vec3 L, vec3 V, vec3 N){ 
    vec3 H = normalize(L + V);
    
    // Calcolo dei termini
    float D = D_torrance(N, H, uRoughness);
    float G = G_torrance(N, H, V, L);
    vec3  F = F_torrance(V, H, uSpecularColor); // F è vec3
    
    float NdotL = max(dot(N, L), 0.0);
    float NdotV = max(dot(N, V), 0.0);

    // Formula Speculare Cook-Torrance completa
    // Denominatore: 4 * (N.L) * (N.V)
    vec3 numerator = D * G * F;
    float denominator = 4.0 * NdotL * NdotV + 0.0001; 
    
    vec3 specular = numerator / denominator;
    
    // Di solito si aggiunge una componente diffusa (Lambert) per realismo
    vec3 diffuse = uDiffuseColor * NdotL;

    return (uAmbientColor + diffuse + specular) * uLightColor;
}

void main(void) 
{    
    vec3 L = normalize(vLDirVS);
    vec3 V = normalize(-vPosVS);
    vec3 N = normalize(vNormalVS);

    if(uShadingMode == 1){
        // Flat shading usando derivate spaziali
        vec3 N_flat = normalize(cross(dFdx(vPosVS), dFdy(vPosVS)));
        color = vec4(phong(L, V, N_flat), 1.0);
    }
    else if(uShadingMode == 2){
        color = vec4(vColor, 1.0);
    }
    else if(uShadingMode == 3){
        color = vec4(phong(L, V, N), 1.0);
    }
    else if(uShadingMode == 4){
        color = vec4(cook_torrance(L, V, N), 1.0);
    }
    else {
        // Default: Visualizzazione normali
        color = vec4(N * 0.5 + 0.5, 1.0);
    }
}