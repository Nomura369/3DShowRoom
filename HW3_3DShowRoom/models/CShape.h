#pragma once
#include <glm/glm.hpp>
#include <glew/include/GL/glew.h>
#include "../common/CMaterial.h"

class CShape
{
public:
	enum TextureFlag {
		TEX_NONE = 0,
		TEX_DIFFUSE   = 1 << 0, // 1
		TEX_LIGHTMAP  = 1 << 1,	// 2
		TEX_NORMALMAP = 1 << 2, // 4
		TEX_CUBEMAP  = 1 << 3, // 8
		// .. future flags
	};

	CShape();
	virtual ~CShape();
	virtual void draw();
	virtual void drawRaw(); //���ϥ� shader
	virtual void reset();
	virtual void update(float dt);
	void setupVertexAttributes();
	void setShaderID(GLuint shaderID, int shadeingmode = 1); // �w�]�ϥζǤJ�� vertex color
	void setColor(glm::vec4 vColor); // �]�w�ҫ����C��
	void setScale(glm::vec3 vScale); // �]�w�ҫ����Y���
	void setPos(glm::vec3 vPt); // �]�w�ҫ�����m
	void setRotate(float angle, const glm::vec3& axis);		// �]�w�ҫ������ਤ�׻P����b
	void updateRotate(float angle, const glm::vec3& axis);	// ��s�P�s�W�ҫ������ਤ�׻P����b
	void setTransformMatrix(glm::mat4 mxMatrix);
	void updateMatrix();
	glm::vec3 getPos(); // ���o�ҫ�����m
	glm::mat4 getModelMatrix();
	glm::mat4 getTransMatrix();
	GLuint getShaderProgram();

	// ����޲z
	void setMaterial(const CMaterial& material);
	const CMaterial& getMaterial();
	void uploadMaterial();

	// ����P���ܶK�Ϯy��
	void setMirror(bool uAxis, bool vAxis); // U�b �P V�b �O�_�n��g
	void setTiling(float uTiling, float vTiling);  // �� U�b �P V�b���ƫ��K������
	void setLightMapTiling(float uTiling, float vTiling);  // �� LightMap U �b�P V �b �i�歫�ƫ��K������

	// �]�w Texture Flags �Ѽ�
	void setTextureMode(int flags);
	void uploadTextureFlags();

	// �p�� tangent �P bitangent�A�����b�ҫ��򥻸�Ƨ�����~��I�s
	void genTangentBitangent();

	void setCollisionDist(float dist);
	float getCollisionDist();
	void setIsActive(bool isActive);
	bool getIsActive();

protected:
	int _vtxCount, _vtxAttrCount, _idxCount; // ���I��, ���I�ݩʼ�,�I�����ޭȼ�
	GLfloat* _points;
	GLuint* _idx;
	GLuint _vao, _vbo, _ebo;
	GLuint _shaderProg;
	GLint _modelMxLoc;
	GLint _shadingModeLoc, _uShadingMode; //�W��Ҧ����i�J�I, �W��Ҧ�
	GLint _colorLoc;	// �W��Ҧ����i�J�I
	bool _bRotation, _bScale, _bPos, _bObjColor;
	bool _bTransform, _bOnTransform;
	// _bTransform : true �N���]�w�s���ഫ�x�}
	// _bOnTransform : true �N���g�]�w�L�ഫ�x�}�A�Ω�P�_�O�_�ݭn��s model matrix

	glm::vec4 _color; // �ϥ� RGBA
	glm::vec3 _scale; // �ҫ����Y���
	glm::vec3 _pos;  // �ҫ�����m

	// �]�w�ҫ�����l�P�B�ʤU���Ѽ�
	GLfloat   _rotAngle; // �ҫ������ਤ��
	glm::vec3 _rotAxis; // �ҫ�������b
	glm::mat4 _mxRotation; // �ҫ��ثe������x�}
	glm::mat4 _mxScale, _mxTrans, _mxTRS; // �ҫ����Y��B�첾�P�Y�����첾����X�x�}
	glm::mat4 _mxTransform, _mxFinal; // �B�~�W�[���ഫ�x�}�P�̲ת��ҫ��x�}

	// ����
	CMaterial _material;

	//  �P�K�ϳ]�w�������ܼ�
	int   _textureFlags = TEX_NONE;    // �� int �s�Ҧ� flag
	GLint _texFlagsLoc = -1;
	GLint _diffMapLoc;   // diffuse map �����K�ϳ椸���i�J�I
	GLint _lightMapLoc;  // light map  �����K�ϳ椸���i�J�I
	GLint _normalMapLoc; // normal map �����K�ϳ椸���i�J�I
	GLint _cubeMapLoc;  // cubic map �����K�ϳ椸���i�J�I

	float _collisionDist;
	bool _isActive;
};