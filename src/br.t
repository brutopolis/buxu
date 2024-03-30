-- bruter module
-- bruter module
-- bruter module
local _bruterPath = debug.getinfo(1).source;

local br = 
{
    -- version
    version = "0.2.0b",
    
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
    
    -- preprocessors
    -- preprocessors
    -- preprocessors
    preprocessors = 
    {
        sugar = function(source)
            local nstr = br.utils.string.replace3(source, "%s+"," ")
            nstr = br.utils.string.replace3(nstr, "\n", "")
            nstr = br.utils.string.replace3(nstr, "  ", "")
            nstr = br.utils.string.replace(nstr, "; ", ";")
            nstr = br.utils.string.replace(nstr, " ;", ";")
            nstr = br.utils.string.replace(nstr, " ; ", ";")
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
        if string.byte(args[i],1) == 36 then
            local name = br.utils.string.replace(args[i], "%$", '');
            newargs[i] = br.recursiveget(name);
        elseif (string.byte(args[i],1) > 47 and string.byte(args[i],1) < 58) or (string.byte(args[i],1) == 45 and (#args[i] > 1 and string.byte(args[i],2) > 47 and string.byte(args[i],2) < 58)) then
            newargs[i] = tonumber(args[i]);
        elseif args[i] == "true" then
            newargs[i] = true;
        elseif args[i] == "false" then
            newargs[i] = false;
        elseif args[i] == "{}" then
            newargs[i] = {};
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
br.parse = function(src)
    src = br.preprocess(src);
    local splited = br.utils.string.split3(src, ";");
    local func = "";
    for i = 1, #splited - 1 do
        br.debugprint("\n" .. br.utils.console.colorstring("[DEBUG LINE]", "cyan") .. ": parsing line " .. i);
        
        local splited_args = br.utils.string.split2(splited[i], " ");
        local func = splited_args[1];
        local args = br.parseargs(br.utils.array.slice(splited_args, 2, #splited_args));
        local _function = br.recursiveget(func);
        if _function then
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
    br.dobr(br.bruterpath .. "libr/" .. name .. "/" .. name .. ".br");
end

-- setter
-- setter
-- setter
br.recursiveset = function(argname, value)
    if br.utils.string.includes(argname, ".") then
        br.temp = value; 
        local result_txt = "br";
        local splited = br.utils.string.split2(argname, ".");
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
        --print("return br." .. argname) -- from a.b.c to ["a"]["b"]["c"]
        local result_txt = "return br";
        local splited = br.utils.string.split2(argname, ".");
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
br.set = function(as, value)
    br.recursiveset(as,value);
end

br.setfrom = function(varname, funcname, ...)
    local args = {...};
    local result;
    if type(funcname) == "string" then
        result = br.recursiveget(funcname)(unpack(args or {}));
    elseif type(funcname) == "function" then
        result = funcname(unpack(args or {}));
    else 
        
    end
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
    br.debugprint(br.utils.console.colorstring("[DEBUG INFO]", "magenta") .. ": loading string: " .. str)
    return ((terralib.loadstring(str))());
end

-- loadfile
br.loadfile = function(path)
    return(terralib.loadfile(path)());
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


-- module aliases
-- module aliases
-- module aliases
br["?"] = br.loadstring;
br["?file"] = br.loadfile;

return br;