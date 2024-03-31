br.math = {};

for k, v in pairs(math) do
	br.math[k] = v;
end

br["+"] = function(...)
	local args = {...};
	local result = args[1];
	for i = 2, #args do
		result = result + args[i];
	end
	return result;
end

br["-"] = function(...)
	local args = {...};
	local result = args[1];
	for i = 2, #args do
		result = result - args[i];
	end
	return result;
end

br["*"] = function(...)
	local args = {...};
	local result = args[1];
	for i = 2, #args do
		result = result * args[i];
	end
	return result;
end

br["/"] = function(a,b) return a / b; end
br["^"] = function(a,b) return a ^ b; end
br["%"] = function(a,b) return a % b; end