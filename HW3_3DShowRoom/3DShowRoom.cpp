// 計分

//#define GLM_ENABLE_EXPERIMENTAL 1

#include <iostream>
#include <fstream>
#include <sstream>
#include <array>

#include <glew/include/GL/glew.h>
#include <glfw/include/GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>
//#include <glm/gtx/string_cast.hpp>

#include "common/initshader.h"
#include "common/arcball.h"
#include "common/wmhandler.h"
#include "common/CCamera.h"
#include "common/CShaderPool.h"
#include "common/CTexturePool.h"
#include "../models/CQuad.h"
#include "../models/CTorusKnot.h"
#include "../models/CBox.h"
#include "../models/CSphere.h"
#include "../models/CCapsule.h"
#include "../models/CCup.h"
#include "../models/CShape.h"

#include "common/CLight.h"
#include "common/CMaterial.h"
#include "../models/CObjModel.h"
#include "common/CButton.h"

#define SCREEN_WIDTH  800
#define SCREEN_HEIGHT 800
#define ROOM_NUM 6

// 一般模型宣告區
//CCapsule g_capsule;
//CCup g_cup;
//CTorusKnot g_knot(4);
CBox g_room[ROOM_NUM];
CQuad g_walls[ROOM_NUM * 4]; // 所有房間的牆面
CQuad g_floor[ROOM_NUM]; // 所有房間的地板
CSphere g_sphere;

// obj 檔模型宣告區
//CObjModel g_objModel;

// 鏡頭與場景宣告區
glm::vec3 g_eyeloc(6.0f, 6.0f, 6.0f); // 鏡頭位置, 預設在 (8,8,8) 
CCube g_centerloc; // view center 預設在 (0,0,0)，不做任何描繪操作

GLuint g_shadingProg;
GLuint g_uiShadingProg;

// 全域光源 (位置在 5,5,0)
CLight g_light(glm::vec3(5.0f, 5.0f, 0.0f)); // 預設為點光源
//CLight g_capSpotLight(glm::vec3(10.0f, 10.0f, -10.0f), glm::vec3(8.0f, 0.5f, -8.0f)); // 照亮 capsule 模型
//CLight g_cupSpotLight(glm::vec3(-10.0f, 10.0f, -10.0f), glm::vec3(-8.0f, 0.5f, -8.0f)); // 照亮 cup 模型
//CLight g_knotSpotLight(glm::vec3(0.0f, 10.0f, 10.0f), glm::vec3(0.0f, 0.5f, 8.0f)); // 照亮 knot 模型
//bool g_bOnBtnActive[4] = { false, false, false, false }; // 判斷按鈕們是否被按下

// 全域材質（可依模型分別設定）
CMaterial g_matBeige;   // 淺米白
CMaterial g_matGray;    //  深麥灰材質
CMaterial g_matWaterBlue;
CMaterial g_matWaterGreen;
CMaterial g_matWaterRed;
CMaterial g_matWoodHoney;
CMaterial g_matWoodLightOak;
CMaterial g_matWoodBleached;

// 貼圖宣告區
TextureData g_texData[2]; 

// 2D 素材宣告區
//std::array<CButton, 4> g_button = {
//    CButton(50.0f, 50.0f, glm::vec4(1.0f, 0.85f, 0.2f, 1.0f), glm::vec4(0.7f, 0.5f, 0.0f, 1.0f)), // 點光源 yellow
//    CButton(50.0f, 50.0f, glm::vec4(1.0f, 0.7f, 0.85f, 1.0f), glm::vec4(0.65f, 0.35f, 0.5f, 1.0f)), // 藥丸聚光燈 pink
//    CButton(50.0f, 50.0f, glm::vec4(0.65f, 0.9f, 1.0f, 1.0f), glm::vec4(0.3f, 0.55f, 0.7f, 1.0f)), // 杯子聚光燈 blue
//    CButton(50.0f, 50.0f, glm::vec4(0.65f, 1.0f, 0.75f, 1.0f), glm::vec4(0.3f, 0.65f, 0.45f, 1.0f)), // 紐結聚光燈 green
//};

