#pragma once
#include <string>
#include <d3d9.h>
#include <rw/rwcore.h>
#include "imgui/imgui.h"
#include "utils/datastore.h"

struct RwD3D9Raster
{
    union
    {
        IDirect3DTexture9* texture;
        IDirect3DSurface9* surface;
    };
    unsigned char* palette;
    unsigned char alpha;
    unsigned char cubeTextureFlags; /* 0x01 IS_CUBEMAP_TEX */
    unsigned char textureFlags; /* 0x10 IS_COMPRESSED */
    unsigned char lockedLevel;
    IDirect3DSurface9* lockedSurface;
    D3DLOCKED_RECT lockedRect;
    D3DFORMAT format;
    IDirect3DSwapChain9* swapChain;
    HWND* hwnd;
};

struct RwRasterEx : public RwRaster
{
    RwD3D9Raster *m_pRenderResource;
};

struct TextureResource
{
    std::string m_FileName;
    std::string m_CategoryName;
    RwTexture *m_pRwTexture = nullptr;
    void *m_pTexture = nullptr;
};

enum eResourceType
{
    TYPE_IMAGE,
    TYPE_TEXT,
    TYPE_IMAGE_TEXT, // priotizes images
    TYPE_TEXT_IMAGE, // priotizes texts
};

/*
	Global Resource Handler Class
	Handles loading & unloading both text (json) & image files
*/
class ResourceStore
{
private:
    // Loads a image texture from it's path
    void LoadTextureResource(std::string&& path);

public:
    ImGuiTextFilter m_Filter = "";
    std::vector<std::string> m_Categories = {"All", "Custom"};
    std::string m_Selected = "All";
    std::string m_FileName;
    std::unique_ptr<DataStore> m_pData;
    std::vector<std::unique_ptr<TextureResource>> m_ImagesList;
    ImVec2 m_ImageSize;
    eResourceType m_Type;
    bool m_bTexturesLoaded = false;

    ResourceStore(const char* text, eResourceType type = TYPE_IMAGE, ImVec2 imageSize = ImVec2(64, 64));

    RwTexture* FindRwTextureByName(const std::string& name);
    IDirect3DTexture9** FindTextureByName(const std::string& name);
};