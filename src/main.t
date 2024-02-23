local bruterPath = debug.getinfo(1).source:match("@?(.*/)");
--remove the "src/"
bruterPath = string.sub(bruterPath, 1, #bruterPath-4);

package.terrapath = package.terrapath .. bruterPath .. "?.t;" .. bruterPath .. "src/?.t;" .. bruterPath .. "src/?/?.t;"
package.terrapath = package.terrapath .. bruterPath .. "?.lua;" .. bruterPath .. "lib/?.lua;" .. bruterPath .. "lib/?/?.lua;"

local version = "0.0.2"

local utils = require 'luatils.init'
--local tocstr = require('lib.tocstr')

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
    -- type
    types = {"int", "float", "double", "char", "string", "void", "byte"},
    variables = {},
    constants = {},
    defines = {},
    typedefs = {byte="unsigned char"},
    includes = {},
    rawtext = {},
    hash = {}
};
-- vm functions
vm.functions = 
{
    include = function(path)
        table.insert(vm.includes, "<" .. path .. ">")
    end,
    includelocal = function(path)
        table.insert(vm.includes, "\"" .. path .. "\"")
    end,
    define = function(name, ...)
        vm.defines[name] = table.concat({...}, " ")
    end,
    typedef = function(name, ...)
        vm.typedefs[name] = table.concat({...}, " ")
    end,
    raw = function(...)
        table.insert(vm.rawtext, table.concat({...}, " "))
    end,
    global = function(type, name, value)
        if vm.hash[name] then
            error("variable \"" .. name .. "\" already declared")
        end
        vm.variables[name] = {type = type, value = value}
        vm.hash[name] = {type = type}
    end,
    globalconstant = function(type, name, value)
        if vm.hash[name] then
            error("constant \"" .. name .. "\" already declared")
        end
        vm.constants[name] = {type = type, value = value}
        vm.hash[name] = {type = type}
    end,
    set = function(name, value)
        if not vm.hash[name] then
            error("variable \"" .. name .. "\" not declared")
        end
        vm.variables[name].value = value
    end,
    ["dummy"] = function(a)
        print("dummy")
        print("dummy")
        print("dummy")
    end,
}

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
    print("  -p        Print the C code to the console")
    print("  --print   Print the C code to the console")
    os.exit(0)
elseif arg[1] == nil then
    print("No source file specified")
    os.exit(1)
end

if  utils.array.includes(arg, "-c") or 
    utils.array.includes(arg, "--compile") or 
    utils.array.includes(arg, "-p") or
    utils.array.includes(arg, "--print") then

    local temp = utils.table.find(arg, "-c") or 
                utils.table.find(arg, "--compile") or
                utils.table.find(arg, "-p") or
                utils.table.find(arg, "--print")

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

function argsFindout(args)
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
function bruteparse()
    local splited = utils.string.split(vm.source, ";");
    local func = "";
    for i = 1, #splited-1 do
        if(utils.string.includes(splited[i], ":")) then
            local temp = utils.string.split(splited[i], ":");
            local name = temp[1];
            local temp2 = utils.string.split(temp[2], " ");
            func = temp2[1];
            local args = argsFindout(utils.array.slice(temp2, 2, #temp2));
            vm.variables[name] = vm.functions[func](unpack(args or {}));
        else
            local temp = utils.string.split(splited[i], " ");
            local func = temp[1];
            vm.functions[func](unpack(utils.array.slice(temp, 2, #temp)));
        end
    end
end

function writeC()
    local c = "";
    for i = 1, #vm.includes do
        c = c .. "#include " .. vm.includes[i] .. "\n";
    end
    for k, v in pairs(vm.defines) do
        c = c .. "#define " .. k .. " " .. v .. ";\n";
    end
    for k, v in pairs(vm.constants) do
        c = c .. "const " .. v.type .. " " .. k .. " = " .. v.value .. ";\n";
    end

    for k, v in pairs(vm.typedefs) do
        c = c .. "typedef " .. v .. " " .. k .. ";\n";
    end
    
    c = c .. "\nint main() \n{\n";-- opens main function
    
    for k, v in pairs(vm.variables) do
        c = c .. v.type .. (v.value and (" " .. k .. " = " .. v.value) or 'nil') .. ";\n";
    end
    for i = 1, #vm.rawtext do
        c = c .. vm.rawtext[i] .. "\n";
    end

    c = c .. "  return 0;\n}\n";-- closes main function
    
    return c;
end

bruteparse();
local ccode = writeC();

utils.file.save.text("temp.c", ccode);
local loadedcode = terralib.includec("temp.c");
os.execute("rm temp.c")

if vm.outputPath ~= "" then
    if utils.array.includes(arg, "-p") or utils.array.includes(arg, "--print") then
        print(ccode);
    elseif utils.string.includes(vm.outputPath, ".c") then
        utils.file.save.text(vm.outputPath, ccode);
    else
        --functions.table
        terralib.saveobj(vm.outputPath,{main=loadedcode.main}, nil, nil, false);
    end
end

--example: type$var:func arg type$arg arg;


terra main()
    --c.printf("teste = %s\n", tocstr128(condensed_args));
end

-- compile the executable
-- terralib.saveobj("main",{main = main}, nil, nil, false);