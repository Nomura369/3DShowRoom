#include <glew/include/GL/glew.h>
#include <glm/gtc/type_ptr.hpp>
#include <vector>

#include "CQuad.h"
// �N�K�Ϯy���X�R��T�աA�򥻶K�ϡBlight map�Btangent space normal map
CQuad::CQuad() : CShape() 
{
	// _vtxAttrCount 19:���ޭ� 9�B10���򥻶K�ϡB11 12 �� light map�B13 14 �� TBNM�B
	//  15 �� 18 �� tangent �P bitangent �V�q�P�k�V�q���զX�C3+3+3+2+2+3+3 = 19�C
	//  tangent space �K�Ϯy�лP diffuse �K�Ϫ��y�Ф@�P
	_vtxCount = 4; _vtxAttrCount = 19; _idxCount = 6;  

	_points = new GLfloat[_vtxCount * _vtxAttrCount]{
		// ��m             // �C��           // �k�V�q         // �K�Ϯy�� //light map  //tangent     //bitangent
		-0.5f, -0.5f, 0.0f, 0.5f, 0.7f, 0.4f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,0.0f,0.0f, 0.0f,0.0f,0.0f, // ���U  1.0f,0.0f,0.0f,    0.0f,1.0f,0.0f,
		 0.5f, -0.5f, 0.0f, 0.5f, 0.7f, 0.4f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f,0.0f,0.0f, 0.0f,0.0f,0.0f, // �k�U  1.0f,0.0f,0.0f,    0.0f,1.0f,0.0f,
		 0.5f,  0.5f, 0.0f, 0.5f, 0.7f, 0.4f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f,0.0f,0.0f, 0.0f,0.0f,0.0f, // �k�W  1.0f,0.0f,0.0f,    0.0f,1.0f,0.0f,
		-0.5f,  0.5f, 0.0f, 0.5f, 0.7f, 0.4f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,0.0f,0.0f, 0.0f,0.0f,0.0f  // ���W  1.0f,0.0f,0.0f,    0.0f,1.0f,0.0f 
	};

	_idx = new GLuint[_idxCount]{ 0, 1, 2, 2, 3, 0 };

	genTangentBitangent();
}

CQuad::~CQuad()
{
	glDeleteBuffers(1, &_vbo);  //������ VBO �P EBO
	glDeleteBuffers(1, &_ebo);
	glDeleteVertexArrays(1, &_vao); //�A���� VAO
	if (_points != NULL) delete[] _points;
	if (_idx != NULL) delete[] _idx;
}

void CQuad::draw()
{
	glUseProgram(_shaderProg);
	uploadTextureFlags();
	uploadMaterial();
	glBindVertexArray(_vao);
	updateMatrix();
	glUniform1i(_shadingModeLoc, _uShadingMode);
	if (_bObjColor) glUniform4fv(_colorLoc, 1, glm::value_ptr(_color));
	glDrawElements(GL_TRIANGLES, _idxCount, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

void CQuad::drawRaw()
{
	glBindVertexArray(_vao);
	uploadTextureFlags();
	uploadMaterial();
	updateMatrix();
	glUniform1i(_shadingModeLoc, _uShadingMode);
	if (_bObjColor) glUniform4fv(_colorLoc, 1, glm::value_ptr(_color));
	glDrawElements(GL_TRIANGLES, _idxCount, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

void CQuad::update(float dt)
{

}

void CQuad::reset() {
	CShape::reset();
	// �p���ݭn�A�i�[�J��L�S�w��|��Ϊ����]�ʧ@
}

