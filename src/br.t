-- bruter module
-- bruter module
-- bruter module
local _bruterPath = debug.getinfo(1).source;

local br = 
{
    -- compiled objects exports
    exports = {},

    -- other
    comment = function()end,
    utils = require("lib.luatils"),
    -- preprocessors
    -- preprocessors
    -- preprocessors
    vm = 
    {
        -- version
        version = "0.2.4",
        -- source and output
        source = "",
        outputpath = "",
        -- current path
        bruterpath = string.sub(_bruterPath, 2, #_bruterPath-8),
        -- debug mode
        debug = false,
        
        preprocessors = 
        {
            sugar = function(source)
                local nstr = br.utils.string.replace3(source, "%s+"," ")
                
                nstr = br.utils.string.replace(nstr, "%}", "%} ")
                nstr = br.utils.string.replace(nstr, "%{", " %{")
                nstr = br.utils.string.replace(nstr, "%( ", "%(")
                nstr = br.utils.string.replace(nstr, " %) ", "%)")
                nstr = br.utils.string.replace(nstr, " %)", "%)")
                nstr = br.utils.string.replace(nstr, "%(", " %(")
                
                --if the first two chars ate " (" remove the space
                if string.byte(nstr,1) == 32 and string.byte(nstr,2) == 40 then
                    nstr = string.sub(nstr, 2, #nstr);
                end
                
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
    },
}

-- math and logic functions
-- math and logic functions
-- math and logic functions

br.math = {};

br["+"] = function(...)
	local args = {...};
	local result = args[1];
	for i = 2, #args do
		result = result + args[i];
	end
	return result;
end

br["-"] = function(...)
	local args = {...};
	local result = args[1];
	for i = 2, #args do
		result = result - args[i];
	end
	return result;
end

br["*"] = function(...)
	local args = {...};
	local result = args[1];
	for i = 2, #args do
		result = result * args[i];
	end
	return result;
end

br["/"] = function(a,b) return a / b; end
br["^"] = function(a,b) return a ^ b; end
br["%"] = function(a,b) return a % b; end

br["<"] = function(a,b) return a < b; end
br["<="] = function(a,b) return a <= b; end
br[">"] = function(a,b) return a > b; end
br[">="] = function(a,b) return a >= b; end
br["=="] = function(a,b) return a == b; end
br["!="] = function(a,b) return a ~= b; end

-- parse the arguments
-- parse the arguments
-- parse the arguments
br.vm.parsearg = function(larg)
    local result = larg;
    if larg == nil or larg == "" then
        return nil;
    end
    -- if a variable, get it
    if string.byte(larg,1) == 36 then -- variable
        local name = br.utils.string.replace(larg, "%$", '');
        result = br.vm.recursiveget(name);
    
    -- if a sentence enclose by parentesis, remove them and parse it
    elseif string.byte(larg,1) == 40 then -- sentence
        result = br.vm.parse(string.sub(larg, 2, #larg - 1),true);
    
    -- if enclose by keys {} or backticks remove them
    elseif string.byte(larg,1) == 96 or string.byte(larg,1) == 123 then
        result = string.sub(larg, 2, #larg - 1);
        
    -- if a number
    elseif (string.byte(larg,1) > 47 and string.byte(larg,1) < 58) or (string.byte(larg,1) == 45 and (#larg > 1 and string.byte(larg,2) > 47 and string.byte(larg,2) < 58)) then -- number
        result = tonumber(larg);


    elseif larg == "true" then
        result = true;
    elseif larg == "false" then
        result = false;
    elseif larg == "nil" then
        result = nil;
    end
    
    return result;
end

br.vm.parseargs = function(args)
    local newargs = br.utils.array.clone(args);
    for i = 1, #args do
        newargs[i] = br.vm.parsearg(args[i]);
    end
    return newargs;
end

-- preprocess the source
-- preprocess the source
-- preprocess the source
br.vm.preprocess = function(_src)
    local result = _src .. '';
    for k, v in pairs(br.vm.preprocessors) do
        result = v(result);
    end
    return result;
end

br.vm.debugprint = function(...)
    if br.vm.debug then
        print(...);
    end
end

-- parse the source file
-- parse the source file
-- parse the source file
br.vm.parse = function(src, isSentence)
    if isSentence then 
        src = "vm.fakeset " .. src;
    end
    src = br.vm.preprocess(src);
    local splited = br.utils.string.split3(src, ";");
    local func = "";
    for i = 1, #splited - 1 do
        br.vm.debugprint("\n" .. br.utils.console.colorstring("[DEBUG LINE]", "cyan") .. ": parsing line " .. i);
        br.vm.debugprint(br.utils.console.colorstring("[DEBUG CODE]", "cyan") .. ": " .. splited[i] .. br.utils.console.colorstring("\n[CODE END]", "cyan"));
        local splited_args = br.utils.string.split3(splited[i], " ");
        
        local func = splited_args[1];
        table.remove(splited_args, 1);

        -- first char is a variable or or a sentence it parse it as arg first, else, its a funcion name
        if string.byte(func,1) == 36 or string.byte(func,1) == 40 or string.byte(func,1) == 96 or string.byte(func,1) == 123 or (string.byte(func,1) > 47 and string.byte(func,1) < 58) or (string.byte(func,1) == 45 and (#func > 1 and string.byte(func,2) > 47 and string.byte(func,2) < 58)) then
            func = br.vm.parsearg(func);
        end
        
        local args = br.vm.parseargs(splited_args);
        local _function = type(func) == "function" and func or br.vm.recursiveget(func);
        
        if _function and isSentence then
            
            -- command debbuger
            br.vm.debugprint(func .. "{")
            for k,v in pairs(splited_args) do
                br.vm.debugprint("    " .. k .. " =",v);
            end
            br.vm.debugprint("}");
            
            br.vm.debugprint(br.utils.console.colorstring("[DEBUG DONE]", "green") .. ": line " .. i .. " ok\n");
            
            -- in a sentence the code execution stops on the first return it gets
            local result = _function(unpack(args or {}));
            if result then
                return result;
            end
        elseif _function then
            
            -- command debbuger
            br.vm.debugprint(func .. "{")
            for k,v in pairs(splited_args) do
                br.vm.debugprint("    " .. k .. " =",v);
            end
            br.vm.debugprint("}");

            br.vm.debugprint(br.utils.console.colorstring("[DEBUG DONE]", "green") .. ": line " .. i .. " ok\n");
            _function(unpack(args or {}));
        elseif br.exit then -- if on repl
            br.vm.debugprint(br.utils.console.colorstring("Error", "red") .. " parsing the following code:");
            br.vm.debugprint(src);
            br.vm.debugprint(br.utils.console.colorstring("[DEBUG FAIL]", "red") .. ": function " .. func .. " not found\n");
        else
            --if first char is a parentesis and the last too
            if string.byte(splited[i],1) == 40 and string.byte(splited[i],#splited[i]) == 41 then
                br.vm.debugprint(br.utils.console.colorstring("Warning", "yellow") .. " parsing the following line:");
                br.vm.debugprint(splited[i]);
                if _function then
                    br.vm.debugprint(br.utils.console.colorstring("[DEBUG DONE]", "yellow") .. ": function " .. func .. " not found, but the code seems to be enclosed in a sentence, so the execution was continued");
                else
                    br.vm.debugprint(br.utils.console.colorstring("[DEBUG FAIL]", "yellow") .. ": unamed function not found, but the code seems to be enclosed in a sentence, so the execution was continued");
                end
            else -- if not
                br.vm.debugprint(br.utils.console.colorstring("Error", "red") .. " parsing the following code:");
                br.vm.debugprint(src);
                if type(_function) == "string" or type(_function) == "number" or type(_function) == "boolean" then
                    br.vm.debugprint(br.utils.console.colorstring("[DEBUG FAIL]", "red") .. ": function " .. func .. " not found, as code seems not to be enclosed in a sentence, the execution was stopped");
                    error("function " .. func .. " not found, as code seems not to be enclosed in a sentence, the execution was stopped");
                else
                    br.vm.debugprint(br.utils.console.colorstring("[DEBUG FAIL]", "red") .. ": unamed function not found, as code seems not to be enclosed in a sentence, the execution was stopped");
                    error("unamed function not found, as code seems not to be enclosed in a sentence, the execution was stopped");
                end
            end
        end
    end
end

br.repl = function()
    --exit function
    br.vm._replExit = false;
    br.exit = function()
        br.vm._replExit = true;
    end
    -- version, only print if not in a breakpoint repl
    if not br.vm._inBreakpoint then
        print("bruter v" .. br.vm.version);
    end
    
    local line = "";
    local count = 0;
    while not br.vm._replExit do
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
            br.vm.parse(line .. buffer);
            line = "";
        else
            line = line .. buffer;
        end
    end
end

br.breakpoint = function()
    if not br.vm.debug then
        print(br.utils.console.colorstring("[WARNING]", "red") .. ": a breakpoint been ignored because debug mode is not enabled.");
        return;
    end
    br.vm._inBreakpoint = true;
    print(br.utils.console.colorstring("[BREAKPOINT]", "red") .. ": entering breakpoint repl, type 'exit' to continue");
    br.repl();
    if br.vm.debug then
        print(br.utils.console.colorstring("[BREAKPOINT DONE]", "green") .. ": continuing execution");
    else
        print("\n" .. br.utils.console.colorstring("[BREAKPOINT DONE]", "yellow") .. ": continuing execution, but debug mode is not enabled anymore, so breakpoints will be ignored.\n");
    end
    br.vm._inBreakpoint = false;
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
    if br.utils.file.exist(br.vm.bruterpath .. "libr/" .. name .. "/" .. name .. ".br") then
        br.bruter.include(br.vm.bruterpath .. "libr/" .. name .. "/" .. name .. ".br");
    elseif br.utils.file.exist(br.vm.bruterpath .. "libr/" .. name .. "/" .. name .. ".lua") then
        terralib.loadfile(br.vm.bruterpath .. "libr/" .. name .. "/" .. name .. ".lua")();
    elseif br.utils.file.exist(br.vm.bruterpath .. "libr/" .. name .. "/" .. name .. ".t") then
        terralib.loadfile(br.vm.bruterpath .. "libr/" .. name .. "/" .. name .. ".t")();
    elseif br.utils.file.exist(br.vm.bruterpath .. "libr/" .. name .. ".br") then
        br.bruter.include(br.vm.bruterpath .. "libr/" .. name .. ".br");
    elseif br.utils.file.exist(br.vm.bruterpath .. "libr/" .. name .. ".lua") then
        terralib.loadfile(br.vm.bruterpath .. "libr/" .. name .. ".lua")();
    elseif br.utils.file.exist(br.vm.bruterpath .. "libr/" .. name .. ".t") then
        terralib.loadfile(br.vm.bruterpath .. "libr/" .. name .. ".t")();
    else
        br.vm.debugprint(br.utils.console.colorstring("[ERROR]", "red") .. ": library not found: " .. name);
    end
end

br.bruter = {};

br.bruter.include = function(path)
    local c = br.utils.file.load.text(path);
    br.vm.parse(c);
end

br.bruter.includestring = function(str)
    br.vm.parse(str);
end



br["terra"] = {};

-- loadstring (lua/terra)
br["terra"].loadstring = function(str)
    br.vm.debugprint(br.utils.console.colorstring("[DEBUG INFO]", "magenta") .. ": loading string: " .. str)
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

-- setter
-- setter
-- setter
br.vm.recursiveset = function(argname, value)
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
br.vm.recursiveget = function(argname)
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
br.vm.setvalue = function(varname, value, ...)
    br.vm.recursiveset(varname,value);
    return value;
end

br.vm.setfrom = function(varname, funcname, ...)
    local args = {...};
    local result;
    if type(funcname) == "string" then
        result = br.vm.recursiveget(funcname)(unpack(args or {}));
    elseif type(funcname) == "function" then
        result = funcname(unpack(args or {}));
    end
    br.vm.recursiveset(varname, result);
    return result;
end

br.vm.fakesetfrom = function(funcname, ...)
    local args = {...};
    local result;
    if type(funcname) == "string" then
        result = br.vm.recursiveget(funcname)(unpack(args or {}));
    elseif type(funcname) == "function" then
        result = funcname(unpack(args or {}));
    end
    return result;
end

br.vm.fakeset = function(value, ...)
    if value == "from" then
        return br.vm.fakesetfrom(...);
    elseif #({...}) > 0 then
        return br.vm.fakesetfrom(value, ...);
    elseif type(value) == "function" then
        return br.vm.fakesetfrom(value, ...);
    else
        local target = br.vm.recursiveget(value);
        if target then
            if type(target) == "function" then
                return br.vm.fakesetfrom(value, ...);
            end
        end
    end
    return value;
end

-- set
br.set = function(varname, value, ...)
    if value == "from" then
        return br.vm.setfrom(varname, ...);
    end
    br.vm.recursiveset(varname,value);
    return value;
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
        target = br.vm.recursiveget(target);
    elseif type(target) == "nil" then
        target = br;
    end

    br.vm.debugprint(br.utils.console.colorstring("[HELP INFO]", "magenta") .. ": help for (" .. type(target) .. ")", target);
    
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
                --print(#v)
                if #target[v] > 32 then
                    print(br.utils.console.colorstring(v, "yellow"), string.sub(target[v], 1, 32) .. "...");
                else
                    print(br.utils.console.colorstring(v, "yellow"), target[v]);  
                end
                --print(br.utils.console.colorstring(v, "yellow"), target[v]);  
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

        br.vm.debugprint(br.utils.console.colorstring("[HELP DONE]", "green") .. ": help for (" .. type(target) .. ")", target);

    else
        br.vm.debugprint(br.utils.console.colorstring("[HELP ERROR]", "red") .. ": invalid target for help function, target has type " .. type(target));    
    end
end

br.print = print;

br["[]"] = function(...)
    return {...};
end

br["debug"] = function(wish)
    if wish == true then
        br.vm.debug = true;
    elseif wish == false then
        br.vm.debug = false;
    else 
        br.vm.debug = not br.vm.debug;
    end

    if br.vm.debug then
        print(br.utils.console.colorstring("[DEBUG INFO]", "green") .. ": debug mode enabled");
    else
        print(br.utils.console.colorstring("[DEBUG INFO]", "red") .. ": debug mode disabled");
    end
end

br["if"] = function(condition, codestr, _else, codestr2)
    if condition then
        if type(codestr) == "string" then
            return br.vm.parse(codestr, true);
        elseif type(codestr) == "function" then
            return codestr();
        end
    else
        if _else then
            print(_else)
            if type(_else) == "function" then
                return _else();
            elseif type(_else) == "string" and _else == "else" then
                if _else == "else" then
                    if type(codestr2) == "string" then
                        return br.vm.parse(codestr2, true);
                    elseif type(codestr2) == "function" then
                        return codestr2();
                    end
                else
                    return br.vm.parse(_else, true);
                end
            end
        end
    end
end

br["&&"] = function(...)
    for k,v in pairs({...}) do
        if not v then
            return false;
        end
    end
    return true;
end

br["||"] = function(...)
    for k,v in pairs({...}) do
        if v then
            return true;
        end
    end
    return false;
end

br["!"] = function(value)
    return not value;
end

br["while"] = function(condition, codestr)
    --print (condition, br.vm.parse(condition))
    local _cond = br.vm.parse(condition, true);
    while _cond do
        if type(codestr) == "string" then
            br.vm.parse(codestr, true);
        elseif type(codestr) == "function" then
            codestr();
        end
        _cond = br.vm.parse(condition, true);
    end
end

return br;
