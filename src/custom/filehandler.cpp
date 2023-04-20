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
    float engineAcceleration = pHandling->m_transmissionData.m_fEngineAcceleration * 25000.0f;
    engineAcceleration /= pHandling->m_transmissionData.m_nDriveType == 52 ? 4.0f : 2.0f;
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
    const char* path = GAME_PATH((char*)"/data/vehicles.ide");
    if (!std::filesystem::exists(path))
    {
        Log::Print<eLogLevel::Warn>("Vehicle.ide not found");
        return;
    }

    FILE *pFile= fopen(path, "r");
    if (pFile != NULL)
    {
        static const uint8_t bufSize = 128;
        char buf[bufSize];
        while (fgets(buf, bufSize, pFile) != NULL)
        {
            if (buf[0] == 'e' && buf[1] == 'n' && buf[2] == 'd')
            {
                break;
            }

            if (buf[0] < '0' || buf[0] > '9')
            {
                continue;
            }

            // replace comma and remove tabs
            uint8_t sz = bufSize;
            for(uint8_t i = 0; i < sz; ++i)
            {
                if (buf[i] == ',')
                {
                    buf[i] = ' ';
                    continue;
                }

                if (buf[i] == ' ' || buf[i] == '\t' )
                {
                    for(uint8_t j = i; j < sz; ++j)
                    {
                        buf[j] = buf[j+1];
                    }
                    --sz;
                }
            }

            int id;
            char model[32], txd[32], type[32], handling[32];
            if (sscanf(buf, "%d %s %s %s %s", &id, model, txd, type, handling) == 5)
            {
                storeMap[id] = std::string(handling);
            }
        }
        fclose(pFile);
    }
}
#endif

void FileHandler::FetchColorData(std::vector<std::vector<float>>& storeVec)
{
    const char* path = GAME_PATH((char*)"/data/carcols.dat");
    if (!std::filesystem::exists(path))
    {
        Log::Print<eLogLevel::Warn>("Carcols.dat not found");
        return;
    }

    FILE *pFile= fopen(path, "r");
    if (pFile != NULL)
    {
        static const uint8_t bufSize = 16;
        char buf[bufSize];
        while (fgets(buf, bufSize, pFile) != NULL)
        {
            if (buf[0] == '#' || buf[0] == '\n')
            {
                continue;
            }

            if (buf[0] == 'e' && buf[1] == 'n' && buf[2] == 'd')
            {
                break;
            }

            // replace comma and dots with spaces
            for(uint8_t i = 0; i < bufSize; ++i)
            {
                if (buf[i] == ',' || buf[i] == '.')
                {
                    buf[i] = ' ';
                }
            }

            int r = 0, g = 0, b = 0;
            if (sscanf(buf, "%d %d %d", &r, &g, &b) == 3)
            {
                storeVec.push_back({r / 255.0f, g / 255.0f, b / 255.0f});
            }
        }
        fclose(pFile);
    }
}
