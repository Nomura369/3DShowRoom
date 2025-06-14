#include <iostream>
#include <glew/include/GL/glew.h>
#include <glm/gtc/type_ptr.hpp>
#include <array>

#include "CCamera.h"
#include "wmhandler.h"
#include "arcball.h"
#include "CMaterial.h"
#include "CLight.h"
#include "CButton.h"

#include "../models/CQuad.h"
#include "../models/CCube.h"
#include "../models/CSphere.h"
#include "../models/CTeapot.h"

//#define SPOT_TARGET

extern Arcball g_arcball;

bool  g_bCamRoting = false; // 用於鏡頭的旋轉
bool  g_bfirstMouse = true;   // 滑鼠左鍵首次按下，預設為 true
float g_lastX = 400, g_lastY = 400; // 滑鼠移動的距離
float g_mouseSens = 0.005f;   // 位移鏡頭的靈敏度

extern CCube g_centerloc;
extern GLuint g_shadingProg;
extern glm::vec3 g_eyeloc;
//extern CLight g_light;

extern CMaterial g_matWaterGreen;
extern CSphere  g_sphere; 

//extern std::array<CButton, 4> g_button;
extern bool g_isNpr;
extern bool g_isGradient;
extern float g_colorTime;

Arcball g_arcball; //保留未用

// 房間邊界設定
struct RoomAABB {
    glm::vec3 roomMin; // 房間的左下角（或最低點）
    glm::vec3 roomMax; // 房間的右上角（或最高點）

    bool isInsideRoom(const glm::vec3& pos) {
        if ((pos.x >= roomMin.x && pos.x <= roomMax.x) &&
            (pos.y >= roomMin.y && pos.y <= roomMax.y) &&
            (pos.z >= roomMin.z && pos.z <= roomMax.z)) {
            return true;
        }
        else {
            std::cout << "碰到牆了（如果移動鍵失靈可以先縮放後再試一次）" << std::endl << std::endl;
            return false;
        }  
    }
};
RoomAABB theRoom = {
    glm::vec3(-15.0f, -15.0f, -11.0f), // roomMin
    glm::vec3(15.0f, 15.0f, 11.0f), // roomMax
};

// 位移用函式（不然程式碼太雜了）
void moveForward(bool isForward) {
    float speed = 0.1f;
    float direction = isForward ? 1.0f : -1.0f;

    glm::vec3 g_eyeloc = CCamera::getInstance().getViewLocation();
    glm::vec3 centerPos = g_centerloc.getPos();

    glm::vec3 front = glm::normalize(centerPos - g_eyeloc); // 眼睛指向目標方向

    glm::vec3 eyeloc = g_eyeloc + front * speed * direction;
    glm::vec3 centerloc = centerPos + front * speed * direction;

    /*if (theRoom.isInsideRoom(eyeloc) && theRoom.isInsideRoom(centerloc)) {
        g_eyeloc = eyeloc;
        g_centerloc.setPos(centerloc);
    }*/
    g_eyeloc = eyeloc;
    g_centerloc.setPos(centerloc);

    // 更新攝影機與 view matrix
    CCamera::getInstance().updateViewCenter(g_eyeloc, g_centerloc.getPos());
    glm::mat4 mxView = CCamera::getInstance().getViewMatrix();
    GLint viewLoc = glGetUniformLocation(g_shadingProg, "mxView");
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(mxView));
}
void moveRight(bool isRight) {
    float speed = 0.1f;
    float direction = isRight ? 1.0f : -1.0f;

    glm::vec3 g_eyeloc = CCamera::getInstance().getViewLocation();
    glm::vec3 centerPos = g_centerloc.getPos();

    glm::vec3 front = glm::normalize(centerPos - g_eyeloc); // 鏡頭看向的方向
    glm::vec3 up = CCamera::getInstance().getUpVector();
    glm::vec3 right = glm::normalize(glm::cross(front, up)); // 右向量

    glm::vec3 eyeloc = g_eyeloc + right * speed * direction;
    glm::vec3 centerloc = centerPos + right * speed * direction;

    /*if (theRoom.isInsideRoom(eyeloc) && theRoom.isInsideRoom(centerloc)) {
        g_eyeloc = eyeloc;
        g_centerloc.setPos(centerloc);
    }*/
    g_eyeloc = eyeloc;
    g_centerloc.setPos(centerloc);

    // 更新攝影機與 view matrix
    CCamera::getInstance().updateViewCenter(g_eyeloc, g_centerloc.getPos());
    glm::mat4 mxView = CCamera::getInstance().getViewMatrix();
    GLint viewLoc = glGetUniformLocation(g_shadingProg, "mxView");
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(mxView));
}


