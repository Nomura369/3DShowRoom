#include <cstdio>
#include <cstdlib>

#include <glew/include/GL/glew.h>
#include <glm/gtc/matrix_transform.hpp>

#include "CBulletManager.h"
#include "../models/CShape.h"

CBulletManager& CBulletManager::getInstance()
{
	static CBulletManager instance;
	return instance;
}

CBulletManager::CBulletManager() {
	pHead = pTail = pGet = nullptr;
	_shaderProg = 0;
}

void CBulletManager::instantiate(glm::vec3 bulletPos, glm::vec3 targetPos) { // �@���B�z�@���l�u
	if (pHead == nullptr) { // �쵲��C���ŮɡA�إ߲Ĥ@�Ӹ`�I
		pHead = new NODE;
		pHead->bullet = new CBullet; // �ͦ��l�u
		pHead->Link = nullptr;
		pTail = pHead;
	}
	else {
		pGet = new NODE;
		pGet->bullet = new CBullet; // �ͦ��l�u
		pGet->Link = nullptr;
		pTail->Link = pGet;
		pTail = pGet;
	}
	// �]�w�l�u���򥻸��
	CBullet* currentBullet = pTail->bullet;
	currentBullet->setupVertexAttributes();
	currentBullet->setShaderID(_shaderProg, 3);
	// �Ȯɤ��]�w Material �M Texture
	
	currentBullet->setScale(glm::vec3(0.1f, 0.1f, 0.1f));
	currentBullet->setPos(bulletPos); // ��l��m�۷���q����m
	currentBullet->setTargetPos(targetPos); // �g���ؼЬ۷����u��m
	currentBullet->updateDirection();
}

void CBulletManager::draw() { // �@���B�z�Ҧ��l�u
	pGet = pHead;
	while (pGet != nullptr) {
		if (pGet->bullet != nullptr) {
			pGet->bullet->uploadMaterial();
			pGet->bullet->uploadTextureFlags();
			pGet->bullet->drawRaw();
		}
		pGet = pGet->Link;
	}
}

void CBulletManager::update(float dt) { // �@���B�z�Ҧ��l�u
	pGet = pHead;

	while (pGet != nullptr) {
		if (pGet->bullet != nullptr) {
			pGet->bullet->update(dt);
			pGet = pGet->Link;
		}
	}
}

//void CBulletManager::handleDeath() { // �@���B�z����
//	PNODE prev = nullptr;
//	pGet = pHead;
//
//	// �z�פW�̥��ͦ����|�̥����X���� + �P�ĤH�I����|����
//	while (pGet != nullptr) {
//		if (pGet->bullet != nullptr && !pGet->bullet->getIsActive()) {
//			PNODE toDelete = pGet;
//			if (prev == nullptr) {
//				pHead = pGet->Link;
//			}
//			else {
//				prev->Link = pGet->Link;
//			}
//			pGet = pGet->Link;
//			delete toDelete;
//			toDelete = nullptr;
//		}
//		else {
//			prev = pGet;
//			pGet = pGet->Link;
//		}
//	}
//}

CBulletManager::~CBulletManager() {
	while (pHead != nullptr) { //�C����������̫e����
		PNODE temp = pHead;
		pHead = pHead->Link;
		if (temp != nullptr) delete temp;
	}
}

void CBulletManager::setShaderProg(GLuint shaderProg) {
	_shaderProg = shaderProg;
}