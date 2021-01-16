#pragma once

class Util
{
private:
	static bool LoadTextureFromFileDx11(const char* filename, ID3D11ShaderResourceView** out_srv);

public:
	static void ClearCharTasksVehCheck(CPed* ped);
	static CPed *GetClosestPed();
	static CVehicle *GetClosestVehicle();
	static int GetLargestGangInZone();
	static void LoadTexturesInDirRecursive(const char * path, const char * file_ext, std::vector<std::string>& category_vec, std::vector<std::unique_ptr<TextureStructure>> &store_vec);
	static bool IsOnMission();
	static std::string GetLocationName(CVector *pos);
	static void RainbowValues(int &r, int&g, int &b, float speed);

	// This below source is taken from MoonAdditions https://github.com/THE-FYP/MoonAdditions
	// MIT License

	// Copyright (c) 2012 DK22Pac
	// Copyright (c) 2017 FYP

	// Permission is hereby granted, free of charge, to any person obtaining a copy
	// of this software and associated documentation files (the "Software"), to deal
	// in the Software without restriction, including without limitation the rights
	// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
	// copies of the Software, and to permit persons to whom the Software is
	// furnished to do so, subject to the following conditions:

	// The above copyright notice and this permission notice shall be included in all
	// copies or substantial portions of the Software.

	// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
	// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
	// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
	// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
	// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
	// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
	// SOFTWARE.

	static RwTexture* LoadTextureFromPngFile(fs::path path);
	static void UnloadTexture(RwTexture* texture9);
};

