#include "pch.h"
#include "filehandler.h"

// TODO: Clean up this mess, use structures instead?
void FileHandler::GenerateHandlingFile(int pHandling, std::map<int, std::string>& storeMap)
{
    FILE* fp = fopen("handling.txt", "w");

    std::string handlingId = storeMap[FindPlayerPed()->m_pVehicle->m_nModelIndex];
    float fMass = patch::Get<float>(pHandling + 0x4);
    float fTurnMass = patch::Get<float>(pHandling + 0xC);
    float fDragMult = patch::Get<float>(pHandling + 0x10);
    float CentreOfMassX = patch::Get<float>(pHandling + 0x14);
    float CentreOfMassY = patch::Get<float>(pHandling + 0x18);
    float CentreOfMassZ = patch::Get<float>(pHandling + 0x1C);
    int nPercentSubmerged = patch::Get<int>(pHandling + 0x20);
    float fTractionMultiplier = patch::Get<float>(pHandling + 0x28);
    float fTractionLoss = patch::Get<float>(pHandling + 0xA4);
    float TractionBias = patch::Get<float>(pHandling + 0xA8);
    float fEngineAcceleration = patch::Get<float>(pHandling + 0x7C) * 12500;
    float fEngineInertia = patch::Get<float>(pHandling + 0x80);
    int nDriveType = patch::Get<BYTE>(pHandling + 0x74);
    int nEngineType = patch::Get<BYTE>(pHandling + 0x75);
    float BrakeDeceleration = patch::Get<float>(pHandling + 0x94) * 2500;
    float BrakeBias = patch::Get<float>(pHandling + 0x98);
    int ABS = patch::Get<BYTE>(pHandling + 0x9C);
    float SteeringLock = patch::Get<float>(pHandling + 0xA0);
    float SuspensionForceLevel = patch::Get<float>(pHandling + 0xAC);
    float SuspensionDampingLevel = patch::Get<float>(pHandling + 0xB0);
    float SuspensionHighSpdComDamp = patch::Get<float>(pHandling + 0xB4);
    float Suspension_upper_limit = patch::Get<float>(pHandling + 0xB8);
    float Suspension_lower_limit = patch::Get<float>(pHandling + 0xBC);
    float Suspension_bias = patch::Get<float>(pHandling + 0xC0);
    float Suspension_anti_dive_multiplier = patch::Get<float>(pHandling + 0xC4);
    float fCollisionDamageMultiplier = patch::Get<float>(pHandling + 0xC8) * 0.338;
    int nMonetaryValue = patch::Get<int>(pHandling + 0xD8);
    int MaxVelocity = patch::Get<float>(pHandling + 0x84);
    MaxVelocity = MaxVelocity * 206 + (MaxVelocity - 0.918668) * 1501;
    int modelFlags = patch::Get<int>(pHandling + 0xCC);
    int handlingFlags = patch::Get<int>(pHandling + 0xD0);
    int front_lights = patch::Get<BYTE>(pHandling + 0xDC);
    int rear_lights = patch::Get<BYTE>(pHandling + 0xDD);
    int vehicle_anim_group = patch::Get<BYTE>(pHandling + 0xDE);
    int nNumberOfGears = patch::Get<BYTE>(pHandling + 0x76);
    float fSeatOffsetDistance = patch::Get<float>(pHandling + 0xD4);

    // TODO: make this more readable
    fprintf(
        fp,
        "\n%s\t%.5g\t%.5g\t%.5g\t%.5g\t%.5g\t%.5g\t%d\t%.5g\t%.5g\t%.5g\t%d\t%d\t%.5g\t%.5g\t%c\t%c\t%.5g\t%.5g\t%d\t%.5g\t%.5g\t%.5g\t%.5g\t%.5g\t%.5g\t%.5g\t%.5g\t%.5g\t%.5g\t%d\t%d\t%d\t%d\t%d\t%d",
        handlingId.c_str(), fMass, fTurnMass, fDragMult, CentreOfMassX, CentreOfMassY, CentreOfMassZ, nPercentSubmerged,
        fTractionMultiplier, fTractionLoss, TractionBias, nNumberOfGears,
        MaxVelocity, fEngineAcceleration, fEngineInertia, nDriveType, nEngineType, BrakeDeceleration, BrakeBias, ABS,
        SteeringLock, SuspensionForceLevel, SuspensionDampingLevel,
        SuspensionHighSpdComDamp, Suspension_upper_limit, Suspension_lower_limit, Suspension_bias,
        Suspension_anti_dive_multiplier, fSeatOffsetDistance,
        fCollisionDamageMultiplier, nMonetaryValue, modelFlags, handlingFlags, front_lights, rear_lights,
        vehicle_anim_group);

    fclose(fp);
}

void FileHandler::FetchColorData(std::vector<std::vector<float>>& storeVec)
{
    std::string m_FilePath = GAME_PATH((char*)"/data/carcols.dat");
    if (!std::filesystem::exists(m_FilePath))
    {
        gLog << "Carcols.dat not found";
        return;
    }

    std::ifstream file(m_FilePath);
    std::string line;
    bool isCar, isCol;
    int count = 0;

    while (getline(file, line))
    {
        // skip commented & empty lines
        if (line[0] == '#' || line == "")
        {
            continue;
        }

        // section blocks
        if (line[0] == 'c' && line[1] == 'a' && line[2] == 'r')
        {
            isCar = true;
            continue;
        }

        if (line[0] == 'c' && line[1] == 'o' && line[2] == 'l')
        {
            isCol = true;
            continue;
        }

        if (line[0] == 'e' && line[1] == 'n' && line[2] == 'd')
        {
            isCar = false;
            isCol = false;
            continue;
        }

        if (isCol)
        {
            try
            {
                std::string temp;
                std::stringstream ss(line);

                // fix one instance where . is used instead of ,
                std::replace(temp.begin(), temp.end(), '.', ',');

                // Format: red, green, blue
                int r,g,b;
                getline(ss, temp, ',');
                r = std::stoi(temp);
                getline(ss, temp, ',');
                g = std::stoi(temp);
                getline(ss, temp, ',');
                b = std::stoi(temp);

                storeVec.push_back({r / 255.0f, g / 255.0f, b / 255.0f});
                ++count;
            }
            catch (...)
            {
                gLog << "Error parsing carcols.dat, " << line << std::endl;
            }
        }
    }

    file.close();
}

void FileHandler::FetchHandlingID(std::map<int, std::string>& storeMap)
{
    std::string m_FilePath = GAME_PATH((char*)"/data/vehicles.ide");
    if (!std::filesystem::exists(m_FilePath))
    {
        gLog << "Vehicle.ide not found";
        return;
    }

    std::ifstream file(m_FilePath);
    std::string line;
    while (getline(file, line))
    {
        /*
        	Format: model, modelname, txdname, type, handlingId, ...
        	Skip if first thing isn't model id
        */
        if (line[0] <= '0' || line[0] >= '9')
        {
            continue;
        }

        // running inside try block to handle user errors, mostly commas
        try
        {
            std::string temp;
            std::stringstream ss(line);

            // get model
            getline(ss, temp, ',');
            int model = std::stoi(temp);

            // get modelname, txd, type, handlingId
            getline(ss, temp, ',');
            getline(ss, temp, ',');
            getline(ss, temp, ',');
            getline(ss, temp, ',');

            temp.erase(std::remove_if(temp.begin(), temp.end(), isspace), temp.end());

            storeMap[model] = temp;
        }
        catch (...)
        {
            gLog << "Error parsing vehicles.ide, " << line << std::endl;
        }
    }

    file.close();
}