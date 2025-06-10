#include <glm/gtc/type_ptr.hpp>
#include <vector>

#include "CShape.h"
#include "../common/typedefs.h"

CShape::CShape()
{
	_vtxCount = _vtxAttrCount = _idxCount = 0;
	_vao = 0; _vbo = 0; _ebo = 0;
	_shaderProg = 0;
	_scale = glm::vec3(1.0f, 1.0f, 1.0f);
	_color = glm::vec4(0.5f, 0.5f, 0.5f, 1.0f);
	_pos = glm::vec3(0.0f, 0.0f, 0.0f);
	_rotAngle = 0.0f;
	_rotAxis = glm::vec3(0.0f,1.0f,0.0f);
	_bRotation = _bScale = _bPos = _bTransform = _bOnTransform = false;
	_mxScale = glm::mat4(1.0f);
	_mxTrans = glm::mat4(1.0f);
	_mxRotation = glm::mat4(1.0f);
	_mxTRS = glm::mat4(1.0f);
	_mxTransform = glm::mat4(1.0f);
	_mxFinal = glm::mat4(1.0f);
	_colorLoc = _modelMxLoc = 0;
	_points = nullptr; _idx = nullptr;
	_uShadingMode = 1; // 預設上色模式，1 : vertex color, 2: uniform color(object color)
	_bObjColor = false; // 預設不使用物件顏色
	_shadingModeLoc = 0; // 上色模式的進入點
	_diffMapLoc = -1;  _lightMapLoc = -1; _normalMapLoc = -1; _cubeMapLoc = -1;
}

CShape::~CShape()
{

}

void CShape::setupVertexAttributes()
{
	// 設定 VAO、VBO 與 EBO
	glGenVertexArrays(1, &_vao);
	glGenBuffers(1, &_vbo);
	glGenBuffers(1, &_ebo);

	// Bind the Vertex Array Object first, then bind and set vertex buffer(s) and attribute pointer(s).
	glBindVertexArray(_vao);

	// 設定 VBO
	glBindBuffer(GL_ARRAY_BUFFER, _vbo);
	glBufferData(GL_ARRAY_BUFFER, _vtxCount * _vtxAttrCount * sizeof(float), _points, GL_STATIC_DRAW);

	// 設定 EBO
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, _idxCount * sizeof(GLuint), _idx, GL_STATIC_DRAW);

	// 位置屬性
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, _vtxAttrCount * sizeof(float), BUFFER_OFFSET(0));
	glEnableVertexAttribArray(0);

	// 顏色屬性
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, _vtxAttrCount * sizeof(float), BUFFER_OFFSET(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	//法向量屬性
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, _vtxAttrCount * sizeof(float), BUFFER_OFFSET(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	//貼圖座標屬性
	glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, _vtxAttrCount * sizeof(float), BUFFER_OFFSET(9 * sizeof(float)));
	glEnableVertexAttribArray(3);

	//light map 座標屬性
	glVertexAttribPointer(4, 2, GL_FLOAT, GL_FALSE, _vtxAttrCount * sizeof(float), BUFFER_OFFSET(11 * sizeof(float)));
	glEnableVertexAttribArray(4);

	// tangent 向量屬性
	glVertexAttribPointer(5, 3, GL_FLOAT, GL_FALSE, _vtxAttrCount * sizeof(float), BUFFER_OFFSET(13 * sizeof(float)));
	glEnableVertexAttribArray(5);

	// bitangent 向量屬性
	glVertexAttribPointer(6, 3, GL_FLOAT, GL_FALSE, _vtxAttrCount * sizeof(float), BUFFER_OFFSET(16 * sizeof(float)));
	glEnableVertexAttribArray(6);

	glBindVertexArray(0); // 解除對 VAO 的綁定
}

