#include "pch.h"
#include "CFileLoader.h"
#include "extensions/Paths.h"

ResourceStore::ResourceStore(const char* text, eResourceType type, ImVec2 imageSize)
: m_ImageSize(imageSize)
{
    if (type != eResourceType::TYPE_TEXT)
    {
        /*  
            Textures need to be loaded from main thread
            Loading it directly here doesn't work
        */
        Events::processScriptsEvent += [text, this]()
        {
            if (!m_bTexturesLoaded)
            {
                LoadTextureResource(text);
                m_bTexturesLoaded = true;
            }
        };

        if (type == eResourceType::TYPE_BOTH)
        {
            goto loadJson;
        }
    }
    else
    {
        loadJson:
        m_pJson = std::make_unique<CJson>(text);

        // Generate categories
        for (auto element : m_pJson->m_Data.items())
        {
            m_Categories.push_back(element.key());
        }
    }
}

static void* GetTextureFromRaster(RwTexture* pTexture)
{
	RwRasterEx* raster = (RwRasterEx*)(&pTexture->raster->parent);

	return (&raster->m_pRenderResource->texture);
}

void ResourceStore::LoadTextureResource(std::string&& name)
{
    std::string fullPath = PLUGIN_PATH((char*)"CheatMenu\\") + name + ".txd";
	RwTexDictionary* pRwTexDictionary = CFileLoader::LoadTexDictionary(fullPath.c_str());

	if (pRwTexDictionary)
	{
        RwLinkList *pRLL = (RwLinkList*)pRwTexDictionary->texturesInDict.link.next;
        RwTexDictionary *pEndDic;
        do
        {
            pEndDic = (RwTexDictionary*)pRLL->link.next;
            RwTexture *pTex = (RwTexture*)&pRLL[-1];

            m_ImagesList.push_back(std::make_unique<STextureResource>());
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
            std::string itemNames;
            if (!std::count(m_Categories.begin(), m_Categories.end(), itemNames))
            {
                m_Categories.push_back(itemNames);
            }
            pRLL = (RwLinkList*)pEndDic;
        }
        while ( pEndDic != (RwTexDictionary*)&pRwTexDictionary->texturesInDict );
	}
}