// 投影矩陣
GLint g_viewLoc;
GLint g_projLoc;
glm::mat4 g_mxView;
glm::mat4 g_mxProj;
GLint g_uiViewLoc;
GLint g_uiProjLoc;
glm::mat4 g_mxUiView;
glm::mat4 g_mxUiProj;

bool g_isNpr = false; // 切換照明風格（是否為卡通）
bool g_isGradient = false; // 是否自動漸變照明色調
float g_colorTime = 0.0f; // 計算自動變色的時間

void genMaterial();

//----------------------------------------------------------------------------
void loadScene(void)
{
    genMaterial(); // 產生材質
    g_shadingProg = CShaderPool::getInstance().getShader("v_shader.glsl", "f_shader.glsl");  
    g_uiShadingProg = CShaderPool::getInstance().getShader("v_uishader.glsl", "f_uishader.glsl");
    
    // 設定燈光
    g_light.setShaderID(g_shadingProg, "uLight[0]");
    //g_capSpotLight.setShaderID(g_shadingProg, "uLight[1]");
    //g_capSpotLight.setCutOffDeg(20.0f, 60.0f, 1.5f); // 第三引數為聚焦指數（optional）
    //g_cupSpotLight.setShaderID(g_shadingProg, "uLight[2]");
    //g_cupSpotLight.setCutOffDeg(20.0f, 60.0f, 1.5f);
    //g_knotSpotLight.setShaderID(g_shadingProg, "uLight[3]");
    //g_knotSpotLight.setCutOffDeg(20.0f, 60.0f, 1.5f);

    glUniform1i(glGetUniformLocation(g_shadingProg, "uLightNum"), 1);
    glUniform1i(glGetUniformLocation(g_shadingProg, "uIsNpr"), 0); // 切換照明風格（是否為卡通）

    // 設定貼圖
    g_texData[0] = CTexturePool::getInstance().getTexture("texture/wall.png", true); // 開啟 mipmap
    g_texData[1] = CTexturePool::getInstance().getTexture("texture/floor_.png"); // 不開啟 mipmap

    //g_capsule.setupVertexAttributes();
    //g_capsule.setShaderID(g_shadingProg, 3);
    //g_capsule.setPos(glm::vec3(8.0f, 0.5f, -8.0f));
    //g_capsule.setRotate(90, glm::vec3(1, 0, 0));
    //g_capsule.setMaterial(g_matWaterRed);

    //g_cup.setupVertexAttributes();
    //g_cup.setShaderID(g_shadingProg, 3);
    //g_cup.setPos(glm::vec3(-8.0f, 0.5f, -8.0f));
    //g_cup.setMaterial(g_matWaterBlue);

    //g_objModel.setupVertexAttributes();
    //g_objModel.setShaderID(g_shadingProg, 3);
    //g_objModel.setScale(glm::vec3(5.0f, 5.0f, 5.0f));
    //g_objModel.setPos(glm::vec3(0.0f, 0.0005f, 0.0f)); // 房間正中間
    //g_objModel.setMaterial(g_matWoodLightOak);
 
    //g_knot.setupVertexAttributes();
    //g_knot.setShaderID(g_shadingProg, 3);
    //g_knot.setScale(glm::vec3(0.4f, 0.4f, 0.4f));
    //g_knot.setPos(glm::vec3(0.0f, 0.5f, 8.0f));
    //g_knot.setMaterial(g_matWaterGreen);

    glm::vec3 roomPos(0.0f, 5.95f, 0.0f);
    glm::vec3 offsetX(-30.1f, 0.0f, 0.0f);
    glm::vec3 offsetZ(0.0f, 0.0f, -30.1f);
    for (int room = 0; room < ROOM_NUM; ++room) {
        g_room[room].setupVertexAttributes();
        g_room[room].setShaderID(g_shadingProg, 3);
        g_room[room].setScale(glm::vec3(30.0f, 12.0f, 30.0f));
        g_room[room].setMaterial(g_matWoodBleached);

        // 將房間放置在 3x2 矩陣中
        glm::vec3 offset = offsetX * (float)(room % 2) + offsetZ * (float)(room / 2);
        g_room[room].setPos(roomPos + offset);
    }

    glm::vec3 floorPos(0.0f, 0.0f, 0.0f);
    for (int floor = 0; floor < ROOM_NUM; ++floor) {
        g_floor[floor].setupVertexAttributes();
        g_floor[floor].setShaderID(g_shadingProg, 3);
        g_floor[floor].setScale(glm::vec3(30.0f, 30.0f, 1.0f));
        g_floor[floor].setRotate(-90, glm::vec3(1, 0, 0)); // 攤平並翻正
        g_floor[floor].setMaterial(g_matWoodBleached);
        g_floor[floor].setTextureMode(CShape::TEX_DIFFUSE);
        g_floor[floor].setTiling(90, 90);

        glm::vec3 offset = offsetX * (float)(floor % 2) + offsetZ * (float)(floor / 2);
        g_floor[floor].setPos(floorPos + offset);
    }
    
    for (int room = 0; room < ROOM_NUM; ++room) {
        glm::vec3 offset = offsetX * (float)(room % 2) + offsetZ * (float)(room / 2);

        for (int side = 0; side < 4; ++side) {
            int idx = room * 4 + side;

            g_walls[idx].setupVertexAttributes();
            g_walls[idx].setShaderID(g_shadingProg, 3);
            g_walls[idx].setScale(glm::vec3(30.0f, 12.0f, 30.0f));
            g_walls[idx].setMaterial(g_matWoodBleached);
            g_walls[idx].setTextureMode(CShape::TEX_DIFFUSE);

            glm::vec3 wallPos;
            float rotationY = 0.0f;

            switch (side) {
            case 0: // 右牆
                wallPos = glm::vec3(0.0f, 0.0f, 14.95f);
                rotationY = 0.0f + 180.0f;
                break;
            case 1: // 後牆
                wallPos = glm::vec3(14.95f, 0.0f, 0.0f);
                rotationY = 90.0f + 180.0f;
                break;
            case 2: // 左牆
                wallPos = glm::vec3(0.0f, 0.0f, -14.95f);
                rotationY = 180.0f - 180.0f;
                break;
            case 3: // 前牆
                wallPos = glm::vec3(-14.95f, 0.0f, 0.0f);
                rotationY = 270.0f - 180.0f;
                break;
            }

            g_walls[idx].setPos(roomPos + offset + wallPos);
            g_walls[idx].setRotate(rotationY, glm::vec3(0, 1, 0));
        }
    }
    
    g_centerloc.setScale(glm::vec3(30.0f, 12.0f, 30.0f)); // 設定 center 位置

    CCamera::getInstance().updateView(g_eyeloc); // 設定 eye 位置
    CCamera::getInstance().updateCenter(glm::vec3(0,4,0));
	CCamera::getInstance().updatePerspective(45.0f, (float)SCREEN_WIDTH / SCREEN_HEIGHT, 0.1f, 100.0f);
    g_mxView = CCamera::getInstance().getViewMatrix();
    g_mxProj = CCamera::getInstance().getProjectionMatrix();

    g_viewLoc = glGetUniformLocation(g_shadingProg, "mxView"); 	// 取得 view matrix 變數的位置 v
    glUniformMatrix4fv(g_viewLoc, 1, GL_FALSE, glm::value_ptr(g_mxView));

    g_projLoc = glGetUniformLocation(g_shadingProg, "mxProj"); 	// 取得投影矩陣變數的位置 v
    glUniformMatrix4fv(g_projLoc, 1, GL_FALSE, glm::value_ptr(g_mxProj));

    // UI 設定
    /*g_button[0].setScreenPos(525.0f, 50.0f);
    g_button[0].init(g_uiShadingProg);
    g_button[1].setScreenPos(600.0f, 50.0f);
    g_button[1].init(g_uiShadingProg);
    g_button[2].setScreenPos(675.0f, 50.0f);
    g_button[2].init(g_uiShadingProg);
    g_button[3].setScreenPos(750.0f, 50.0f);
    g_button[3].init(g_uiShadingProg);*/
    
    g_mxUiView = glm::mat4(1.0f);
    g_mxUiProj = glm::ortho(0.0f, (float)SCREEN_WIDTH, 0.0f, (float)SCREEN_HEIGHT, -1.0f, 1.0f);

    g_uiViewLoc = glGetUniformLocation(g_uiShadingProg, "mxView"); 	// 取得 view matrix 變數的位置 v
    glUniformMatrix4fv(g_uiViewLoc, 1, GL_FALSE, glm::value_ptr(g_mxUiView));

    g_uiProjLoc = glGetUniformLocation(g_uiShadingProg, "mxProj"); 	// 取得投影矩陣變數的位置 v
    glUniformMatrix4fv(g_uiProjLoc, 1, GL_FALSE, glm::value_ptr(g_mxUiProj));
  
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // 設定清除 back buffer 背景的顏色
    glEnable(GL_DEPTH_TEST); // 啟動深度測試
}
//----------------------------------------------------------------------------

