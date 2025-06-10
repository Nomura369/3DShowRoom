#include <glm/gtc/type_ptr.hpp>
#include <vector>

#include "CShape.h"
#include "../common/typedefs.h"

CShape::CShape()
{
	_vtxCount = _vtxAttrCount = _idxCount = 0;
	_vao = 0; _vbo = 0; _ebo = 0;
	_shaderProg = 0;
	_scale = glm::vec3(1.0f, 1.0f, 1.0f);
	_color = glm::vec4(0.5f, 0.5f, 0.5f, 1.0f);
	_pos = glm::vec3(0.0f, 0.0f, 0.0f);
	_rotAngle = 0.0f;
	_rotAxis = glm::vec3(0.0f,1.0f,0.0f);
	_bRotation = _bScale = _bPos = _bTransform = _bOnTransform = false;
	_mxScale = glm::mat4(1.0f);
	_mxTrans = glm::mat4(1.0f);
	_mxRotation = glm::mat4(1.0f);
	_mxTRS = glm::mat4(1.0f);
	_mxTransform = glm::mat4(1.0f);
	_mxFinal = glm::mat4(1.0f);
	_colorLoc = _modelMxLoc = 0;
	_points = nullptr; _idx = nullptr;
	_uShadingMode = 1; // �w�]�W��Ҧ��A1 : vertex color, 2: uniform color(object color)
	_bObjColor = false; // �w�]���ϥΪ����C��
	_shadingModeLoc = 0; // �W��Ҧ����i�J�I
	_diffMapLoc = -1;  _lightMapLoc = -1; _normalMapLoc = -1; _cubeMapLoc = -1;
}

CShape::~CShape()
{

}

void CShape::setupVertexAttributes()
{
	// �]�w VAO�BVBO �P EBO
	glGenVertexArrays(1, &_vao);
	glGenBuffers(1, &_vbo);
	glGenBuffers(1, &_ebo);

	// Bind the Vertex Array Object first, then bind and set vertex buffer(s) and attribute pointer(s).
	glBindVertexArray(_vao);

	// �]�w VBO
	glBindBuffer(GL_ARRAY_BUFFER, _vbo);
	glBufferData(GL_ARRAY_BUFFER, _vtxCount * _vtxAttrCount * sizeof(float), _points, GL_STATIC_DRAW);

	// �]�w EBO
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, _idxCount * sizeof(GLuint), _idx, GL_STATIC_DRAW);

	// ��m�ݩ�
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, _vtxAttrCount * sizeof(float), BUFFER_OFFSET(0));
	glEnableVertexAttribArray(0);

	// �C���ݩ�
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, _vtxAttrCount * sizeof(float), BUFFER_OFFSET(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	//�k�V�q�ݩ�
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, _vtxAttrCount * sizeof(float), BUFFER_OFFSET(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	//�K�Ϯy���ݩ�
	glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, _vtxAttrCount * sizeof(float), BUFFER_OFFSET(9 * sizeof(float)));
	glEnableVertexAttribArray(3);

	//light map �y���ݩ�
	glVertexAttribPointer(4, 2, GL_FLOAT, GL_FALSE, _vtxAttrCount * sizeof(float), BUFFER_OFFSET(11 * sizeof(float)));
	glEnableVertexAttribArray(4);

	// tangent �V�q�ݩ�
	glVertexAttribPointer(5, 3, GL_FLOAT, GL_FALSE, _vtxAttrCount * sizeof(float), BUFFER_OFFSET(13 * sizeof(float)));
	glEnableVertexAttribArray(5);

	// bitangent �V�q�ݩ�
	glVertexAttribPointer(6, 3, GL_FLOAT, GL_FALSE, _vtxAttrCount * sizeof(float), BUFFER_OFFSET(16 * sizeof(float)));
	glEnableVertexAttribArray(6);

	glBindVertexArray(0); // �Ѱ��� VAO ���j�w
}

