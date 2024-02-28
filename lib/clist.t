local starter = [[#include <stdio.h>
#include <stdlib.h>    
#include <stdbool.h>
]]

local c = terralib.includecstring(starter)

function init(type)


    local listType = struct{
        array:&type;
        size:int;
    }

    listType.methods.new = terra()
        var list:listType;
        list.array = [&type](c.malloc(0));
        list.size = 0;
        return list;
    end

    terra listType:push(value:type)
        self.size = self.size + 1;
        self.array = [&type](c.realloc(self.array, self.size * terralib.sizeof(type)));
        self.array[self.size - 1] = value;
    end

    terra listType:insert(value:type, index:int)
        self.size = self.size + 1;
        self.array = [&type](c.realloc(self.array, self.size * terralib.sizeof(type)));
        for i = self.size - 1, index + 1, -1 do
            self.array[i] = self.array[i - 1];
        end
        self.array[index] = value;
    end

    terra listType:set(value:type, index:int)
        self.array[index] = value;
    end

    terra listType:get(index:int)
        return self.array[index];
    end

    terra listType:size()
        return self.size;
    end

    terra listType:pop()
        self.size = self.size - 1;
        self.array = [&type](c.realloc(self.array, self.size * terralib.sizeof(type)));
    end

    terra listType:concat(other:listType)
        self.size = self.size + other.size;
        self.array = [&type](c.realloc(self.array, self.size * terralib.sizeof(type)));
        for i = 0, other.size - 1 do
            self.array[self.size - other.size + i] = other.array[i];
        end
    end

    terra listType:remove(index:int)
        for i = index, self.size - 2 do
            self.array[i] = self.array[i + 1];
        end
        self.size = self.size - 1;
        self.array = [&type](c.realloc(self.array, self.size * terralib.sizeof(type)));
    end

    terra listType:free()
        c.free(self.array);
    end

    return listType
end

--the same in pure terra



return init