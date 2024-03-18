-- bruter module
-- bruter module
-- bruter module
local br = 
{
    -- version
    version = "0.1.1c",
    
    -- source and output
    source = "",
    outputpath = "",
    
    -- compiled objects exports
    exports = {},

    -- other
    debug = false,
    comment = function()end,
    utils = require("lib.luatils"),
    list = require("lib.list"),
    String = require("lib.string"),

    -- preprocessors and lineprocessors
    -- preprocessors and lineprocessors
    -- preprocessors and lineprocessors
    preprocessors = 
    {
        sugar = function(source)
            local nstr = br.utils.string.replace(source, "\n"," ")
            nstr = br.utils.string.replace(nstr, "\\n", "\n")
            while br.utils.string.includes(nstr, "  ") do
                nstr = br.utils.string.replace(nstr, "  ", " ")
            end
            nstr = br.utils.string.replace(nstr, " : ", ":")
            nstr = br.utils.string.replace(nstr, " :", ":")
            nstr = br.utils.string.replace(nstr, ": ", ":")
            nstr = br.utils.string.replace(nstr, " ;", ";")
            nstr = br.utils.string.replace(nstr, "; ", ";")
            nstr = br.utils.string.replace(nstr, " ; ", ";")
            nstr = br.utils.string.replace(nstr, "}", " }")
            nstr = br.utils.string.replace(nstr, "{%s+}", "{}")
            return nstr
        end
    },
    lineprocessors = 
    {
        setter = function(source)
            if(br.utils.string.includes(source, ":")) then
                local temp = br.utils.string.split3(source, " ");
                if br.utils.string.includes(temp[1], ":") then
                    
                    local rest = "";
                    for i = 2, #temp do
                        rest = rest .. " " .. temp[i];
                    end
                    local splited = br.utils.string.split(temp[1], ":");
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

-- list types
-- list types
-- list types
ListInt8 = br.list(int8);
ListInt16 = br.list(int16);
ListInt32 = br.list(int32);
ListInt64 = br.list(int64);
ListUInt8 = br.list(uint8);
ListUInt16 = br.list(uint16);
ListUInt32 = br.list(uint32);
ListUInt64 = br.list(uint64);
ListDouble = br.list(double);
ListFloat = br.list(float);
ListString = br.list(br.String);
ListInt = ListInt32;
ListListInt8 = br.list(ListInt8);
ListListInt16 = br.list(ListInt16);
ListListInt32 = br.list(ListInt32);
ListListInt64 = br.list(ListInt64);
ListListUInt8 = br.list(ListUInt8);
ListListUInt16 = br.list(ListUInt16);
ListListUInt32 = br.list(ListUInt32);
ListListUInt64 = br.list(ListUInt64);
ListListDouble = br.list(ListDouble);
ListListFloat = br.list(ListFloat);
ListListInt = ListListInt32;
ListListString = br.list(ListString);

-- parse the arguments
-- parse the arguments
-- parse the arguments
br.parseargs = function(args)
    local newargs = br.utils.array.clone(args);
    for i = 1, #args do
        if string.byte(args[i],1) == 36 then
            local name = br.utils.string.replace(args[i], "%$", '');
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
    local splited = br.utils.string.split3(src, ";");
    local func = "";
    for i = 1, #splited - 1 do
        br.debugprint("\n[DEBUG INFO]: parsing line " .. i);
        br.debugprint("pre: ", splited[i]);

        splited[i] = br.lineprocess(splited[i]);
        
        br.debugprint("pos: ", splited[i]);
        
        local splited_args = br.utils.string.split2(splited[i], " ");
        local func = splited_args[1];
        local args = br.parseargs(br.utils.array.slice(splited_args, 2, #splited_args));
        local _function = br.recursiveget(func);
        if _function then
            br.debugprint(func, br.utils.stringify(args))
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
        local clearbuffer = br.utils.string.replace(buffer, "%s+", "");
        local ok = true;

        if br.utils.string.includes(buffer, "`") then
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
    if br.utils.string.includes(argname, ".") then
        if (type(value) == "table") then
            if (value.module) then
                br.temp = value;
                terralib.loadstring("br." .. argname .. " = br.temp")();
                br.temp = nil;
            else
                terralib.loadstring("br." .. argname .. " = " .. br.utils.stringify(value))()
            end
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
    if br.utils.string.includes(argname, ".") then
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
    local c = br.utils.file.load.text(path);
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
    
    if br.utils.string.includes(path, ".lua") or br.utils.string.includes(path, ".t") then
        temp = br.require(path);
    elseif br.utils.string.includes(path, ".c") or br.utils.string.includes(path, ".h") then
        temp = br.include(path);
    else
        temp = br.require(path);
    end
    temp.module = true;
    
    return temp;
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
    print("br contains:")
    print(result:sub(1, #result - 2) .. ";");
    return result;
end

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