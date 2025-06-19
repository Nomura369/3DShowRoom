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

    // �p��¦V��v������V
    glm::vec3 direction = glm::normalize(_cameraPos - pos);
    glm::vec3 offsetPos = pos + direction * 0.5f; // �קK�u�նK�i���
    bh.pos = offsetPos;

    bh.quad->setShaderID(shaderProg);
    bh.quad->setupVertexAttributes();
    bh.quad->setColor(glm::vec4(0.1f, 0.1f, 0.1f, 1.0f)); // �`�Ǽu�զ�

    holes.push_back(bh);
}


void CBulletHoles::update() {
    for (auto& bh : holes) {
        if (bh.quad != nullptr) {
            // �ϥ� glm::lookAt ���ʹ¦V��v��������
            glm::mat4 lookMat = glm::inverse(glm::lookAt(
                bh.pos,
                _cameraPos,
                glm::vec3(0, 1, 0) // �W��V�A�q�`�ϥ� Y �b
            ));

            // �զX���� model matrix�Gtranslate �� rotate �� scale
            glm::mat4 model =
                glm::translate(glm::mat4(1.0f), bh.pos) *
                glm::mat4(glm::mat3(lookMat)) * // �h�� lookMat ���첾����
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

CBulletHoles::~CBulletHoles() { // ����귽
    for (auto& bh : holes) {
        if (bh.quad != nullptr) delete bh.quad;
    }
}
