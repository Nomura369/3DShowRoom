﻿// 計分
// v (12 %)畫出六個房間，並達成以下的條件（編譯要能過且正常執行才算）
//   每一面牆都必須有貼圖 v 
//   必須至少有三個房間是使用燈光照明，而且是使用 Per Pixel Lighting v 
//   每一個房間中都必須有放置裝飾品(部分必須有貼圖) v 
// v (2 %) 必須有房間顯示至少(含)一個 OBJ 檔的模型，而且包含貼圖
// v (8 %)環境與操控
//   (2 %) 鏡頭的移動是根據目前的視角方向 v 
//   (2 %) 不會穿牆 v
//   (2 %) 會被機關觸動的動態移動光源，使用 Per Pixel Lighting v -> 手電筒
//   (2 %) 至少三面牆壁上半透明的玻璃 v 
// v (8 %)圖學相關功能的使用，必須在程式碼中以註解清楚標明
//   (1 %) 針對特定物件實現 Billboards 的功能 v -> 彈孔
//   (1 %) 使用到  Mipmapped 的功能 （有具體的說明在程式碼中）v -> 壁紙貼圖 
//   (2 %) 有房間使用到 Light Map 的功能 （有具體的說明在程式碼中）v -> 地板
//   (2 %) 有物件使用到 Normal Map 的功能 （有具體的說明在程式碼中）v -> 茶壺
//   (2 %) 有物件使用到 Environment Map 的功能(有具體的說明在程式碼中）v -> 杯子 
// v (6 %) 其他你覺得可以展示的技術，包含物理或是數學的運算
//   (3 %)發射子彈並且在牆壁上留下彈孔 v 
//   (3 %)可以破壞房間內的擺設 v 
// 上述總分：36
// (4 %) 創意分數
// 老師給分，請自行發揮
// 鏡頭移動時的晃動效果（如果會晃得很大力純屬意外）v 
// 只有透明牆可以穿過（對使用者而言） v 
// 按 c 鍵可切換手槍 or 手電筒，空白鍵使用當前道具 v  
// 手電筒可以照出隱藏於地板的 lightmap v 

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
#include "../models/CDonut.h"
#include "../models/CTeapot.h"

#include "common/CLight.h"
#include "common/CMaterial.h"
#include "../models/CObjModel.h"
#include "common/png_loader.h"
#include "common/CBulletManager.h"
#include "common/CAim.h"
#include "common/CBulletHoles.h"

#define SCREEN_WIDTH  800
#define SCREEN_HEIGHT 800
#define ROOM_NUM 6

// 一般模型宣告區
CCapsule g_capsule[2];
CCup g_cup[3];
CTorusKnot g_knot[4] = {
    CTorusKnot(4),
    CTorusKnot(4),
    CTorusKnot(4),
    CTorusKnot(4),
};
CDonut g_donut[5];
CTeapot g_teapot[6] = {
    CTeapot(5), CTeapot(5), CTeapot(5),
    CTeapot(5), CTeapot(5), CTeapot(5),
};

CBox g_room[ROOM_NUM];
CQuad g_walls[ROOM_NUM * 4]; // 所有房間的牆面
CQuad g_floor[ROOM_NUM]; // 所有房間的地板
CSphere g_sphere;

// obj 檔模型宣告區
CObjModel g_objModel;

// 鏡頭與場景宣告區
glm::vec3 g_eyeloc(6.0f, 6.0f, 6.0f); // 鏡頭位置, 預設在 (8,8,8) 
CCube g_centerloc; // view center 預設在 (0,0,0)，不做任何描繪操作

GLuint g_shadingProg;
GLuint g_uiShadingProg;

