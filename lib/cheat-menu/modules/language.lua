local module = {}

local tlanguage = 
{
    text = {},
    selected = imgui.new.int(0),
    current  = fconfig.get('tlanguage.current',"English"),
    names    = {},
    list     = {},
    path  =  tcheatmenu.dir .. "languages//",
}

module.tlanguage = tlanguage

function module.LoadCurrentLanguage()
    local path =  string.format( "%s%s.json",tlanguage.path,tlanguage.current)

    local file = io.open(path,'r')
    if file then
        local data = file:read("*all")
        io.close(file)
        if data then
            tlanguage.text = decodeJson(data)
        end
    end
end

function module.LoadLanguages()
    local mask = tlanguage.path .. "*.json"
    local handle,file = findFirstFile(mask)
    if handle and file then
        while handle and file do 
            table.insert( tlanguage.names,string.sub( file, 1,-6))
            file = findNextFile(handle)
        end
        tlanguage.list = imgui.new['const char*'][#tlanguage.names](tlanguage.names)
        findClose(handle)
    end
    for i=1,#tlanguage.names,1 do
        if tlanguage.names[i] == tlanguage.current then
            tlanguage.selected[0] = i-1
            break
        end
    end

    if not pcall(module.LoadCurrentLanguage) then
        print("Unable to load language :" .. tlanguage.current)
    end
end

function module.GetText(s)
    if tlanguage.text == nil then return "No Text Found" end

    local t = tlanguage.text
    
    for key in s:gmatch('[^.]+') do
      if t[ key ] == nil then return "No Text Found" end
      t = t[ key ]
    end

    if t == nil then
        return "No Text Found"
    else
        return t
    end
end

return module