uchar* m_nAmbientRed = (uchar*)BY_GAME(0x560C61, 0x9388C8);
uchar* m_nAmbientGreen = (uchar*)BY_GAME(0x55F4D6, 0x78D508); 
uchar* m_nAmbientBlue = (uchar*)BY_GAME(0x55F4E8, 0x813D48); 

uchar* m_nAmbientRed_Obj = (uchar*)BY_GAME(0x55F4FA, 0x7D53B0);
uchar* m_nAmbientGreen_Obj = (uchar*)BY_GAME(0x55F50C, 0x7D9370);
uchar* m_nAmbientBlue_Obj = (uchar*)BY_GAME(0x55F51E, 0x94C038); 

uchar* m_nSkyTopRed = (uchar*)BY_GAME(0x55F531, 0x94BE98); 
uchar* m_nSkyTopGreen = (uchar*)BY_GAME(0x55F53D, 0x933A58); 
uchar* m_nSkyTopBlue = (uchar*)BY_GAME(0x55F549, 0x94A170); 

uchar* m_nSkyBottomRed = (uchar*)BY_GAME(0x55F555, 0x7E44A0); 
uchar* m_nSkyBottomGreen = (uchar*)BY_GAME(0x55F561, 0x7E9CD0); 
uchar* m_nSkyBottomBlue = (uchar*)BY_GAME(0x55F56D, 0x92D420); 

uchar* m_nSunCoreRed = (uchar*)BY_GAME(0x55F59D, 0x944098); 
uchar* m_nSunCoreGreen = (uchar*)BY_GAME(0x55F5A9, 0x7D94C0); 
uchar* m_nSunCoreBlue = (uchar*)BY_GAME(0x55F5B5, 0x8614F8); 

uchar* m_nSunCoronaRed = (uchar*)BY_GAME(0x55F579, 0x78CCD8); 
uchar* m_nSunCoronaGreen = (uchar*)BY_GAME(0x55F585, 0x7DBC08); 
uchar* m_nSunCoronaBlue = (uchar*)BY_GAME(0x55F591, 0x7D1818); 

uchar* m_nLowCloudsRed = (uchar*)BY_GAME(0x55F653, 0x8100C0); 
uchar* m_nLowCloudsGreen = (uchar*)BY_GAME(0x55F65F, 0x94AEA0); 
uchar* m_nLowCloudsBlue = (uchar*)BY_GAME(0x55F66B, 0x94BDF0); 

uchar* m_fLightsOnGroundBrightness = (uchar*)BY_GAME(0x55F640, 0x92D378); 
uchar* m_nPoleShadowStrength = (uchar*)BY_GAME(0x55F60F, 0x812090); 
uchar* m_nLightShadowStrength = (uchar*)BY_GAME(0x55F603, 0x811828); 
uchar* m_nShadowStrength = (uchar*)BY_GAME(0x55F5F7, 0x92D260); 
uchar* m_fSpriteBrightness = (uchar*)BY_GAME(0x55F5E4, 0x7D5308); 
uchar* m_fSpriteSize = (uchar*)BY_GAME(0x55F5D2, 0x94B848); 
uchar* m_fSunSize = (uchar*)BY_GAME(0x55F5C0, 0x818720); 
short* m_fFogStart = (short*)BY_GAME(0x55F62E, 0x94D638); 
short* m_fFarClip = (short*)BY_GAME(0x55F61B, 0x94D4E8); 

uchar* m_fWaterRed = (uchar*)BY_GAME(0x55F69C, 0x7E5560); 
uchar* m_fWaterGreen = (uchar*)BY_GAME(0x55F6B0, 0x7DE148); 
uchar* m_fWaterBlue = (uchar*)BY_GAME(0x55F6C3, 0x862248); 
uchar* m_fWaterAlpha = (uchar*)BY_GAME(0x55F6D6, 0x7DDAE0); 

#ifdef GTASA
uchar* m_fPostFx2Alpha = (uchar*)0x55F77D; 
uchar* m_fPostFx2Blue = (uchar*)0x55F767; 
uchar* m_fPostFx2Green = (uchar*)0x55F751; 
uchar* m_fPostFx2Red = (uchar*)0x55F73B; 
uchar* m_fPostFx1Alpha = (uchar*)0x55F725; 
uchar* m_fPostFx1Blue = (uchar*)0x55F70F; 
uchar* m_fPostFx1Green = (uchar*)0x55F6FC; 
uchar* m_fPostFx1Red = (uchar*)0x55F6E9; 

uchar* m_nFluffyCloudsBottomRed = (uchar*)0x55F677; 
uchar* m_nFluffyCloudsBottomGreen = (uchar*)0x55F683; 
uchar* m_nFluffyCloudsBottomBlue = (uchar*)0x55F690; 

uchar* m_nDirectionalMult = (uchar*)0x55F7C7; 
uchar* m_nWaterFogAlpha = (uchar*)0x55F7B8; 
uchar* m_nHighLightMinIntensity = (uchar*)0x55F7A9; 
uchar* m_fCloudAlpha = (uchar*)0x55F793; 

#elif GTAVC
uchar* m_nAmbientBlRed = (uchar*)0x78D5B0;
uchar* m_nAmbientBlGreen = (uchar*)0x861430; 
uchar* m_nAmbientBlBlue = (uchar*)0x811740; 

uchar* m_nAmbientBlRed_Obj = (uchar*)0x7D9418;
uchar* m_nAmbientBlGreen_Obj = (uchar*)0x818FC0; 
uchar* m_nAmbientBlBlue_Obj = (uchar*)0x813B68; 

uchar* m_nDirRed = (uchar*)0x7E45E0;
uchar* m_nDirGreen = (uchar*)0x7DDDF8; 
uchar* m_nDirBlue = (uchar*)0x92D1B8; 

uchar* m_nTopCloudsRed = (uchar*)0x7F3C80; 
uchar* m_nTopCloudsGreen = (uchar*)0x813F70; 
uchar* m_nTopCloudsBlue = (uchar*)0x938980; 

uchar* m_nBottomCloudsRed = (uchar*)0x7D3D98; 
uchar* m_nBottomCloudsGreen = (uchar*)0x7FFE38; 
uchar* m_nBottomCloudsBlue = (uchar*)0x7D9630; 

uchar* m_nBlurRed = (uchar*)0x94B790; 
uchar* m_nBlurGreen = (uchar*)0x8621A0; 
uchar* m_nBlurBlue = (uchar*)0x945728; 
#endif