void render(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // 設定 back buffer 的背景顏色
    glUseProgram(g_shadingProg);

    //上傳光源與相機位置
    g_light.updateToShader();
    /*g_capSpotLight.updateToShader();
    g_cupSpotLight.updateToShader();
    g_knotSpotLight.updateToShader();*/
    glUniform3fv(glGetUniformLocation(g_shadingProg, "viewPos"), 1, glm::value_ptr(g_eyeloc));
    glUniform3fv(glGetUniformLocation(g_shadingProg, "lightPos"), 1, glm::value_ptr(g_light.getPos()));

    // 先切換回 3d 投影畫模型，再切換到 2d 投影畫 UI
    g_mxView = CCamera::getInstance().getViewMatrix();
    g_mxProj = CCamera::getInstance().getProjectionMatrix();
    glUniformMatrix4fv(g_viewLoc, 1, GL_FALSE, glm::value_ptr(g_mxView));
    glUniformMatrix4fv(g_projLoc, 1, GL_FALSE, glm::value_ptr(g_mxProj));

    g_light.drawRaw();
    //g_capSpotLight.drawRaw(); // g_capSpotTarget 不用特別繪製
    //g_cupSpotLight.drawRaw();
    //g_knotSpotLight.drawRaw();

    /*g_capsule.uploadMaterial();
    g_capsule.drawRaw();
    g_cup.uploadMaterial();
    g_cup.drawRaw();
    g_objModel.uploadMaterial();
    g_objModel.drawRaw();
    g_knot.uploadMaterial();
    g_knot.drawRaw();*/
    
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, 0); // 不綁定貼圖
    for (int i = 0; i < ROOM_NUM; i++) {
        g_room[i].uploadMaterial();
        g_room[i].uploadTextureFlags();
        g_room[i].drawRaw();
    }

    glBindTexture(GL_TEXTURE_2D, g_texData[1].id); // 綁定貼圖 
    for (int i = 0; i < ROOM_NUM; i++) {
        g_floor[i].uploadMaterial();
        g_floor[i].uploadTextureFlags();
        g_floor[i].drawRaw();
    }
    
    glBindTexture(GL_TEXTURE_2D, g_texData[0].id); // 綁定貼圖 
    for (int i = 0; i < ROOM_NUM * 4; i++) {
        g_walls[i].uploadMaterial();
        g_walls[i].uploadTextureFlags();
        g_walls[i].drawRaw();
    }

    /*glUseProgram(g_uiShadingProg);
    glUniformMatrix4fv(g_uiViewLoc, 1, GL_FALSE, glm::value_ptr(g_mxUiView));
    glUniformMatrix4fv(g_uiProjLoc, 1, GL_FALSE, glm::value_ptr(g_mxUiProj));

    for (int i = 0; i < 4; i++) {
        g_button[i].draw();
    }*/
}
//----------------------------------------------------------------------------

