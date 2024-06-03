br.promise = {};
br.promise.new = function(func,...)
    local promise = {};
    promise.data = br.vm.effil.table {...}
    promise.thread = br.vm.effil.thread(func)(...);
    promise.wait = function()
        return promise.thread:wait();
    end
    return promise;
end
br.promise.all = function(promises)
    local done = false;
    local results = {};
    while not done do
        done = true;
        for i,v in pairs(promises) do
            if v and v.thread:status() ~= "running" then
                results[i] = v.thread:get() or nil;
                promises[i] = nil;
            else
                done = false;
            end
        end
    end
    return results;
end