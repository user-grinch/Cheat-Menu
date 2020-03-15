script_name("gsx-data")
script_authors("Fabio","Um Geek")
script_url("https://forum.mixmods.com.br/f5-scripts-codigos/t214-gsx-garage-save-extender")
script_version(2)

--[[

	=========================================================================
	-- Verifique se o 'gsx-data' esta disponivel para uso.

	if (script.find('gsx-data')) then 
		print('OK!')
	end 
	
	=========================================================================
	-- carregue o 'gsx-data'

	local gsx = import('gsx-data')
	
	=========================================================================
	--functions:

	return nil = gsx.set(car,string,value)
	return value = gsx.get(car,string)
	return table = gsx.data(car)
	return bool = gsx.loaded()
	
]]

local ffi = require "ffi"
local this = thisScript()
local memory = require "memory"
local s_data = "INFO_DATA"
local t_data = {}
local b_game_started = false
local wait_time = 5

ffi.cdef[[
typedef uint32_t GSXCVehicle;
typedef struct { float x, y, z; } CVector;
typedef struct __attribute__((packed, aligned(1))) {
    CVector pos;
    uint32_t handling_flags;
    uint8_t flags;
    uint8_t field_11;
    uint16_t model;
    uint16_t carmods[15];
    uint8_t colour[4];
    uint8_t radio_station;
    uint8_t extra1;
    uint8_t extra2;
    uint8_t bomb_type;
    uint8_t paintjob;
    uint8_t nitro_count;
    uint8_t angleX;
    uint8_t angleY;
    uint8_t angleZ;
    uint8_t field_3F;
} CStoredCar;
typedef struct __attribute__((packed, aligned(1))) { GSXCVehicle veh; int32_t status; size_t when; } journalNews;
typedef struct __attribute__((packed, aligned(1))) { GSXCVehicle veh; int32_t status; } apiCarNotify;
typedef struct __attribute__((packed, aligned(1))) { GSXCVehicle veh; int32_t status; CStoredCar *gameStoredData; } externalCallbackStructure;
typedef void(__cdecl externalCbFun_t)(const externalCallbackStructure*);
int __cdecl addNotifyCallback(externalCbFun_t fun);
int __cdecl addNotifyTempCallback(externalCbFun_t fun);
void __cdecl removeNotifyCallback(int cbRef);
void __cdecl removeNotifyTempCallback(int cbRef);
int __cdecl getNewCarGrgForeach(size_t *i, apiCarNotify *out);
void __cdecl setDataToSaveLaterVehPtr(GSXCVehicle veh, const char *name, int size, void *ptr, bool forceCopyNow);
void __cdecl pushDirectlyToSavedData(GSXCVehicle veh, const char *name, int size, void *ptr);
int __cdecl dataToSaveLaterExists(GSXCVehicle veh, const char *name);
void __cdecl removeToLoadDataVehPtr(GSXCVehicle veh, const char *name);
void __cdecl removeToSaveLaterVehPtr(GSXCVehicle veh, const char *name);
int __cdecl dataToLoadExists(GSXCVehicle veh, const char *name);
void*  __cdecl getLoadDataByVehPtr(GSXCVehicle veh, const char *name);
const char*  __cdecl GSX_getCompileTime();
const char*  __cdecl GSX_getVersionString();
float __cdecl GSX_getVersionNum();
int __cdecl getDataToLoadSize(GSXCVehicle veh, const char *name);
int __cdecl getDataToSaveSize(GSXCVehicle veh, const char *name);
]]

local gsx = ffi.load("gsx.asi")
local dataToLoadExists = ffi.cast("int (*)(int,const char*)",gsx.dataToLoadExists);
local getDataToLoadSize = ffi.cast("int (*)(int,const char*)",gsx.getDataToLoadSize);
local getLoadDataByVehPtr = ffi.cast("int (*)(int,const char*)",gsx.getLoadDataByVehPtr);
local pushDirectlyToSavedData = ffi.cast("int (*)(int,const char*,int,int)",gsx.pushDirectlyToSavedData);
local dataToSaveLaterExists = ffi.cast("int (*)(int,const char*,int,int)",gsx.dataToSaveLaterExists);

local garageClosed = true
local pNotifyCallbac = gsx.addNotifyCallback(function(c)
	garageClosed = (c.status==1)
	
    if garageClosed then
        local zdata = encodeJson(t_data[c.veh])
        if zdata ~= nil then
            local pdata = memory.strptr(zdata);
            pushDirectlyToSavedData(c.veh,s_data,#zdata,pdata);
        end
    else
        if dataToLoadExists(c.veh,s_data) == 1 then
            b_game_started = false
            local pdata = getLoadDataByVehPtr(c.veh,s_data)
            local size = getDataToLoadSize(c.veh,s_data) 
            t_data[c.veh] = decodeJson(memory.tostring(pdata,size,false))
        end
    end
end)

EXPORTS = {
    
    -- retorna tudo o que tem sobre o veiculo salvo pelo gsx-data.lua
    data = function(car)
        if doesVehicleExist(car) then
            return t_data[getCarPointer(car)]
        end
        return nil
    end,

    -- grava um valor (string, bool ou number) na linha especificada com o nome 'str'
    set = function(car,str,val)
        if doesVehicleExist(car) then 
            local veh = getCarPointer(car)
            if t_data[veh] == nil then 
                t_data[veh] = {}
            end
            t_data[veh].veh_h = car
            t_data[veh][str] = val
        end
    end, 

    -- retorna o valor gravado na linha 'str'
    get = function(car,str)
        if doesVehicleExist(car) then 
            local veh = getCarPointer(car)
            if t_data[veh] then
                return t_data[veh][str]
            end
        end
        return nil
    end,  
    
    -- remove tudo salvo pelo gsx-data.lua no veiculo
    clear = function(car)
        if doesVehicleExist(car) then
            t_data[getCarPointer(car)] = nil
        end
    end, 

    -- faz uma nova cunsulta limpando dados anteriores
    reload = function()
        t_data = {}
    end,

    -- retorna false se o gsx-data.lua não estiver feito a primeira consulta.
    loaded = function()
        return b_game_started
    end,
	
	closed = function()
		return garageClosed
	end,
	
	version = this.version,
}

function main()
    local t_cars = {}
    while true do 
		
        for i, car in ipairs(getAllVehicles()) do
            local veh = getCarPointer(car)
            if t_data[veh] == nil or not b_game_started then 
                if dataToLoadExists(veh,s_data) == 1 then
                    local pdata = getLoadDataByVehPtr(veh,s_data)
                    local size = getDataToLoadSize(veh,s_data) 
                    t_data[veh] = decodeJson(memory.tostring(pdata,size,false))
                    t_cars[car] = veh
                end
            end
        end

        for car, veh in pairs(t_cars) do
            if not doesVehicleExist(car) then 
                t_data[veh] = nil
                t_cars[car] = nil
            end
        end
        
        wait( wait_time )
		
        b_game_started = true
    end
end
 
function onScriptTerminate(script, quitGame)
    if gsx and script == thisScript() then 
        gsx.removeNotifyCallback(pNotifyCallbac) 
    end
end