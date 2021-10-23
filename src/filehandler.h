#pragma once
#include <map>
#include <vector>
#include <string>

class FileHandler
{
public:
    FileHandler() = delete;
    FileHandler(FileHandler&) = delete;

    /*
        Parses data/carcols.dat file and stores color data
        TODO: Implement something that also parses modloader data
    */
    static void FetchColorData(std::vector<std::vector<float>>& storeVec);
    /*
        Parses data/vehicles.ide file and stores handingId in a map
        TODO: Implement something that also parses modloader data
    */
    static void FetchHandlingID(std::map<int, std::string>& storeMap);
    static void GenerateHandlingFile(int pHandling, std::map<int, std::string>& storeMap);
};

