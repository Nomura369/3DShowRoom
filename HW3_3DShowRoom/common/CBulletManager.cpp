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

void CBulletManager::instantiate(glm::vec3 bulletPos, glm::vec3 targetPos) { // 一次處理一顆子彈
	if (pHead == nullptr) { // 鏈結串列為空時，建立第一個節點
		pHead = new NODE;
		pHead->bullet = new CBullet; // 生成子彈
		pHead->Link = nullptr;
		pTail = pHead;
	}
	else {
		pGet = new NODE;
		pGet->bullet = new CBullet; // 生成子彈
		pGet->Link = nullptr;
		pTail->Link = pGet;
		pTail = pGet;
	}
	// 設定子彈的基本資料
	CBullet* currentBullet = pTail->bullet;
	currentBullet->setupVertexAttributes();
	currentBullet->setShaderID(_shaderProg, 3);
	// 暫時不設定 Material 和 Texture
	
	currentBullet->setScale(glm::vec3(0.1f, 0.1f, 0.1f));
	currentBullet->setPos(bulletPos); // 初始位置相當於手電筒位置
	currentBullet->setTargetPos(targetPos); // 射擊目標相當於視線位置
	currentBullet->updateDirection();
}

void CBulletManager::draw() { // 一次處理所有子彈
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

void CBulletManager::update(float dt) { // 一次處理所有子彈
	pGet = pHead;

	while (pGet != nullptr) {
		if (pGet->bullet != nullptr) {
			pGet->bullet->update(dt);
			pGet = pGet->Link;
		}
	}
}

//void CBulletManager::handleDeath() { // 一次處理全部
//	PNODE prev = nullptr;
//	pGet = pHead;
//
//	// 理論上最先生成的會最先飛出視窗 + 與敵人碰撞後會消失
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
	while (pHead != nullptr) { //每次都先釋放最前面的
		PNODE temp = pHead;
		pHead = pHead->Link;
		if (temp != nullptr) delete temp;
	}
}

void CBulletManager::setShaderProg(GLuint shaderProg) {
	_shaderProg = shaderProg;
}