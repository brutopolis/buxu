local bruterPath = debug.getinfo(1).source:match("@?(.*/)");
--remove the "src/"
bruterPath = string.sub(bruterPath, 1, #bruterPath-4);

package.terrapath = package.terrapath .. bruterPath .. "?.t;" .. bruterPath .. "src/?.t;" .. bruterPath .. "src/?/?.t;"
package.terrapath = package.terrapath .. bruterPath .. "?.lua;" .. bruterPath .. "lib/?.lua;" .. bruterPath .. "lib/?/?.lua;"

local version = "0.0.3"

local utils = require 'luatils.init'
local tocstr = require('lib.tocstr')
local String = require('lib.string')
-- load the main.c
local c = terralib.includec(bruterPath .. "src/main.c");

local source;
local outputPath;
local condensed_args = arg[0] .. " " .. table.concat(arg, " ");

local vm = 
{
    -- file
    source = "",
    output = "",
    outputPath = "",
    globals = {},
    constants = {},
    compiledFunctions = {},
    types = {'int', 'int8', 'int16', 'int32', 'int64', 'uint', 'uint8', 'uint16', 'uint32', 'uint64', 'bool', 'float', 'double'}
};

-- vm functions
vm.functions = 
{
    module = function(path)
        local temp = require(path);
        for k,v in pairs(temp) do
            vm.compiledFunctions[k] = v;
        end
    end,
    using = function(path)
        local temp = require(path);
        for k,v in pairs(temp) do
            vm.functions[k] = v;
        end
    end,
    global = function(name, type, value)
        vm.globals[name] = global(type, value, name);
    end
}

local list = require('lib.clist')(vm.types);

-- parse the arguments

if utils.array.includes(arg, "-v") or utils.array.includes(arg, "--version") then
    print("bruter version " .. version)
    os.exit(0)
elseif utils.array.includes(arg, "--help") or utils.array.includes(arg,"-h") then
    print("Usage: bruter -c <source file> [-o <output file>] [-h] [-v] [--version] [--help]")
    print("Options:")
    print("  --help    Display this information")
    print("  -h        Display this information")
    print("  -v        Display the version")
    print("  --version Display the version")
    print("  -c        Compile the source file")
    print("  --compile Compile the source file")
    print("  -o        Output the compiled file to the specified path")
    print("  --output  Output the compiled file to the specified path")
    os.exit(0)
elseif arg[1] == nil then
    print("No source file specified")
    os.exit(1)
end

if  utils.array.includes(arg, "-c") or 
    utils.array.includes(arg, "--compile") then

    local temp = utils.table.find(arg, "-c") or 
                utils.table.find(arg, "--compile")

    temp = arg[temp + 1]
    vm.source = utils.file.load.text(temp)
end

if utils.array.includes(arg, "-o") or utils.array.includes(arg, "--output") then
    local temp = utils.table.find(arg, "-o") or utils.table.find(arg, "--output")
    vm.outputPath = arg[temp + 1]
end

function cleanSource(source)
    local nstr = utils.string.replace(source, "\n","")
    nstr = utils.string.replace(nstr, "\\n", "\n")
    nstr = utils.string.replace(nstr, " : ", ":")
    nstr = utils.string.replace(nstr, " :", ":")
    nstr = utils.string.replace(nstr, ": ", ":")
    nstr = utils.string.replace(nstr, " ;", ";")
    nstr = utils.string.replace(nstr, "; ", ";")
    return nstr
end

function parseArgs(args)
    for i = 1, #args do
        if utils.string.includes(args[i], "$") then
            local temp = utils.string.split(args[i], "$");
            local name = temp[1];
            local type = temp[2];
            if not vm.hash[name] then
                error("variable \"" .. name .. "\" not declared")
            end
            args[i] = vm.variables[name].value;
        elseif (string.byte(args[i],1) > 47 and string.byte(args[i],1) < 58) or string.byte(args[i],1) == 45 then
            args[i] = tonumber(args[i]);
        end
    end
end

vm.source = cleanSource(vm.source)

function parseSourceFile()
    local splited = utils.string.split(vm.source, ";");
    local func = "";
    for i = 1, #splited-1 do
        if(utils.string.includes(splited[i], ":")) then
            local temp = utils.string.split(splited[i], ":");
            local name = temp[1];
            local temp2 = utils.string.split(temp[2], " ");
            func = temp2[1];
            local args = parseArgs(utils.array.slice(temp2, 2, #temp2));
            vm.variables[name] = vm.functions[func](unpack(args or {}));
        else
            local temp = utils.string.split(splited[i], " ");
            local func = temp[1];
            vm.functions[func](unpack(utils.array.slice(temp, 2, #temp)));
        end
    end
end

parseSourceFile();

terra main()
    var listaint = list.int.new();
    var str = String.new();
    var tobeReplaced = String.new();
    var tobeReplaced2 = String.new();
    tobeReplaced:push(('d')[0])
    tobeReplaced:push(('e')[0])
    tobeReplaced:push(('f')[0])
    tobeReplaced2:push(('1')[0])
    tobeReplaced2:push(('2')[0])
    
    str:push(('a')[0])
    str:push(('b')[0])
    str:push(('c')[0])
    str:push(('d')[0])
    str:push(('e')[0])
    str:push(('f')[0])
    str:push(('g')[0])
    str:push(('h')[0])
    str:push(('i')[0])
    str:push(('d')[0])
    str:push(('e')[0])
    str:push(('f')[0])
    str:push(('k')[0])
    str:push(('l')[0])
    str:push(('m')[0])
    str:replace(tobeReplaced, tobeReplaced2)
    listaint:push(12312);
    listaint:push(2);
    listaint:insert(333321, 0);

    list.int.push(listaint, 4);

    listaint:set(0, 123);
    c.printf("listaint size = %s\n", str.list.array);
    c.printf("teste = %s\n", tocstr.tocstr128(condensed_args));
end

vm.compiledFunctions.main = main;

if vm.outputPath ~= "" then
    local fns = {}
    for k,v in pairs(vm.compiledFunctions) do
        fns[k] = v;
    end
    terralib.saveobj(vm.outputPath,fns, nil, nil, false);
end

--example: type$var:func arg type$arg arg;




-- compile the executable
-- terralib.saveobj("main",{main = main}, nil, nil, false);