// 滑鼠按鈕按下後 callback function(回呼函式) ---------------
void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
    double xpos, ypos;
    glfwGetCursorPos(window, &xpos, &ypos);
    int width, height;
    glfwGetWindowSize(window, &width, &height);
    // Arcball* arcball = static_cast<Arcball*>(glfwGetWindowUserPointer(window));
    g_arcball.onMouseButton(button, action, xpos, ypos);
    //std::cout << "button = " << button << "action = " << action << "mods = " << mods << std::endl;
    if (button == GLFW_MOUSE_BUTTON_LEFT )
    {
		if (action == GLFW_PRESS)
		{
            g_bCamRoting = true;
            // 啟動或關閉按鈕們
            /*if (g_button[0].handleClick((float)xpos, height - (float)ypos)) {  }
            if (g_button[1].handleClick((float)xpos, height - (float)ypos)) {  }
            if (g_button[2].handleClick((float)xpos, height - (float)ypos)) {  }
            if (g_button[3].handleClick((float)xpos, height - (float)ypos)) {  }*/
		}
        else if (action == GLFW_RELEASE)
        {
            g_bCamRoting = false; 
			g_bfirstMouse = true; // 滑鼠釋放時，重新設定滑鼠左鍵尚未按下
        }      
    }
}
// ---------------------------------------------------------------------------------------

// 滑鼠在指定視窗上移動時的 callback function (回呼函式)
void cursorPosCallback(GLFWwindow* window, double xpos, double ypos) {
    int width, height;
    glfwGetWindowSize(window, &width, &height);
    g_arcball.onCursorMove(xpos, ypos, width, height);
    // Arcball* arcball = static_cast<Arcball*>(glfwGetWindowUserPointer(window));
    //std::cout << "x = " << xpos << "y = " << ypos << std::endl;
    if ( g_bCamRoting )  // 進入控制鏡頭模式
    {
        if (g_bfirstMouse) {  // 左鍵首次按下
            g_lastX = (float)xpos; g_lastY = (float)ypos; g_bfirstMouse = false;
            return;
        }
        float deltaX = ((float)xpos - g_lastX);
        float deltaY = ((float)ypos - g_lastY);
        g_lastX = (float)xpos; g_lastY = (float)ypos;

        CCamera::getInstance().processMouseMovement(deltaX, deltaY, g_mouseSens);
        glm::mat4 mxView = CCamera::getInstance().getViewMatrix();
        GLint viewLoc = glGetUniformLocation(g_shadingProg, "mxView"); 	// 取得 view matrix 變數位置
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(mxView));
        g_eyeloc = CCamera::getInstance().getViewLocation();
    }
}
// ---------------------------------------------------------------------------------------
// 當視窗大小改變的 callback function
void framebufferSizeCallback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

// ---------------------------------------------------------------------------------------
void scrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
    // yoffset 滑鼠滾輪移動的垂直位移
    // 以 yoffset 來改變 eye 與 center 間的距離，改變 radius 即可

    CCamera::getInstance().updateRadius((float)yoffset * -0.2f);
    glm::mat4 mxView = CCamera::getInstance().getViewMatrix();
    GLint viewLoc = glGetUniformLocation(g_shadingProg, "mxView"); 	// 取得 mxView 變數的位置
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(mxView));
    g_eyeloc = CCamera::getInstance().getViewLocation();

    //std::cout << "Scroll event: xoffset = " << xoffset << ", yoffset = " << yoffset << std::endl;
}

