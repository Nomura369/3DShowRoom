// 由 CSphere.cpp 改動而來
#include <glew/include/GL/glew.h>
#include <glm/gtc/type_ptr.hpp>

#include <cmath>
#include <iostream>

#include "CBullet.h"
#include "../common/typedefs.h"
#include "../common/CBulletHoles.h"


CBullet::CBullet(float radius, GLuint sectors, GLuint stacks, GLuint pattern) : CShape() {
    generateSphere(radius, sectors, stacks, pattern);
    _targetPos = glm::vec3(0.0f, 0.0f, 0.0f);
    _direction = glm::vec3(0.0f, 0.0f, 0.0f);
}

CBullet::~CBullet() {
    glDeleteBuffers(1, &_vbo);
    glDeleteBuffers(1, &_ebo);
    glDeleteVertexArrays(1, &_vao);
    if (_points) delete[] _points;
    if (_idx)    delete[] _idx;
}

void CBullet::draw() {
    glUseProgram(_shaderProg);
    updateMatrix();
    glBindVertexArray(_vao);
    glUniform1i(_shadingModeLoc, _uShadingMode);
    if (_bObjColor) glUniform4fv(_colorLoc, 1, glm::value_ptr(_color));
    glDrawElements(GL_TRIANGLES, _idxCount, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void CBullet::drawRaw() {
    updateMatrix();
    glBindVertexArray(_vao);
    glUniform1i(_shadingModeLoc, _uShadingMode);
    if (_bObjColor) glUniform4fv(_colorLoc, 1, glm::value_ptr(_color));
    glDrawElements(GL_TRIANGLES, _idxCount, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void CBullet::reset() {
    CShape::reset();
}

void CBullet::update(float dt) {
    float speed = 8.0f; // 位移速度
    _pos += _direction * speed * dt;
    setPos(_pos);

    // 限制子彈的位移範圍
    for (auto& wall : _walls) {
        if (wall.checkCollision(_pos, WALL_BULLET)) {
            CBulletHoles::getInstance().addBulletHole(getShaderProgram(), _pos);
            _isActive = false;
            break; // 子彈撞到牆就直接停，不用再繼續檢查
        }
    }
    for (auto & barrier : _floorsAndCeilings) {
        if (barrier.checkCollision(_pos, WALL_BULLET)) {
            // 打到天花板和地板不用生成彈孔
            _isActive = false;
            break; // 子彈撞到牆就直接停，不用再繼續檢查
        }
    }
}

void CBullet::generateSphere(float radius, GLuint sectors, GLuint stacks, GLuint pattern) {
    _vtxCount = (GLuint)(stacks + 1) * (sectors + 1);
    _idxCount = (GLuint)stacks * sectors * 6;
    _vtxAttrCount = 11;

    _points = new GLfloat[_vtxCount * _vtxAttrCount];
    _idx = new GLuint[_idxCount];

    glm::vec4 topColor, bottomColor;
    switch (pattern) {
    case 1:
    default:
        topColor = glm::vec4(0.00f, 0.50f, 0.70f, 1.0f);
        bottomColor = glm::vec4(0.30f, 0.80f, 0.90f, 1.0f);
        break;
    case 2:
        topColor = glm::vec4(1.00f, 0.50f, 0.20f, 1.0f);
        bottomColor = glm::vec4(1.00f, 0.80f, 0.40f, 1.0f);
        break;
    case 3:
        topColor = glm::vec4(0.50f, 0.20f, 0.70f, 1.0f);
        bottomColor = glm::vec4(0.90f, 0.40f, 0.90f, 1.0f);
        break;
    case 4:
        topColor = glm::vec4(0.20f, 0.60f, 0.20f, 1.0f);
        bottomColor = glm::vec4(0.70f, 0.90f, 0.40f, 1.0f);
        break;
    case 5:
        topColor = glm::vec4(0.00f, 0.50f, 0.70f, 1.0f);
        bottomColor = glm::vec4(1.00f, 0.80f, 0.40f, 1.0f);
        break;
    case 6:
        topColor = glm::vec4(0.90f, 0.40f, 0.90f, 1.0f);
        bottomColor = glm::vec4(0.20f, 0.60f, 0.20f, 1.0f);
        break;
    }

    float sectorStep = 2.0f * M_PI / sectors;
    float stackStep = M_PI / stacks;

    GLuint vi = 0;
    for (unsigned int i = 0; i <= stacks; ++i) {
        float v = float(i) / stacks;
        float stackAngle = M_PI / 2.0f - i * stackStep;
        float xy = radius * cosf(stackAngle);
        float y = radius * sinf(stackAngle);  // Y 軸為垂直軸

        glm::vec4 color = glm::mix(topColor, bottomColor, v);

        for (unsigned int j = 0; j <= sectors; ++j) {
            float sectorAngle = j * sectorStep;
            float x = xy * cosf(sectorAngle);
            float z = -xy * sinf(sectorAngle);  // OpenGL RHS，逆時針方向

            glm::vec3 pos(x, y, z);
            glm::vec3 normal = glm::normalize(pos);
            glm::vec2 texCoords = glm::vec2((float)j / sectors, v);

            _points[vi++] = pos.x;
            _points[vi++] = pos.y;
            _points[vi++] = pos.z;
            _points[vi++] = color.r;
            _points[vi++] = color.g;
            _points[vi++] = color.b;
            _points[vi++] = normal.x;
            _points[vi++] = normal.y;
            _points[vi++] = normal.z;
            _points[vi++] = texCoords.s;
            _points[vi++] = texCoords.t;
        }
    }

    GLuint ii = 0;
    for (unsigned int i = 0; i < stacks; ++i) {
        for (unsigned int j = 0; j < sectors; ++j) {
            unsigned int first = i * (sectors + 1) + j;
            unsigned int second = first + sectors + 1;

            _idx[ii++] = first;
            _idx[ii++] = second;
            _idx[ii++] = first + 1;

            _idx[ii++] = first + 1;
            _idx[ii++] = second;
            _idx[ii++] = second + 1;
        }
    }
}

void CBullet::setTargetPos(glm::vec3 targetPos) {
    _targetPos = targetPos;
}

void CBullet::updateDirection() {
    _direction = glm::normalize(_targetPos - _pos);
}