// 全域光源 (位置在 5,5,0)
std::array<CLight, ROOM_NUM> g_lights = {
    CLight (glm::vec3(0.0f, 10.0f, 0.0f)), // 預設為點光源，每個房間一盞
    CLight (glm::vec3(-30.1f, 10.0f, 0.0f)),
    CLight (glm::vec3(0.0f, 10.0f, -30.1f)),
    CLight (glm::vec3(-30.1f, 10.0f, -30.1f)),
    CLight (glm::vec3(0.0f, 10.0f, -60.2f)),
    CLight (glm::vec3(-30.1f, 10.0f, -60.2f)),
};
// 手電筒，聚光燈形式（從略低於相機的位置出發，往視線前方照）
CLight g_flashlight(glm::vec3(6.0f, 5.5f, 6.0f), glm::vec3(0.0f, 4.0f, 0.0f)); 

// 全域材質（可依模型分別設定）
CMaterial g_matBeige;   // 淺米白
CMaterial g_matGray;    //  深麥灰材質
CMaterial g_matWaterBlue;
CMaterial g_matWaterGreen;
CMaterial g_matWaterRed;
CMaterial g_matWoodHoney;
CMaterial g_matWoodLightOak;
CMaterial g_matWoodBleached;
CMaterial g_matGlass;

// 貼圖宣告區
TextureData g_texData[8]; 
GLuint g_uiCubeMap; // 環境貼圖專用

// 投影矩陣
GLint g_viewLoc;
GLint g_projLoc;
glm::mat4 g_mxView;
glm::mat4 g_mxProj;
GLint g_uiViewLoc;
GLint g_uiProjLoc;
glm::mat4 g_mxUiView;
glm::mat4 g_mxUiProj;

// 狀態宣告區
bool g_itemChange = false; // 0 為手電筒（預設），1 為手槍

// UI 素材宣告區
CAim g_aim; // 手槍的準心標示（使用預設值繪製）

void genMaterial();

