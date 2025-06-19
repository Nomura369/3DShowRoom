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

    // 禁用拷貝與賦值
    CBulletHoles(const CBulletHoles&) = delete;
    CBulletHoles& operator=(const CBulletHoles&) = delete;

    // 新增彈孔
    void addBulletHole(GLuint shaderProg, const glm::vec3& pos, float size = 0.5f);

    // 更新所有彈孔，使用攝影機位置（billboard用）
    void update();

    // 繪製所有彈孔
    void draw();

    void setCameraPos(glm::vec3 cameraPos);

private:
    CBulletHoles();
    ~CBulletHoles();

    struct BulletHole {
        CQuad* quad = nullptr;
        glm::vec3 pos;
        float size;
        float rotationY; // Y軸旋轉角度 (朝向攝影機)
    };

    std::vector<BulletHole> holes;

    glm::vec3 _cameraPos;
};