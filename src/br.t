local utils = require("luatils.init");

local br = 
{
    source = "",
    outputPath = "",
    variables = 
    {
        comment = function()end,
        utils = utils,
    },
    exports = {},
    currentcmd = "",
}

-- module functions
-- module functions
-- module functions

-- export
br.variables.export = function(name, as)
    if as then
        br.exports[as] = br.variables[name];
    else
        br.exports[name] = br.variables[name];
    end
end

-- set
br.variables.set = function(name, value)
    br.variables[name] = value;
end
br.variables.include = function(path)
    return terralib.includec(path);
end
br.variables.includestring = function(txt)
    return terralib.includecstring(txt);
end
br.variables.require = function(path)
    return require(path);
end

-- dobr
br.variables.dobr = function(path)
    local c = utils.file.load.text(path);
    c = cleanSource(c);
    parseSourceFile(c);
end

-- dobrstring
br.variables.dobrstring = function(str)
    str = cleanSource(str);
    parseSourceFile(str);
end

-- loadstring
br.variables.loadstring = function(...)
    local args = {...};
    local result = "";
    for i = 1, #args do
        result = result .. " " .. args[i];
    end
    print(result)
    result = "return(" .. result .. ")"
    return ((terralib.loadstring(result))());
end

-- loadfile
br.variables.loadfile = function(path)
    return(terralib.loadfile(path)());
end

-- emptyobject
br.variables.emptyobject = function()
    return {};
end

-- module
br.variables.module = function(path)
    local temp;
    if utils.string.includes(path, ".br") then
        br.variables.dobr(path);
        temp = br.variables.temp.brmodule or {};
    elseif utils.string.includes(path, ".lua") or utils.string.includes(path, ".t")then
        temp = br.variables.require(path);
    elseif utils.string.includes(path, ".c") then
        temp = br.variables.include(path);
    else
        temp = br.variables.require(path);
    end
    if temp == nil then
        print("Error: module " .. path .. " not found");
        os.exit(1);
    end
    for k,v in pairs(temp) do
        if k == "exports" then
            for k,v in pairs(v) do
                br.exports[k] = v;
            end
        else
            br.variables[k] = v;
        end
    end
end

-- math functions
-- math functions
-- math functions
br.variables["="] = function(value)
    return value;
end 
br.variables["+"] = function(a, b)
    return a + b;
end 
br.variables["-"] = function(a, b)
    return a - b;
end 
br.variables["*"] = function(a, b)
    return a * b;
end 
br.variables["/"] = function(a, b)
    return a / b;
end 
br.variables["%"] = function(a, b)
    return a % b;
end 
br.variables["^"] = function(a, b)
    return a ^ b;
end 
br.variables["=="] = function(a, b)
    return a == b;
end 
br.variables["~="] = function(a, b)
    return a ~= b;
end 
br.variables[">"] = function(a, b)
    return a > b;
end 
br.variables["<"] = function(a, b)
    return a < b;
end 
br.variables[">="] = function(a, b)
    return a >= b;
end 
br.variables["<="] = function(a, b)
    return a <= b;
end

-- math aliases
-- math aliases
-- math aliases
br.variables.ret = br.variables["="];
br.variables.add = br.variables["+"];
br.variables.sub = br.variables["-"];
br.variables.mul = br.variables["*"];
br.variables.div = br.variables["/"];
br.variables.mod = br.variables["%"];
br.variables.pow = br.variables["^"];
br.variables.equals = br.variables["=="];
br.variables.notequals = br.variables["~="];
br.variables.bigger = br.variables[">"];
br.variables.smaller = br.variables["<"];
br.variables.biggerorequals = br.variables[">="];
br.variables.smallerorequals = br.variables["<="];

-- module aliases
-- module aliases
-- module aliases
br.variables.dobrs = br.variables.dobrstring;
br.variables["?"] = br.variables.loadstring;
br.variables["?file"] = br.variables.loadfile;
br.variables["{}"] = br.variables.emptyobject;

-- other aliases
-- other aliases
-- other aliases
br.variables["#"] = br.variables.comment;

return br;