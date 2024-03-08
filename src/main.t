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

-- libs
-- libs
-- libs
list = require('lib.list');
String = require('lib.string');

-- brutevm
-- brutevm
-- brutevm
br = require "br"
utils = br.variables.utils;
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
    print("bruter version " .. br.version)
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

if arg[1] == nil then
    print("No source file specified\nuse --help for help.")
    os.exit(1)
end

-- read and clean the source file
-- read and clean the source file
-- read and clean the source file
br.source = utils.file.load.text(arg[1]);

-- set the output path if specified
-- set the output path if specified
-- set the output path if specified
if utils.array.includes(arg, "-o") or utils.array.includes(arg, "--output") then
    local temp = utils.table.find(arg, "-o") or utils.table.find(arg, "--output")
    br.outputPath = arg[temp + 1]
end

-- parse the arguments
-- parse the arguments
-- parse the arguments
function parseArgs(args)
    local newargs = utils.array.clone(args);
    for i = 1, #args do
        if string.byte(args[i],1) == 36 then
            local name = utils.string.replace(args[i], "%$", '');
            newargs[i] = br.variables.recursiveget(name);
        elseif (string.byte(args[i],1) > 47 and string.byte(args[i],1) < 58) or string.byte(args[i],1) == 45 then
            newargs[i] = tonumber(args[i]);
        end
    end
    return newargs;
end

-- preprocess the source
-- preprocess the source
-- preprocess the source

function preprocess(_src)
    local result = _src .. '';
    for k, v in pairs(br.preprocessors) do
        result = v(result);
    end
    return result;
end

-- parse the source file
-- parse the source file
-- parse the source file
function parseSourceFile(src)
    src = preprocess(src);
    local splited = utils.string.split(src, ";");
    local func = "";
    for i = 1, #splited - 1 do
        local splited_args = utils.string.split(splited[i], " ");
        if(utils.string.includes(splited_args[1], ":")) then
            local name_and_func = utils.string.split(splited_args[1], ":");
            local name, func = name_and_func[1], name_and_func[2];
            local args = parseArgs(utils.array.slice(splited_args, 2, #splited_args));
            br.variables.recursiveset(name, br.variables.recursiveget(func)(unpack(args or {})));
        else
            local func = splited_args[1];
            local args = parseArgs(utils.array.slice(splited_args, 2, #splited_args));
            local _function = br.variables.recursiveget(func);
            if _function then
                print(func, utils.stringify(args))
                _function(unpack(args or {}));
            else
                print(src)
                error("function " .. func .. " not found")
            end
        end
    end
end

-- run the parser
-- run the parser
-- run the parser
parseSourceFile(br.source);

-- save the output if specified
-- save the output if specified
-- save the output if specified
if br.outputPath ~= "" then
    terralib.saveobj(br.outputPath,br.exports, nil, nil, false);
end