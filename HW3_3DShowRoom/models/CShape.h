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
	virtual void drawRaw(); //不使用 shader
	virtual void reset();
	virtual void update(float dt);
	void setupVertexAttributes();
	void setShaderID(GLuint shaderID, int shadeingmode = 1); // 預設使用傳入的 vertex color
	void setColor(glm::vec4 vColor); // 設定模型的顏色
	void setScale(glm::vec3 vScale); // 設定模型的縮放比
	void setPos(glm::vec3 vPt); // 設定模型的位置
	void setRotate(float angle, const glm::vec3& axis);		// 設定模型的旋轉角度與旋轉軸
	void updateRotate(float angle, const glm::vec3& axis);	// 更新與新增模型的旋轉角度與旋轉軸
	void setTransformMatrix(glm::mat4 mxMatrix);
	void updateMatrix();
	glm::vec3 getPos(); // 取得模型的位置
	glm::mat4 getModelMatrix();
	glm::mat4 getTransMatrix();
	GLuint getShaderProgram();

	// 材質管理
	void setMaterial(const CMaterial& material);
	const CMaterial& getMaterial();
	void uploadMaterial();

	// 控制與改變貼圖座標
	void setMirror(bool uAxis, bool vAxis); // U軸 與 V軸 是否要鏡射
	void setTiling(float uTiling, float vTiling);  // 對 U軸 與 V軸重複拼貼的次數
	void setLightMapTiling(float uTiling, float vTiling);  // 對 LightMap U 軸與 V 軸 進行重複拼貼的次數

	// 設定 Texture Flags 參數
	void setTextureMode(int flags);
	void uploadTextureFlags();

	// 計算 tangent 與 bitangent，必須在模型基本資料完成後才能呼叫
	void genTangentBitangent();

	void setCollisionDist(float dist);
	float getCollisionDist();
	void setIsActive(bool isActive);
	bool getIsActive();

protected:
	int _vtxCount, _vtxAttrCount, _idxCount; // 頂點數, 頂點屬性數,點頂索引值數
	GLfloat* _points;
	GLuint* _idx;
	GLuint _vao, _vbo, _ebo;
	GLuint _shaderProg;
	GLint _modelMxLoc;
	GLint _shadingModeLoc, _uShadingMode; //上色模式的進入點, 上色模式
	GLint _colorLoc;	// 上色模式的進入點
	bool _bRotation, _bScale, _bPos, _bObjColor;
	bool _bTransform, _bOnTransform;
	// _bTransform : true 代表有設定新的轉換矩陣
	// _bOnTransform : true 代表曾經設定過轉換矩陣，用於判斷是否需要更新 model matrix

	glm::vec4 _color; // 使用 RGBA
	glm::vec3 _scale; // 模型的縮放值
	glm::vec3 _pos;  // 模型的位置

	// 設定模型的初始與運動下的參數
	GLfloat   _rotAngle; // 模型的旋轉角度
	glm::vec3 _rotAxis; // 模型的旋轉軸
	glm::mat4 _mxRotation; // 模型目前的旋轉矩陣
	glm::mat4 _mxScale, _mxTrans, _mxTRS; // 模型的縮放、位移與縮放旋轉位移的整合矩陣
	glm::mat4 _mxTransform, _mxFinal; // 額外增加的轉換矩陣與最終的模型矩陣

	// 材質
	CMaterial _material;

	//  與貼圖設定有關的變數
	int   _textureFlags = TEX_NONE;    // 用 int 存所有 flag
	GLint _texFlagsLoc = -1;
	GLint _diffMapLoc;   // diffuse map 對應貼圖單元的進入點
	GLint _lightMapLoc;  // light map  對應貼圖單元的進入點
	GLint _normalMapLoc; // normal map 對應貼圖單元的進入點
	GLint _cubeMapLoc;  // cubic map 對應貼圖單元的進入點

	float _collisionDist;
	bool _isActive;
};