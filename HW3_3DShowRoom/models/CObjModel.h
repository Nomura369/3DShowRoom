#pragma once
#include <vector>

#include "CShape.h"
#include "..\common\CObjLoader.h"
// #include "..\common\CMaterial.h"

class CObjModel : public CShape
{
public:
	CObjModel();
	virtual ~CObjModel();
	virtual void draw() override;
	virtual void drawRaw() override;
	virtual void reset() override;
	virtual void update(float dt) override;

private:
	void generateCObjModel();
	std::vector<Triangle> _tris; // 構成該模型的所有三角形
};