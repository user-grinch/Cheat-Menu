#include "plugin.h"

unsigned char* m_nDirectionalMult{ (unsigned char*)patch::GetPointer(0x55F7C7)};		 //m_nDirectionalMult[184]
			
unsigned char* m_nWaterFogAlpha{ (unsigned char*)patch::GetPointer(0x55F7B8)};			 //m_nWaterFogAlpha[184]
			
unsigned char* m_nHighLightMinIntensity{ (unsigned char*)patch::GetPointer(0x55F7A9)};   //m_nHighLightMinIntensity[184]
			
unsigned char* m_fCloudAlpha{ (unsigned char*)patch::GetPointer(0x55F793)};              //m_fCloudAlpha[184]
			
unsigned char* m_fPostFx2Alpha{ (unsigned char*)patch::GetPointer(0x55F77D)};            //m_fPostFx2Alpha[184]
			
unsigned char* m_fPostFx2Blue{ (unsigned char*)patch::GetPointer(0x55F767)};             //m_fPostFx2Blue[184]
			
unsigned char* m_fPostFx2Green{ (unsigned char*)patch::GetPointer(0x55F751)};            //m_fPostFx2Green[184]
			
unsigned char* m_fPostFx2Red{ (unsigned char*)patch::GetPointer(0x55F73B)};              //m_fPostFx2Red[184]
				
unsigned char* m_fPostFx1Alpha{ (unsigned char*)patch::GetPointer(0x55F725)};            //m_fPostFx1Alpha[184]
				
unsigned char* m_fPostFx1Blue{ (unsigned char*)patch::GetPointer(0x55F70F)};             //m_fPostFx1Blue[184]
			
unsigned char* m_fPostFx1Green{ (unsigned char*)patch::GetPointer(0x55F6FC)};            //m_fPostFx1Green[184]
			
unsigned char* m_fPostFx1Red{ (unsigned char*)patch::GetPointer(0x55F6E9)};              //m_fPostFx1Red[184]
			
unsigned char* m_fWaterAlpha{ (unsigned char*)patch::GetPointer(0x55F6D6)};              //m_fWaterAlpha[184]
			
unsigned char* m_fWaterBlue{ (unsigned char*)patch::GetPointer(0x55F6C3)};               //m_fWaterBlue[184]
				
unsigned char* m_fWaterGreen{ (unsigned char*)patch::GetPointer(0x55F6B0)};              //m_fWaterGreen[184]
				
unsigned char* m_fWaterRed{ (unsigned char*)patch::GetPointer(0x55F69C)};                //m_fWaterRed[184]
				 
unsigned char* m_nFluffyCloudsBottomBlue{ (unsigned char*)patch::GetPointer(0x55F690)};  //m_nFluffyCloudsBottomBlue[184]
				
unsigned char* m_nFluffyCloudsBottomGreen{ (unsigned char*)patch::GetPointer(0x55F683)}; //m_nFluffyCloudsBottomGreen[184]
				 
unsigned char* m_nFluffyCloudsBottomRed{ (unsigned char*)patch::GetPointer(0x55F677)};   //m_nFluffyCloudsBottomRed[184]
				
unsigned char* m_nLowCloudsBlue{ (unsigned char*)patch::GetPointer(0x55F66B)};           //m_nLowCloudsBlue[184]
			
unsigned char* m_nLowCloudsGreen{ (unsigned char*)patch::GetPointer(0x55F65F)};          //m_nLowCloudsGreen[184]
				
unsigned char* m_nLowCloudsRed{ (unsigned char*)patch::GetPointer(0x55F653)};            //m_nLowCloudsRed[184]

unsigned char* m_fLightsOnGroundBrightness{ (unsigned char*)patch::GetPointer(0x55F640)}; //m_fLightsOnGroundBrightness[184]

short* m_fFogStart{ (short*)patch::GetPointer(0x55F62E)};                 //m_fFogStart[184]

short* m_fFarClip{ (short*)patch::GetPointer(0x55F61B)};                  //m_fFarClip[184]

unsigned char* m_nPoleShadowStrength{ (unsigned char*)patch::GetPointer(0x55F60F)};      //m_nPoleShadowStrength[184]

unsigned char* m_nLightShadowStrength{ (unsigned char*)patch::GetPointer(0x55F603)};     //m_nLightShadowStrength[184]

unsigned char* m_nShadowStrength{ (unsigned char*)patch::GetPointer(0x55F5F7)};          //m_nShadowStrength[184]

unsigned char* m_fSpriteBrightness{ (unsigned char*)patch::GetPointer(0x55F5E4)};                   //m_fSpriteBrightness[184]

unsigned char* m_fSpriteSize{ (unsigned char*)patch::GetPointer(0x55F5D2)};                        //m_fSpriteSize[184]

unsigned char* m_fSunSize{ (unsigned char*)patch::GetPointer(0x55F5C0)};                           //m_fSunSize[184]

unsigned char* m_nSunCoronaBlue{ (unsigned char*)patch::GetPointer(0x55F5B5)};          //m_nSunCoronaBlue[184]

unsigned char* m_nSunCoronaGreen{ (unsigned char*)patch::GetPointer(0x55F5A9)};         //m_nSunCoronaGreen[184]

unsigned char* m_nSunCoronaRed{ (unsigned char*)patch::GetPointer(0x55F59D)};           //m_nSunCoronaRed[184]

unsigned char* m_nSunCoreBlue{ (unsigned char*)patch::GetPointer(0x55F591)};           //m_nSunCoreBlue[184]

unsigned char* m_nSunCoreGreen{ (unsigned char*)patch::GetPointer(0x55F585)};          //m_nSunCoreGreen[184]

unsigned char* m_nSunCoreRed{ (unsigned char*)patch::GetPointer(0x55F579)};            //m_nSunCoreRed[184]

unsigned char* m_nSkyBottomBlue{ (unsigned char*)patch::GetPointer(0x55F56D)};         //m_nSkyBottomBlue[184]

unsigned char* m_nSkyBottomGreen{ (unsigned char*)patch::GetPointer(0x55F561)};        //m_nSkyBottomGreen[184]

unsigned char* m_nSkyBottomRed{ (unsigned char*)patch::GetPointer(0x55F555)};         //m_nSkyBottomRed[184]

unsigned char* m_nSkyTopBlue{ (unsigned char*)patch::GetPointer(0x55F549)};            //m_nSkyTopBlue[45]

unsigned char* m_nSkyTopGreen{ (unsigned char*)patch::GetPointer(0x55F53D)};           //m_nSkyTopGreen[45]

unsigned char* m_nSkyTopRed{ (unsigned char*)patch::GetPointer(0x55F531)};             //m_nSkyTopRed[45]

unsigned char* m_nAmbientBlue_Obj{ (unsigned char*)patch::GetPointer(0x55F51E)};      //m_nAmbientBlue_Obj[184]

unsigned char* m_nAmbientGreen_Obj{ (unsigned char*)patch::GetPointer(0x55F50C)};     //m_nAmbientGreen_Obj[184]

unsigned char* m_nAmbientRed_Obj{ (unsigned char*)patch::GetPointer(0x55F4FA)};       //m_nAmbientRed_Obj[184]

unsigned char* m_nAmbientBlue{ (unsigned char*)patch::GetPointer(0x55F4E8)};          //m_nAmbientBlue[184]

unsigned char* m_nAmbientGreen{ (unsigned char*)patch::GetPointer(0x55F4D6)};         //m_nAmbientGreen[184]

unsigned char* m_nAmbientRed{ (unsigned char*)patch::GetPointer(0x560C61)};           //m_nAmbientRed[184]
