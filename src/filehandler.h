#pragma once
#include <map>
#include <vector>

/*
    FileHandler class
    Handles files IO and processing
*/
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

#ifdef GTASA
    /*
        Parses data/vehicles.ide file and stores handingId in a map
        TODO: Implement something that also parses modloader data
    */
    static void FetchHandlingID(std::map<int, std::string>& storeMap);
    static void GenerateHandlingFile(tHandlingData *pHandling, std::map<int, std::string>& storeMap);
#endif
};