// 當鍵盤按鍵按下時的 callback function (回呼函式)
// key : GLFW_KEY_0、 GLFW_KEY_a、GLFW_KEY_ESCAPE、GLFW_KEY_SPACE
//       完整定義在這邊 https://www.glfw.org/docs/latest/group__keys.html
// action : 
//          GLFW_PRESS：按鍵被按下。
//          GLFW_RELEASE：按鍵被釋放。
//          GLFW_REPEAT：按鍵被重複觸發（長按時會觸發多次）
// mods : 
//          GLFW_MOD_SHIFT：Shift 鍵被按下。
//          GLFW_MOD_CONTROL：Ctrl 鍵被按下。
//          GLFW_MOD_ALT：Alt 鍵被按下。
//          GLFW_MOD_SUPER：Windows 鍵或 Command 鍵被按下。
//          GLFW_MOD_CAPS_LOCK：Caps Lock 鍵被啟用。
//          GLFW_MOD_NUM_LOCK：Num Lock 鍵被啟用。
// 
//  注意：1. 一個按鍵按下到放開會產生兩次 keyCallback，
//           根據功能需求判斷是按下或是放掉去執行對應的功能
//        2. 使用 glfw 英文字母沒有分大小寫，都是傳回大寫的 ASCII 編碼
//           搭配檢查左右 shift 鍵是否按下是大寫還是小寫(假設 caps 鍵沒有被按下)
//       
void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    // 紅色系照明（依照 ambient、diffuse 和 specular 的順序）
    glm::vec4 reds[3] = { glm::vec4(0.15f, 0.05f, 0.05f, 1.0f),
                          glm::vec4(0.8f, 0.2f, 0.2f, 1.0f),
                          glm::vec4(0.9f, 0.4f, 0.4f, 1.0f)
    };
    // 綠色系照明
    glm::vec4 greens[3] = { glm::vec4(0.05f, 0.15f, 0.05f, 1.0f),
                            glm::vec4(0.2f, 0.8f, 0.2f, 1.0f),
                            glm::vec4(0.4f, 0.9f, 0.4f, 1.0f)
    };
    // 藍色系照明
    glm::vec4 blues[3] = { glm::vec4(0.05f, 0.05f, 0.15f, 1.0f),
                           glm::vec4(0.2f, 0.2f, 0.8f, 1.0f),
                           glm::vec4(0.4f, 0.4f, 0.9f, 1.0f)
    };

    switch (key)
    {
    case GLFW_KEY_ESCAPE:
        if (action == GLFW_PRESS) { glfwSetWindowShouldClose(window, true); }
        break;
    case GLFW_KEY_SPACE:
        break;
    default: // 針對英文字母大小寫進行處理
        if (action == GLFW_PRESS || action == GLFW_REPEAT) {
            // 檢查 Shift 鍵(左右兩邊各一個)是否被按下
            bool isShiftPressed = (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) ||
                (glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS);
            // 判斷字母鍵的大小寫，預設使用鍵盤是沒有按下 CAPS 鍵
            if (key >= GLFW_KEY_A && key <= GLFW_KEY_Z) {
                char letter = (isShiftPressed) ? ('A' + (key - GLFW_KEY_A)) : ('a' + (key - GLFW_KEY_A));
                //std::cout << "key = " << letter << std::endl;
                switch (letter) {
                case 'W':
                case 'w':
                    CCamera::getInstance().setIsWalking(true);
                    moveForward(true);
                    break;
                case 'S':
                case 's':
                    CCamera::getInstance().setIsWalking(true);
                    moveForward(false);
                    break;
                case 'A':
                case 'a':
                    CCamera::getInstance().setIsWalking(true);
                    moveRight(false);
                    break;
                case 'D':
                case 'd':
                    CCamera::getInstance().setIsWalking(true);
                    moveRight(true);
                    break;
                case 'N':
                case 'n':
                    // 切換照明風格（是否為卡通）
                    g_isNpr = !g_isNpr;
                    if (g_isNpr) {
                        std::cout << "目前為 NPR 模式" << std::endl << std::endl;
                    }
                    else {
                        std::cout << "目前為 Per-Pixel Lighting 模式" << std::endl << std::endl;
                    }
                    break;
                    //case 'C':
                    //case 'c':
                    //    // 切換位移視角
                    //    g_isCameraBasedMoving = !g_isCameraBasedMoving;
                    //    if (g_isCameraBasedMoving) {
                    //        std::cout << "目前以「鏡頭方向」進行位移" << std::endl << std::endl;
                    //    }
                    //    else {
                    //        std::cout << "目前以「世界座標方向」進行位移" << std::endl << std::endl;
                    //    }
                    //    break;
                    //case 'R':
                    //case 'r':
                    //    if (!g_isGradient) {
                    //        // 將房間換成紅色系照明
                    //        g_light.setAmbient(reds[0]);
                    //        g_light.setDiffuse(reds[1]);
                    //        g_light.setSpecular(reds[2]);
                    //        std::cout << "紅色系房間" << std::endl << std::endl;
                    //    }
                    //    break;
                    //case 'G':
                    //case 'g':
                    //    if (!g_isGradient) {
                    //        // 將房間換成綠色系照明
                    //        g_light.setAmbient(greens[0]);
                    //        g_light.setDiffuse(greens[1]);
                    //        g_light.setSpecular(greens[2]);
                    //        std::cout << "綠色系房間" << std::endl << std::endl;
                    //    }
                    //    break;
                    //case 'B':
                    //case 'b':
                    //    if (!g_isGradient) {
                    //        // 將房間換成藍色系照明
                    //        g_light.setAmbient(blues[0]);
                    //        g_light.setDiffuse(blues[1]);
                    //        g_light.setSpecular(blues[2]);
                    //        std::cout << "藍色系房間" << std::endl << std::endl;
                    //    }
                    //    break;
                    //case 'H':
                    //case 'h':
                    //    // 將房間換成預設照明
                    //    if (!g_isGradient) {
                    //        g_light.setAmbient(glm::vec4(0.1f));
                    //        g_light.setDiffuse(glm::vec4(0.8f));
                    //        g_light.setSpecular(glm::vec4(1.0f));
                    //        std::cout << "預設房間" << std::endl << std::endl;
                    //    }
                    //    break;
                case 'L':
                case 'l':
                    // 是否漸變照明色彩
                    g_isGradient = !g_isGradient;
                    if (!g_isGradient) {
                        g_colorTime = 0.0f;
                        std::cout << "關閉自動變色" << std::endl << std::endl;
                    }
                    else std::cout << "開啟自動變色" << std::endl << std::endl;
                    break;
                }
            }
        }
        else if (action == GLFW_RELEASE) {
            // 檢查 Shift 鍵(左右兩邊各一個)是否被按下
            bool isShiftPressed = (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) ||
                (glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS);
            // 判斷字母鍵的大小寫，預設使用鍵盤是沒有按下 CAPS 鍵
            if (key >= GLFW_KEY_A && key <= GLFW_KEY_Z) {
                char letter = (isShiftPressed) ? ('A' + (key - GLFW_KEY_A)) : ('a' + (key - GLFW_KEY_A));
                //std::cout << "key = " << letter << std::endl;
                switch (letter) {
                case 'W':
                case 'w':
                case 'S':
                case 's':
                case 'A':
                case 'a':
                case 'D':
                case 'd':
                    CCamera::getInstance().setIsWalking(false);
                    break;
                }
            }

        }
    }
}