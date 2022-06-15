#include "pch.h"
#include "filehandler.h"

#ifdef GTASA
void FileHandler::GenerateHandlingFile(tHandlingData *pHandling, std::map<int, std::string>& storeMap)
{
    FILE* fp = fopen("handling.txt", "w");

    std::string handlingId = storeMap[FindPlayerPed()->m_pVehicle->m_nModelIndex];
    float mass = pHandling->m_fMass;
    float turnMass = pHandling->m_fTurnMass;
    float dragMult = pHandling->m_fDragMult;
    CVector centreOfMass = pHandling->m_vecCentreOfMass;
    int percentSubmerged = pHandling->m_nPercentSubmerged;
    float tractionMultiplier = pHandling->m_fTractionMultiplier;
    float tractionLoss = pHandling->m_fTractionLoss;
    float tractionBias = pHandling->m_fTractionBias;
    float engineAcceleration = pHandling->m_transmissionData.m_fEngineAcceleration * 12500;
    float engineInertia = pHandling->m_transmissionData.m_fEngineInertia;
    int driveType = pHandling->m_transmissionData.m_nDriveType;
    int engineType = pHandling->m_transmissionData.m_nEngineType;
    float brakeDeceleration = pHandling->m_fBrakeDeceleration * 2500;
    float brakeBias = pHandling->m_fBrakeBias;
    int abs = pHandling->m_bABS;
    float steeringLock = pHandling->m_fSteeringLock;
    float susForceLevel = pHandling->m_fSuspensionForceLevel;
    float susDampingLevel = pHandling->m_fSuspensionDampingLevel;
    float susHighSpdComDamp = pHandling->m_fSuspensionHighSpdComDamp;
    float susUpperLimit = pHandling->m_fSuspensionUpperLimit;
    float susLowerLimit = pHandling->m_fSuspensionLowerLimit;
    float susBias = pHandling->m_fSuspensionBiasBetweenFrontAndRear;
    float antiDiveMul = pHandling->m_fSuspensionAntiDiveMultiplier;
    float cdm = pHandling->m_fCollisionDamageMultiplier / (2000.0f * (1.0 / mass));
    int monetaryValue = pHandling->m_nMonetaryValue;
    int maxVelocity = pHandling->m_transmissionData.m_fMaxGearVelocity / *(float*)0xC2B9BC;
    int modelFlags = pHandling->m_nModelFlags;
    int handlingFlags = pHandling->m_nHandlingFlags;
    int frontLight = pHandling->m_nFrontLights;
    int rearLight = pHandling->m_nRearLights;
    int animGrp = pHandling->m_nAnimGroup;
    int numGears = pHandling->m_transmissionData.m_nNumberOfGears;
    float seatOffsetDistance = pHandling->m_fSeatOffsetDistance;

    // TODO: make this more readable
    fprintf(
        fp,
        "\n%s\t%.5g\t%.5g\t%.5g\t%.5g\t%.5g\t%.5g\t%d\t%.5g\t%.5g\t%.5g\t%d\t%d\t%.5g\t%.5g\t%c\t%c\t%.5g\t%.5g\t%d\t%.5g\t%.5g\t%.5g\t%.5g\t%.5g\t%.5g\t%.5g\t%.5g\t%.5g\t%.5g\t%d\t%x\t%x\t%d\t%d\t%d",
        handlingId.c_str(), mass, turnMass, dragMult, centreOfMass.x, centreOfMass.y, centreOfMass.z, percentSubmerged,
        tractionMultiplier, tractionLoss, tractionBias, numGears,
        maxVelocity, engineAcceleration, engineInertia, driveType, engineType, brakeDeceleration, brakeBias, abs,
        steeringLock, susForceLevel, susDampingLevel,
        susHighSpdComDamp, susUpperLimit, susLowerLimit, susBias,
        antiDiveMul, seatOffsetDistance,
        cdm, monetaryValue, modelFlags, handlingFlags, frontLight, rearLight,
        animGrp);

    fclose(fp);
}

void FileHandler::FetchHandlingID(std::map<int, std::string>& storeMap)
{
    std::string m_FilePath = GAME_PATH((char*)"/data/vehicles.ide");
    if (!std::filesystem::exists(m_FilePath))
    {
        Log::Print<eLogLevel::Warn>("Vehicle.ide not found");
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
            Log::Print<eLogLevel::Warn>("Failed parsing vehicles.ide, {}", line);
        }
    }

    file.close();
}
#endif

void FileHandler::FetchColorData(std::vector<std::vector<float>>& storeVec)
{
    std::string m_FilePath = GAME_PATH((char*)"/data/carcols.dat");
    if (!std::filesystem::exists(m_FilePath))
    {
        Log::Print<eLogLevel::Warn>("Carcols.dat not found");
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
                Log::Print<eLogLevel::Warn>("Failed parsing carcols.dat, {}", line);
            }
        }
    }

    file.close();
}
