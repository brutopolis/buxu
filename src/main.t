bruterPath = debug.getinfo(1).source:match("@?(.*/)");
--remove the "src/"
bruterPath = string.sub(bruterPath, 1, #bruterPath-4);

package.terrapath = package.terrapath .. bruterPath .. "?.t;" .. bruterPath .. "src/?.t;" .. bruterPath .. "src/?/?.t;"
package.terrapath = package.terrapath .. bruterPath .. "?.lua;" .. bruterPath .. "lib/?.lua;" .. bruterPath .. "lib/?/?.lua;"

version = "0.0.7"

utils = require 'luatils.init'
tocstr = require('lib.tocstr')
list = require('lib.list');
String = require('lib.string');
-- load the main.c
c = terralib.includec(bruterPath .. "src/main.c");

condensed_args = arg[0] .. " " .. table.concat(arg, " ");

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

vm = 
{
    source = "",
    outputPath = "",
    variables = {},
    exports = {},
    currentcmd = "",
}
-- vm variables
vm.variables = 
{
    module = function(path)
        local temp = require(path);
        for k,v in pairs(temp) do
            if k == "exports" then
                for k,v in pairs(v) do
                    vm.exports[k] = v;
                end
            else
                vm.variables[k] = v;
            end
        end
    end,
    global = function(name, value)
        vm.variables[name] = global(value);
    end,
    set = function(name, value)
        vm.variables[name] = value;
    end,
    export = function(name, as)
        if as then
            vm.exports[as] = vm.variables[name];
        else
            vm.exports[name] = vm.variables[name];
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
}

vm.variables.eval = vm.variables.loadstring;
vm.variables["?"] = vm.variables.loadstring;
vm.variables["?file"] = vm.variables.loadfile;
vm.variables["#"] = vm.variables.comment;
vm.variables["{}"] = vm.variables.emptyobject;
-- parse the arguments

if utils.array.includes(arg, "-v") or utils.array.includes(arg, "--version") then
    print("bruter version " .. version)
    os.exit(0)
elseif utils.array.includes(arg, "--help") or utils.array.includes(arg,"-h") then
    print("Usage: bruter <source file> [-o <output file>] [-h] [-v] [--version] [--help]")
    print("Options:")
    print("  --help    Display this information")
    print("  -h        Display this information")
    print("  -v        Display the version")
    print("  --version Display the version")
    print("  -o        Output the compiled file to the specified path")
    print("  --output  Output the compiled file to the specified path")
    os.exit(0)
elseif arg[1] == nil then
    print("No source file specified")
    os.exit(1)
end

vm.source = utils.file.load.text(arg[1]);

if utils.array.includes(arg, "-o") or utils.array.includes(arg, "--output") then
    local temp = utils.table.find(arg, "-o") or utils.table.find(arg, "--output")
    vm.outputPath = arg[temp + 1]
end

function recursiveset(argname, value)
    if utils.string.includes(argname, ".") then
        if (type(value) == "table") then
            terralib.loadstring("vm.variables." .. argname .. " = " .. utils.stringify(value))()
        else
            terralib.loadstring("vm.variables." .. argname .. " = " .. value)();
        end
    else
        vm.variables[argname] = value;
    end
end

function recursiveget(argname)
    if utils.string.includes(argname, ".") then
        local result = terralib.loadstring("return vm.variables." .. argname)();
        return result;
    else
        return vm.variables[argname];
    end
end

function cleanSource(source)
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

function parseArgs(args)
    local newargs = utils.array.clone(args);
    for i = 1, #args do
        if string.byte(args[i],1) == 36 then
            local name = utils.string.replace(args[i], "%$", '');
            newargs[i] = recursiveget(name);
        elseif (string.byte(args[i],1) > 47 and string.byte(args[i],1) < 58) or string.byte(args[i],1) == 45 then
            newargs[i] = tonumber(args[i]);
        end
    end
    return newargs;
end

vm.source = cleanSource(vm.source)

function parseSourceFile()
    local splited = utils.string.split(vm.source, ";");
    local func = "";
    for i = 1, #splited - 1 do
        local splited_args = utils.string.split(splited[i], " ");
        if(utils.string.includes(splited_args[1], ":")) then
            local name_and_func = utils.string.split(splited_args[1], ":");
            local name, func = name_and_func[1], name_and_func[2];
            local args = parseArgs(utils.array.slice(splited_args, 2, #splited_args));
            --print(name, func, args)
            recursiveset(name, recursiveget(func)(unpack(args or {})));
        else
            local func = splited_args[1];
            local args = parseArgs(utils.array.slice(splited_args, 2, #splited_args));
            local _function = recursiveget(func);
            if _function then
                print(func, utils.stringify(args))
                _function(unpack(args or {}));
            else
                print(vm.source)
                error("function " .. func .. " not found")
            end
        end
    end
end

parseSourceFile();

if vm.outputPath ~= "" then
    terralib.saveobj(vm.outputPath,vm.exports, nil, nil, false);
end