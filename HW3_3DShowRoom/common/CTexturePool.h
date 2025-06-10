// CTexturePool.h
#pragma once
#include <string>
#include <unordered_map>
#include <glew/include/GL/glew.h>

// 影像資料：包含 GL 貼圖 ID 與寬高
struct TextureData {
    GLuint id;
    int    width;
    int    height;
};

// CTexturePool：單例貼圖緩存，並保存貼圖寬高
class CTexturePool {
public:
    static CTexturePool& getInstance();

    /// 取得或載入影像，並回傳資料（id、寬、高）
    const TextureData& getTexture(const std::string& path, bool bMipMap = false);

    /// 僅查詢已載入的影像資料，不存在時 id=0, width=height=0
    TextureData getTextureData(const std::string& path) const;

private:
    CTexturePool();
    ~CTexturePool();
    CTexturePool(const CTexturePool&) = delete;
    CTexturePool& operator=(const CTexturePool&) = delete;

    void cleanup();  // 釋放所有貼圖

    std::unordered_map<std::string, TextureData> m_pool;
};