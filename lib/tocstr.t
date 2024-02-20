

local tocstr = {};

function create_tocstr(buffer_size) 
    return [[
        char buffer]] .. buffer_size .. [[[]] .. buffer_size ..  [[];

        char* tocstr]] .. buffer_size .. [[(char* b) 
        {
            for(int i = 0; i < ]] .. buffer_size ..  [[; i++)
            {
                buffer]] .. buffer_size .. [[[i] = b[i];
                if(b[i] == 0)
                {
                    break;
                }
            }
            return buffer]] .. buffer_size .. [[;
        }

    ]];
end

local sizes = { 1, 2, 4, 8, 16, 32, 64, 128, 256, 512, 1024, 2048, 4096, 8192, 16384, 32768, 65536, 131072, 262144, 524288, 1048576 };

local ccode = [[
    #include <stdlib.h>
    #include <stdio.h>
    
]]

for i = 1, #sizes do
    ccode = ccode .. create_tocstr(sizes[i]);
end

tocstr = terralib.includecstring(ccode);

return(tocstr);