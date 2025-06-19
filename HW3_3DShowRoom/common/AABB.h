#pragma once

#include <glm/glm.hpp>
#include <iostream>

enum CollisionTag { // �w�q�A�νd��]�Q�I�� vs. �I���̡^
    ROOM_USER = 0,
    WALL_BULLET = 1,
};

struct AABB {
    glm::vec3 minPos; // �ж��̪����U���]�γ̧C�I�^
    glm::vec3 maxPos; // �ж��̪��k�W���]�γ̰��I�^

    bool checkCollision(const glm::vec3& pos, const int tag) {
        if (tag == ROOM_USER) {
            if ((pos.x >= minPos.x && pos.x <= maxPos.x) &&
                (pos.y >= minPos.y && pos.y <= maxPos.y) &&
                (pos.z >= minPos.z && pos.z <= maxPos.z)) {
                return false;
            }
            else {
                std::cout << "�A�I����F�]�p�G�����䥢�F�i�H���Y���A�դ@���^" << std::endl << std::endl;
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

        std::cout << "���w�q AABB ���I�����O" << std::endl << std::endl;
        return false;
    }
};