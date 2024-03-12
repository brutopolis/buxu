local utils = require("luatils.init");
local list = require("lib.list");
local String = require("lib.string");

-- list types
-- list types
-- list types
ListInt8 = list(int8);
ListInt16 = list(int16);
ListInt32 = list(int32);
ListInt64 = list(int64);
ListUInt8 = list(uint8);
ListUInt16 = list(uint16);
ListUInt32 = list(uint32);
ListUInt64 = list(uint64);
ListDouble = list(double);
ListFloat = list(float);
ListString = list(String);
ListInt = ListInt32;
ListListInt8 = list(ListInt8);
ListListInt16 = list(ListInt16);
ListListInt32 = list(ListInt32);
ListListInt64 = list(ListInt64);
ListListUInt8 = list(ListUInt8);
ListListUInt16 = list(ListUInt16);
ListListUInt32 = list(ListUInt32);
ListListUInt64 = list(ListUInt64);
ListListDouble = list(ListDouble);
ListListFloat = list(ListFloat);
ListListInt = ListListInt32;
ListListString = list(ListString);

-- bruter module
-- bruter module
-- bruter module
local br = 
{
    -- version
    version = "0.1.1a",
    
    -- source and output
    source = "",
    outputpath = "",
    
    -- compiled objects exports
    exports = {},

    -- other
    debug = false,
    comment = function()end,
    utils = utils,
    list = list,
    String = String,

    -- preprocessors and lineprocessors
    -- preprocessors and lineprocessors
    -- preprocessors and lineprocessors
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
            newargs[i] = br.recursiveget(name);
        elseif (string.byte(args[i],1) > 47 and string.byte(args[i],1) < 58) or string.byte(args[i],1) == 45 then
            newargs[i] = tonumber(args[i]);
        end
    end
    return newargs;
end

-- preprocess the source
-- preprocess the source
-- preprocess the source
br.preprocess = function(_src)
    local result = _src .. '';
    for k, v in pairs(br.preprocessors) do
        result = v(result);
    end
    return result;
end

br.lineprocess = function(_src)
    local result = _src .. '';
    for k, v in pairs(br.lineprocessors) do
        result = v(result);
    end
    return result;
end

br.debugprint = function(...)
    if br.debug then
        print(...);
    end
end

-- parse the source file
-- parse the source file
-- parse the source file
br.parse = function(src)
    src = br.preprocess(src);
    local splited = utils.string.split3(src, ";");
    local func = "";
    for i = 1, #splited - 1 do
        br.debugprint("\n[DEBUG INFO]: parsing line " .. i);
        br.debugprint("pre: ", splited[i]);

        splited[i] = br.lineprocess(splited[i]);
        
        br.debugprint("pos: ", splited[i]);
        
        local splited_args = utils.string.split2(splited[i], " ");
        local func = splited_args[1];
        local args = parseArgs(utils.array.slice(splited_args, 2, #splited_args));
        local _function = br.recursiveget(func);
        if _function then
            br.debugprint(func, utils.stringify(args))
            br.debugprint("[DEBUG DONE]: line " .. i .. " ok\n");
            _function(unpack(args or {}));
        elseif br.exit then -- if on repl
            br.debugprint("Error parsing the following code:");
            br.debugprint(src);
            br.debugprint("[DEBUG FAIL]: function " .. func .. " not found\n");
        else
            br.debugprint("Error parsing the following code:");
            br.debugprint(src);
            br.debugprint("[DEBUG FAIL]: function " .. func .. " not found");
            error("function " .. func .. " not found");
        end
    end
end

br.repl = function()
    --exit function
    br._replExit = false;
    br.exit = function()
        br._replExit = true;
    end
    -- version
    print("bruter v" .. br.version);
    
    local line = "";
    local count = 0;
    while not br._replExit do
        io.write("br> ");
        local buffer = io.read();
        local clearbuffer = utils.string.replace(buffer, "%s+", "");
        local ok = true;

        if utils.string.includes(buffer, "`") then
            for i = 1, #buffer do
                if buffer:sub(i, i) == "`" then
                    count = count + 1;
                end
            end

            if count % 2 ~= 0 then
                ok = false;
            else
                count = 0;
            end
        end

        if string.byte(clearbuffer,#clearbuffer) == 59 and ok then
            br.parse(line .. buffer);
            line = "";
        elseif buffer == "exit;" then
            break;
        else
            line = line .. buffer;
        end
    end
end

-- module functions
-- module functions
-- module functions

-- export
br.export = function(name, as)
    if as then
        br.exports[as] = br[name];
    else
        br.exports[name] = br[name];
    end
end

-- setter
-- setter
-- setter
br.recursiveset = function(argname, value)
    if utils.string.includes(argname, ".") then
        if (type(value) == "table") then
            terralib.loadstring("br." .. argname .. " = " .. utils.stringify(value))()
        else
            terralib.loadstring("br." .. argname .. " = " .. value)();
        end
    else
        br[argname] = value;
    end
end

-- getter
-- getter
-- getter
br.recursiveget = function(argname)
    if utils.string.includes(argname, ".") then
        print("return br." .. argname)
        local result = terralib.loadstring("return br." .. argname)();
        return result;
    else
        return br[argname];
    end
end

-- set
br.set = function(name, value)
    br[name] = value;
end

br.setf = function(varname, funcname, ...)
    local args = {...};
    local result;
    br.recursiveset(varname, br.recursiveget(funcname)(unpack(args or {})));
end

br.includec = function(path)
    return terralib.includec(path);
end
br.includecstring = function(txt)
    return terralib.includecstring(txt);
end
br.require = function(path)
    return require(path);
end

-- dobr
br.dobr = function(path)
    local c = utils.file.load.text(path);
    br.parse(c);
end

-- dobrstring
br.dobrstring = function(str)
    str = cleanSource(str);
    br.parse(str);
end

-- loadstring
br.loadstring = function(str)
    br.debugprint("[DEBUG INFO]: loading string: " .. str)
    return ((terralib.loadstring(str))());
end

-- loadfile
br.loadfile = function(path)
    return(terralib.loadfile(path)());
end

-- emptyobject
br.emptyobject = function()
    return {};
end

-- module
br.module = function(path)
    local temp;
    if utils.string.includes(path, ".lua") or utils.string.includes(path, ".t")then
        temp = br.require(path);
    elseif utils.string.includes(path, ".c") or utils.string.includes(path, ".h") then
        temp = br.include(path);
    else
        temp = br.require(path);
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
            br[k] = v;
        end
    end
end

--string functions
--string functions
--string functions

br.string = function(...)
    local args = {...};
    return table.concat(args, " ");
end

-- math functions
-- math functions
-- math functions
br["="] = function(value)
    return value;
end 
br["+"] = function(a, b)
    return a + b;
end 
br["-"] = function(a, b)
    return a - b;
end 
br["*"] = function(a, b)
    return a * b;
end 
br["/"] = function(a, b)
    return a / b;
end 
br["%"] = function(a, b)
    return a % b;
end 
br["^"] = function(a, b)
    return a ^ b;
end 
br["=="] = function(a, b)
    return a == b;
end 
br["~="] = function(a, b)
    return a ~= b;
end 
br[">"] = function(a, b)
    return a > b;
end 
br["<"] = function(a, b)
    return a < b;
end 
br[">="] = function(a, b)
    return a >= b;
end 
br["<="] = function(a, b)
    return a <= b;
end

-- data list functions
-- data list functions
-- data list functions

br.list = function()
    for k,v in pairs(br) do 
        print(k, v);
    end 
end

br.listnames = function()
    local result = "";
    for k,v in pairs(br) do 
        result = result .. k .. ", ";
    end
    print("br contains: \n")
    print(result:sub(1, #result - 2) .. ";");
    return result;
end

-- math aliases
-- math aliases
-- math aliases
br.ret = br["="];
br.add = br["+"];
br.sub = br["-"];
br.mul = br["*"];
br.div = br["/"];
br.mod = br["%"];
br.pow = br["^"];
br.equals = br["=="];
br.notequals = br["~="];
br.bigger = br[">"];
br.smaller = br["<"];
br.biggerorequals = br[">="];
br.smallerorequals = br["<="];

-- module aliases
-- module aliases
-- module aliases
br.dobrs = br.dobrstring;
br["?"] = br.loadstring;
br["?file"] = br.loadfile;
br["{}"] = br.emptyobject;

-- other aliases
-- other aliases
-- other aliases
br["#"] = br.comment;
br.help = br.listnames;

return br;