void CShape::setShaderID(GLuint shaderID, int shadeingmode)
{
	_shaderProg = shaderID;
	_uShadingMode = shadeingmode;
	glUseProgram(_shaderProg);
	_modelMxLoc = glGetUniformLocation(_shaderProg, "mxModel"); 	// 取得 mxModel 變數的位置
	glUniformMatrix4fv(_modelMxLoc, 1, GL_FALSE, glm::value_ptr(_mxTRS));
	_shadingModeLoc = glGetUniformLocation(_shaderProg, "uShadingMode"); 	// 取得 iColorType 變數的位置
	glUniform1i(_shadingModeLoc, _uShadingMode);

	_diffMapLoc = glGetUniformLocation(_shaderProg, "uDiffuseMap");
	glUniform1i(_diffMapLoc, 0); // uDiffuseMap 對應 GL_TEXTURE0
	_lightMapLoc = glGetUniformLocation(_shaderProg, "uLightMap");
	glUniform1i(_lightMapLoc, 1); // uLightMap 對應 GL_TEXTURE1
	_normalMapLoc = glGetUniformLocation(_shaderProg, "uNormalMap");
	glUniform1i(_normalMapLoc, 2); // uNormalMap 對應 GL_TEXTURE2
	_cubeMapLoc = glGetUniformLocation(_shaderProg, "uEnvMap");
	glUniform1i(_cubeMapLoc, 3); // uEnvMap 對應 GL_TEXTURE3

	// 取得 uniform 變數 uTextureFlags 位置
	_texFlagsLoc = glGetUniformLocation(_shaderProg, "uTextureFlags");
	glUniform1i(_texFlagsLoc, _textureFlags); // 預設 TEX_NONE
}

void CShape::setColor(glm::vec4 vColor)
{
	// 透過 glUniform 傳入模型的顏色
	_color = vColor;
	_colorLoc = glGetUniformLocation(_shaderProg, "ui4Color"); 	// 取得 ui4Color 變數的位置
	glUniform4fv(_colorLoc, 1, glm::value_ptr(_color));
	_uShadingMode = 2; // 設定上色模式為 uniform color
	glUniform1i(_shadingModeLoc, _uShadingMode);
	_bObjColor = true;
}

void CShape::draw() {
	//glUseProgram(_shaderProg);
	//updateMatrix();
	//glBindVertexArray(_vao);
	// 預設不呼叫實際的繪製指令，由子類別決定是否直接呼叫 glDrawElements()
}

void CShape::drawRaw()
{

}

void CShape::update(float dt)
{
	
}

void CShape::genTangentBitangent()
{
	// 計算每一個頂點的 tangent 與 bitangent 向量
	std::vector<glm::vec3> tangents(_vtxCount, glm::vec3(0.0f));
	std::vector<glm::vec3> bitangents(_vtxCount, glm::vec3(0.0f));
	for (int i = 0; i < _idxCount; i += 3) {
		unsigned i0 = _idx[i + 0], i1 = _idx[i + 1], i2 = _idx[i + 2];
		glm::vec3 p0 = glm::vec3(_points[i0 * _vtxAttrCount + 0], _points[i0 * _vtxAttrCount + 1], _points[i0 * _vtxAttrCount + 2]);
		glm::vec3 p1 = glm::vec3(_points[i1 * _vtxAttrCount + 0], _points[i1 * _vtxAttrCount + 1], _points[i1 * _vtxAttrCount + 2]);
		glm::vec3 p2 = glm::vec3(_points[i2 * _vtxAttrCount + 0], _points[i2 * _vtxAttrCount + 1], _points[i2 * _vtxAttrCount + 2]);
		glm::vec2 uv0 = glm::vec2(_points[i0 * _vtxAttrCount + 9], _points[i0 * _vtxAttrCount + 10]);
		glm::vec2 uv1 = glm::vec2(_points[i1 * _vtxAttrCount + 9], _points[i1 * _vtxAttrCount + 10]);
		glm::vec2 uv2 = glm::vec2(_points[i2 * _vtxAttrCount + 9], _points[i2 * _vtxAttrCount + 10]);
		// auto& p0 = positions[i0], & p1 = positions[i1], & p2 = positions[i2];
		// auto& uv0 = uvs[i0], &uv1 = uvs[i1], & uv2 = uvs[i2];
		glm::vec3 e1 = p1 - p0, e2 = p2 - p0;
		glm::vec2 d1 = uv1 - uv0, d2 = uv2 - uv0;
		float f = 1.0f / (d1.x * d2.y - d2.x * d1.y);
		glm::vec3 triT = f * (e1 * d2.y - e2 * d1.y);
		glm::vec3 triB = f * (-e1 * d2.x + e2 * d1.x);
		tangents[i0] += triT; tangents[i1] += triT; tangents[i2] += triT;
		bitangents[i0] += triB; bitangents[i1] += triB; bitangents[i2] += triB;
	}
	// 這之前對應投影片 144 頁所導出的 T 與 B 的計算公式
	// 
	// 4. Gram–Schmidt 正交化 & 規一化，重建 bitangent
	//    將 TBN 變成標準正交基底
	for (int i = 0; i < _vtxCount; ++i) {
		// glm::vec3 n = normals[i];
		glm::vec3 n = glm::vec3(_points[i * _vtxAttrCount + 6], _points[i * _vtxAttrCount + 7], _points[i * _vtxAttrCount + 8]);
		glm::vec3 t = tangents[i];
		t = glm::normalize(t - n * glm::dot(n, t));
		glm::vec3 b = glm::cross(n, t);
		tangents[i] = t;
		bitangents[i] = b;
	}

	// 5. 把計算好的 tangent/bitangent 寫回 _points
	for (int i = 0; i < _vtxCount; ++i) {
		int base = i * _vtxAttrCount;
		// tangent
		_points[base + 13] = tangents[i].x;
		_points[base + 14] = tangents[i].y;
		_points[base + 15] = tangents[i].z;
		// bitangent
		_points[base + 16] = bitangents[i].x;
		_points[base + 17] = bitangents[i].y;
		_points[base + 18] = bitangents[i].z;
	}
}