void CShape::setShaderID(GLuint shaderID, int shadeingmode)
{
	_shaderProg = shaderID;
	_uShadingMode = shadeingmode;
	glUseProgram(_shaderProg);
	_modelMxLoc = glGetUniformLocation(_shaderProg, "mxModel"); 	// ���o mxModel �ܼƪ���m
	glUniformMatrix4fv(_modelMxLoc, 1, GL_FALSE, glm::value_ptr(_mxTRS));
	_shadingModeLoc = glGetUniformLocation(_shaderProg, "uShadingMode"); 	// ���o iColorType �ܼƪ���m
	glUniform1i(_shadingModeLoc, _uShadingMode);

	_diffMapLoc = glGetUniformLocation(_shaderProg, "uDiffuseMap");
	glUniform1i(_diffMapLoc, 0); // uDiffuseMap ���� GL_TEXTURE0
	_lightMapLoc = glGetUniformLocation(_shaderProg, "uLightMap");
	glUniform1i(_lightMapLoc, 1); // uLightMap ���� GL_TEXTURE1
	_normalMapLoc = glGetUniformLocation(_shaderProg, "uNormalMap");
	glUniform1i(_normalMapLoc, 2); // uNormalMap ���� GL_TEXTURE2
	_cubeMapLoc = glGetUniformLocation(_shaderProg, "uEnvMap");
	glUniform1i(_cubeMapLoc, 3); // uEnvMap ���� GL_TEXTURE3

	// ���o uniform �ܼ� uTextureFlags ��m
	_texFlagsLoc = glGetUniformLocation(_shaderProg, "uTextureFlags");
	glUniform1i(_texFlagsLoc, _textureFlags); // �w�] TEX_NONE
}

void CShape::setColor(glm::vec4 vColor)
{
	// �z�L glUniform �ǤJ�ҫ����C��
	_color = vColor;
	_colorLoc = glGetUniformLocation(_shaderProg, "ui4Color"); 	// ���o ui4Color �ܼƪ���m
	glUniform4fv(_colorLoc, 1, glm::value_ptr(_color));
	_uShadingMode = 2; // �]�w�W��Ҧ��� uniform color
	glUniform1i(_shadingModeLoc, _uShadingMode);
	_bObjColor = true;
}

void CShape::draw() {
	//glUseProgram(_shaderProg);
	//updateMatrix();
	//glBindVertexArray(_vao);
	// �w�]���I�s��ڪ�ø�s���O�A�Ѥl���O�M�w�O�_�����I�s glDrawElements()
}

void CShape::drawRaw()
{

}

void CShape::update(float dt)
{
	
}

