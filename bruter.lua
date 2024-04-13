local bruterPath = debug.getinfo(1).source:match("@?(.*/)");
--remove the "src/"
bruterPath = string.sub(bruterPath, 1, #bruterPath-4);

if package.terrapath then
    package.terrapath = package.terrapath .. bruterPath .. "?.t;" .. bruterPath .. "src/?.t;" .. bruterPath .. "src/?/?.t;"
    package.terrapath = package.terrapath .. bruterPath .. "?.lua;" .. bruterPath .. "lib/?.lua;" .. bruterPath .. "lib/?/?.lua;"
else 
    package.path = package.path .. bruterPath .. "?.t;" .. bruterPath .. "src/?.t;" .. bruterPath .. "src/?/?.t;"
    package.path = package.path .. bruterPath .. "?.lua;" .. bruterPath .. "lib/?.lua;" .. bruterPath .. "lib/?/?.lua;"
end


br = dofile(bruterPath .. "/src/br.t");

return br