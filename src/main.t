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

-- brutevm
-- brutevm
-- brutevm
br = require "br"

-- parse the compiler/interpreter arguments
-- parse the compiler/interpreter arguments
-- parse the compiler/interpreter arguments
if br.utils.array.includes(arg, "--debug") then
    br.debug = true;
    local position = br.utils.table.find(arg, "--debug");
    print("[WARNING]: Debug mode enabled");
    table.remove(arg, position);
end

-- set the output path if specified
-- set the output path if specified
-- set the output path if specified
if br.utils.array.includes(arg, "-o") or br.utils.array.includes(arg, "--output") then
    local temp = br.utils.table.find(arg, "-o") or br.utils.table.find(arg, "--output")
    br.outputpath = arg[temp + 1]
    table.remove(arg, temp)
    table.remove(arg, temp)
end

-- check for help and version flags
-- check for help and version flags
-- check for help and version flags
if br.utils.array.includes(arg, "-v") or br.utils.array.includes(arg, "--version") then
    print("bruter version " .. br.version)
    os.exit(0)
elseif br.utils.array.includes(arg, "--help") or br.utils.array.includes(arg,"-h") then
    print("Usage: bruter <source file> [-o <output file>] [-h] [-v] [--help] [--version] [--debug]")
    print("Options:")
    print("  --help    Display this information")
    print("  -h        Display this information")
    print("  --version Display the version")
    print("  -v        Display the version")
    print("  --output  Output the compiled file to the specified path")
    print("  -o        Output the compiled file to the specified path")
    print("  --debug   Enable debug mode")
    os.exit(0)
elseif arg[1] == nil then
    print("No source file specified, starting in REPL instead...")
    br.repl()
    os.exit(0)
end

-- read and clean the source file
-- read and clean the source file
-- read and clean the source file
br.source = br.utils.file.load.text(arg[1]);

-- run the parser
-- run the parser
-- run the parser
br.parse(br.source);

-- save the output if specified
-- save the output if specified
-- save the output if specified
if br.outputpath ~= "" then
    terralib.saveobj(br.outputpath, br.exports, nil, nil, true);
end