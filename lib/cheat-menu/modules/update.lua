local module = {}

function module.CheckUpdates()
    require("socket")
    local https = require("ssl.https")
    local body, code, headers, status = https.request("https://raw.githubusercontent.com/inanahammad/Cheat-Menu/master/cheat-menu.lua")
    
    if not body then error(code) print(status) printHelpString(flanguage.GetText("update.FailedToUpdate")) return end
    tcheatMenu.update.version_number = tonumber(body:match("script_version_number%((%d+)%)"))
    if tcheatMenu.update.version_number ~= script.this.version_num then
        tcheatMenu.update.available = true
    end
end

function module.DownloadUpdates()
    require("socket")
    local https = require("ssl.https")
    local one, code, headers, status = https.request {
        url = "https://github.com/inanahammad/Cheat-Menu/archive/master.zip",
        protocol = "TLSv1.3",
        options = "all",
        verify = "none",
      }
    if not body then error(code) return end
    local f = assert(io.open('master.zip', 'wb')) -- open in "binary" mode
    f:write(body)
    f:close()
end

return module