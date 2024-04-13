local br = dofile("../bruter.lua"); -- using dofile because using require() would require the path to be in package.path

-- local br = require("bruter"); also works
br.vm.debug = 1;
br.bruter.include("../example/roguelike.br");
br.repl();