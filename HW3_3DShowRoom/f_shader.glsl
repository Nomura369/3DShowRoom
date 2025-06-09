#version 330 core
#define MAX_LIGHT_NUM 6

in vec3 vColor;
in vec3 vNormal;      // 頂點的法向量 (N)
in vec3 vLight;       // 頂點指向光源的 Light(L) 向量
in vec3 vView;        // 頂點指向 view 的 view(V) 向量
in vec3 v3Pos;

uniform int  uShadingMode;     // 1=頂點色, 2=物件色, 3=Per-Pixel
uniform vec4 ui4Color;         // 模型顏色

struct LightSource {
    vec3 position;
    vec4 ambient;
    vec4 diffuse;
    vec4 specular;
    float constant;
    float linear;
    float quadratic;
    //  Spot Light 專用
    vec3 direction;
    float cutOff;
    float outerCutOff;
    float exponent;
};

uniform int uLightNum;
uniform LightSource uLight[MAX_LIGHT_NUM]; // 為多盞燈光預留空間

struct Material {
    vec4 ambient;   // ka
    vec4 diffuse;   // kd
    vec4 specular;  // ks
    float shininess;
};
uniform Material uMaterial;
out vec4 FragColor;

uniform int uIsNpr; // 是否將照明切換成卡通風格

void main() {
    // 模式切換：1 = 頂點顏色、2 = 模型顏色，3 = Per Pixel Lighting
    if( uShadingMode == 1) { FragColor = vec4(vColor, 1.0);  return; }
    if( uShadingMode == 2 ){ FragColor = ui4Color; return; }

    // 初始化燈光數值，以便進行累加
    vec4 ambient = vec4(0.0);
    vec4 diffuse = vec4(0.0);
    vec4 specular = vec4(0.0);
    
    // 3 = Per-Pixel Phong
    vec3 N = normalize(vNormal);
    vec3 L;
    vec3 V = normalize(vView);
    vec3 R;

    for(int i = 0; i < uLightNum; i++){ // 累加多盞照明的效果
        // 3 = Per-Pixel Phong
        L = normalize(uLight[i].position - v3Pos);
        R = reflect(-L, N);

        // Attenuation
        float dist = length(uLight[i].position - v3Pos);
        float atten = 1.0 / (uLight[i].constant
                           + uLight[i].linear   * dist
                           + uLight[i].quadratic* dist * dist);
        
        // Spot Light 漸變（如有設定 cutOff > 0）
        if(uLight[i].cutOff > 0.0) {
            float theta = dot(L, normalize(-uLight[i].direction));
            
            float intensity;
            if (uIsNpr != 0) {
                // 只有落在 cutOff 範圍內才有光
                intensity = theta > uLight[i].cutOff ? 1.0 : 0.0;
            } 
            else {
                // 原本的柔邊計算
                intensity = clamp(
                    (theta - uLight[i].outerCutOff) /
                    (uLight[i].cutOff - uLight[i].outerCutOff),
                    0.0, 1.0
                );
                if (abs(uLight[i].exponent - 1.0) >= 0.001){
                    intensity = pow(intensity, uLight[i].exponent);
                }
            }
        atten *= intensity;
        }
        
        
        // Ambient
        ambient += uLight[i].ambient * uMaterial.ambient * atten;

        // Diffuse
        float diff = max(dot(N, L), 0.0);
        if(uIsNpr != 0){
            float levels = 3.0;
            float d = floor(diff*levels)/levels;  // 向下取整數
            diffuse += uLight[i].diffuse * d * uMaterial.diffuse * atten;
        } 
        else diffuse += uLight[i].diffuse * diff * uMaterial.diffuse * atten;

        // Specular
        float spec;
        if(uIsNpr != 0){  // quantized specular：只在高亮區顯示
            float specAngle = max(dot(R,V),0.0);
            spec = specAngle > 0.9 ? 1.0 : 0.0;
        } 
        else spec = pow(max(dot(R, V), 0.0), uMaterial.shininess);
        specular += uLight[i].specular * spec * uMaterial.specular * atten;
    }

    vec4 color = ambient + diffuse + specular; // 計算最終顏色

    if(uIsNpr != 0){
         // 簡易輪廓：法線與視線夾角大者畫黑，也能有輪廓線的效果
        float edge = abs(dot(N,V));
        if( edge < 0.2 ){ color = vec4(0.0, 0.0, 0.0, 1.0); }
    }

    color.w = 1.0f;
    FragColor  = color;
}