void CShape::genTangentBitangent()
{
	// �p��C�@�ӳ��I�� tangent �P bitangent �V�q
	std::vector<glm::vec3> tangents(_vtxCount, glm::vec3(0.0f));
	std::vector<glm::vec3> bitangents(_vtxCount, glm::vec3(0.0f));
	for (int i = 0; i < _idxCount; i += 3) {
		unsigned i0 = _idx[i + 0], i1 = _idx[i + 1], i2 = _idx[i + 2];
		glm::vec3 p0 = glm::vec3(_points[i0 * _vtxAttrCount + 0], _points[i0 * _vtxAttrCount + 1], _points[i0 * _vtxAttrCount + 2]);
		glm::vec3 p1 = glm::vec3(_points[i1 * _vtxAttrCount + 0], _points[i1 * _vtxAttrCount + 1], _points[i1 * _vtxAttrCount + 2]);
		glm::vec3 p2 = glm::vec3(_points[i2 * _vtxAttrCount + 0], _points[i2 * _vtxAttrCount + 1], _points[i2 * _vtxAttrCount + 2]);
		glm::vec2 uv0 = glm::vec2(_points[i0 * _vtxAttrCount + 9], _points[i0 * _vtxAttrCount + 10]);
		glm::vec2 uv1 = glm::vec2(_points[i1 * _vtxAttrCount + 9], _points[i1 * _vtxAttrCount + 10]);
		glm::vec2 uv2 = glm::vec2(_points[i2 * _vtxAttrCount + 9], _points[i2 * _vtxAttrCount + 10]);
		// auto& p0 = positions[i0], & p1 = positions[i1], & p2 = positions[i2];
		// auto& uv0 = uvs[i0], &uv1 = uvs[i1], & uv2 = uvs[i2];
		glm::vec3 e1 = p1 - p0, e2 = p2 - p0;
		glm::vec2 d1 = uv1 - uv0, d2 = uv2 - uv0;
		float f = 1.0f / (d1.x * d2.y - d2.x * d1.y);
		glm::vec3 triT = f * (e1 * d2.y - e2 * d1.y);
		glm::vec3 triB = f * (-e1 * d2.x + e2 * d1.x);
		tangents[i0] += triT; tangents[i1] += triT; tangents[i2] += triT;
		bitangents[i0] += triB; bitangents[i1] += triB; bitangents[i2] += triB;
	}
	// �o���e������v�� 144 ���ҾɥX�� T �P B ���p�⤽��
	// 
	// 4. Gram�VSchmidt ����� & �W�@�ơA���� bitangent
	//    �N TBN �ܦ��зǥ����
	for (int i = 0; i < _vtxCount; ++i) {
		// glm::vec3 n = normals[i];
		glm::vec3 n = glm::vec3(_points[i * _vtxAttrCount + 6], _points[i * _vtxAttrCount + 7], _points[i * _vtxAttrCount + 8]);
		glm::vec3 t = tangents[i];
		t = glm::normalize(t - n * glm::dot(n, t));
		glm::vec3 b = glm::cross(n, t);
		tangents[i] = t;
		bitangents[i] = b;
	}

	// 5. ��p��n�� tangent/bitangent �g�^ _points
	for (int i = 0; i < _vtxCount; ++i) {
		int base = i * _vtxAttrCount;
		// tangent
		_points[base + 13] = tangents[i].x;
		_points[base + 14] = tangents[i].y;
		_points[base + 15] = tangents[i].z;
		// bitangent
		_points[base + 16] = bitangents[i].x;
		_points[base + 17] = bitangents[i].y;
		_points[base + 18] = bitangents[i].z;
	}
}

void CShape::setScale(glm::vec3 vScale)
{
	_scale = vScale;
	_bScale = true;
	_mxScale = glm::scale(glm::mat4(1.0f), _scale);
}

void CShape::setPos(glm::vec3 vPt)
{
	_pos = vPt;
	_bPos = true;
	_mxTrans = glm::translate(glm::mat4(1.0f), _pos);
}

glm::vec3 CShape::getPos() { return _pos; }
glm::mat4 CShape::getTransMatrix() { return _mxTrans; }


void CShape::setRotate(float angle, const glm::vec3& axis)
{
	float rad = glm::radians(angle); 	// glm::angleAxis �ݭn����
	_mxRotation = glm::rotate(glm::mat4(1.0f), rad, glm::normalize(axis)); // �p�����b�U���ਤ�שҹ�������x�}
	_bRotation = true;
}
void CShape::updateRotate(float angle, const glm::vec3& axis)
{
	float rad = glm::radians(angle); 	// glm::angleAxis �ݭn����
	_mxRotation *= glm::rotate(glm::mat4(1.0f), rad, glm::normalize(axis)); // �p�����b�U���ਤ�שҹ�������x�}
	_bRotation = true;
}

void CShape::updateMatrix()
{
	if (_bScale || _bPos || _bRotation )
	{
		_mxTRS = _mxTrans * _mxRotation * _mxScale;
		if (_bOnTransform == true) _mxFinal = _mxTransform * _mxTRS;
		else _mxFinal = _mxTRS;
		_bScale = _bPos = _bRotation = false;
	}
	if (_bTransform) {
		_mxFinal = _mxTransform * _mxTRS;
		_bTransform = false;
	}
	// �p�h�Ӽҫ��ϥάۦP�� shader program,�]�C�@�Ӽҫ��� mxTRS �����P�A�ҥH�C��frame���n��s
	glUniformMatrix4fv(_modelMxLoc, 1, GL_FALSE, glm::value_ptr(_mxFinal));
}

void CShape::setTransformMatrix(glm::mat4 mxMatrix)
{
	_bOnTransform = _bTransform = true;
	_mxTransform = mxMatrix;
}

glm::mat4 CShape::getModelMatrix() { return _mxFinal; }
GLuint CShape::getShaderProgram() { return _shaderProg; }

