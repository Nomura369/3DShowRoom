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

private:
    void generateSphere(float radius, GLuint sectors, GLuint stacks, GLuint pattern);
    glm::vec3 _targetPos, _direction, _userPos;
    
    AABB _walls[24] = {
        // Room 0
        { glm::vec3(-15.0f, -0.1f, 13.45f), glm::vec3(15.0f, 11.95f, 16.45f) },  // front (Z=14.95±1.5)
        { glm::vec3(13.45f, -0.1f, -15.0f), glm::vec3(16.45f, 11.95f, 15.0f) },  // right (X=14.95±1.5)
        { glm::vec3(-15.0f, -0.1f, -16.45f), glm::vec3(15.0f, 11.95f, -13.45f) },// back  (Z=-14.95±1.5)
        { glm::vec3(-16.45f, -0.1f, -15.0f), glm::vec3(-13.45f, 11.95f, 15.0f) },// left  (X=-14.95±1.5)

        // Room 1
        { glm::vec3(-45.0f, -0.1f, 13.45f), glm::vec3(-15.0f, 11.95f, 16.45f) },
        { glm::vec3(-16.45f, -0.1f, -15.0f), glm::vec3(-13.45f, 11.95f, 15.0f) },
        { glm::vec3(-45.0f, -0.1f, -16.45f), glm::vec3(-15.0f, 11.95f, -13.45f) },
        { glm::vec3(-46.45f, -0.1f, -15.0f), glm::vec3(-43.45f, 11.95f, 15.0f) },

        // Room 2
        { glm::vec3(-15.0f, -0.1f, -16.45f), glm::vec3(15.0f, 11.95f, -13.45f) },
        { glm::vec3(13.45f, -0.1f, -45.0f), glm::vec3(16.45f, 11.95f, -15.0f) },
        { glm::vec3(-15.0f, -0.1f, -46.45f), glm::vec3(15.0f, 11.95f, -43.45f) },
        { glm::vec3(-16.45f, -0.1f, -45.0f), glm::vec3(-13.45f, 11.95f, -15.0f) },

        // Room 3
        { glm::vec3(-45.0f, -0.1f, -16.45f), glm::vec3(-15.0f, 11.95f, -13.45f) },
        { glm::vec3(-16.45f, -0.1f, -45.0f), glm::vec3(-13.45f, 11.95f, -15.0f) },
        { glm::vec3(-45.0f, -0.1f, -46.45f), glm::vec3(-15.0f, 11.95f, -43.45f) },
        { glm::vec3(-46.45f, -0.1f, -45.0f), glm::vec3(-43.45f, 11.95f, -15.0f) },

        // Room 4
        { glm::vec3(-15.0f, -0.1f, -46.45f), glm::vec3(15.0f, 11.95f, -43.45f) },
        { glm::vec3(13.45f, -0.1f, -75.0f), glm::vec3(16.45f, 11.95f, -45.0f) },
        { glm::vec3(-15.0f, -0.1f, -76.45f), glm::vec3(15.0f, 11.95f, -73.45f) },
        { glm::vec3(-16.45f, -0.1f, -75.0f), glm::vec3(-13.45f, 11.95f, -45.0f) },

        // Room 5
        { glm::vec3(-45.0f, -0.1f, -46.45f), glm::vec3(-15.0f, 11.95f, -43.45f) },
        { glm::vec3(-16.45f, -0.1f, -75.0f), glm::vec3(-13.45f, 11.95f, -45.0f) },
        { glm::vec3(-45.0f, -0.1f, -76.45f), glm::vec3(-15.0f, 11.95f, -73.45f) },
        { glm::vec3(-46.45f, -0.1f, -75.0f), glm::vec3(-43.45f, 11.95f, -45.0f) },
    };

    AABB _floorsAndCeilings[12] = {
        // Room 0 地板
        { glm::vec3(-15.0f, -0.15f, -15.0f), glm::vec3(15.0f, -0.05f, 15.0f) },
        // Room 0 天花板
        { glm::vec3(-15.0f, 11.9f, -15.0f), glm::vec3(15.0f, 12.0f, 15.0f) },

        // Room 1 地板
        { glm::vec3(-45.0f, -0.15f, -15.0f), glm::vec3(-15.0f, -0.05f, 15.0f) },
        // Room 1 天花板
        { glm::vec3(-45.0f, 11.9f, -15.0f), glm::vec3(-15.0f, 12.0f, 15.0f) },

        // Room 2 地板
        { glm::vec3(-15.0f, -0.15f, -45.0f), glm::vec3(15.0f, -0.05f, -15.0f) },
        // Room 2 天花板
        { glm::vec3(-15.0f, 11.9f, -45.0f), glm::vec3(15.0f, 12.0f, -15.0f) },

        // Room 3 地板
        { glm::vec3(-45.0f, -0.15f, -45.0f), glm::vec3(-15.0f, -0.05f, -15.0f) },
        // Room 3 天花板
        { glm::vec3(-45.0f, 11.9f, -45.0f), glm::vec3(-15.0f, 12.0f, -15.0f) },

        // Room 4 地板
        { glm::vec3(-15.0f, -0.15f, -75.0f), glm::vec3(15.0f, -0.05f, -45.0f) },
        // Room 4 天花板
        { glm::vec3(-15.0f, 11.9f, -75.0f), glm::vec3(15.0f, 12.0f, -45.0f) },

        // Room 5 地板
        { glm::vec3(-45.0f, -0.15f, -75.0f), glm::vec3(-15.0f, -0.05f, -45.0f) },
        // Room 5 天花板
        { glm::vec3(-45.0f, 11.9f, -75.0f), glm::vec3(-15.0f, 12.0f, -45.0f) },
    };
};
