local module = {}

local log_name = script.this.name .. ".log"
local file_path = string.format("%s//%s", getWorkingDirectory(), log_name)

function module.Write(text,level)
    level = level or "DEBUG"

    if WRITE_INFO_TO_LOG then
        local file = io.open(file_path,'a')
        file:write(string.format("[%s] %s\n",level,text))
        file:close()
    end
end

function module.Close()
    module.Write("Log closed","INFO")
end

function module.Start()
    os.remove(file_path)
    module.Write("Log started","INFO")
    module.Write(string.format("Script version %s (%d)",script.this.version,script.this.version_num),"INFO")
    module.Write("Please provide both 'moonloader.log' & '" .. log_name .. "' for debugging","INFO")
end

function module.ProcessingComponent(comp_name)
    module.Write(string.format("Processing component %s",comp_name))
end

return module