#include <glew/include/GL/glew.h>
#include <glm/gtc/type_ptr.hpp>

#include "CAim.h"

CAim::CAim(float length, float thick) : CSprite2D()
{
    _vtxAttrCount = 6; // x, y, z, r, g, b
    _vtxCount = 16;
    _idxCount = 24;
    _points = new GLfloat[_vtxCount * _vtxAttrCount];
    _idx = new GLuint[_idxCount];
    
    float l = length * 0.5f;  // 長度一半
    float t = thick * 0.5f; // 粗細一半

    // 頂點順序：上、下、左、右，每個矩形 4 個頂點
    glm::vec2 rects[4][4] = {
        // 上：從 (–t, t) 到 (+t, l)
        { {-t, t}, {+t, t}, {+t, l}, {-t, l} },
        // 下
        { {-t, -l}, {+t, -l}, {+t, -t}, {-t, -t} },
        // 左
        { {-l, -t}, {-t, -t}, {-t, +t}, {-l, +t} },
        // 右
        { {+t, -t}, {+l, -t}, {+l, +t}, {+t, +t} }
    };

    // 填入頂點資料
    for (int r = 0; r < 4; ++r) {
        for (int i = 0; i < 4; ++i) {
            int idx = (r * 4 + i) * _vtxAttrCount;
            _points[idx + 0] = rects[r][i].x;
            _points[idx + 1] = rects[r][i].y;
            _points[idx + 2] = 0.0f;
            _points[idx + 3] = _color.r;
            _points[idx + 4] = _color.g;
            _points[idx + 5] = _color.b;
        }
    }

    // 填入 index（每 4 個頂點畫 2 個三角形）
    for (int r = 0; r < 4; ++r) {
        int base = r * 4;
        int idxBase = r * 6;
        _idx[idxBase + 0] = base + 0;
        _idx[idxBase + 1] = base + 1;
        _idx[idxBase + 2] = base + 2;
        _idx[idxBase + 3] = base + 2;
        _idx[idxBase + 4] = base + 3;
        _idx[idxBase + 5] = base + 0;
    }
}

CAim::~CAim() {
    //glDeleteBuffers(1, &_vbo);  //先釋放 VBO 與 EBO
    //glDeleteBuffers(1, &_ebo);
    //glDeleteVertexArrays(1, &_vao); //再釋放 VAO
    //glDeleteProgram(_shaderProg);  //釋放 shader program
    //if (_points != NULL) delete[] _points;
    //if (_idx != NULL) delete[] _idx;
}

void CAim::draw() {
    glUseProgram(_shaderProg);
    glBindVertexArray(_vao);
    updateMatrix();
    glUniform1i(_coloringModeLoc, _coloringMode);
    if (_bObjColor) glUniform4fv(_colorLoc, 1, glm::value_ptr(_color));
    glDrawElements(GL_TRIANGLES, _idxCount, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}


void CAim::reset() {
    CSprite2D::reset();
}

void CAim::update(float dt) {
    
}