#pragma once
#include <glm/glm.hpp>
#include <glew/include/GL/glew.h>
#include <string>

class CMaterial {
public:
    // 建構子：預設材質色與高光係數
    CMaterial(glm::vec4 ambient = glm::vec4(0.2f), glm::vec4 diffuse = glm::vec4(0.8f),
              glm::vec4 specular = glm::vec4(1.0f), float shininess = 32.0f );
    ~CMaterial();
    void setAmbient(glm::vec4 amb);
    glm::vec4 getAmbient();
    void setDiffuse(glm::vec4 diff);
    glm::vec4 getDiffuse();
    void setSpecular(glm::vec4 spec);
    glm::vec4 getSpecular();
    void setShininess(float shininess);
    float getShininess();
    // 將材質參數上傳到指定 shader program
    // uniformName: GLSL 中對應的 Material struct 名稱，例如 "material"
    void uploadToShader(GLuint shaderProg, std::string uniformName);

    // 自訂 == 運算子 -> 若成員變數的值皆相同即代表兩個物件相等
    bool operator==(const CMaterial& other) const {
        return (_ambient == other._ambient && _diffuse == other._diffuse
                && _specular == other._specular && _shininess == other._shininess);
    }

private:
    glm::vec4 _ambient;
    glm::vec4 _diffuse;
    glm::vec4 _specular;
    float     _shininess;
};