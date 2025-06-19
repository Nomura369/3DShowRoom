// CBulletHoles.cpp
#include "CBulletHoles.h"
#include <glm/gtc/matrix_transform.hpp>
#include <algorithm>
#include <cmath>
#include <iostream> 

CBulletHoles::CBulletHoles() {
    _cameraPos = glm::vec3(0.0f, 0.0f, 0.0f);
}

void CBulletHoles::addBulletHole(GLuint shaderProg, const glm::vec3& pos, float size) {
    BulletHole bh;
    bh.quad = new CQuad;
    bh.size = size;

    // 計算朝向攝影機的方向
    glm::vec3 direction = glm::normalize(_cameraPos - pos);
    glm::vec3 offsetPos = pos + direction * 0.5f; // 避免彈孔貼進牆壁
    bh.pos = offsetPos;

    bh.quad->setShaderID(shaderProg);
    bh.quad->setupVertexAttributes();
    bh.quad->setColor(glm::vec4(0.1f, 0.1f, 0.1f, 1.0f)); // 深灰彈孔色

    holes.push_back(bh);
}


void CBulletHoles::update() {
    for (auto& bh : holes) {
        if (bh.quad != nullptr) {
            // 使用 glm::lookAt 產生朝向攝影機的旋轉
            glm::mat4 lookMat = glm::inverse(glm::lookAt(
                bh.pos,
                _cameraPos,
                glm::vec3(0, 1, 0) // 上方向，通常使用 Y 軸
            ));

            // 組合完整 model matrix：translate → rotate → scale
            glm::mat4 model =
                glm::translate(glm::mat4(1.0f), bh.pos) *
                glm::mat4(glm::mat3(lookMat)) * // 去除 lookMat 的位移部分
                glm::scale(glm::mat4(1.0f), glm::vec3(bh.size));

            bh.quad->setTransformMatrix(model);
        }
    }
}


void CBulletHoles::draw() {
    for (auto& bh : holes) {
        bh.quad->uploadMaterial();
        bh.quad->uploadTextureFlags();
        bh.quad->drawRaw();
    }
}

void CBulletHoles::setCameraPos(glm::vec3 cameraPos) {
    _cameraPos = cameraPos;
}

CBulletHoles::~CBulletHoles() { // 釋放資源
    for (auto& bh : holes) {
        if (bh.quad != nullptr) delete bh.quad;
    }
}
