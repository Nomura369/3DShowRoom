#include <cstdio>
#include <cstdlib>

#include <glew/include/GL/glew.h>
#include <glm/gtc/matrix_transform.hpp>

#include "CBulletManager.h"

CBulletManager& CBulletManager::getInstance()
{
	static CBulletManager instance;
	return instance;
}

CBulletManager::CBulletManager() {
	pHead = pTail = pGet = nullptr;
	_shaderProg = 0;
	_cooldown = 0.15f;
	_timer = 0.0f;
}

void CBulletManager::instantiate(glm::vec3 bulletPos, glm::vec3 targetPos) { // 一次處理一顆子彈
	if (_timer < _cooldown) return; // 冷卻時間未到，不允許射擊
	_timer = 0; // 重置冷卻時間
	
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
	// 暫時不設定 Material
	currentBullet->setTextureMode(CShape::TEX_DIFFUSE);
	currentBullet->setScale(glm::vec3(0.1f, 0.1f, 0.1f));
	currentBullet->setPos(bulletPos); // 初始位置相當於手電筒位置
	currentBullet->setTargetPos(targetPos); // 射擊目標相當於視線位置
	currentBullet->updateDirection();
}

void CBulletManager::draw() { // 一次處理所有子彈
	pGet = pHead;
	while (pGet != nullptr) {
		if (pGet->bullet != nullptr && pGet->bullet->getIsActive()) {
			pGet->bullet->uploadMaterial();
			pGet->bullet->uploadTextureFlags();
			pGet->bullet->drawRaw();
			/*std::cout << "畫出子彈" << std::endl;*/
		}
		pGet = pGet->Link;
	}
}

void CBulletManager::update(float dt) { // 一次處理所有子彈
	_timer += dt;
	
	pGet = pHead;

	while (pGet != nullptr && pGet->bullet->getIsActive()) {
		pGet->bullet->update(dt);
		pGet = pGet->Link;
	}
}

void CBulletManager::handleDeath() { // 一次處理全部
	PNODE prev = nullptr;
	pGet = pHead;

	// 理論上最先生成的會最先飛出房間 + 與裝飾物碰撞後會消失
	while (pGet != nullptr) {
		if (pGet->bullet != nullptr && !pGet->bullet->getIsActive()) {
			PNODE toDelete = pGet;
			if (prev == nullptr) {
				pHead = pGet->Link;
			}
			else {
				prev->Link = pGet->Link;
			}
			pGet = pGet->Link;
			delete toDelete->bullet;
			delete toDelete;
			toDelete = nullptr;
		}
		else {
			prev = pGet;
			pGet = pGet->Link;
		}
	}
}

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

void CBulletManager::addToCollision(CShape* model) {
	_models.push_back(model);
}

void CBulletManager::checkCollision() { // 一次處理全部
	PNODE curr = pHead;
	while (curr != nullptr) {
		CBullet* bullet = curr->bullet;
		if (bullet && bullet->getIsActive()) {
			glm::vec3 bulletPos = bullet->getPos();
			float bulletRadius = 0.1f;

			for (auto model : _models) {
				if (!model->getIsActive()) continue;

				float dist2 = glm::distance(bulletPos, model->getPos());
				float combinedRadius = bulletRadius + model->getCollisionDist();

				if (dist2 <= combinedRadius * combinedRadius) {
					// 命中
					bullet->setIsActive(false);
					model->setIsActive(false);

					std::cout << "子彈打中物件" << std::endl << std::endl;
					break; // 不繼續比對這顆子彈
				}
			}
		}
		curr = curr->Link;
	}
}