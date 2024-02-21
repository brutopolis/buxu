package.terrapath = package.terrapath .. ";lib/?.t;lib/?/?.t;lib/?/?/?.t;"
package.terrapath = package.terrapath .. ";lib/?.lua;lib/?/?.lua;lib/?/?/?.lua;"

local version = "0.0.1"

local utils = require 'luatils.init'
--local tocstr = require('lib.tocstr')

-- load the main.c
local c = terralib.includec("src/main.c");

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
    defines = {byte="unsigned char"},
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
    raw = function(...)
        table.insert(vm.rawtext, table.concat({...}, " "))
    end,
    declare = function(type, name, value)
        if vm.hash[name] then
            error("variable \"" .. name .. "\" already declared")
        end
        vm.variables[name] = {type = type, value = value}
        vm.hash[name] = {type = type}
    end,
    constant = function(type, name, value)
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
    os.exit(0)
elseif arg[1] == nil then
    print("No source file specified")
    os.exit(1)
end

if utils.array.includes(arg, "-c") or utils.array.includes(arg, "--compile") then
    local temp = utils.table.find(arg, "-c") or utils.table.find(arg, "--compile")
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

vm.source = cleanSource(vm.source)
function bruteparse()
    local splited = utils.string.split(vm.source, ";");
    local func = "";
    print("splited: " .. #splited)
    for i = 1, #splited-1 do
        if(utils.string.includes(splited[i], ":")) then
            if utils.string.includes(splited[i],"$") then
                local temp = utils.string.split(splited[i], " ");
                local temp2 = utils.string.split(temp[1], ":");
                local temp3 = utils.string.split(temp2[1], "%$");
                local type = temp3[1];
                local varname = temp3[2];
                print("type: " .. temp3[2])
                vm.variables[varname] = vm.functions[temp2[2]](utils.array.slice(temp, 3, #temp));
            else
                local temp = utils.string.split(splited[i], " ");
                local temp2 = utils.string.split(temp[1], ":");
                if vm.hash(temp2[1]) == nil then 
                    error("\"" .. temp2[1] .. "\" not declared, in line " .. i)
                else
                    local type = vm.hash(temp2[1]);
                    local varname = temp2[1];
                    vm[vm.contants[varname] or vm.variables[varname]] = vm.functions[temp2[2]](utils.array.slice(temp, 2, #temp));
                end
                func = temp2[2];
                vm.functions[func](utils.array.slice(temp, 2, #temp));


            end
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
        c = c .. "#define " .. v .. " " .. k .. ";\n";
    end
    for k, v in pairs(vm.constants) do
        c = c .. "const " .. v.type .. " " .. k .. " = " .. v.value .. ";\n";
    end
    for k, v in pairs(vm.variables) do
        c = c .. v.type .. " " .. k .. " = " .. v.value .. ";\n";
    end
    for i = 1, #vm.rawtext do
        c = c .. vm.rawtext[i] .. "\n";
    end
    return c;
end

bruteparse();
local c = writeC();
print(c);--print c file example
utils.file.save.text("main.c", c);--saving c file example

--example: type$var:func arg type$arg arg;


terra main()
    --c.printf("teste = %s\n", tocstr128(condensed_args));
end

-- compile the executable
-- terralib.saveobj("main",{main = main}, nil, nil, false);