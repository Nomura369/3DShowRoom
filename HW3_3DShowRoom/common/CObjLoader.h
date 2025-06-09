#pragma once
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <glm/glm.hpp>

struct Triangle { // 代表構成模型的每個三角形
	glm::vec3 v0, v1, v2; // 三角形的三個頂點
	glm::vec3 vn0, vn1, vn2; // 三個頂點的法向量
	glm::vec3 vt0, vt1, vt2; // 三個頂點的貼圖座標
};

class CObjLoader // Singleton 
{
public:
    static CObjLoader& getInstance() {
        static CObjLoader instance;
        return instance;
    }

	bool loadObj(const std::string& filename, std::vector<Triangle>& triangles);

private:
    CObjLoader();
};
