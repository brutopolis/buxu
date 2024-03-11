local utils = require("luatils.init");

local br = 
{
    version = "0.0.8b",
    source = "",
    outputPath = "",
    data = 
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
            newargs[i] = br.data.recursiveget(name);
        elseif (string.byte(args[i],1) > 47 and string.byte(args[i],1) < 58) or string.byte(args[i],1) == 45 then
            newargs[i] = tonumber(args[i]);
        end
    end
    return newargs;
end

-- preprocess the source
-- preprocess the source
-- preprocess the source
br.data.preprocess = function(_src)
    local result = _src .. '';
    for k, v in pairs(br.preprocessors) do
        result = v(result);
    end
    return result;
end

br.data.lineprocess = function(_src)
    local result = _src .. '';
    for k, v in pairs(br.lineprocessors) do
        result = v(result);
    end
    return result;
end

-- parse the source file
-- parse the source file
-- parse the source file
br.data.parse = function(src)
    src = br.data.preprocess(src);
    local splited = utils.string.split3(src, ";");
    local func = "";
    for i = 1, #splited - 1 do
        print("splited: ", splited[i]);
        splited[i] = br.data.lineprocess(splited[i]);
        print("processed: ", splited[i]);
        local splited_args = utils.string.split2(splited[i], " ");
        local func = splited_args[1];
        local args = parseArgs(utils.array.slice(splited_args, 2, #splited_args));
        local _function = br.data.recursiveget(func);
        if _function then
            print(func, utils.stringify(args))
            _function(unpack(args or {}));
        else
            print(src)
            error("function " .. func .. " not found")
        end
    end
end

br.data.repl = function()
    print("bruter v" .. br.version);
    while true do
        io.write("br> ");
        local line = io.read();
        if line == "exit" then
            break;
        end
        br.data.parse(line);
    end
end

-- module functions
-- module functions
-- module functions

-- export
br.data.export = function(name, as)
    if as then
        br.exports[as] = br.data[name];
    else
        br.exports[name] = br.data[name];
    end
end

-- setter
-- setter
-- setter
br.data.recursiveset = function(argname, value)
    if utils.string.includes(argname, ".") then
        if (type(value) == "table") then
            terralib.loadstring("br.data." .. argname .. " = " .. utils.stringify(value))()
        else
            terralib.loadstring("br.data." .. argname .. " = " .. value)();
        end
    else
        br.data[argname] = value;
    end
end

-- getter
-- getter
-- getter
br.data.recursiveget = function(argname)
    if utils.string.includes(argname, ".") then
        print("return br.data." .. argname)
        local result = terralib.loadstring("return br.data." .. argname)();
        return result;
    else
        return br.data[argname];
    end
end

-- set
br.data.set = function(name, value)
    br.data[name] = value;
end

br.data.setf = function(varname, funcname, ...)
    local args = {...};
    local result;
    print(funcname)
    br.data.recursiveset(varname, br.data.recursiveget(funcname)(unpack(args or {})));
end

br.data.includec = function(path)
    return terralib.includec(path);
end
br.data.includecstring = function(txt)
    return terralib.includecstring(txt);
end
br.data.require = function(path)
    return require(path);
end

-- dobr
br.data.dobr = function(path)
    local c = utils.file.load.text(path);
    br.data.parse(c);
end

-- dobrstring
br.data.dobrstring = function(str)
    str = cleanSource(str);
    br.data.parse(str);
end

-- loadstring
br.data.loadstring = function(str)
    print("str: ", str)
    return ((terralib.loadstring(str))());
end

-- loadfile
br.data.loadfile = function(path)
    return(terralib.loadfile(path)());
end

-- emptyobject
br.data.emptyobject = function()
    return {};
end

-- module
br.data.module = function(path)
    local temp;
    if utils.string.includes(path, ".lua") or utils.string.includes(path, ".t")then
        temp = br.data.require(path);
    elseif utils.string.includes(path, ".c") or utils.string.includes(path, ".h") then
        temp = br.data.include(path);
    else
        temp = br.data.require(path);
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
            br.data[k] = v;
        end
    end
end

--string functions
--string functions
--string functions

br.data.string = function(...)
    local args = {...};
    return table.concat(args, " ");
end

-- math functions
-- math functions
-- math functions
br.data["="] = function(value)
    return value;
end 
br.data["+"] = function(a, b)
    return a + b;
end 
br.data["-"] = function(a, b)
    return a - b;
end 
br.data["*"] = function(a, b)
    return a * b;
end 
br.data["/"] = function(a, b)
    return a / b;
end 
br.data["%"] = function(a, b)
    return a % b;
end 
br.data["^"] = function(a, b)
    return a ^ b;
end 
br.data["=="] = function(a, b)
    return a == b;
end 
br.data["~="] = function(a, b)
    return a ~= b;
end 
br.data[">"] = function(a, b)
    return a > b;
end 
br.data["<"] = function(a, b)
    return a < b;
end 
br.data[">="] = function(a, b)
    return a >= b;
end 
br.data["<="] = function(a, b)
    return a <= b;
end

-- math aliases
-- math aliases
-- math aliases
br.data.ret = br.data["="];
br.data.add = br.data["+"];
br.data.sub = br.data["-"];
br.data.mul = br.data["*"];
br.data.div = br.data["/"];
br.data.mod = br.data["%"];
br.data.pow = br.data["^"];
br.data.equals = br.data["=="];
br.data.notequals = br.data["~="];
br.data.bigger = br.data[">"];
br.data.smaller = br.data["<"];
br.data.biggerorequals = br.data[">="];
br.data.smallerorequals = br.data["<="];

-- module aliases
-- module aliases
-- module aliases
br.data.dobrs = br.data.dobrstring;
br.data["?"] = br.data.loadstring;
br.data["?file"] = br.data.loadfile;
br.data["{}"] = br.data.emptyobject;

-- other aliases
-- other aliases
-- other aliases
br.data["#"] = br.data.comment;

return br;