void CShape::setScale(glm::vec3 vScale)
{
	_scale = vScale;
	_bScale = true;
	_mxScale = glm::scale(glm::mat4(1.0f), _scale);
}

void CShape::setPos(glm::vec3 vPt)
{
	_pos = vPt;
	_bPos = true;
	_mxTrans = glm::translate(glm::mat4(1.0f), _pos);
}

glm::vec3 CShape::getPos() { return _pos; }
glm::mat4 CShape::getTransMatrix() { return _mxTrans; }


void CShape::setRotate(float angle, const glm::vec3& axis)
{
	float rad = glm::radians(angle); 	// glm::angleAxis 需要弧度
	_mxRotation = glm::rotate(glm::mat4(1.0f), rad, glm::normalize(axis)); // 計算旋轉軸下旋轉角度所對應旋轉矩陣
	_bRotation = true;
}
void CShape::updateRotate(float angle, const glm::vec3& axis)
{
	float rad = glm::radians(angle); 	// glm::angleAxis 需要弧度
	_mxRotation *= glm::rotate(glm::mat4(1.0f), rad, glm::normalize(axis)); // 計算旋轉軸下旋轉角度所對應旋轉矩陣
	_bRotation = true;
}

void CShape::updateMatrix()
{
	if (_bScale || _bPos || _bRotation )
	{
		_mxTRS = _mxTrans * _mxRotation * _mxScale;
		if (_bOnTransform == true) _mxFinal = _mxTransform * _mxTRS;
		else _mxFinal = _mxTRS;
		_bScale = _bPos = _bRotation = false;
	}
	if (_bTransform) {
		_mxFinal = _mxTransform * _mxTRS;
		_bTransform = false;
	}
	// 如多個模型使用相同的 shader program,因每一個模型的 mxTRS 都不同，所以每個frame都要更新
	glUniformMatrix4fv(_modelMxLoc, 1, GL_FALSE, glm::value_ptr(_mxFinal));
}

void CShape::setTransformMatrix(glm::mat4 mxMatrix)
{
	_bOnTransform = _bTransform = true;
	_mxTransform = mxMatrix;
}

glm::mat4 CShape::getModelMatrix() { return _mxFinal; }
GLuint CShape::getShaderProgram() { return _shaderProg; }