void update(float dt)
{
    //g_light.setMotionEnabled(g_button[0].isActive()); // 讓點光源進行圓周運動
    //g_capSpotLight.setLightOn(g_button[1].isActive());
    //g_cupSpotLight.setLightOn(g_button[2].isActive());
    //g_knotSpotLight.setLightOn(g_button[3].isActive());

    //if (g_isGradient) {
    //    float speed = 0.4f;
    //    g_colorTime += dt * speed;

    //    // 範圍 [0,1] 之間變化
    //    float r = sin(g_colorTime * 2.0f) * 0.5f + 0.5f;
    //    float g = sin(g_colorTime * 2.0f + 2.0f) * 0.5f + 0.5f;
    //    float b = sin(g_colorTime * 2.0f + 4.0f) * 0.5f + 0.5f;

    //    glm::vec4 colorAmbient = glm::vec4(r * 0.1f, g * 0.1f, b * 0.1f, 1.0f);
    //    glm::vec4 colorDiffuse = glm::vec4(r * 0.8f, g * 0.8f, b * 0.8f, 1.0f);
    //    glm::vec4 colorSpecular = glm::vec4(r * 1.0f, g * 1.0f, b * 1.0f, 1.0f);

    //    g_light.setAmbient(colorAmbient);
    //    g_light.setDiffuse(colorDiffuse);
    //    g_light.setSpecular(colorSpecular);
    //    g_light.updateToShader();
    //}
    
    g_light.update(dt);
}

