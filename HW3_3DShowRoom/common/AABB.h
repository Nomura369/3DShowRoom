#pragma once

#include <glm/glm.hpp>
#include <iostream>

enum CollisionTag { // 定義適用範圍（被碰撞 vs. 碰撞者）
    ROOM_USER = 0,
    WALL_BULLET = 1,
};

struct AABB {
    glm::vec3 minPos; // 房間們的左下角（或最低點）
    glm::vec3 maxPos; // 房間們的右上角（或最高點）

    bool checkCollision(const glm::vec3& pos, const int tag) {
        if (tag == ROOM_USER) {
            if ((pos.x >= minPos.x && pos.x <= maxPos.x) &&
                (pos.y >= minPos.y && pos.y <= maxPos.y) &&
                (pos.z >= minPos.z && pos.z <= maxPos.z)) {
                return false;
            }
            else {
                std::cout << "你碰到牆了（如果移動鍵失靈可以先縮放後再試一次）" << std::endl << std::endl;
                return true;
            }
        }
        else if (tag == WALL_BULLET) {
            if ((pos.x >= minPos.x && pos.x <= maxPos.x) &&
                (pos.y >= minPos.y && pos.y <= maxPos.y) &&
                (pos.z >= minPos.z && pos.z <= maxPos.z)) {
                return true;
            }
            else {
                return false;
            }
        }

        std::cout << "未定義 AABB 的碰撞類別" << std::endl << std::endl;
        return false;
    }
};