#pragma once

#ifdef GTASA
uchar *m_nAmbientRed = (uchar *)0x560C61;
uchar *m_nAmbientGreen = (uchar *)0x55F4D6;
uchar *m_nAmbientBlue = (uchar *)0x55F4E8;

uchar *m_nAmbientRed_Obj = (uchar *)0x55F4FA;
uchar *m_nAmbientGreen_Obj = (uchar *)0x55F50C;
uchar *m_nAmbientBlue_Obj = (uchar *)0x55F51E;

uchar *m_nSkyTopRed = (uchar *)0x55F531;
uchar *m_nSkyTopGreen = (uchar *)0x55F53D;
uchar *m_nSkyTopBlue = (uchar *)0x55F549;

uchar *m_nSkyBottomRed = (uchar *)0x55F555;
uchar *m_nSkyBottomGreen = (uchar *)0x55F561;
uchar *m_nSkyBottomBlue = (uchar *)0x55F56D;

uchar *m_nSunCoreRed = (uchar *)0x55F59D;
uchar *m_nSunCoreGreen = (uchar *)0x55F5A9;
uchar *m_nSunCoreBlue = (uchar *)0x55F5B5;

uchar *m_nSunCoronaRed = (uchar *)0x55F579;
uchar *m_nSunCoronaGreen = (uchar *)0x55F585;
uchar *m_nSunCoronaBlue = (uchar *)0x55F591;

uchar *m_nLowCloudsRed = (uchar *)0x55F653;
uchar *m_nLowCloudsGreen = (uchar *)0x55F65F;
uchar *m_nLowCloudsBlue = (uchar *)0x55F66B;

uchar *m_fLightsOnGroundBrightness = (uchar *)0x55F640;
uchar *m_nPoleShadowStrength = (uchar *)0x55F60F;
uchar *m_nLightShadowStrength = (uchar *)0x55F603;
uchar *m_nShadowStrength = (uchar *)0x55F5F7;
uchar *m_fSpriteBrightness = (uchar *)0x55F5E4;
uchar *m_fSpriteSize = (uchar *)0x55F5D2;
uchar *m_fSunSize = (uchar *)0x55F5C0;
short *m_fFogStart = (short *)0x55F62E;
short *m_fFarClip = (short *)0x55F61B;

uchar *m_fWaterRed = (uchar *)0x55F69C;
uchar *m_fWaterGreen = (uchar *)0x55F6B0;
uchar *m_fWaterBlue = (uchar *)0x55F6C3;
uchar *m_fWaterAlpha = (uchar *)0x55F6D6;

uchar *m_fPostFx2Alpha = (uchar *)0x55F77D;
uchar *m_fPostFx2Blue = (uchar *)0x55F767;
uchar *m_fPostFx2Green = (uchar *)0x55F751;
uchar *m_fPostFx2Red = (uchar *)0x55F73B;
uchar *m_fPostFx1Alpha = (uchar *)0x55F725;
uchar *m_fPostFx1Blue = (uchar *)0x55F70F;
uchar *m_fPostFx1Green = (uchar *)0x55F6FC;
uchar *m_fPostFx1Red = (uchar *)0x55F6E9;

uchar *m_nFluffyCloudsBottomRed = (uchar *)0x55F677;
uchar *m_nFluffyCloudsBottomGreen = (uchar *)0x55F683;
uchar *m_nFluffyCloudsBottomBlue = (uchar *)0x55F690;

uchar *m_nDirectionalMult = (uchar *)0x55F7C7;
uchar *m_nWaterFogAlpha = (uchar *)0x55F7B8;
uchar *m_nHighLightMinIntensity = (uchar *)0x55F7A9;
uchar *m_fCloudAlpha = (uchar *)0x55F793;

#elif GTAVC
uchar *m_nAmbientRed = (uchar *)0x9388C8;
uchar *m_nAmbientGreen = (uchar *)0x78D508;
uchar *m_nAmbientBlue = (uchar *)0x813D48;

uchar *m_nAmbientRed_Obj = (uchar *)0x7D53B0;
uchar *m_nAmbientGreen_Obj = (uchar *)0x7D9370;
uchar *m_nAmbientBlue_Obj = (uchar *)0x94C038;

uchar *m_nSkyTopRed = (uchar *)0x94BE98;
uchar *m_nSkyTopGreen = (uchar *)0x933A58;
uchar *m_nSkyTopBlue = (uchar *)0x94A170;

uchar *m_nSkyBottomRed = (uchar *)0x7E44A0;
uchar *m_nSkyBottomGreen = (uchar *)0x7E9CD0;
uchar *m_nSkyBottomBlue = (uchar *)0x92D420;

uchar *m_nSunCoreRed = (uchar *)0x944098;
uchar *m_nSunCoreGreen = (uchar *)0x7D94C0;
uchar *m_nSunCoreBlue = (uchar *)0x8614F8;

uchar *m_nSunCoronaRed = (uchar *)0x78CCD8;
uchar *m_nSunCoronaGreen = (uchar *)0x7DBC08;
uchar *m_nSunCoronaBlue = (uchar *)0x7D1818;

