#pragma once
#include "../models/CBullet.h"

// 定義子彈管理器類別，主要利用鏈結串列控管所有子彈（Singleton pattern）
class CBulletManager
{
private:
	static CBulletManager* instance;
	CBulletManager(); // 建立鏈結串列

	typedef struct TNode {
		CBullet* bullet;
		struct TNode* Link;
	} NODE, * PNODE; // 子彈節點宣告
	PNODE pHead, pTail, pGet;

	GLuint _shaderProg;

public:
	// 確保外部不得複製此類別的實例
	CBulletManager(CBulletManager& other) = delete;
	void operator = (const CBulletManager&) = delete;

	// 獲取該類別的唯一實例的靜態函數
	static CBulletManager& getInstance();

	void setShaderProg(GLuint shaderProg);
	void instantiate(glm::vec3 bulletPos, glm::vec3 targetPos); // 生成並設定子彈
	void draw(); // 一次畫出鏈結串列中現有的全部子彈
	void update(float dt); // 一次更新全部子彈的移動狀態
	//void handleDeath(); // 一次處理 _isActive == false 的子彈
	~CBulletManager(); // 釋放鏈結串列
	
};