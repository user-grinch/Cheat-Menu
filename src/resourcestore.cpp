#include "extensions/Paths.h"
#include "pch.h"

ResourceStore::ResourceStore(const char* text, eResourceType type, ImVec2 imageSize)
    : m_ImageSize(imageSize)
{
    if (type == eResourceType::TYPE_TEXT || type == eResourceType::TYPE_BOTH)
    {
        m_pData = std::make_unique<DataStore>(text);

        if (type == eResourceType::TYPE_TEXT)
        {
            // Generate categories
            for (auto [k, v] : m_pData->Items())
            {
                m_Categories.push_back(std::string(k.str()));
            }
        }
    }

    if (type == eResourceType::TYPE_IMAGE || type == eResourceType::TYPE_BOTH)
    {
        /*
            Textures need to be loaded from main thread
            Loading it directly here doesn't work
            TODO: 
                Maybe enabling a dx9 flag fixes this?
                Switch to initScriptsEvent
        */
        Events::processScriptsEvent += [text, this]()
        {
            if (!m_bTexturesLoaded)
            {
                LoadTextureResource(text);
                m_bTexturesLoaded = true;
            }
        };
    }
}

// Get dx9 texture object from RwTexture*
static IDirect3DTexture9** GetTextureFromRaster(RwTexture* pTexture)
{
    RwRasterEx* raster = (RwRasterEx*)(&pTexture->raster->parent);

    return (&raster->m_pRenderResource->texture);
}

RwTexDictionary* LoadTexDictionary(char const* filename) {
    return plugin::CallAndReturnDynGlobal<RwTexDictionary*, char const*>(0x5B3860, filename);
}

void ResourceStore::LoadTextureResource(std::string&& name)
{
    std::string fullPath = PLUGIN_PATH((char*)"CheatMenu\\") + name + ".txd";

    if (!std::filesystem::exists(fullPath))
    {
        //Log::PrintWarn("Failed to load {}", fullPath);
        return;
    }

    RwTexDictionary* pRwTexDictionary = LoadTexDictionary(fullPath.c_str());

    if (pRwTexDictionary)
    {
        RwLinkList *pRLL = (RwLinkList*)pRwTexDictionary->texturesInDict.link.next;
        RwTexDictionary *pEndDic;
        do
        {
            pEndDic = (RwTexDictionary*)pRLL->link.next;
            RwTexture *pTex = (RwTexture*)&pRLL[-1];

            m_ImagesList.push_back(std::make_unique<TextureResource>());
            m_ImagesList.back().get()->m_pRwTexture = pTex;

            // Fetch IDirec9Texture9* from RwTexture*
            m_ImagesList.back().get()->m_pTexture = GetTextureFromRaster(pTex);

            // Naming format in Txd `Category$TextureName`
            std::stringstream ss(pTex->name);
            std::string str;

            getline(ss, str, '$');
            m_ImagesList.back().get()->m_CategoryName = str;

            if (name == "clothes")
            {
                // pass full name
                m_ImagesList.back().get()->m_FileName = pTex->name;
            }
            else
            {
                getline(ss, str, '$');
                m_ImagesList.back().get()->m_FileName = str;
            }

            // Genereate categories
            if (!std::count(m_Categories.begin(), m_Categories.end(), m_ImagesList.back().get()->m_CategoryName))
            {
                m_Categories.push_back(m_ImagesList.back().get()->m_CategoryName);
            }
            pRLL = (RwLinkList*)pEndDic;
        }
        while ( pEndDic != (RwTexDictionary*)&pRwTexDictionary->texturesInDict );
    }
}