void releaseAll()
{

}

int main() {
    // ------- 檢查與建立視窗  ---------------  
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }

    // 設定 OpenGL 版本與 Core Profile
    //glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); // OpenGL 3.3
    //glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    //glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); //只啟用 OpenGL 3.3 Core Profile（不包含舊版 OpenGL 功能）
    //glfwWindowHint(GLFW_RESIZABLE, GL_FALSE); // 禁止視窗大小改變

    // 建立 OpenGL 視窗與該視窗執行時所需的的狀態、資源和環境(context 上下文)
    GLFWwindow* window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "3D Show Room", nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    // 設定將這個視窗的資源(OpenGL 的圖形上下文）與當前執行緒綁定，讓該執行緒能夠操作該視窗的資源
    glfwMakeContextCurrent(window);

    // 設定視窗大小, 這樣 OpenGL 才能知道如何將視窗的內容繪製到正確的位置
    // 基本上寬與高設定成視窗的寬與高即可
    glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

    if (glewInit() != GLEW_OK) {
        std::cerr << "Failed to initialize GLEW" << std::endl;
        return -1;
    }
    // ---------------------------------------

    // 設定相關事件的 callback 函式，以便事件發生時，能呼叫對應的函式
    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);// 視窗大小被改變時
    glfwSetKeyCallback(window, keyCallback);                        // 有鍵盤的按鍵被按下時
    glfwSetMouseButtonCallback(window, mouseButtonCallback);        // 有滑鼠的按鍵被按下時
    glfwSetCursorPosCallback(window, cursorPosCallback);            // 滑鼠在指定的視窗上面移動時
	glfwSetScrollCallback(window, scrollCallback);			        // 滑鼠滾輪滾動時

    // 呼叫 loadScene() 建立與載入 GPU 進行描繪的幾何資料 
    loadScene();

    /*std::cout << "wasd/WASD 移動" << std::endl;
    std::cout << "n/N 切換照明模式" << std::endl;
    std::cout << "c/C 切換位移方式" << std::endl;
    std::cout << "rgb/RGB 改變點光源色調" << std::endl;
    std::cout << "h/H 重設點光源色調" << std::endl;
    std::cout << "l/L 自動漸變點光源色調" << std::endl << std::endl;*/
    
    float lastTime = (float)glfwGetTime();
    while (!glfwWindowShouldClose(window)) {
        float currentTime = (float)glfwGetTime();
        float deltaTime = currentTime - lastTime; // 計算前一個 frame 到目前為止經過的時間
        lastTime = currentTime;
        update(deltaTime);      // 呼叫 update 函式，並將 deltaTime 傳入，讓所有動態物件根據時間更新相關內容
        render();
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    releaseAll(); // 程式結束前釋放所有的資源
    glfwTerminate();
    return 0;
}

