#include <glew/include/GL/glew.h>
#include <glm/gtc/type_ptr.hpp>
#include <vector>

#include "CQuad.h"
// 將貼圖座標擴充到三組，基本貼圖、light map、tangent space normal map
CQuad::CQuad() : CShape() 
{
	// _vtxAttrCount 19:索引值 9、10為基本貼圖、11 12 為 light map、13 14 為 TBNM、
	//  15 到 18 為 tangent 與 bitangent 向量與法向量的組合。3+3+3+2+2+3+3 = 19。
	//  tangent space 貼圖座標與 diffuse 貼圖的座標一致
	_vtxCount = 4; _vtxAttrCount = 19; _idxCount = 6;  

	_points = new GLfloat[_vtxCount * _vtxAttrCount]{
		// 位置             // 顏色           // 法向量         // 貼圖座標 //light map  //tangent     //bitangent
		-0.5f, -0.5f, 0.0f, 0.5f, 0.7f, 0.4f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,0.0f,0.0f, 0.0f,0.0f,0.0f, // 左下  1.0f,0.0f,0.0f,    0.0f,1.0f,0.0f,
		 0.5f, -0.5f, 0.0f, 0.5f, 0.7f, 0.4f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f,0.0f,0.0f, 0.0f,0.0f,0.0f, // 右下  1.0f,0.0f,0.0f,    0.0f,1.0f,0.0f,
		 0.5f,  0.5f, 0.0f, 0.5f, 0.7f, 0.4f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f,0.0f,0.0f, 0.0f,0.0f,0.0f, // 右上  1.0f,0.0f,0.0f,    0.0f,1.0f,0.0f,
		-0.5f,  0.5f, 0.0f, 0.5f, 0.7f, 0.4f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,0.0f,0.0f, 0.0f,0.0f,0.0f  // 左上  1.0f,0.0f,0.0f,    0.0f,1.0f,0.0f 
	};

	_idx = new GLuint[_idxCount]{ 0, 1, 2, 2, 3, 0 };

	genTangentBitangent();
}

CQuad::~CQuad()
{
	glDeleteBuffers(1, &_vbo);  //先釋放 VBO 與 EBO
	glDeleteBuffers(1, &_ebo);
	glDeleteVertexArrays(1, &_vao); //再釋放 VAO
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
	// 如有需要，可加入其他特定於四邊形的重設動作
}

