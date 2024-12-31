
Bruter = new Promise((resolve, reject) => {
    Module.onRuntimeInitialized = () => {
        const BruterAPI = {};
        
        BruterAPI.destroy = (vm_index) => {
            Module._wasm_destroy_vm(vm_index);
        };
        
        BruterAPI.eval = (vm_index, code) => {
            const lengthBytes = Module.lengthBytesUTF8(code) + 1;
            const ptr = Module._malloc(lengthBytes);
            Module.stringToUTF8(code, ptr, lengthBytes);
            let result = Module._wasm_eval(vm_index, ptr);
            Module._free(ptr);

            result = Module.UTF8ToString(result);
            if (result === "null")
            {
                console.log("result is null");
                return null;
            }
            else if(result[0] == 1) // number
            {
                result = parseFloat(result.substring(1));
            }
            else if(result[0] == 2) // string
            {
                result = result.substring(1);
            }
            else // integer/pointer/any 
            {
                result = parseInt(result.substring(1));
            }

            return result;
        };
        
        BruterAPI.new = () => {
            let _vm = { index: Module._wasm_new_vm() };
            _vm.eval = (code) => BruterAPI.eval(_vm.index, code);
            _vm.destroy = () => BruterAPI.destroy(_vm.index);
            return _vm;
        };
        
        resolve(BruterAPI);
    };
    
    Module.onAbort = () => {
        reject("could not load bruter wasm");
    };
});

window.Bruter = Bruter;