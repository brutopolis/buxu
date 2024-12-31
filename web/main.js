async function main() 
{
    Bruter = await Bruter; // Aguarda o carregamento

    const vm = Bruter.new();
    console.log("vm index:", vm.index);

    const result = vm.eval('return 45');

    // you might want to use this code to test instead
    /* 
        hash.set "_size" 100000;
        hash.set "___index" 0;
        repeat _size "incr ___index;print ___index";
        return 0;
    */
   
    console.log("result:", result);

    vm.destroy();
    console.log("vm destroyed");
}
main();