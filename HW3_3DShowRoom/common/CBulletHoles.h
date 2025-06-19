#pragma once
#include <vector>
#include <glm/glm.hpp>
#include "../models/CQuad.h"

class CBulletHoles {
public:
    static CBulletHoles& getInstance() {
        static CBulletHoles instance;
        return instance;
    }

    // �T�Ϋ����P���
    CBulletHoles(const CBulletHoles&) = delete;
    CBulletHoles& operator=(const CBulletHoles&) = delete;

    // �s�W�u��
    void addBulletHole(GLuint shaderProg, const glm::vec3& pos, float size = 0.5f);

    // ��s�Ҧ��u�աA�ϥ���v����m�]billboard�Ρ^
    void update();

    // ø�s�Ҧ��u��
    void draw();

    void setCameraPos(glm::vec3 cameraPos);

private:
    CBulletHoles();
    ~CBulletHoles();

    struct BulletHole {
        CQuad* quad = nullptr;
        glm::vec3 pos;
        float size;
        float rotationY; // Y�b���ਤ�� (�¦V��v��)
    };

    std::vector<BulletHole> holes;

    glm::vec3 _cameraPos;
};