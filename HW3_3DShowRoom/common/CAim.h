#pragma once
#include "CSprite2D.h"

// 定義手槍準心類別
class CAim : public CSprite2D
{
public:
	CAim(float length = 50.0f, float thick = 3.0f);
	virtual ~CAim();
	virtual void draw() override;
	virtual void reset() override;
	virtual void update(float dt) override;
};
