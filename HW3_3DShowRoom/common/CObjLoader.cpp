#include "CObjLoader.h"

CObjLoader::CObjLoader() {
    // 即使沒有特別初始化東西，還是要寫一下
}

bool CObjLoader::loadObj(const std::string& filename, std::vector<Triangle>& triangles) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "無法開啟檔案：" << filename << std::endl;
        return false;
    }

    std::vector<glm::vec3> vertices; // 暫存讀進來的頂點位置
    std::vector<glm::vec3> normals; // 暫存讀進來的頂點法向量
    std::vector<glm::vec3> textures; // 暫存讀進來的頂點貼圖座標
    std::string line; // obj 檔中的每一行資料

    while (std::getline(file, line)) {
        // 解析字串
        std::istringstream iss(line);
        std::string prefix;
        iss >> prefix;

        // 讀入頂點位置、貼圖座標與法向量
        if (prefix == "v") {
            float x, y, z;
            iss >> x >> y >> z;
            vertices.emplace_back(x, y, z);
        }
        else if (prefix == "vt") {
            float x, y, z;
            iss >> x >> y >> z;
            textures.emplace_back(x, y, z);
        }
        else if (prefix == "vn") {
            float x, y, z;
            iss >> x >> y >> z;
            normals.emplace_back(x, y, z);
        }
        // 讀入面（支援三角面）
        else if (prefix == "f") {
            // f 的格式為 v/ vt/ vn，三串一組
            int vIdx[3] = { -1, -1, -1 };
            int vnIdx[3] = { -1, -1, -1 };
            int vtIdx[3] = { -1, -1, -1 };

            for (int i = 0; i < 3; ++i) {
                // 解析字串（假設都沒有無效資料）
                std::string vertStr;
                iss >> vertStr;

                std::istringstream viss(vertStr);
                std::string idxStr;
                
                // 讀頂點索引（v）
                if (std::getline(viss, idxStr, '/')) {
                    if (!idxStr.empty())
                        vIdx[i] = std::stoi(idxStr) - 1; // obj 檔的索引從 1 開始
                }

                // 讀貼圖座標索引（vt）
                if (std::getline(viss, idxStr, '/')) {
                    if (!idxStr.empty())
                        vtIdx[i] = std::stoi(idxStr) - 1;
                }

                // 讀法向量索引（vn）
                if (std::getline(viss, idxStr, '/')) {
                    if (!idxStr.empty())
                        vnIdx[i] = std::stoi(idxStr) - 1;
                }
            }

            Triangle tri;
            tri.v0 = vertices[vIdx[0]];
            tri.v1 = vertices[vIdx[1]];
            tri.v2 = vertices[vIdx[2]];
            tri.vt0 = (vtIdx[0] >= 0 && vtIdx[0] < textures.size()) ? textures[vtIdx[0]] : glm::vec3(0.0f);
            tri.vt1 = (vtIdx[1] >= 0 && vtIdx[1] < textures.size()) ? textures[vtIdx[1]] : glm::vec3(0.0f);
            tri.vt2 = (vtIdx[2] >= 0 && vtIdx[2] < textures.size()) ? textures[vtIdx[2]] : glm::vec3(0.0f);
            tri.vn0 = (vnIdx[0] >= 0 && vnIdx[0] < normals.size()) ? normals[vnIdx[0]] : glm::vec3(0.0f);
            tri.vn1 = (vnIdx[1] >= 0 && vnIdx[1] < normals.size()) ? normals[vnIdx[1]] : glm::vec3(0.0f);
            tri.vn2 = (vnIdx[2] >= 0 && vnIdx[2] < normals.size()) ? normals[vnIdx[2]] : glm::vec3(0.0f);

            triangles.push_back(tri);
        }
    }

    file.close();
    return true;
}