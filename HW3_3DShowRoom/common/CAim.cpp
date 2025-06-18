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
    
    float l = length * 0.5f;  // ���פ@�b
    float t = thick * 0.5f; // �ʲӤ@�b

    // ���I���ǡG�W�B�U�B���B�k�A�C�ӯx�� 4 �ӳ��I
    glm::vec2 rects[4][4] = {
        // �W�G�q (�Vt, t) �� (+t, l)
        { {-t, t}, {+t, t}, {+t, l}, {-t, l} },
        // �U
        { {-t, -l}, {+t, -l}, {+t, -t}, {-t, -t} },
        // ��
        { {-l, -t}, {-t, -t}, {-t, +t}, {-l, +t} },
        // �k
        { {+t, -t}, {+l, -t}, {+l, +t}, {+t, +t} }
    };

    // ��J���I���
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

    // ��J index�]�C 4 �ӳ��I�e 2 �ӤT���Ρ^
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
    //glDeleteBuffers(1, &_vbo);  //������ VBO �P EBO
    //glDeleteBuffers(1, &_ebo);
    //glDeleteVertexArrays(1, &_vao); //�A���� VAO
    //glDeleteProgram(_shaderProg);  //���� shader program
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