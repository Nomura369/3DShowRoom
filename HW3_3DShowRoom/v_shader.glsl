// vshader.glsl -> combine phong and npr
#version 330 core

layout(location=0) in vec3 aPos;
layout(location=1) in vec3 aColor;
layout(location=2) in vec3 aNormal;
layout(location=3) in vec2 aTex;
layout(location=4) in vec2 aLightMapTex;
layout(location=5) in vec3 aTangent;
layout(location=6) in vec3 aBitangent;

uniform mat4 mxModel;
uniform mat4 mxView;
uniform mat4 mxProj;

uniform vec3 viewPos;
uniform vec3 lightPos;

out vec3 vColor;
out vec3 v3Pos;
out vec3 vNormal;
out vec3 vLight;     // NPR 專用
out vec3 vView;      // NPR 專用
out vec3 vTangent;
out vec3 vBitangent;
out vec2 vTexCoord; // 輸出貼圖座標
out vec2 vLightMapUV;

void main() {
    vec4 worldPos = mxModel * vec4(aPos, 1.0);
    v3Pos = worldPos.xyz;

    mat3 normalM = mat3(mxModel);
    vNormal = normalize(normalM * aNormal); // vNormal = normalize((mat3(mxModel) * aNormal));
    vTangent = normalize(normalM * aTangent);
    vBitangent = normalize(normalM * aBitangent);

    vColor = aColor;
    vTexCoord = aTex;
    vLightMapUV = aLightMapTex;

    // NPR 專用
    vLight = normalize(lightPos - v3Pos);
    vView  = normalize(viewPos - v3Pos);

    gl_Position = mxProj * mxView * worldPos;
}