//----------------------------------------------------------------------------
void loadScene(void)
{
    genMaterial(); // 產生材質
    g_shadingProg = CShaderPool::getInstance().getShader("v_shader.glsl", "f_shader.glsl");  
    g_uiShadingProg = CShaderPool::getInstance().getShader("v_uishader.glsl", "f_uishader.glsl");
    
    // 設定燈光
    g_lights[0].setShaderID(g_shadingProg, "uLight[0]");
    g_lights[1].setShaderID(g_shadingProg, "uLight[1]");
    g_lights[2].setShaderID(g_shadingProg, "uLight[2]");
    g_lights[3].setShaderID(g_shadingProg, "uLight[3]");
    g_lights[4].setShaderID(g_shadingProg, "uLight[4]");
    g_lights[5].setShaderID(g_shadingProg, "uLight[5]");
    for (int i = 0; i < ROOM_NUM; i++) {
        g_lights[i].setIntensity(0.15f);
    }

    g_flashlight.setShaderID(g_shadingProg, "uLight[6]");
    g_flashlight.setCutOffDeg(20.0f, 35.0f, 20.0f); // 第三引數為聚焦指數（optional）
    g_flashlight.setIntensity(0.5f);
    g_flashlight.setLightOn(false); // 預設手電筒是關的

    glUniform1i(glGetUniformLocation(g_shadingProg, "uLightNum"), ROOM_NUM + 1);
    glUniform1i(glGetUniformLocation(g_shadingProg, "uIsNpr"), 0); // 切換照明風格（是否為卡通）

    // 設定貼圖
    g_texData[0] = CTexturePool::getInstance().getTexture("texture/wall.png", true); // 開啟 mipmap
    g_texData[1] = CTexturePool::getInstance().getTexture("texture/floor.png"); // 不開啟 mipmap
    g_texData[2] = CTexturePool::getInstance().getTexture("texture/wood.png"); // 不開啟 mipmap
    g_texData[3] = CTexturePool::getInstance().getTexture("texture/bread.png"); // 不開啟 mipmap
    g_texData[4] = CTexturePool::getInstance().getTexture("texture/wall_alpha.png", true); // 開啟 mipmap
    g_texData[5] = CTexturePool::getInstance().getTexture("texture/lightMap.png"); // lightmap
    g_texData[6] = CTexturePool::getInstance().getTexture("texture/normalMap.png", true); // normalmap
    g_texData[7] = CTexturePool::getInstance().getTexture("texture/bulletHole.png"); // 不開啟 mipmap
    g_uiCubeMap = CubeMap_load_SOIL();

    // 設定模型
    for (int i = 0; i < 2; i++) {
        g_capsule[i].setupVertexAttributes();
        g_capsule[i].setShaderID(g_shadingProg, 3);
        g_capsule[i].setRotate(90, glm::vec3(1, 0, 0));
        g_capsule[i].setRotate(90, glm::vec3(0, 0, 1));
        g_capsule[i].setScale(glm::vec3(1.5f, 1.5f, 1.5f));
        g_capsule[i].setMaterial(g_matWaterRed);
        g_capsule[i].setTextureMode(CShape::TEX_NONE);
        CBulletManager::getInstance().addToCollision(&g_capsule[i]);
    }
    g_capsule[0].setPos(glm::vec3(-30.1f, 0.7f, -1.5f));
    g_capsule[1].setPos(glm::vec3(-30.1f, 0.7f, 1.5f));
    

    for (int i = 0; i < 3; i++) {
        g_cup[i].setupVertexAttributes();
        g_cup[i].setShaderID(g_shadingProg, 3);
        g_cup[i].setScale(glm::vec3(1.5f, 1.5f, 1.5f));
        g_cup[i].setMaterial(g_matWaterBlue);
        g_cup[i].setTextureMode(CShape::TEX_CUBEMAP);
        g_cup[i].setCollisionDist(2.0f);
        CBulletManager::getInstance().addToCollision(&g_cup[i]);
    }
    g_cup[0].setPos(glm::vec3(3.0f, 0.1f, -30.1f));
    g_cup[1].setPos(glm::vec3(-1.5f, 0.1f, -30.1f + 2.598f));
    g_cup[2].setPos(glm::vec3(-1.5f, 0.1f, -30.1f - 2.598f));

    g_objModel.setupVertexAttributes();
    g_objModel.setShaderID(g_shadingProg, 3);
    g_objModel.setScale(glm::vec3(5.0f, 5.0f, 5.0f));
    g_objModel.setPos(glm::vec3(0.0f, 0.0005f, 0.0f)); // 房間1正中間
    g_objModel.setMaterial(g_matWoodLightOak);
    g_objModel.setTextureMode(CShape::TEX_DIFFUSE);
    g_objModel.setCollisionDist(1.9f);
    CBulletManager::getInstance().addToCollision(&g_objModel);
 
    for (int i = 0; i < 4; i++) {
        g_knot[i].setupVertexAttributes();
        g_knot[i].setShaderID(g_shadingProg, 3);
        g_knot[i].setScale(glm::vec3(0.7f, 0.7f, 0.7f));
        g_knot[i].setMaterial(g_matWaterGreen);
        g_knot[i].setTextureMode(CShape::TEX_NONE);
        CBulletManager::getInstance().addToCollision(&g_knot[i]);
    }
    g_knot[0].setPos(glm::vec3(-27.1f, 0.8f, -30.1f));
    g_knot[1].setPos(glm::vec3(-30.1f, 0.8f, -27.1f));
    g_knot[2].setPos(glm::vec3(-33.1f, 0.8f, -30.1f));
    g_knot[3].setPos(glm::vec3(-30.1f, 0.8f, -33.1f));

    for (int i = 0; i < 5; i++) {
        g_donut[i].setupVertexAttributes();
        g_donut[i].setShaderID(g_shadingProg, 3);
        g_donut[i].setScale(glm::vec3(2.0f, 2.0f, 2.0f));
        g_donut[i].setMaterial(g_matWoodHoney);
        g_donut[i].setTextureMode(CShape::TEX_DIFFUSE);
        CBulletManager::getInstance().addToCollision(&g_donut[i]);
    }
    g_donut[0].setPos(glm::vec3(4.0f, 0.3f, -60.2f));
    g_donut[1].setPos(glm::vec3(1.24f, 0.3f, -56.41f));
    g_donut[2].setPos(glm::vec3(-3.24f, 0.3f, -58.27f));
    g_donut[3].setPos(glm::vec3(-3.24f, 0.3f, -62.13f));
    g_donut[4].setPos(glm::vec3(1.24f, 0.3f, -63.99f));

    for (int i = 0; i < 6; i++) {
        g_teapot[i].setupVertexAttributes();
        g_teapot[i].setShaderID(g_shadingProg, 3);
        g_teapot[i].setScale(glm::vec3(0.65f, 0.65f, 0.65f));
        g_teapot[i].setMaterial(g_matGray);
        g_teapot[i].setTextureMode(CShape::TEX_NORMALMAP);
        g_teapot[i].setCollisionDist(1.2f);
        CBulletManager::getInstance().addToCollision(&g_teapot[i]);
        if (i >= 2 && i <= 4) {
            g_teapot[i].setRotate(180, glm::vec3(0, 1, 0));
        }
    } 
    g_teapot[0].setPos(glm::vec3(-26.1f, 0.1f, -60.2f));   
    g_teapot[1].setPos(glm::vec3(-28.1f, 0.1f, -57.736f)); 
    g_teapot[2].setPos(glm::vec3(-32.1f, 0.1f, -57.736f)); 
    g_teapot[3].setPos(glm::vec3(-34.1f, 0.1f, -60.2f));   
    g_teapot[4].setPos(glm::vec3(-32.1f, 0.1f, -62.664f)); 
    g_teapot[5].setPos(glm::vec3(-28.1f, 0.1f, -62.664f)); 

    CBulletManager::getInstance().setShaderProg(g_shadingProg);
    
    // 設定場景
    glm::vec3 roomPos(0.0f, 5.95f, 0.0f);
    glm::vec3 offsetX(-30.05f, 0.0f, 0.0f);
    glm::vec3 offsetZ(0.0f, 0.0f, -30.05f);
    for (int room = 0; room < ROOM_NUM; room++) {
        int row = room / 2; // Z軸方向（前後）
        int col = room % 2; // X軸方向（左右）

        // 因為是直接測試得出的結果，不確定實際的相對位置為何
        bool drawFront = (row == 0); // 最後排才畫 front
        bool drawBack = (row == 2); // 最前排才畫 back
        bool drawLeft = (col == 1); // 中間會少一面牆
        bool drawRight = (col == 0); // 中間會少另一面牆

        g_room[room].setPoints(drawFront, drawBack, drawLeft, drawRight);
        g_room[room].setupVertexAttributes();
        g_room[room].setShaderID(g_shadingProg, 3);
        g_room[room].setScale(glm::vec3(30.0f, 12.0f, 30.0f));
        g_room[room].setMaterial(g_matWoodBleached);

        // 相當於將房間置於 2 * 3 的矩陣中
        glm::vec3 offset = offsetX * (float)(col)+offsetZ * (float)(row);
        g_room[room].setPos(roomPos + offset);
    }

    glm::vec3 floorPos(0.0f, 0.0f, 0.0f);
    for (int floor = 0; floor < ROOM_NUM; floor++) {
        g_floor[floor].setupVertexAttributes();
        g_floor[floor].setShaderID(g_shadingProg, 3);
        g_floor[floor].setScale(glm::vec3(30.0f, 30.0f, 1.0f));
        g_floor[floor].setRotate(-90, glm::vec3(1, 0, 0)); // 攤平並翻正
        g_floor[floor].setMaterial(g_matWoodBleached);
        g_floor[floor].setTextureMode(CShape::TEX_DIFFUSE | CShape::TEX_LIGHTMAP);
        g_floor[floor].setTiling(90, 90);

        glm::vec3 offset = offsetX * (float)(floor % 2) + offsetZ * (float)(floor / 2);
        g_floor[floor].setPos(floorPos + offset);
    }
    
    for (int room = 0; room < ROOM_NUM; room++) {
        int row = room / 2; // Z軸方向（前後）
        int col = room % 2; // X軸方向（左右）
        glm::vec3 offset = offsetX * (float)col + offsetZ * (float)row;

        for (int side = 0; side < 4; ++side) {
            int idx = room * 4 + side;

            g_walls[idx].setupVertexAttributes();
            g_walls[idx].setShaderID(g_shadingProg, 3);
            g_walls[idx].setScale(glm::vec3(30.0f, 12.0f, 3.0f));
            g_walls[idx].setMaterial(g_matWoodBleached);

            glm::vec3 wallPos;
            float rotationY = 0.0f;

            switch (side) {
              case 0: // 前牆（從椅子正面來看）
                  wallPos = glm::vec3(0.0f, 0.0f, 14.95f);
                  rotationY = 180.0f;
                  if (row != 0) { // 互通牆
                      g_walls[idx].setMaterial(g_matGlass);
                  }
                  break;
              case 1: // 右牆
                  wallPos = glm::vec3(14.95f, 0.0f, 0.0f);
                  rotationY = 270.0f;
                  if (col == 1) { // 互通牆
                      g_walls[idx].setMaterial(g_matGlass);
                  }
                  break;
              case 2: // 後牆
                  wallPos = glm::vec3(0.0f, 0.0f, -14.95f);
                  rotationY = 0.0f;
                  if (row != 2) { // 互通牆
                      g_walls[idx].setMaterial(g_matGlass);
                  }
                  break;
              case 3: // 左牆
                  wallPos = glm::vec3(-14.95f, 0.0f, 0.0f);
                  rotationY = 90.0f;
                  if (col != 1) { // 互通牆
                      g_walls[idx].setMaterial(g_matGlass);
                  }
                  break;
            }

            g_walls[idx].setPos(roomPos + offset + wallPos);
            g_walls[idx].setRotate(rotationY, glm::vec3(0, 1, 0));
            g_walls[idx].setTextureMode(CShape::TEX_DIFFUSE);
        }
    }
    
    g_centerloc.setPos(glm::vec3(0.0f, 4.0f, 0.0f)); // 設定 center 位置

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
    g_aim.setupVertexAttributes();
    g_aim.setShaderID(g_uiShadingProg);
    g_aim.setPos(glm::vec3(400.0f, 400.0f, 0.0f)); // 視窗中心
    g_aim.setColor(glm::vec4(1.0f, 1.0f, 1.0f, 0.1f));
    
    g_mxUiView = glm::mat4(1.0f);
    g_mxUiProj = glm::ortho(0.0f, (float)SCREEN_WIDTH, 0.0f, (float)SCREEN_HEIGHT, -1.0f, 1.0f);

    g_uiViewLoc = glGetUniformLocation(g_uiShadingProg, "mxView"); 	// 取得 view matrix 變數的位置 v
    glUniformMatrix4fv(g_uiViewLoc, 1, GL_FALSE, glm::value_ptr(g_mxUiView));

    g_uiProjLoc = glGetUniformLocation(g_uiShadingProg, "mxProj"); 	// 取得投影矩陣變數的位置 v
    glUniformMatrix4fv(g_uiProjLoc, 1, GL_FALSE, glm::value_ptr(g_mxUiProj));
  
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // 設定清除 back buffer 背景的顏色
    glEnable(GL_DEPTH_TEST); // 啟動深度測試
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); // 透明度繪圖控制
}
//----------------------------------------------------------------------------

