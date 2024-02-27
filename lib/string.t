char = int8
local list = require("lib.clist")({"char"})
local String = list.char.terratype

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

return String