void genMaterial()
{
    // 設定材質
    g_matBeige.setAmbient(glm::vec4(0.0918f, 0.0906f, 0.0863f, 1.0f));
    g_matBeige.setDiffuse(glm::vec4(0.8258f, 0.8152f, 0.7765f, 1.0f));
    g_matBeige.setSpecular(glm::vec4(0.25f, 0.25f, 0.25f, 1.0f));
    g_matBeige.setShininess(32.0f);

    g_matGray.setAmbient(glm::vec4(0.0690f, 0.06196f, 0.05451f, 1.0f));
    g_matGray.setDiffuse(glm::vec4(0.6212f, 0.5576f, 0.4906f, 1.0f));
    g_matGray.setSpecular(glm::vec4(0.20f, 0.20f, 0.20f, 1.0f));
    g_matGray.setShininess(16.0f);

    g_matWaterBlue.setAmbient(glm::vec4(0.080f, 0.105f, 0.120f, 1.0f));
    g_matWaterBlue.setDiffuse(glm::vec4(0.560f, 0.740f, 0.840f, 1.0f));
    g_matWaterBlue.setSpecular(glm::vec4(0.20f, 0.20f, 0.20f, 1.0f));
    g_matWaterBlue.setShininess(32.0f);

    g_matWaterGreen.setAmbient(glm::vec4(0.075f, 0.120f, 0.090f, 1.0f));
    g_matWaterGreen.setDiffuse(glm::vec4(0.540f, 0.840f, 0.640f, 1.0f));
    g_matWaterGreen.setSpecular(glm::vec4(0.20f, 0.20f, 0.20f, 1.0f));
    g_matWaterGreen.setShininess(32.0f);

    g_matWaterRed.setAmbient(glm::vec4(0.125f, 0.075f, 0.075f, 1.0f));
    g_matWaterRed.setDiffuse(glm::vec4(0.860f, 0.540f, 0.540f, 1.0f));
    g_matWaterRed.setSpecular(glm::vec4(0.20f, 0.20f, 0.20f, 1.0f));
    g_matWaterRed.setShininess(32.0f);

    g_matWoodHoney.setAmbient(glm::vec4(0.180f, 0.164f, 0.130f, 1.0f));
    g_matWoodHoney.setDiffuse(glm::vec4(0.720f, 0.656f, 0.520f, 1.0f));
    g_matWoodHoney.setSpecular(glm::vec4(0.30f, 0.30f, 0.30f, 1.0f));
    g_matWoodHoney.setShininess(24.0f);

    g_matWoodLightOak.setAmbient(glm::vec4(0.200f, 0.180f, 0.160f, 1.0f));
    g_matWoodLightOak.setDiffuse(glm::vec4(0.800f, 0.720f, 0.640f, 1.0f));
    g_matWoodLightOak.setSpecular(glm::vec4(0.30f, 0.30f, 0.30f, 1.0f));
    g_matWoodLightOak.setShininess(24.0f);

    g_matWoodBleached.setAmbient(glm::vec4(0.220f, 0.215f, 0.205f, 1.0f));
    g_matWoodBleached.setDiffuse(glm::vec4(0.880f, 0.860f, 0.820f, 1.0f));
    g_matWoodBleached.setSpecular(glm::vec4(0.30f, 0.30f, 0.30f, 1.0f));
    g_matWoodBleached.setShininess(24.0f);
}