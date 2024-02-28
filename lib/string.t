char = int8
local list = require("lib.clist")(
    {
        char =
        {
            ctype="char", terratype = int8
        },
        String =
        {
            ctype="struct{char* array;size_t size;}"
        }
    }
)
local String = list.char.terratype
local StringList = list.String.terratype

terra String:replace(target: String, replacement: String)
    var result:String = String.new();
    var targetSize = target:size();
    var sourceSize = self:size();

    var i = 0;
    while i < sourceSize do
        var match = true;
        for j = 0, targetSize - 1 do
            if i + j >= sourceSize or self:get(i + j) ~= target:get(j) then
                match = false;
                break;
            end
        end
        if match then
            result:concat(replacement);
            i = i + targetSize;
        else
            result:push(self:get(i));
            i = i + 1;
        end
    end

    self.list = result.list;
end

terra String:split(delimiter: char):StringList
    var result:StringList = StringList.new();
    var temp:String = String.new();
    var size = self:size();
    for i = 0, size - 1 do
        if self:get(i) == delimiter then
            result:push(temp);
            temp = String.new();
        else
            temp:push(self:get(i));
        end
    end
    result:push(temp);
    return result;
end

return String