uchar *m_nLowCloudsRed = (uchar *)0x8100C0;
uchar *m_nLowCloudsGreen = (uchar *)0x94AEA0;
uchar *m_nLowCloudsBlue = (uchar *)0x94BDF0;

uchar *m_fLightsOnGroundBrightness = (uchar *)0x92D378;
uchar *m_nPoleShadowStrength = (uchar *)0x812090;
uchar *m_nLightShadowStrength = (uchar *)0x811828;
uchar *m_nShadowStrength = (uchar *)0x92D260;
uchar *m_fSpriteBrightness = (uchar *)0x7D5308;
uchar *m_fSpriteSize = (uchar *)0x94B848;
uchar *m_fSunSize = (uchar *)0x818720;
short *m_fFogStart = (short *)0x94D638;
short *m_fFarClip = (short *)0x94D4E8;

uchar *m_fWaterRed = (uchar *)0x7E5560;
uchar *m_fWaterGreen = (uchar *)0x7DE148;
uchar *m_fWaterBlue = (uchar *)0x862248;
uchar *m_fWaterAlpha = (uchar *)0x7DDAE0;

uchar *m_nAmbientBlRed = (uchar *)0x78D5B0;
uchar *m_nAmbientBlGreen = (uchar *)0x861430;
uchar *m_nAmbientBlBlue = (uchar *)0x811740;

uchar *m_nAmbientBlRed_Obj = (uchar *)0x7D9418;
uchar *m_nAmbientBlGreen_Obj = (uchar *)0x818FC0;
uchar *m_nAmbientBlBlue_Obj = (uchar *)0x813B68;

uchar *m_nDirRed = (uchar *)0x7E45E0;
uchar *m_nDirGreen = (uchar *)0x7DDDF8;
uchar *m_nDirBlue = (uchar *)0x92D1B8;

uchar *m_nTopCloudsRed = (uchar *)0x7F3C80;
uchar *m_nTopCloudsGreen = (uchar *)0x813F70;
uchar *m_nTopCloudsBlue = (uchar *)0x938980;

uchar *m_nBottomCloudsRed = (uchar *)0x7D3D98;
uchar *m_nBottomCloudsGreen = (uchar *)0x7FFE38;
uchar *m_nBottomCloudsBlue = (uchar *)0x7D9630;

uchar *m_nBlurRed = (uchar *)0x94B790;
uchar *m_nBlurGreen = (uchar *)0x8621A0;
uchar *m_nBlurBlue = (uchar *)0x945728;

#else // GTA3

int *m_nAmbientRed = (int *)0x86AF78;
int *m_nAmbientGreen = (int *)0x665308;
int *m_nAmbientBlue = (int *)0x72CF88;

int *m_nDirRed = (int *)0x6FAB78;
int *m_nDirGreen = (int *)0x6F4528;
int *m_nDirBlue = (int *)0x83CE58;

int *m_nSkyTopRed = (int *)0x87FB90;
int *m_nSkyTopGreen = (int *)0x8460A8;
int *m_nSkyTopBlue = (int *)0x87B158;

int *m_nSkyBottomRed = (int *)0x6FA960;
int *m_nSkyBottomGreen = (int *)0x70D6A8;
int *m_nSkyBottomBlue = (int *)0x83D288;

int *m_nSunCoreRed = (int *)0x878360;
int *m_nSunCoreGreen = (int *)0x6EE088;
int *m_nSunCoreBlue = (int *)0x773A68;

int *m_nSunCoronaRed = (int *)0x664B60;
int *m_nSunCoronaGreen = (int *)0x6F01E0;
int *m_nSunCoronaBlue = (int *)0x6E6340;

short *m_nShadowStrength = (short *)0x83CFD8;
short *m_nLightShadowStrength = (short *)0x72B0F8;
short *m_nTreeShadowStrength = (short *)0x733450;
float *m_fSunSize = (float *)0x733510;
float *m_fSpriteSize = (float *)0x87F820;
float *m_fSpriteBrightness = (float *)0x6E96F0;
float *m_fFarClip = (float *)0x8804E0;
float *m_fFogStart = (float *)0x8806C8;
float *m_fLightsOnGroundBrightness = (float *)0x83D108;

int *m_nLowCloudsRed = (int *)0x726770;
int *m_nLowCloudsGreen = (int *)0x87BF08;
int *m_nLowCloudsBlue = (int *)0x87FA10;

int *m_nTopCloudsRed = (int *)0x70F2B0;
int *m_nTopCloudsGreen = (int *)0x72D288;
int *m_nTopCloudsBlue = (int *)0x86B108;

int *m_nBottomCloudsRed = (int *)0x6E8DA8;
int *m_nBottomCloudsGreen = (int *)0x715AA8;
int *m_nBottomCloudsBlue = (int *)0x6EE2D0;

float *m_fPostFxRed = (float *)0x87C7E0;
float *m_fPostFxGreen = (float *)0x774C10;
float *m_fPostFxBlue = (float *)0x8784E0;
float *m_fPostFxAlpha = (float *)0x733690;
#endif
