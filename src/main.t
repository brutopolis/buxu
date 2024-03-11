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
utils = br.data.utils;
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
if utils.array.includes(arg, "--debug") then
    br.data.debug = true;
    local position = utils.table.find(arg, "--debug");
    print("[WARNING]: Debug mode enabled");
    table.remove(arg, position);
end

if utils.array.includes(arg, "-v") or utils.array.includes(arg, "--version") then
    print("bruter version " .. br.version)
    os.exit(0)
elseif utils.array.includes(arg, "--help") or utils.array.includes(arg,"-h") then
    print("Usage: bruter <source file> [-o <output file>] [-h] [-v] [--help] [--version] [--debug]")
    print("Options:")
    print("  --help    Display this information")
    print("  -h        Display this information")
    print("  -v        Display the version")
    print("  --version Display the version")
    print("  -o        Output the compiled file to the specified path")
    print("  --output  Output the compiled file to the specified path")
    print("  --debug   Enable debug mode")
    os.exit(0)
elseif arg[1] == nil then
    print("No source file specified, starting in REPL instead...")
    br.data.repl()
    os.exit(0)
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

-- run the parser
-- run the parser
-- run the parser
br.data.parse(br.source);

-- save the output if specified
-- save the output if specified
-- save the output if specified
if br.outputPath ~= "" then
    terralib.saveobj(br.outputPath,br.exports, nil, nil, false);
end