var vm;

async function main() 
{
    Bruter = await Bruter; // Aguarda o carregamento

    vm = Bruter.new();
    console.log("vm index:", vm.index);

    const result = vm.eval(`return 45`);
   
    console.log("result:", result);

    console.log("vm destroyed");
}
main();