void CShape::reset()
{
	_scale = glm::vec3(1.0f, 1.0f, 1.0f);
	_color = glm::vec4(0.5f, 0.5f, 0.5f, 1.0f);
	_pos = glm::vec3(0.0f, 0.0f, 0.0f);
	_rotAngle = 0.0f;
	_rotAxis = glm::vec3(0.0f, 1.0f, 0.0f);

	_bRotation = _bScale = _bPos = _bTransform = _bOnTransform = false;
	_mxScale = glm::mat4(1.0f);
	_mxTrans = glm::mat4(1.0f);
	_mxTRS = glm::mat4(1.0f);
	_mxRotation = glm::mat4(1.0f);
	_mxTransform = glm::mat4(1.0f);
	_mxFinal = glm::mat4(1.0f);
	_uShadingMode = 1; // 預設上色模式，1 : vertex color, 2: uniform color(object color)
	_bObjColor = false; // 預設不使用物件顏色
}


void CShape::setMaterial(const CMaterial& material) {
	_material = material;
}

void CShape::uploadMaterial()  {
	_material.uploadToShader(_shaderProg, "uMaterial");
}

//_vtxCount = _vtxAttrCount
// 控制與改變貼圖座標
void CShape::setMirror(bool uAxis, bool vAxis) // U軸 與 V軸 是否要鏡射
{
	int i;
	if (uAxis) {
		if (vAxis) { // U V 軸同時鏡射
			for (i = 0; i < _vtxCount*_vtxAttrCount; i += _vtxAttrCount) { // 將每一個 vertex 的貼圖座標用 1 去減
				_points[i+9] *= -1.0f;  _points[i+10] *= -1.0f;
			}
		}
		else { // 只有 U 軸鏡射
			for (i = 0; i < _vtxCount * _vtxAttrCount; i += _vtxAttrCount) { // 將每一個 vertex 的貼圖座標用 1 去減
				_points[i + 10] *= -1.0f; // x 就是 U 軸
			}
		}
	}
	else if (vAxis) { // 只有 V 軸鏡射
		for (i = 0; i < _vtxCount * _vtxAttrCount; i += _vtxAttrCount) { // 將每一個 vertex 的貼圖座標用 1 去減
			_points[i + 9] *= -1.0f; // y 為 V 軸
		}
	}
	else;
	glBindBuffer(GL_ARRAY_BUFFER, _vbo);
	glBufferData(GL_ARRAY_BUFFER, _vtxCount * _vtxAttrCount * sizeof(float), _points, GL_STATIC_DRAW);
	glBindVertexArray(0);
}

void CShape::setTiling(float uTiling, float vTiling)  // 對 U軸 與 V軸 進行拼貼的縮放
{
	int i;
	for (i = 0; i < _vtxCount * _vtxAttrCount; i += _vtxAttrCount) { 
		_points[i + 9] *= uTiling;  _points[i + 10] *= vTiling;  // diffuse 貼圖對應的儲存位置
	}
	glBindBuffer(GL_ARRAY_BUFFER, _vbo);
	glBufferData(GL_ARRAY_BUFFER, _vtxCount * _vtxAttrCount * sizeof(float), _points, GL_STATIC_DRAW);
	glBindVertexArray(0);
}

void CShape::setLightMapTiling(float uTiling, float vTiling)  // 對 LightMap U軸 與 V軸 進行拼貼的縮放
{
	int i;
	for (i = 0; i < _vtxCount * _vtxAttrCount; i += _vtxAttrCount) { // 將每一個 vertex 的貼圖座標用 1 去減
		_points[i + 11] *= uTiling;  _points[i + 12] *= vTiling;
	}
	glBindBuffer(GL_ARRAY_BUFFER, _vbo);
	glBufferData(GL_ARRAY_BUFFER, _vtxCount * _vtxAttrCount * sizeof(float), _points, GL_STATIC_DRAW);
	glBindVertexArray(0);
}
void CShape::setTextureMode(int flags)
{
	_textureFlags = flags;
}

void CShape::uploadTextureFlags()
{
	glUniform1i(_texFlagsLoc, _textureFlags);
}