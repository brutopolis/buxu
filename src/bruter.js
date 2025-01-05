// bruter.js

let bruter_loader = async function() 
{
    const BruterAPI = {};
    
    let _module = await _Bruter();
    
    BruterAPI.destroy = (vm_index) => {
        _module._wasm_destroy_vm(vm_index);
    };
    
    BruterAPI.eval = (vm_index, code) => {
        const lengthBytes = _module.lengthBytesUTF8(code) + 1;
        const ptr = _module._malloc(lengthBytes);
        _module.stringToUTF8(code, ptr, lengthBytes);
        let result = _module._wasm_eval(vm_index, ptr);
        _module._free(ptr);

        result = _module.UTF8ToString(result);
        
        if (result === "null")
        {
            console.log("result is null");
            return null;
        }
        else if(result[0] == '\x01') // number
        {
            result = parseFloat(result.substring(1));
        }
        else if(result[0] == '\x02') // string
        {
            result = result.substring(1);
        }
        else // integer/pointer/any 
        {
            result = parseInt(result.substring(1));
        }

        return result;
    };
    
    BruterAPI.new = (index) => {
        let _vm = { index: index || _module._wasm_new_vm() };
        _vm.eval = (code) => BruterAPI.eval(_vm.index, code);
        _vm.destroy = () => BruterAPI.destroy(_vm.index);
        return _vm;
    };

    BruterAPI.module = _module;

    return BruterAPI;
}

window.Bruter = bruter_loader();