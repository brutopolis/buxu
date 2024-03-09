local utils = require("luatils.init");

local br = 
{
    version = "0.0.8b",
    source = "",
    outputPath = "",
    variables = 
    {
        comment = function()end,
        utils = utils,
    },
    exports = {},
    currentcmd = "",
    preprocessors = 
    {
        sugar = function(source)
            local nstr = utils.string.replace(source, "\n"," ")
            nstr = utils.string.replace(nstr, "\\n", "\n")
            while utils.string.includes(nstr, "  ") do
                nstr = utils.string.replace(nstr, "  ", " ")
            end
            nstr = utils.string.replace(nstr, " : ", ":")
            nstr = utils.string.replace(nstr, " :", ":")
            nstr = utils.string.replace(nstr, ": ", ":")
            nstr = utils.string.replace(nstr, " ;", ";")
            nstr = utils.string.replace(nstr, "; ", ";")
            nstr = utils.string.replace(nstr, " ; ", ";")
            nstr = utils.string.replace(nstr, "}", " }")
            nstr = utils.string.replace(nstr, "{%s+}", "{}")
            return nstr
        end
    },
    lineprocessors = 
    {
        setter = function(source)
            if(utils.string.includes(source, ":")) then
                local temp = utils.string.split3(source, " ");
                if utils.string.includes(temp[1], ":") then
                    
                    local rest = "";
                    for i = 2, #temp do
                        rest = rest .. " " .. temp[i];
                    end
                    local splited = utils.string.split(temp[1], ":");
                    local name = splited[1];
                    local funcname = splited[2];
                    local result = "setf " .. name .. " " .. funcname .. rest;
                    return result;
                else
                    return source;
                end
            else
                return source;
            end
        end
    },
}

-- parse the arguments
-- parse the arguments
-- parse the arguments
local function parseArgs(args)
    local newargs = utils.array.clone(args);
    for i = 1, #args do
        if string.byte(args[i],1) == 36 then
            local name = utils.string.replace(args[i], "%$", '');
            newargs[i] = br.variables.recursiveget(name);
        elseif (string.byte(args[i],1) > 47 and string.byte(args[i],1) < 58) or string.byte(args[i],1) == 45 then
            newargs[i] = tonumber(args[i]);
        end
    end
    return newargs;
end

-- preprocess the source
-- preprocess the source
-- preprocess the source

br.variables.preprocess = function(_src)
    local result = _src .. '';
    for k, v in pairs(br.preprocessors) do
        result = v(result);
    end
    return result;
end

br.variables.lineprocess = function(_src)
    local result = _src .. '';
    for k, v in pairs(br.lineprocessors) do
        result = v(result);
    end
    return result;
end

-- parse the source file
-- parse the source file
-- parse the source file
br.variables.parse = function(src)
    src = br.variables.preprocess(src);
    local splited = utils.string.split3(src, ";");
    local func = "";
    for i = 1, #splited - 1 do
        print("splited: ", splited[i]);
        splited[i] = br.variables.lineprocess(splited[i]);
        print("processed: ", splited[i]);
        local splited_args = utils.string.split2(splited[i], " ");
        local func = splited_args[1];
        local args = parseArgs(utils.array.slice(splited_args, 2, #splited_args));
        local _function = br.variables.recursiveget(func);
        if _function then
            print(func, utils.stringify(args))
            _function(unpack(args or {}));
        else
            print(src)
            error("function " .. func .. " not found")
        end
    end
end

br.variables.repl = function()
    print("bruter v" .. br.version);
    while true do
        io.write("br> ");
        local line = io.read();
        if line == "exit" then
            break;
        end
        br.variables.parse(line);
    end
end

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

-- setter
-- setter
-- setter
br.variables.recursiveset = function(argname, value)
    if utils.string.includes(argname, ".") then
        if (type(value) == "table") then
            terralib.loadstring("br.variables." .. argname .. " = " .. utils.stringify(value))()
        else
            terralib.loadstring("br.variables." .. argname .. " = " .. value)();
        end
    else
        br.variables[argname] = value;
    end
end

-- getter
-- getter
-- getter
br.variables.recursiveget = function(argname)
    if utils.string.includes(argname, ".") then
        print("return br.variables." .. argname)
        local result = terralib.loadstring("return br.variables." .. argname)();
        return result;
    else
        return br.variables[argname];
    end
end

-- set
br.variables.set = function(name, value)
    br.variables[name] = value;
end

br.variables.setf = function(varname, funcname, ...)
    local args = {...};
    local result;
    print(funcname)
    br.variables.recursiveset(varname, br.variables.recursiveget(funcname)(unpack(args or {})));
end

br.variables.includec = function(path)
    return terralib.includec(path);
end
br.variables.includecstring = function(txt)
    return terralib.includecstring(txt);
end
br.variables.require = function(path)
    return require(path);
end

-- dobr
br.variables.dobr = function(path)
    local c = utils.file.load.text(path);
    br.variables.parse(c);
end

-- dobrstring
br.variables.dobrstring = function(str)
    str = cleanSource(str);
    br.variables.parse(str);
end

-- loadstring
br.variables.loadstring = function(str)
    print("str: ", str)
    return ((terralib.loadstring(str))());
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
    if utils.string.includes(path, ".lua") or utils.string.includes(path, ".t")then
        temp = br.variables.require(path);
    elseif utils.string.includes(path, ".c") or utils.string.includes(path, ".h") then
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

--string functions
--string functions
--string functions

br.variables.string = function(...)
    local args = {...};
    return table.concat(args, " ");
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