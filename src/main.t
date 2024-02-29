local bruterPath = debug.getinfo(1).source:match("@?(.*/)");
--remove the "src/"
bruterPath = string.sub(bruterPath, 1, #bruterPath-4);

package.terrapath = package.terrapath .. bruterPath .. "?.t;" .. bruterPath .. "src/?.t;" .. bruterPath .. "src/?/?.t;"
package.terrapath = package.terrapath .. bruterPath .. "?.lua;" .. bruterPath .. "lib/?.lua;" .. bruterPath .. "lib/?/?.lua;"

local version = "0.0.6"

local utils = require 'luatils.init'
local tocstr = require('lib.tocstr')
local list = require('lib.list');
local String = require('lib.string');
-- load the main.c
local c = terralib.includec(bruterPath .. "src/main.c");

local condensed_args = arg[0] .. " " .. table.concat(arg, " ");

local ListInt8 = list(int8);
local ListInt16 = list(int16);
local ListInt32 = list(int32);
local ListInt64 = list(int64);
local ListUInt8 = list(uint8);
local ListUInt16 = list(uint16);
local ListUInt32 = list(uint32);
local ListUInt64 = list(uint64);
local ListDouble = list(double);
local ListFloat = list(float);
local ListString = list(String);
local ListInt = ListInt32;
local ListListInt8 = list(ListInt8);
local ListListInt16 = list(ListInt16);
local ListListInt32 = list(ListInt32);
local ListListInt64 = list(ListInt64);
local ListListUInt8 = list(ListUInt8);
local ListListUInt16 = list(ListUInt16);
local ListListUInt32 = list(ListUInt32);
local ListListUInt64 = list(ListUInt64);
local ListListDouble = list(ListDouble);
local ListListFloat = list(ListFloat);
local ListListInt = ListListInt32;

struct Hash
{
    index: int,
    type: rawstring,
    value: rawstring
}
local ListHash = list(Hash);

struct Managed
{
    int8: ListInt8,
    int16: ListInt16,
    int32: ListInt32,
    int64: ListInt64,
    uint8: ListUInt8,
    uint16: ListUInt16,
    uint32: ListUInt32,
    uint64: ListUInt64,
    double: ListDouble,
    float: ListFloat,
    int: ListInt,
    ListInt8: ListListInt8,
    ListInt16: ListListInt16,
    ListInt32: ListListInt32,
    ListInt64: ListListInt64,
    ListUInt8: ListListUInt8,
    ListUInt16: ListListUInt16,
    ListUInt32: ListListUInt32,
    ListUInt64: ListListUInt64,
    ListDouble: ListListDouble,
    ListFloat: ListListFloat,
    ListInt: ListListInt,
    String: ListString
}

struct VirtualMachine
{
    source: rawstring,
    outputPath: rawstring,
    managed: Managed,
    hash: ListHash
}

terra VirtualMachine.methods.new(source: rawstring, outputPath: rawstring)
    var result:VirtualMachine;
    result.source = "";
    result.outputPath = "";
    result.managed.int8 = ListInt8.new();
    result.managed.int16 = ListInt16.new();
    result.managed.int32 = ListInt32.new();
    result.managed.int64 = ListInt64.new();
    result.managed.uint8 = ListUInt8.new();
    result.managed.uint16 = ListUInt16.new();
    result.managed.uint32 = ListUInt32.new();
    result.managed.uint64 = ListUInt64.new();
    result.managed.double = ListDouble.new();
    result.managed.float = ListFloat.new();
    result.managed.int = ListInt.new();
    result.managed.ListInt8 = ListListInt8.new();
    result.managed.ListInt16 = ListListInt16.new();
    result.managed.ListInt32 = ListListInt32.new();
    result.managed.ListInt64 = ListListInt64.new();
    result.managed.ListUInt8 = ListListUInt8.new();
    result.managed.ListUInt16 = ListListUInt16.new();
    result.managed.ListUInt32 = ListListUInt32.new();
    result.managed.ListUInt64 = ListListUInt64.new();
    result.managed.ListDouble = ListListDouble.new();
    result.managed.ListFloat = ListListFloat.new();
    result.managed.ListInt = ListListInt.new();
    result.managed.String = ListString.new();
    result.hash = ListHash.new();
    return result;
end

local compilervm = 
{
    source = "",
    outputPath = "",
    compiledFunctions = {},
    globals = {},
    constants = {},
    exports = {},
    currentcmd = "",
}
-- vm functions
compilervm.functions = 
{
    module = function(path)
        local temp = require(path);
        for k,v in pairs(temp) do
            if k == "exports" then
                for k,v in pairs(v) do
                    compilervm.exports[k] = v;
                end
            else
                compilervm.compiledFunctions[k] = v;
            end
        end
    end,
    global = function(name, type, value)
        print("global " .. name .. " " .. type .. " " .. value);
        compilervm.globals[name] = global(value);
    end,
    auto = function(type, name, value)
        compilervm.managed[type]:push(value);
        local realtype = terralib.loadstring("return " .. type)();
        compilervm.hash[name] = {link = compilervm.managed[type]:size() - 1, type = type, value = value};
    end,
    test = function()
        c.printf("testdasdasdas\n\n");
    end,
    export = function(name, as)
        if as then
            compilervm.exports[as] = compilervm.compiledFunctions[name];
        else
            compilervm.exports[name] = compilervm.compiledFunctions[name];
        end
    end,
}

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

compilervm.source = utils.file.load.text(arg[1]);

if utils.array.includes(arg, "-o") or utils.array.includes(arg, "--output") then
    local temp = utils.table.find(arg, "-o") or utils.table.find(arg, "--output")
    compilervm.outputPath = arg[temp + 1]
end

function cleanSource(source)
    local nstr = utils.string.replace(source, "\n","")
    nstr = utils.string.replace(nstr, "\\n", "\n")
    nstr = utils.string.replace(nstr, " : ", ":")
    nstr = utils.string.replace(nstr, " :", ":")
    nstr = utils.string.replace(nstr, ": ", ":")
    nstr = utils.string.replace(nstr, " ;", ";")
    nstr = utils.string.replace(nstr, "; ", ";")
    while utils.string.includes(nstr, "  ") do
        nstr = utils.string.replace(nstr, "  ", " ")
    end
    return nstr
end

function parseArgs(args)
    local newargs = utils.array.clone(args);
    for i = 1, #args do
        if string.byte(args[i],1) == 36 then
            local name = utils.string.replace(args[i], "%$", '');
            if compilervm.globals[name] == nil then
                error("global " .. name .. " not found")
            end
            newargs[i] = compilervm.globals[name];
        elseif (string.byte(args[i],1) > 47 and string.byte(args[i],1) < 58) or string.byte(args[i],1) == 45 then
            newargs[i] = tonumber(args[i]);
        end
    end
    return newargs;
end

compilervm.source = cleanSource(compilervm.source)

function parseSourceFile()
    local splited = utils.string.split(compilervm.source, ";");
    local func = "";
    for i = 1, #splited - 1 do
        compilervm.currentcmd = splited[i];
        local splited_args = utils.string.split(splited[i], " ");
        if(utils.string.includes(splited_args[1], ":")) then
            local name_and_func = utils.string.split(splited_args[1], ":");
            local name, func = name_and_func[1], name_and_func[2];
            local args = parseArgs(utils.array.slice(splited_args, 2, #splited_args));
            compilervm.globals[name] = (compilervm.functions[func] or compilervm.compiledFunctions[func])(unpack(args or {}));
        else
            
            local func = splited_args[1];
            local args = parseArgs(utils.array.slice(splited_args, 2, #splited_args));
            local _function = compilervm.functions[func] or compilervm.compiledFunctions[func]; 
            if _function then
                _function(unpack(args or {}));
            else
                error("function " .. func .. " not found")
            end
        end
    end
end

parseSourceFile();

terra main()
    var vm = VirtualMachine.new(compilervm.source, compilervm.outputPath);
    -- int lists tests
    var listaint = ListInt.new();
    var listaint2 = ListInt.new();

    var listaintint = ListListInt.new();

    listaintint:push(listaint);
    listaintint:push(listaint2);

    listaint2:push(1);
    listaint2:push(2);
    listaint2:push(3);

    listaint:push(12312);
    listaint:push(2);
    listaint:insert(333321, 0);
    c.printf("[1][2] = %d\n", listaintint:get(1):get(2));
    -- string tests
    var str = String.new();
    str:push(('a')[0]);
    str:push(('b')[0]);
    str:push(('c')[0]);
    str:push(('d')[0]);
    str:push(('e')[0]);
    str:push(('f')[0]);
    str:push(('g')[0]);
    str:push(('h')[0]);
    str:push(('i')[0]);
    str:push(('j')[0]);
    str:push(('d')[0]);
    str:push(('e')[0]);
    str:push(('f')[0]);
    str:push(('g')[0]);
    str:push(('h')[0]);
    str:push(('i')[0]);

    var str2 = String.fromString("def");

    var str3:String = String.fromString("xyz");
    
    str:replace(str2, str3);
    
    str:print("str = ");
    str3:print("str3 = ");

    var str4 = String.fromString("y");

    var strsplited = str:split(str4);

    c.printf("strsplited[0] = %s\n", strsplited:get(0).array);
    c.printf("strsplited[1] = %s\n", strsplited:get(1).array);
    c.printf("strsplited[2] = %s\n", strsplited:get(2).array);

    c.printf("teste = %s\n", condensed_args);
end

compilervm.compiledFunctions.main = main;
compilervm.exports.main = main;

if compilervm.outputPath ~= "" then
    terralib.saveobj(compilervm.outputPath,compilervm.exports, nil, nil, false);
end