void CShape::reset()
{
	_scale = glm::vec3(1.0f, 1.0f, 1.0f);
	_color = glm::vec4(0.5f, 0.5f, 0.5f, 1.0f);
	_pos = glm::vec3(0.0f, 0.0f, 0.0f);
	_rotAngle = 0.0f;
	_rotAxis = glm::vec3(0.0f, 1.0f, 0.0f);

	_bRotation = _bScale = _bPos = _bTransform = _bOnTransform = false;
	_mxScale = glm::mat4(1.0f);
	_mxTrans = glm::mat4(1.0f);
	_mxTRS = glm::mat4(1.0f);
	_mxRotation = glm::mat4(1.0f);
	_mxTransform = glm::mat4(1.0f);
	_mxFinal = glm::mat4(1.0f);
	_uShadingMode = 1; // �w�]�W��Ҧ��A1 : vertex color, 2: uniform color(object color)
	_bObjColor = false; // �w�]���ϥΪ����C��
}


void CShape::setMaterial(const CMaterial& material) {
	_material = material;
}

void CShape::uploadMaterial()  {
	_material.uploadToShader(_shaderProg, "uMaterial");
}

//_vtxCount = _vtxAttrCount
// ����P���ܶK�Ϯy��
void CShape::setMirror(bool uAxis, bool vAxis) // U�b �P V�b �O�_�n��g
{
	int i;
	if (uAxis) {
		if (vAxis) { // U V �b�P����g
			for (i = 0; i < _vtxCount*_vtxAttrCount; i += _vtxAttrCount) { // �N�C�@�� vertex ���K�Ϯy�Х� 1 �h��
				_points[i+9] *= -1.0f;  _points[i+10] *= -1.0f;
			}
		}
		else { // �u�� U �b��g
			for (i = 0; i < _vtxCount * _vtxAttrCount; i += _vtxAttrCount) { // �N�C�@�� vertex ���K�Ϯy�Х� 1 �h��
				_points[i + 10] *= -1.0f; // x �N�O U �b
			}
		}
	}
	else if (vAxis) { // �u�� V �b��g
		for (i = 0; i < _vtxCount * _vtxAttrCount; i += _vtxAttrCount) { // �N�C�@�� vertex ���K�Ϯy�Х� 1 �h��
			_points[i + 9] *= -1.0f; // y �� V �b
		}
	}
	else;
	glBindBuffer(GL_ARRAY_BUFFER, _vbo);
	glBufferData(GL_ARRAY_BUFFER, _vtxCount * _vtxAttrCount * sizeof(float), _points, GL_STATIC_DRAW);
	glBindVertexArray(0);
}

void CShape::setTiling(float uTiling, float vTiling)  // �� U�b �P V�b �i����K���Y��
{
	int i;
	for (i = 0; i < _vtxCount * _vtxAttrCount; i += _vtxAttrCount) { 
		_points[i + 9] *= uTiling;  _points[i + 10] *= vTiling;  // diffuse �K�Ϲ������x�s��m
	}
	glBindBuffer(GL_ARRAY_BUFFER, _vbo);
	glBufferData(GL_ARRAY_BUFFER, _vtxCount * _vtxAttrCount * sizeof(float), _points, GL_STATIC_DRAW);
	glBindVertexArray(0);
}

void CShape::setLightMapTiling(float uTiling, float vTiling)  // �� LightMap U�b �P V�b �i����K���Y��
{
	int i;
	for (i = 0; i < _vtxCount * _vtxAttrCount; i += _vtxAttrCount) { // �N�C�@�� vertex ���K�Ϯy�Х� 1 �h��
		_points[i + 11] *= uTiling;  _points[i + 12] *= vTiling;
	}
	glBindBuffer(GL_ARRAY_BUFFER, _vbo);
	glBufferData(GL_ARRAY_BUFFER, _vtxCount * _vtxAttrCount * sizeof(float), _points, GL_STATIC_DRAW);
	glBindVertexArray(0);
}
void CShape::setTextureMode(int flags)
{
	_textureFlags = flags;
}

void CShape::uploadTextureFlags()
{
	glUniform1i(_texFlagsLoc, _textureFlags);
}