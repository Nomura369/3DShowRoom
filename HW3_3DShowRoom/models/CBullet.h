// 由 CSphere.h 改動而來
#pragma once

#include "CShape.h"

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
    glm::vec3 _targetPos, _direction;
};
