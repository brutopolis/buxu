-- bruter module
-- bruter module
-- bruter module
local _bruterPath = debug.getinfo(1).source;

local br = 
{
    -- version
    version = "0.2.2",
    
    -- current path
    bruterpath = string.sub(_bruterPath, 2, #_bruterPath-8),

    -- source and output
    source = "",
    outputpath = "",
    
    -- compiled objects exports
    exports = {},

    -- other
    debug = false,
    comment = function()end,
    utils = require("lib.luatils"),
    lastsetten = {},
    -- preprocessors
    -- preprocessors
    -- preprocessors
    preprocessors = 
    {
        sugar = function(source)
            local nstr = br.utils.string.replace3(source, "%s+"," ")
            
            nstr = br.utils.string.replace(nstr, "%( ", "%(")
            nstr = br.utils.string.replace(nstr, " %) ", "%)")
            nstr = br.utils.string.replace(nstr, " %)", "%)")
            
            nstr = br.utils.string.replace3(nstr, "\n", "")
            nstr = br.utils.string.replace3(nstr, "  ", "")
            
            nstr = br.utils.string.replace(nstr, "; ", ";")
            nstr = br.utils.string.replace(nstr, " ;", ";")
            nstr = br.utils.string.replace(nstr, " ; ", ";")
            
            -- if last char not ; add it
            if string.byte(nstr, #nstr) ~= 59 then
                nstr = nstr .. ";";
            end
            
            return nstr
        end
    }
}

-- parse the arguments
-- parse the arguments
-- parse the arguments
br.parseargs = function(args)
    local newargs = br.utils.array.clone(args);
    for i = 1, #args do
        print(args[i], string.byte(args[i],1), i)
        if string.byte(args[i],1) == 36 then -- variable
            local name = br.utils.string.replace(args[i], "%$", '');
            newargs[i] = br.recursiveget(name);
        elseif string.byte(args[i],1) == 40 then -- sentence
            -- remove first and last char by their indexes
            newargs[i] = br.parse(string.sub(args[i], 2, #args[i] - 1),true);
            print(i)
            print(newargs[i]);
        elseif (string.byte(args[i],1) > 47 and string.byte(args[i],1) < 58) or (string.byte(args[i],1) == 45 and (#args[i] > 1 and string.byte(args[i],2) > 47 and string.byte(args[i],2) < 58)) then -- number
            newargs[i] = tonumber(args[i]);
        elseif args[i] == "true" then
            newargs[i] = true;
        elseif args[i] == "false" then
            newargs[i] = false;
        elseif args[i] == "nil" then
            newargs[i] = nil;
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

br.debugprint = function(...)
    if br.debug then
        print(...);
    end
end

-- parse the source file
-- parse the source file
-- parse the source file
br.parse = function(src, isSentence)
    if isSentence then 
        src = "set lastsetten from " .. src;
    end
    src = br.preprocess(src);
    local splited = br.utils.string.split3(src, ";");
    local func = "";
    for i = 1, #splited - 1 do
        br.debugprint("\n" .. br.utils.console.colorstring("[DEBUG LINE]", "cyan") .. ": parsing line " .. i);
        
        local splited_args = br.utils.string.split2(splited[i], " ");
        local args = br.parseargs(splited_args);
        local func = args[1];
        table.remove(args, 1);
        local _function = br.recursiveget(func);
        if _function and isSentence then
            br.debugprint(func, br.utils.stringify(args))
            br.debugprint(br.utils.console.colorstring("[DEBUG DONE]", "green") .. ": line " .. i .. " ok\n");
            return(_function(unpack(args or {})));
        elseif _function then
            br.debugprint(func, br.utils.stringify(args))
            br.debugprint(br.utils.console.colorstring("[DEBUG DONE]", "green") .. ": line " .. i .. " ok\n");
            _function(unpack(args or {}));
        elseif br.exit then -- if on repl
            br.debugprint(br.utils.console.colorstring("Error", "red") .. " parsing the following code:");
            br.debugprint(src);
            br.debugprint(br.utils.console.colorstring("[DEBUG FAIL]", "red") .. ": function " .. func .. " not found\n");
        else
            br.debugprint(br.utils.console.colorstring("Error", "red") .. " parsing the following code:");
            br.debugprint(src);
            br.debugprint(br.utils.console.colorstring("[DEBUG FAIL]", "red") .. ": function " .. func .. " not found");
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
    -- version, only print if not in a breakpoint repl
    if not br._inBreakpoint then
        print("bruter v" .. br.version);
    end
    
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
        else
            line = line .. buffer;
        end
    end
end

br.breakpoint = function()
    if not br.debug then
        print(br.utils.console.colorstring("[WARNING]", "red") .. ": a breakpoint been ignored because debug mode is not enabled.");
        return;
    end
    br._inBreakpoint = true;
    print(br.utils.console.colorstring("[BREAKPOINT]", "red") .. ": entering breakpoint repl, type 'exit' to continue");
    br.repl();
    if br.debug then
        print(br.utils.console.colorstring("[BREAKPOINT DONE]", "green") .. ": continuing execution");
    else
        print("\n" .. br.utils.console.colorstring("[BREAKPOINT DONE]", "yellow") .. ": continuing execution, but debug mode is not enabled anymore, so breakpoints will be ignored.\n");
    end
    br._inBreakpoint = false;
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

br.using = function(name)
    -- new 
    if br.utils.file.exist(br.bruterpath .. "libr/" .. name .. "/" .. name .. ".br") then
        br.br.include(br.bruterpath .. "libr/" .. name .. "/" .. name .. ".br");
    elseif br.utils.file.exist(br.bruterpath .. "libr/" .. name .. "/" .. name .. ".lua") then
        terralib.loadfile(br.bruterpath .. "libr/" .. name .. "/" .. name .. ".lua")();
    elseif br.utils.file.exist(br.bruterpath .. "libr/" .. name .. "/" .. name .. ".t") then
        terralib.loadfile(br.bruterpath .. "libr/" .. name .. "/" .. name .. ".t")();
    elseif br.utils.file.exist(br.bruterpath .. "libr/" .. name .. ".br") then
        br.br.include(br.bruterpath .. "libr/" .. name .. ".br");
    elseif br.utils.file.exist(br.bruterpath .. "libr/" .. name .. ".lua") then
        terralib.loadfile(br.bruterpath .. "libr/" .. name .. ".lua")();
    elseif br.utils.file.exist(br.bruterpath .. "libr/" .. name .. ".t") then
        terralib.loadfile(br.bruterpath .. "libr/" .. name .. ".t")();
    else
        br.debugprint(br.utils.console.colorstring("[ERROR]", "red") .. ": library not found: " .. name);
    end
end



br.br = {};

br.br.include = function(path)
    local c = br.utils.file.load.text(path);
    br.parse(c);
end

br.br.includestring = function(str)
    br.parse(str);
end



br["terra"] = {};

-- loadstring (lua/terra)
br["terra"].loadstring = function(str)
    br.debugprint(br.utils.console.colorstring("[DEBUG INFO]", "magenta") .. ": loading string: " .. str)
    return ((terralib.loadstring(str))());
end

-- loadfile (lua/terra)
br["terra"].loadfile = function(path)
    return(terralib.loadfile(path)());
end

-- require lua/terra file
br["terra"].require = function(path)
    return require(path);
end



br.C = {};

-- include C code
br.C.include = function(path)
    return terralib.includec(path);
end

-- include C string
br.C.includestring = function(txt)
    return terralib.includecstring(txt);
end

-- terra aliases
-- terra aliases
-- terra aliases
br["?"] = br["terra"].loadstring;
br["?file"] = br["terra"].loadfile;
br["?require"] = br["terra"].require;

br["ç"] = br.C.includestring;
br["çfile"] = br.C.include;

br["!"] = br.br.includestring;
br["!file"] = br.br.include;

-- setter
-- setter
-- setter
br.recursiveset = function(argname, value)
    if br.utils.string.includes(argname, ".") then
        br.temp = value; 
        local result_txt = "br";
        local splited = br.utils.string.split2(argname, ".");

        if br.utils.array.includes(splited, "") then
            br[argname] = value;
            return;
        end

        for i = 1, #splited - 1 do
            result_txt = result_txt .. "[\"" .. splited[i] .. "\"]";
        end
        
        result_txt = result_txt .. "[\"" .. splited[#splited] .. "\"] = br.temp";
        terralib.loadstring(result_txt)();
        br.temp = nil;
    else
        br[argname] = value;
    end
end

-- getter
-- getter
-- getter
br.recursiveget = function(argname)
    if br.utils.string.includes(argname, ".") then
            
        local result_txt = "return br";
        local splited = br.utils.string.split2(argname, ".");
        if br.utils.array.includes(splited, "") then 
            return br[argname];
        end
        for i = 1, #splited do
            result_txt = result_txt .. "[\"" .. splited[i] .. "\"]";
        end
        local result = terralib.loadstring(result_txt)();
        return result;
    else
        return br[argname];
    end
end

-- set
br.setvalue = function(varname, value)
    br.recursiveset(varname,value);
    return value;
end

br.setfrom = function(varname, funcname, ...)
    local args = {...};
    local result;
    if type(funcname) == "string" then
        result = br.recursiveget(funcname)(unpack(args or {}));
    elseif type(funcname) == "function" then
        result = funcname(unpack(args or {}));
    end
    br.recursiveset(varname, result);
    return result;
end

br.set = function(...) -- if keyword "from" is found, the next is the funcname, the following should be the arguments, and the ones before it should be the variables names, as lua does support multiple returns, the variables will be setted in order
    if br.utils.array.includes({...}, "from") then
        local args = {...};
        local fromindex = br.utils.table.find(args, "from");
        local varnames = br.utils.array.slice(args, 1, fromindex - 1);
        local funcname = args[fromindex + 1];
        local funcargs = br.utils.array.slice(args, fromindex + 2, #args);
        local result = {br.recursiveget(funcname)(unpack(funcargs or {}))};
        for i = 1, #varnames do
            br.recursiveset(varnames[i], result[i]);
        end
        return unpack(result);
    else
        local args = {...};
        local varname = args[1];
        local value = args[2];
        br.recursiveset(varname, value);
        return value;
    end
end


--string functions
--string functions
--string functions

br.string = function(...)
    local args = {...};
    return table.concat(args, " ");
end

-- data list functions
-- data list functions
-- data list functions

br.help = function(target)

    local organize = {tables = {}, functions = {}, numbers = {}, strings = {}, booleans = {}, userdata = {}, other = {}};
    if type(target) == "string" then
        target = br.recursiveget(target);
    elseif type(target) == "nil" then
        target = br;
    end

    br.debugprint(br.utils.console.colorstring("[HELP INFO]", "magenta") .. ": help for (" .. type(target) .. ")", target);
    
    if type(target) == "table" then
        for k,v in pairs(target) do 
            local color = "blue";
            if type(v) == "function" then
                color = "green";
                table.insert(organize.functions, k);
            elseif type(v) == "table" then
                color = "magenta";
                table.insert(organize.tables, k);
            elseif type(v) == "number" then
                color = "white";
                table.insert(organize.numbers, k);
            elseif type(v) == "string" then
                color = "yellow";
                table.insert(organize.strings, k);
            elseif type(v) == "boolean" then
                color = "cyan";
                table.insert(organize.booleans, k);
            elseif type(v) == "userdata" then
                color = "red";
                table.insert(organize.userdata, k);
            end
            
            --print(br.utils.console.colorstring(k, color) .. "(" .. type(v) .. ")", v);
        end
        
        for k,v in pairs(organize) do
            table.sort(v);
        end

        if #organize.tables > 0 then
            print(br.utils.console.colorstring("[", "magenta") .. "tables" .. br.utils.console.colorstring("]", "magenta") .. ":");
            for k,v in pairs(organize.tables) do
                print(br.utils.console.colorstring(v, "magenta"), target[v]);  
            end
        end

        if #organize.functions > 0 then
            print(br.utils.console.colorstring("[", "green") .. "functions" .. br.utils.console.colorstring("]", "green") .. ":");
            for k,v in pairs(organize.functions) do
                print(br.utils.console.colorstring(v, "green"), target[v]);  
            end
        end

        if #organize.numbers > 0 then
            print(br.utils.console.colorstring("[", "white") .. "numbers" .. br.utils.console.colorstring("]", "white") .. ":");
            for k,v in pairs(organize.numbers) do
                print(br.utils.console.colorstring(v, "white"), target[v]);  
            end
        end

        if #organize.strings > 0 then
            print(br.utils.console.colorstring("[", "yellow") .. "strings" .. br.utils.console.colorstring("]", "yellow") .. ":");
            for k,v in pairs(organize.strings) do
                print(br.utils.console.colorstring(v, "yellow"), target[v]);  
            end
        end

        if #organize.booleans > 0 then
            print(br.utils.console.colorstring("[", "cyan") .. "booleans" .. br.utils.console.colorstring("]", "cyan") .. ":");
            for k,v in pairs(organize.booleans) do
                print(br.utils.console.colorstring(v, "cyan"), target[v]);  
            end
        end

        if #organize.userdata > 0 then
            print(br.utils.console.colorstring("[", "red") .. "userdata" .. br.utils.console.colorstring("]", "red") .. ":");
            for k,v in pairs(organize.userdata) do
                print(br.utils.console.colorstring(v, "red"), target[v]);  
            end
        end

        if #organize.other > 0 then
            print(br.utils.console.colorstring("[", "blue") .. "other" .. br.utils.console.colorstring("]", "blue") .. ":");
            for k,v in pairs(organize.other) do
                print(br.utils.console.colorstring(v, "blue"), target[v]);  
            end
        end

        br.debugprint(br.utils.console.colorstring("[HELP DONE]", "green") .. ": help for (" .. type(target) .. ")", target);

    else
        br.debugprint(br.utils.console.colorstring("[HELP ERROR]", "red") .. ": invalid target for help function, target has type " .. type(target));    
    end
end

br.rawhelp = function(target)--just print the names, no color, no types
    if type(target) == "string" then
        for k,v in pairs(br.recursiveget(target)) do
            print(k);
        end
    elseif type(target) == "table" then
        for k,v in pairs(target or br) do 
            print(k);
        end
    elseif type(target) == "nil" then
        for k,v in pairs(br) do 
            print(k);
        end
    else
        br.debugprint(br.utils.console.colorstring("[DEBUG ERROR]", "red") .. ": invalid argument for help function");
    end
end

br["{}"] = function(...)
    return {...};
end

br["..."] = function(...)
    return unpack({...});
end

br["unpack"] = function(target)
    return unpack(target);
end

br["pack"] = function(...)
    local funcname = ({...})[1];
    local args = br.utils.array.slice({...}, 2, #({...}));
    return{funcname(unpack(args))};
end

return br;