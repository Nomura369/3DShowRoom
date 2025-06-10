// CTexturePool.h
#pragma once
#include <string>
#include <unordered_map>
#include <glew/include/GL/glew.h>

// �v����ơG�]�t GL �K�� ID �P�e��
struct TextureData {
    GLuint id;
    int    width;
    int    height;
};

// CTexturePool�G��ҶK�Ͻw�s�A�ëO�s�K�ϼe��
class CTexturePool {
public:
    static CTexturePool& getInstance();

    /// ���o�θ��J�v���A�æ^�Ǹ�ơ]id�B�e�B���^
    const TextureData& getTexture(const std::string& path, bool bMipMap = false);

    /// �Ȭd�ߤw���J���v����ơA���s�b�� id=0, width=height=0
    TextureData getTextureData(const std::string& path) const;

private:
    CTexturePool();
    ~CTexturePool();
    CTexturePool(const CTexturePool&) = delete;
    CTexturePool& operator=(const CTexturePool&) = delete;

    void cleanup();  // ����Ҧ��K��

    std::unordered_map<std::string, TextureData> m_pool;
};