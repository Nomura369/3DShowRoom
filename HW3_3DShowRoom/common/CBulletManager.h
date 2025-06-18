#pragma once
#include "../models/CBullet.h"

// �w�q�l�u�޲z�����O�A�D�n�Q���쵲��C���ީҦ��l�u�]Singleton pattern�^
class CBulletManager
{
private:
	static CBulletManager* instance;
	CBulletManager(); // �إ��쵲��C

	typedef struct TNode {
		CBullet* bullet;
		struct TNode* Link;
	} NODE, * PNODE; // �l�u�`�I�ŧi
	PNODE pHead, pTail, pGet;

	GLuint _shaderProg;

public:
	// �T�O�~�����o�ƻs�����O�����
	CBulletManager(CBulletManager& other) = delete;
	void operator = (const CBulletManager&) = delete;

	// ��������O���ߤ@��Ҫ��R�A���
	static CBulletManager& getInstance();

	void setShaderProg(GLuint shaderProg);
	void instantiate(glm::vec3 bulletPos, glm::vec3 targetPos); // �ͦ��ó]�w�l�u
	void draw(); // �@���e�X�쵲��C���{���������l�u
	void update(float dt); // �@����s�����l�u�����ʪ��A
	//void handleDeath(); // �@���B�z _isActive == false ���l�u
	~CBulletManager(); // �����쵲��C
	
};