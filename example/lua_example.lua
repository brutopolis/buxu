dofile("../bruter.lua");

-- br is a global variable, so you can use it without return
-- local br = require("bruter"); also works(if in upper path)
-- br = require("bruter"); -- also works
-- require("bruter"); -- also works

br.repl();