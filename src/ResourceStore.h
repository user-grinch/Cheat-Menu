#include <string>
#include <rw/rwcore.h>
#include "Json.h"
#include "../depend/imgui/imgui.h"
#include "d3d9.h"

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

struct STextureResource
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
};

using TextureResourceList = std::vector<std::unique_ptr<STextureResource>>;
class ResourceStore
{
private:
    void LoadTextureResource(std::string&& path);

public:
	ImGuiTextFilter m_Filter = "";
	std::vector<std::string> m_Categories = {"All"};
	std::string m_Selected = "All";
	std::unique_ptr<CJson> m_pJson;
	TextureResourceList m_ImagesList;
	bool m_bTexturesLoaded = false;
	
	ResourceStore(const char* text, eResourceType type = TYPE_IMAGE);
};