#include "pch.h"
#include "resourcestore.h"
#include <extensions/Paths.h>

ResourceStore::ResourceStore(const char* text, eResourceType type, ImVec2 imageSize)
    : m_ImageSize(imageSize), m_Type(type), m_FileName(text)
{
    if (m_Type != eResourceType::TYPE_IMAGE)
    {
        m_pData = std::make_unique<DataStore>(text);

        if (m_Type != eResourceType::TYPE_IMAGE_TEXT)
        {
            // Generate categories
            for (auto [k, v] : m_pData->Items())
            {
                m_Categories.push_back(std::string(k.str()));
            }
            UpdateSearchList();
        }
    }

    if (m_Type != eResourceType::TYPE_TEXT)
    {
        /*
            Textures need to be loaded from main thread
            Loading it directly here doesn't work
            TODO:
                Maybe enabling a dx9 flag fixes this?
                Switch to initScriptsEvent
        */
        Events::initGameEvent += [text, this]()
        {
            LoadTextureResource(text);
        };
    }
}

// Get dx9 texture object from RwTexture*
static IDirect3DTexture9** GetTextureFromRaster(RwTexture* pTexture)
{
    RwRasterEx* raster = (RwRasterEx*)(&pTexture->raster->parent);

    return (&raster->m_pRenderResource->texture);
}

RwTexDictionary* LoadTexDictionary(char const* filename)
{
    return plugin::CallAndReturnDynGlobal<RwTexDictionary*, char const*>(0x5B3860, filename);
}

RwTexture* ResourceStore::FindRwTextureByName(const std::string& name)
{
    for (auto& item: m_ImagesList)
    {
        if (item->m_FileName == name)
        {
            return item->m_pRwTexture;
        }
    }
    return nullptr;
}

IDirect3DTexture9** ResourceStore::FindTextureByName(const std::string& name)
{
    RwTexture *pTex = FindRwTextureByName(name);
    if (pTex)
    {
        return GetTextureFromRaster(pTex);
    }
    return nullptr;
}

void ResourceStore::LoadTextureResource(std::string&& name)
{
    std::string fullPath = PLUGIN_PATH((char*)FILE_NAME "\\") + name + ".txd";

    if (!std::filesystem::exists(fullPath))
    {
        Log::Print<eLogLevel::Warn>("Failed to load {}", fullPath);
        return;
    }

    RwTexDictionary* pRwTexDictionary = LoadTexDictionary(fullPath.c_str());

    if (pRwTexDictionary)
    {
        RwLinkList *pRLL = (RwLinkList*)pRwTexDictionary->texturesInDict.link.next;
        RwTexDictionary *pEndDic;
        bool addCategories = (m_Categories.size() < 3); // "All", "Custom"
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

            if (m_Type == TYPE_TEXT_IMAGE)
            {
                // generate categories from text data
                for (auto [k, v] : m_pData->Items())
                {
                    std::string val = v.value_or<std::string>("Unknown");
                    if (val == str)
                    {
                        m_ImagesList.back().get()->m_CategoryName = k.str();
                        break;
                    }
                }
            }
            else
            {
                m_ImagesList.back().get()->m_CategoryName = str;
            }

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
            if (m_Type != TYPE_TEXT_IMAGE &&
                    !std::count(m_Categories.begin(), m_Categories.end(), m_ImagesList.back().get()->m_CategoryName))
            {
                m_Categories.push_back(m_ImagesList.back().get()->m_CategoryName);
            }

            pRLL = (RwLinkList*)pEndDic;
        }
        while ( pEndDic != (RwTexDictionary*)&pRwTexDictionary->texturesInDict );
    }
    m_bSearchListUpdateRequired = true;
}