void render(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // 設定 back buffer 的背景顏色
    glUseProgram(g_shadingProg);

    //上傳光源與相機位置
    for (int i = 0; i < ROOM_NUM; i++) {
        g_lights[i].updateToShader();
    }
    g_flashlight.updateToShader();
    glUniform3fv(glGetUniformLocation(g_shadingProg, "viewPos"), 1, glm::value_ptr(g_eyeloc));
    glUniform3fv(glGetUniformLocation(g_shadingProg, "lightPos"), 1, glm::value_ptr(g_lights[0].getPos())); // 選一個光源當代表就好

    // 先切換回 3d 投影畫模型，再切換到 2d 投影畫 UI
    g_mxView = CCamera::getInstance().getViewMatrix();
    g_mxProj = CCamera::getInstance().getProjectionMatrix();
    glUniformMatrix4fv(g_viewLoc, 1, GL_FALSE, glm::value_ptr(g_mxView));
    glUniformMatrix4fv(g_projLoc, 1, GL_FALSE, glm::value_ptr(g_mxProj));

    for (int i = 0; i < ROOM_NUM; i++) {
        g_lights[i].drawRaw();
    }
    //g_flashlight.drawRaw(); // 不用特別畫立方體出來
    
    glActiveTexture(GL_TEXTURE0); // 啟動一般貼圖
    glBindTexture(GL_TEXTURE_2D, 0); // 不綁定貼圖
    for (int i = 0; i < ROOM_NUM; i++) {
        g_room[i].uploadMaterial();
        g_room[i].uploadTextureFlags();
        g_room[i].drawRaw();
    }

    glBindTexture(GL_TEXTURE_2D, g_texData[1].id); // 綁定一般貼圖
    glActiveTexture(GL_TEXTURE1); // 啟動 lightmap
    glBindTexture(GL_TEXTURE_2D, g_texData[3].id); // 綁定 lightmap
    for (int i = 0; i < ROOM_NUM; i++) {
        g_floor[i].uploadMaterial();
        g_floor[i].uploadTextureFlags();
        g_floor[i].drawRaw();
    }
    
    glActiveTexture(GL_TEXTURE0); // 啟動一般貼圖
    glBindTexture(GL_TEXTURE_2D, g_texData[0].id); // 綁定貼圖 
    for (int i = 0; i < ROOM_NUM * 4; i++) {
        if (g_walls[i].getMaterial() == g_matWoodBleached) {
            g_walls[i].uploadMaterial();
            g_walls[i].uploadTextureFlags();
            g_walls[i].drawRaw();
        }
    }

    glBindTexture(GL_TEXTURE_2D, g_texData[2].id); // 綁定貼圖 
    if (g_objModel.getIsActive()) {
        g_objModel.uploadMaterial();
        g_objModel.uploadTextureFlags();
        g_objModel.drawRaw();
    }

    glBindTexture(GL_TEXTURE_2D, 0); // 不綁定貼圖
    for (int i = 0; i < 2; i++) {
        if (g_capsule[i].getIsActive()) {
            g_capsule[i].uploadMaterial();
            g_capsule[i].uploadTextureFlags();
            g_capsule[i].drawRaw();
        }
    }

    glActiveTexture(GL_TEXTURE3); // 啟用環境貼圖
    glBindTexture(GL_TEXTURE_CUBE_MAP, g_uiCubeMap); // 綁定貼圖
    for (int i = 0; i < 3; i++) {
        if (g_cup[i].getIsActive()) {
            g_cup[i].uploadMaterial();
            g_cup[i].uploadTextureFlags();
            g_cup[i].drawRaw();
        }
    }

    glActiveTexture(GL_TEXTURE0); // 啟動一般貼圖
    glBindTexture(GL_TEXTURE_2D, 0); // 不綁定貼圖
    for (int i = 0; i < 4; i++) {
        if (g_knot[i].getIsActive()) {
            g_knot[i].uploadMaterial();
            g_knot[i].uploadTextureFlags();
            g_knot[i].drawRaw();
        }
    }

    glBindTexture(GL_TEXTURE_2D, g_texData[3].id); // 綁定貼圖 
    for (int i = 0; i < 5; i++) {
        if (g_donut[i].getIsActive()) {
            g_donut[i].uploadMaterial();
            g_donut[i].uploadTextureFlags();
            g_donut[i].drawRaw();
        }
    }

    glActiveTexture(GL_TEXTURE2); // 啟動 normalmap
    glBindTexture(GL_TEXTURE_2D, g_texData[6].id); // 綁定貼圖
    for (int i = 0; i < 6; i++) {
        if (g_teapot[i].getIsActive()) {
            g_teapot[i].uploadMaterial();
            g_teapot[i].uploadTextureFlags();
            g_teapot[i].drawRaw();
        }
    }

    // 開始畫半透明物體
    glEnable(GL_BLEND);
    glDepthMask(GL_FALSE);
    glActiveTexture(GL_TEXTURE0); // 啟動一般貼圖
    glBindTexture(GL_TEXTURE_2D, g_texData[4].id); // 綁定貼圖 
    for (int i = ROOM_NUM * 4 - 1; i >= 0; i--) { // 由遠到近
        if (g_walls[i].getMaterial() == g_matGlass) {
            g_walls[i].uploadMaterial();
            g_walls[i].uploadTextureFlags();
            g_walls[i].drawRaw();
        }
    }
    glBindTexture(GL_TEXTURE_2D, g_texData[7].id); // 綁定貼圖
    CBulletHoles::getInstance().draw();
    // 結束半透明物體的繪製
    glDisable(GL_BLEND);// 關閉 Blending
    glDepthMask(GL_TRUE);// 開啟對 Z-Buffer 的寫入操作

    glBindTexture(GL_TEXTURE_2D, 0); // 不綁定貼圖
    CBulletManager::getInstance().draw();

    // 開始畫半透明物體
    glEnable(GL_BLEND);
    glDepthMask(GL_FALSE);
    if (g_itemChange) { // 繪製手槍準心
        glUseProgram(g_uiShadingProg);
        glUniformMatrix4fv(g_uiViewLoc, 1, GL_FALSE, glm::value_ptr(g_mxUiView));
        glUniformMatrix4fv(g_uiProjLoc, 1, GL_FALSE, glm::value_ptr(g_mxUiProj));
        g_aim.draw();
    }
    // 結束半透明物體的繪製
    glDisable(GL_BLEND);// 關閉 Blending
    glDepthMask(GL_TRUE);// 開啟對 Z-Buffer 的寫入操作
    
}
//----------------------------------------------------------------------------

