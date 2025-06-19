// 由 CSphere.h 改動而來
#pragma once

#include "CShape.h"
#include "../common/AABB.h"

class CBullet : public CShape {
public:
    // radius：球半徑。 sectors：經線(縱向)分段數。 stacks：緯線(橫向)分段數。
    // pattern：顏色選擇 (1–4)
    CBullet(float radius = 1.0f, GLuint sectors = 10, GLuint stacks = 10, GLuint pattern = 4);
    virtual ~CBullet();

    virtual void draw() override;
    virtual void drawRaw() override;
    virtual void reset() override;
    virtual void update(float dt) override;

    void setTargetPos(glm::vec3 targetPos);
    void updateDirection();
    void setIsActive(bool isActive);
    bool getIsActive();

private:
    void generateSphere(float radius, GLuint sectors, GLuint stacks, GLuint pattern);
    glm::vec3 _targetPos, _direction, _userPos;
    bool _isActive;
    
    AABB walls[24] = {
        // Room 0
        { glm::vec3(-15.0f, -0.05f, 14.90f), glm::vec3(15.0f, 11.95f, 15.00f) }, // front
        { glm::vec3(14.90f, -0.05f, -15.0f), glm::vec3(15.00f, 11.95f, 15.0f) }, // right
        { glm::vec3(-15.0f, -0.05f, -15.00f), glm::vec3(15.0f, 11.95f, -14.90f) }, // back
        { glm::vec3(-15.00f, -0.05f, -15.0f), glm::vec3(-14.90f, 11.95f, 15.0f) }, // left

        // Room 1
        { glm::vec3(-45.0f, -0.05f, 14.90f), glm::vec3(-15.0f, 11.95f, 15.00f) },
        { glm::vec3(-15.10f, -0.05f, -15.0f), glm::vec3(-15.00f, 11.95f, 15.0f) },
        { glm::vec3(-45.0f, -0.05f, -15.00f), glm::vec3(-15.0f, 11.95f, -14.90f) },
        { glm::vec3(-45.00f, -0.05f, -15.0f), glm::vec3(-44.90f, 11.95f, 15.0f) },

        // Room 2
        { glm::vec3(-15.0f, -0.05f, -15.10f), glm::vec3(15.0f, 11.95f, -15.00f) },
        { glm::vec3(14.90f, -0.05f, -45.0f), glm::vec3(15.00f, 11.95f, -15.0f) },
        { glm::vec3(-15.0f, -0.05f, -45.00f), glm::vec3(15.0f, 11.95f, -44.90f) },
        { glm::vec3(-15.00f, -0.05f, -45.0f), glm::vec3(-14.90f, 11.95f, -15.0f) },

        // Room 3
        { glm::vec3(-45.0f, -0.05f, -15.10f), glm::vec3(-15.0f, 11.95f, -15.00f) },
        { glm::vec3(-15.10f, -0.05f, -45.0f), glm::vec3(-15.00f, 11.95f, -15.0f) },
        { glm::vec3(-45.0f, -0.05f, -45.00f), glm::vec3(-15.0f, 11.95f, -44.90f) },
        { glm::vec3(-45.00f, -0.05f, -45.0f), glm::vec3(-44.90f, 11.95f, -15.0f) },

        // Room 4
        { glm::vec3(-15.0f, -0.05f, -45.10f), glm::vec3(15.0f, 11.95f, -45.00f) },
        { glm::vec3(14.90f, -0.05f, -75.0f), glm::vec3(15.00f, 11.95f, -45.0f) },
        { glm::vec3(-15.0f, -0.05f, -75.00f), glm::vec3(15.0f, 11.95f, -74.90f) },
        { glm::vec3(-15.00f, -0.05f, -75.0f), glm::vec3(-14.90f, 11.95f, -45.0f) },

        // Room 5
        { glm::vec3(-45.0f, -0.05f, -45.10f), glm::vec3(-15.0f, 11.95f, -45.00f) },
        { glm::vec3(-15.10f, -0.05f, -75.0f), glm::vec3(-15.00f, 11.95f, -45.0f) },
        { glm::vec3(-45.0f, -0.05f, -75.00f), glm::vec3(-15.0f, 11.95f, -74.90f) },
        { glm::vec3(-45.00f, -0.05f, -75.0f), glm::vec3(-44.90f, 11.95f, -45.0f) },
    };
};
