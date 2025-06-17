#version 330 core
#define MAX_LIGHT_NUM 8

in vec3 vColor;
in vec3 v3Pos;
in vec3 vNormal;
in vec3 vTangent;
in vec3 vBitangent;
in vec2 vTexCoord;
in vec2 vLightMapUV;

uniform vec3 viewPos;

uniform int uShadingMode;  // 1=頂點色, 2=模型色, 3=Per-Pixel
uniform int uTextureFlags;
uniform int uIsNpr;

uniform vec4 ui4Color;

uniform sampler2D uDiffuseMap;
uniform sampler2D uLightMap;
uniform sampler2D uNormalMap;
uniform samplerCube uEnvMap;

struct LightSource {
    vec3 position;
    vec4 ambient;
    vec4 diffuse;
    vec4 specular;
    float constant;
    float linear;
    float quadratic;
    vec3 direction;
    float cutOff;
    float outerCutOff;
    float exponent;
};
uniform int uLightNum;
uniform LightSource uLight[MAX_LIGHT_NUM];

struct Material {
    vec4 ambient;
    vec4 diffuse;
    vec4 specular;
    float shininess;
};
uniform Material uMaterial;

out vec4 FragColor;

void main() {
    if (uShadingMode == 1) { FragColor = vec4(vColor, 1.0); return; }
    if (uShadingMode == 2) { FragColor = ui4Color; return; }

    vec3 colorRGB = uMaterial.diffuse.rgb;
    float alpha = uMaterial.diffuse.a;

    // Diffuse Map
    if ((uTextureFlags & 1) != 0) {
        vec4 tex = texture(uDiffuseMap, vTexCoord);
        colorRGB *= tex.rgb;
        alpha *= tex.a;
    }

    if (alpha < 0.1) discard;

    vec3 N = normalize(vNormal);
    if ((uTextureFlags & 4) != 0) {
        vec3 nmap = texture(uNormalMap, vTexCoord).rgb * 2.0 - 1.0;
        mat3 TBN = mat3(normalize(vTangent), normalize(vBitangent), normalize(vNormal));
        N = normalize(TBN * nmap);
    }

    vec3 V = normalize(viewPos - v3Pos);

    vec4 ambient = vec4(0.0);
    vec4 diffuse = vec4(0.0);
    vec4 specular = vec4(0.0);

    float spotlightIntensity = 0.0; // 聚光燈的強度

    for (int i = 0; i < uLightNum; i++) {
        vec3 L = normalize(uLight[i].position - v3Pos);
        vec3 R = reflect(-L, N);

        float dist = length(uLight[i].position - v3Pos);
        float atten = 1.0 / (uLight[i].constant + uLight[i].linear * dist + uLight[i].quadratic * dist * dist);

        // Spot Light
        if (uLight[i].cutOff > 0.0) {
            float theta = dot(L, normalize(-uLight[i].direction));
            float intensity = 1.0;

            if (uIsNpr != 0) {
                intensity = theta > uLight[i].cutOff ? 1.0 : 0.0;
            } else {
                intensity = clamp((theta - uLight[i].outerCutOff) / (uLight[i].cutOff - uLight[i].outerCutOff), 0.0, 1.0);
                if (abs(uLight[i].exponent - 1.0) >= 0.001) {
                    intensity = pow(intensity, uLight[i].exponent);
                }
            }

            atten *= intensity;

            if (i == 6 && uLight[i].ambient.a > 0.0) { // spotlight（可開關）的燈光索引為 6
                spotlightIntensity = atten;
            }
        }

        ambient += uLight[i].ambient * uMaterial.ambient * atten;

        float diff = max(dot(N, L), 0.0);
        if (uIsNpr != 0) {
            float levels = 3.0;
            diff = floor(diff * levels) / levels;
        }
        diffuse += uLight[i].diffuse * vec4(colorRGB, 1.0) * diff * atten;

        float spec = 0.0;
        if (uIsNpr != 0) {
            spec = max(dot(R, V), 0.0) > 0.9 ? 1.0 : 0.0;
        } else {
            spec = pow(max(dot(R, V), 0.0), uMaterial.shininess);
        }
        specular += uLight[i].specular * uMaterial.specular * spec * atten;
    }

    vec3 resultRGB = (ambient + diffuse + specular).rgb;

    // 只有被聚光燈照到的地方才顯示 lightmap 圖案
    if ((uTextureFlags & 2) != 0) {
        vec3 lightTex = texture(uLightMap, vLightMapUV).rgb;
        vec3 lightMapColor = vec3(0.7, 0.85, 0.9);

        float patternFade = smoothstep(0.0, 0.3, spotlightIntensity);
        resultRGB += lightMapColor * 0.8 * lightTex * colorRGB * patternFade;
    }

    // 環境貼圖
    if ((uTextureFlags & 8) != 0) {
        float envIntensity = 0.4;
        vec3 reflectVec = reflect(-V, N);
        reflectVec.y = -reflectVec.y;
        vec3 envColor = texture(uEnvMap, reflectVec).rgb;
        resultRGB = mix(resultRGB, envColor, envIntensity);
    }

    // NPR 邊緣描邊
    if (uIsNpr != 0) {
        float edge = abs(dot(N, V));
        if (edge < 0.2) resultRGB = vec3(0.0);
    }

    FragColor = vec4(resultRGB, alpha);
}