void update(float dt)
{
    for (int i = 0; i < ROOM_NUM; i++) {
        g_lights[i].update(dt);
    }
    
    CCamera::getInstance().update(dt);
    
    // 更新手電筒
    glm::vec3 camPos = CCamera::getInstance().getViewLocation();
    glm::vec3 camFront = glm::normalize(g_centerloc.getPos() - CCamera::getInstance().getViewLocation());
    glm::vec3 flashlightPos = camPos + glm::vec3(0.0f, -0.1f, 0.0f); // 往下偏一點（像是手持）
    glm::vec3 flashlightTarget = flashlightPos + camFront;
    g_flashlight.setPos(flashlightPos);
    g_flashlight.setTarget(flashlightTarget);

    CBulletManager::getInstance().update(dt);

    CBulletHoles::getInstance().setCameraPos(camPos);
    CBulletHoles::getInstance().update();

    // 檢查碰撞
    CBulletManager::getInstance().checkCollision();
    CBulletManager::getInstance().handleDeath();
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
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE); // 禁止視窗大小改變

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

    std::cout << "1. wasd/WASD 移動，透明牆面可穿透" << std::endl;
    std::cout << "2. c/C 切換道具（手電筒或手槍）" << std::endl;
    std::cout << "3. space 使用道具（開關手電筒或發射子彈）" << std::endl << std::endl;
    std::cout << "你現在拿的是「手電筒」" << std::endl << std::endl; // 預設道具
    
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

    g_matGlass.setAmbient(glm::vec4(0.2f, 0.2f, 0.25f, 0.5f)); 
    g_matGlass.setDiffuse(glm::vec4(0.05f, 0.05f, 0.05f, 0.5f));
    g_matGlass.setSpecular(glm::vec4(0.9f, 0.9f, 0.95f, 0.5f));
    g_matGlass.setShininess(60.0f);
}