void ResourceStore::UpdateSearchList(bool favourites, fRtnArg1_t getNameFunc, fRtnBoolArg1_t verifyFunc)
{
    m_nSearchList.clear();
    if (favourites)
    {
        if (m_Type == eResourceType::TYPE_TEXT)
        {
            for (auto [key, val] : *m_pData->GetTable("Favourites"))
            {
                ListLookup lookup;
                lookup.key = std::string(key.str());
                if (m_Filter.PassFilter(lookup.key.c_str()))
                {
                    lookup.cat = "Favourites";
                    lookup.val = val.value_or<std::string>("Unkonwn");
                    m_nSearchList.push_back(std::move(lookup));
                }
            }
        }
        else
        {
            for (auto [key, val] : *m_pData->GetTable("Favourites"))
            {
                for (uint i = 0; i < m_ImagesList.size(); ++i)
                {
                    ImageLookup lookup;
                    lookup.m_FileName = m_ImagesList[i]->m_FileName;
                    lookup.m_ModelName = getNameFunc == nullptr ? "" : getNameFunc(lookup.m_FileName);

                    if (lookup.m_ModelName == key.str() && m_Filter.PassFilter(lookup.m_ModelName.c_str())
                            && (verifyFunc == nullptr || verifyFunc(lookup.m_FileName)))
                    {
                        lookup.m_bCustom = lookup.m_FileName.find("Added") != std::string::npos;
                        lookup.m_pTexture = m_ImagesList[i]->m_pTexture;
                        m_nSearchList.push_back(std::move(lookup));
                        break;
                    }
                }

                if (m_Type == eResourceType::TYPE_IMAGE_TEXT)
                {
                    for (auto [key, val] : *m_pData->GetTable("Custom"))
                    {
                        ImageLookup lookup;
                        lookup.m_FileName =  val.as_string()->value_or("0");
                        lookup.m_ModelName = std::string(key.str());
                        if (lookup.m_ModelName == key.str() && m_Filter.PassFilter(lookup.m_ModelName.c_str())
                                && (m_Selected == "Custom" || m_Selected == "All"))
                        {
                            lookup.m_bCustom = true;
                            m_nSearchList.push_back(std::move(lookup));
                        }
                    }
                }
            }
        }
    }
    else
    {
        if (m_Type == eResourceType::TYPE_TEXT)
        {
            for (auto [cat, table] : m_pData->Items())
            {
                // Don't show favourites in "All"
                if (m_Selected == "All" && cat == "Favourites")
                {
                    continue;
                }
                if (cat.str() == m_Selected || m_Selected == "All")
                {
                    if (!table.as_table())
                    {
                        return;
                    }
                    for (auto [key, val] : *table.as_table()->as_table())
                    {
                        ListLookup lookup;
                        lookup.key = std::string(key.str());
                        if (m_Filter.PassFilter(lookup.key.c_str()))
                        {
                            lookup.cat = cat.str();
                            lookup.val = val.value_or<std::string>("Unkonwn");
                            m_nSearchList.push_back(std::move(lookup));
                        }
                    }
                }
            }
        }
        else
        {
            for (uint i = 0; i < m_ImagesList.size(); ++i)
            {
                ImageLookup lookup;
                lookup.m_FileName = m_ImagesList[i]->m_FileName;
                lookup.m_ModelName = getNameFunc == nullptr ? "" : getNameFunc(lookup.m_FileName);

                if (m_Filter.PassFilter(lookup.m_ModelName.c_str())
                        && (m_ImagesList[i]->m_CategoryName == m_Selected || m_Selected == "All")
                        && (verifyFunc == nullptr || verifyFunc(lookup.m_FileName))
                   )
                {
                    lookup.m_bCustom = false;
                    lookup.m_pTexture = m_ImagesList[i]->m_pTexture;
                    m_nSearchList.push_back(std::move(lookup));
                }
            }

            if (m_Type == eResourceType::TYPE_IMAGE_TEXT)
            {
                for (auto [k, v] : *m_pData->GetTable("Custom"))
                {
                    ImageLookup lookup;
                    lookup.m_FileName =  v.as_string()->value_or("0");
                    lookup.m_ModelName = std::string(k.str());
                    if (m_Filter.PassFilter(lookup.m_ModelName.c_str())
                            && (m_Selected == "Custom" || m_Selected == "All"))
                    {
                        lookup.m_bCustom = true;
                        m_nSearchList.push_back(std::move(lookup));
                    }
                }
            }
        }
    }
    m_nSearchList.shrink_to_fit();
}