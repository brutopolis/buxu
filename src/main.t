-- gets the bruter path
-- gets the bruter path
-- gets the bruter path
bruterPath = debug.getinfo(1).source:match("@?(.*/)");
--remove the "src/"
bruterPath = string.sub(bruterPath, 1, #bruterPath-4);

-- add the path to the terralib
-- add the path to the terralib
-- add the path to the terralib
package.terrapath = package.terrapath .. bruterPath .. "?.t;" .. bruterPath .. "src/?.t;" .. bruterPath .. "src/?/?.t;"
package.terrapath = package.terrapath .. bruterPath .. "?.lua;" .. bruterPath .. "lib/?.lua;" .. bruterPath .. "lib/?/?.lua;"

-- version
-- version
-- version
version = "0.0.7a"

-- libs
-- libs
-- libs
utils = require 'luatils.init'
list = require('lib.list');
String = require('lib.string');

-- brutevm
-- brutevm
-- brutevm
br = require "src.br"

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

-- parse the compiler/interpreter arguments
-- parse the compiler/interpreter arguments
-- parse the compiler/interpreter arguments
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

-- source cleaner
-- source cleaner
-- source cleaner
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

-- read and clean the source file
-- read and clean the source file
-- read and clean the source file
br.source = utils.file.load.text(arg[1]);
br.source = cleanSource(br.source)

-- set the output path if specified
-- set the output path if specified
-- set the output path if specified
if utils.array.includes(arg, "-o") or utils.array.includes(arg, "--output") then
    local temp = utils.table.find(arg, "-o") or utils.table.find(arg, "--output")
    br.outputPath = arg[temp + 1]
end

-- setter
-- setter
-- setter
function recursiveset(argname, value)
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
function recursiveget(argname)
    if utils.string.includes(argname, ".") then
        local result = terralib.loadstring("return br.variables." .. argname)();
        return result;
    else
        return br.variables[argname];
    end
end

-- parse the arguments
-- parse the arguments
-- parse the arguments
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

-- parse the source file
-- parse the source file
-- parse the source file
function parseSourceFile()
    local splited = utils.string.split(br.source, ";");
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
                print(br.source)
                error("function " .. func .. " not found")
            end
        end
    end
end

-- run the parser
-- run the parser
-- run the parser
parseSourceFile();

-- save the output if specified
-- save the output if specified
-- save the output if specified
if br.outputPath ~= "" then
    terralib.saveobj(br.outputPath,br.exports, nil, nil, false);
end