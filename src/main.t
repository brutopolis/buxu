package.terrapath = package.terrapath .. ";lib/?.t;lib/?/?.t;lib/?/?/?.t;"
package.terrapath = package.terrapath .. ";lib/?.lua;lib/?/?.lua;lib/?/?/?.lua;"

local utils = require 'luatils.init'
local tocstr = require 'tocstr'

-- load the main.c
local c = terralib.includec("src/main.c");

local source;
local condensed_args = arg[0] .. " " .. table.concat(arg, " ")

print("Condensed args: " .. condensed_args)

if utils.array.includes(arg, "--help") then
    print("Usage: terralib [options] <source file>")
    print("Options:")
    print("  --help  Display this information")
    --os.exit(0)
elseif utils.array.includes(arg, "-c") then
    local temp = utils.table.find(arg, "-c")
    temp = arg[source + 1]
    source = utils.file.load.text(temp)
end

terra main()
    c.printf("teste = %s\n", tocstr.tocstr128(condensed_args));
end

-- compile the executable
terralib.saveobj("main",{main = main}, nil, nil, false);