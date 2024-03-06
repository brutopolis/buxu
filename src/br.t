
local br = 
{
    source = "",
    outputPath = "",
    variables = {},
    exports = {},
    currentcmd = "",
}
-- br variables
br.variables = 
{
    module = function(path)
        local temp = require(path);
        for k,v in pairs(temp) do
            if k == "exports" then
                for k,v in pairs(v) do
                    br.exports[k] = v;
                end
            else
                br.variables[k] = v;
            end
        end
    end,
    set = function(name, value)
        br.variables[name] = value;
    end,
    export = function(name, as)
        if as then
            br.exports[as] = br.variables[name];
        else
            br.exports[name] = br.variables[name];
        end
    end,
    loadstring = function(...)
        local args = {...};
        local result = "";
        for i = 1, #args do
            result = result .. " " .. args[i];
        end
        print(result)
        result = "return(" .. result .. ")"
        return ((terralib.loadstring(result))());
    end,
    loadfile = function(path)
        return(terralib.loadfile(path)());
    end,
    emptyobject = function()
        return {};
    end,
    ["="] = function(value)
        return value;
    end,
    ["+"] = function(a, b)
        return a + b;
    end,
    ["-"] = function(a, b)
        return a - b;
    end,
    ["*"] = function(a, b)
        return a * b;
    end,
    ["/"] = function(a, b)
        return a / b;
    end,
    ["%"] = function(a, b)
        return a % b;
    end,
    ["^"] = function(a, b)
        return a ^ b;
    end,
    ["=="] = function(a, b)
        return a == b;
    end,
    ["~="] = function(a, b)
        return a ~= b;
    end,
    [">"] = function(a, b)
        return a > b;
    end,
    ["<"] = function(a, b)
        return a < b;
    end,
    [">="] = function(a, b)
        return a >= b;
    end,
    ["<="] = function(a, b)
        return a <= b;
    end,
    comment = function()
    end,
    include = function(path)
        return terralib.includec(path);
    end,
    includestring = function(txt)
        return terralib.includecstring(txt);
    end,
    require = function(path)
        return require(path);
    end,
    dobr = function(path)
        local c = utils.file.load.text(path);
        c = cleanSource(c);
        parseSourceFile(c);
    end,
    dobrstring = function(str)
        str = cleanSource(str);
        parseSourceFile(str);
    end,
    utils = utils,
}

br.variables.eval = br.variables.loadstring;
br.variables["?"] = br.variables.loadstring;
br.variables["?file"] = br.variables.loadfile;
br.variables["#"] = br.variables.comment;
br.variables["{}"] = br.variables.emptyobject;

return br;