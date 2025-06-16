#include <glew/include/GL/glew.h>
#include <glm/gtc/type_ptr.hpp>

#include <vector>

#include "CBox.h"

CBox::CBox() : CShape() 
{
	setPoints();
}

CBox::~CBox()
{
	glDeleteBuffers(1, &_vbo);  //������ VBO �P EBO
	glDeleteBuffers(1, &_ebo);
	glDeleteVertexArrays(1, &_vao); //�A���� VAO
	if (_points != NULL) delete[] _points;
	if (_idx != NULL) delete[] _idx;
}

void CBox::draw()
{
	glUseProgram(_shaderProg);
	updateMatrix();
	glBindVertexArray(_vao);
	glUniform1i(_shadingModeLoc, _uShadingMode);
	if ( _bObjColor ) glUniform4fv(_colorLoc, 1, glm::value_ptr(_color));
	glDrawElements(GL_TRIANGLES, _idxCount, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

void CBox::drawRaw()
{
	updateMatrix();
	glBindVertexArray(_vao);
	glUniform1i(_shadingModeLoc, _uShadingMode);
	if (_bObjColor) glUniform4fv(_colorLoc, 1, glm::value_ptr(_color));
	glDrawElements(GL_TRIANGLES, _idxCount, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

void CBox::update(float dt)
{

}

void CBox::reset() {
	CShape::reset();
	// �p���ݭn�A�i�[�J��L�S�w��|��Ϊ����]�ʧ@
}

void CBox::setPoints(bool front, bool back, bool left, bool right) {
    std::vector<GLfloat> points;
    std::vector<GLuint> indices;
    GLuint baseIdx = 0;

    auto addFace = [&](std::initializer_list<GLfloat> faceVerts, std::initializer_list<GLuint> faceIdx) {
        points.insert(points.end(), faceVerts.begin(), faceVerts.end());
        for (GLuint i : faceIdx)
            indices.push_back(baseIdx + i);
        baseIdx += 4;
    };

    if (front) {
        addFace({
            // z = +0.5, normal = (0, 0, 1)
            -0.5f, -0.5f,  0.5f,  1.0f, 0.6f, 0.6f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f,
             0.5f, -0.5f,  0.5f,  1.0f, 0.6f, 0.6f,   0.0f, 0.0f, 1.0f,   1.0f, 0.0f,
             0.5f,  0.5f,  0.5f,  1.0f, 0.6f, 0.6f,   0.0f, 0.0f, 1.0f,   1.0f, 1.0f,
            -0.5f,  0.5f,  0.5f,  1.0f, 0.6f, 0.6f,   0.0f, 0.0f, 1.0f,   0.0f, 1.0f
            }, { 0, 1, 2, 2, 3, 0 });
    }
    if (back) {
        addFace({
             0.5f, -0.5f, -0.5f,  0.6f, 1.0f, 0.6f,   0.0f, 0.0f, -1.0f,   1.0f, 0.0f,
            -0.5f, -0.5f, -0.5f,  0.6f, 1.0f, 0.6f,   0.0f, 0.0f, -1.0f,   0.0f, 0.0f,
            -0.5f,  0.5f, -0.5f,  0.6f, 1.0f, 0.6f,   0.0f, 0.0f, -1.0f,   0.0f, 1.0f,
             0.5f,  0.5f, -0.5f,  0.6f, 1.0f, 0.6f,   0.0f, 0.0f, -1.0f,   1.0f, 1.0f
            }, { 0, 1, 2, 2, 3, 0 });
    }
    if (left) {
        addFace({
            -0.5f, -0.5f, -0.5f,  0.6f, 0.6f, 1.0f,   -1.0f, 0.0f, 0.0f,   0.0f, 0.0f,
            -0.5f, -0.5f,  0.5f,  0.6f, 0.6f, 1.0f,   -1.0f, 0.0f, 0.0f,   1.0f, 0.0f,
            -0.5f,  0.5f,  0.5f,  0.6f, 0.6f, 1.0f,   -1.0f, 0.0f, 0.0f,   1.0f, 1.0f,
            -0.5f,  0.5f, -0.5f,  0.6f, 0.6f, 1.0f,   -1.0f, 0.0f, 0.0f,   0.0f, 1.0f
            }, { 0, 1, 2, 2, 3, 0 });
    }
    if (right) {
        addFace({
             0.5f, -0.5f,  0.5f,  1.0f, 1.0f, 0.6f,    1.0f, 0.0f, 0.0f,   0.0f, 0.0f,
             0.5f, -0.5f, -0.5f,  1.0f, 1.0f, 0.6f,    1.0f, 0.0f, 0.0f,   1.0f, 0.0f,
             0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 0.6f,    1.0f, 0.0f, 0.0f,   1.0f, 1.0f,
             0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 0.6f,    1.0f, 0.0f, 0.0f,   0.0f, 1.0f
            }, { 0, 1, 2, 2, 3, 0 });
    }
    // top �M bottom �@�߳��e
	addFace({
			-0.5f,  0.5f,  0.5f,  1.0f, 0.6f, 1.0f,   0.0f, 1.0f, 0.0f,   0.0f, 0.0f,
			 0.5f,  0.5f,  0.5f,  1.0f, 0.6f, 1.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f,
			 0.5f,  0.5f, -0.5f,  1.0f, 0.6f, 1.0f,   0.0f, 1.0f, 0.0f,   1.0f, 1.0f,
			-0.5f,  0.5f, -0.5f,  1.0f, 0.6f, 1.0f,   0.0f, 1.0f, 0.0f,   0.0f, 1.0f
			}, { 0, 1, 2, 2, 3, 0 });
	addFace({
			-0.5f, -0.5f, -0.5f,  0.6f, 1.0f, 1.0f,   0.0f, -1.0f, 0.0f,   0.0f, 0.0f,
			 0.5f, -0.5f, -0.5f,  0.6f, 1.0f, 1.0f,   0.0f, -1.0f, 0.0f,   1.0f, 0.0f,
			 0.5f, -0.5f,  0.5f,  0.6f, 1.0f, 1.0f,   0.0f, -1.0f, 0.0f,   1.0f, 1.0f,
			-0.5f, -0.5f,  0.5f,  0.6f, 1.0f, 1.0f,   0.0f, -1.0f, 0.0f,   0.0f, 1.0f
			}, { 0, 1, 2, 2, 3, 0 });

    // �t�m�������������ܼ�
    _vtxCount = baseIdx;
    _vtxAttrCount = 11;
    _idxCount = static_cast<GLuint>(indices.size());

    // �����¸��
    if(_points != nullptr) delete[] _points;
	if (_idx != nullptr) delete[] _idx;

    // �]�w�s���
    _points = new GLfloat[points.size()];
    std::copy(points.begin(), points.end(), _points);

    _idx = new GLuint[indices.size()];
    std::copy(indices.begin(), indices.end(), _idx);
}

