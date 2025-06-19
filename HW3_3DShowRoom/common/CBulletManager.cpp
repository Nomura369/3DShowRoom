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

void CBulletManager::instantiate(glm::vec3 bulletPos, glm::vec3 targetPos) { // �@���B�z�@���l�u
	if (_timer < _cooldown) return; // �N�o�ɶ�����A�����\�g��
	_timer = 0; // ���m�N�o�ɶ�
	
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
	// �Ȯɤ��]�w Material
	currentBullet->setTextureMode(CShape::TEX_DIFFUSE);
	currentBullet->setScale(glm::vec3(0.1f, 0.1f, 0.1f));
	currentBullet->setPos(bulletPos); // ��l��m�۷���q����m
	currentBullet->setTargetPos(targetPos); // �g���ؼЬ۷����u��m
	currentBullet->updateDirection();
}

void CBulletManager::draw() { // �@���B�z�Ҧ��l�u
	pGet = pHead;
	while (pGet != nullptr) {
		if (pGet->bullet != nullptr && pGet->bullet->getIsActive()) {
			pGet->bullet->uploadMaterial();
			pGet->bullet->uploadTextureFlags();
			pGet->bullet->drawRaw();
			/*std::cout << "�e�X�l�u" << std::endl;*/
		}
		pGet = pGet->Link;
	}
}

void CBulletManager::update(float dt) { // �@���B�z�Ҧ��l�u
	_timer += dt;
	
	pGet = pHead;

	while (pGet != nullptr && pGet->bullet->getIsActive()) {
		pGet->bullet->update(dt);
		pGet = pGet->Link;
	}
}

void CBulletManager::handleDeath() { // �@���B�z����
	PNODE prev = nullptr;
	pGet = pHead;

	// �z�פW�̥��ͦ����|�̥����X�ж� + �P�˹����I����|����
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
	while (pHead != nullptr) { //�C����������̫e����
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

void CBulletManager::checkCollision() { // �@���B�z����
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
					// �R��
					bullet->setIsActive(false);
					model->setIsActive(false);

					std::cout << "�l�u��������" << std::endl << std::endl;
					break; // ���~����o���l�u
				}
			}
		}
		curr = curr->Link;
	}
}