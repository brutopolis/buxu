local math = {}
local typenames = {"int8", "int16", "int32", "int64", "uint8", "uint16", "uint32", "uint64", "double", "float"};
local types = {int8, int16, int32, int64, uint8, uint16, uint32, uint64, double, float};
math.exports = {}

for i = 1, #typenames do
    math["add" .. typenames[i]] = terra(a:types[i], b:types[i])
        return a + b;
    end
    math["sub" .. typenames[i]] = terra(a:types[i], b:types[i])
        return a - b;
    end
    math["mul" .. typenames[i]] = terra(a:types[i], b:types[i])
        return a * b;
    end
    math["div" .. typenames[i]] = terra(a:types[i], b:types[i])
        return a / b;
    end
    if typenames[i] ~= "double" and typenames[i] ~= "float" then
        math["mod" .. typenames[i]] = terra(a:types[i], b:types[i])
            return a % b;
        end
    end
    math["pow" .. typenames[i]] = terra(a:types[i], b:types[i])
        var result = a;
        for i:int = 1, b - 1 do
            result = result * a;
        end
        return result;
    end
    math["eq" .. typenames[i]] = terra(a:types[i], b:types[i])
        return a == b;
    end
    if typenames[i]:sub(1, 4) ~= "uint" then
        math["neg" .. typenames[i]] = terra(a:types[i], b:types[i])
            return a ~= b;
        end
    end
    math["sin" .. typenames[i]] = terra(a:types[i])
        --manual sin calculus
        var result = a;
        var n = 1;
        var fact = 1;
        var pow = a;
        for i = 1, 10 do
            fact = fact * (n + 1) * (n + 2);
            pow = pow * a * a;
            if i % 2 == 0 then
                result = result - pow / fact;
            else
                result = result + pow / fact;
            end
            n = n + 2;
        end
        return result;
    end
    local sinfunc = math["sin" .. typenames[i]]
    math["cos" .. typenames[i]] = terra(a:types[i])
        --manual cos calculus
        var result = 1;
        var n = 0;
        var fact = 1;
        var pow = 1;
        for i = 1, 10 do
            fact = fact * (n + 1) * (n + 2);
            pow = pow * a * a;
            if i % 2 == 0 then
                result = result - pow / fact;
            else
                result = result + pow / fact;
            end
            n = n + 2;
        end
        return result;
    end
    local cosfunc = math["cos" .. typenames[i]]
    math["tan" .. typenames[i]] = terra(a:types[i])
        return sinfunc(a) / cosfunc(a);
    end
    math["asin" .. typenames[i]] = terra(a:types[i])
        --manual asin calculus
        var result = a;
        var n = 1;
        var fact = 1;
        var pow = a;
        for i = 1, 10 do
            fact = fact * (n + 1) * (n + 2);
            pow = pow * a * a;
            if i % 2 == 0 then
                result = result - pow / fact;
            else
                result = result + pow / fact;
            end
            n = n + 2;
        end
        return result;
    end
    math["acos" .. typenames[i]] = terra(a:types[i])
        --manual acos calculus
        var result = 1;
        var n = 0;
        var fact = 1;
        var pow = 1;
        for i = 1, 10 do
            fact = fact * (n + 1) * (n + 2);
            pow = pow * a * a;
            if i % 2 == 0 then
                result = result - pow / fact;
            else
                result = result + pow / fact;
            end
            n = n + 2;
        end
        return result;
    end
    math["atan" .. typenames[i]] = terra(a:types[i])
        --manual atan calculus
        var result = a;
        var n = 1;
        var fact = 1;
        var pow = a;
        for i = 1, 10 do
            fact = fact * (n + 1) * (n + 2);
            pow = pow * a * a;
            if i % 2 == 0 then
                result = result - pow / fact;
            else
                result = result + pow / fact;
            end
            n = n + 2;
        end
        return result;
    end
    local atanfunc = math["atan" .. typenames[i]]
    math["atan2" .. typenames[i]] = terra(a:types[i], b:types[i])
        return atanfunc(a / b);
    end
    math["exp" .. typenames[i]] = terra(a:types[i])
        --manual exp calculus
        var result = 1;
        var fact = 1;
        var pow = 1;
        for i = 1, 10 do
            fact = fact * i;
            pow = pow * a;
            result = result + pow / fact;
        end
        return result;
    end
    math["log" .. typenames[i]] = terra(a:types[i])
        --manual log calculus
        var result = 0;
        var pow = a;
        for i = 1, 10 do
            result = result + pow / i;
            pow = pow * a;
        end
        return result;
    end
    local logfunc = math["log" .. typenames[i]]
    math["log10" .. typenames[i]] = terra(a:types[i])
        return logfunc(a) / logfunc(10);
    end
    math["sqrt" .. typenames[i]] = terra(a:types[i])
        --manual sqrt calculus
        var result = a;
        for i = 1, 10 do
            result = (result + a / result) / 2;
        end
        return result;
    end
    math["ceil" .. typenames[i]] = terra(a:types[i])
        return a + 1;
    end
    local ceilfunc = math["ceil" .. typenames[i]]
    math["floor" .. typenames[i]] = terra(a:types[i])
        return a - 1;
    end
    local floorfunc = math["floor" .. typenames[i]]
    math["abs" .. typenames[i]] = terra(a:types[i])
        if a < 0 then
            return -a;
        else
            return a;
        end
    end
    math["min" .. typenames[i]] = terra(a:types[i], b:types[i])
        if a < b then
            return a;
        else
            return b;
        end
    end
    math["max" .. typenames[i]] = terra(a:types[i], b:types[i])
        if a > b then
            return a;
        else
            return b;
        end
    end
    math["round" .. typenames[i]] = terra(a:types[i])
        if a - floorfunc(a) < 0.5 then
            return floorfunc(a);
        else
            return ceilfunc(a);
        end
    end
    math["sign" .. typenames[i]] = terra(a:types[i])
        if a < 0 then
            return -1;
        elseif a > 0 then
            return 1;
        else
            return 0;
        end
    end
    math["clamp" .. typenames[i]] = terra(a:types[i], min:types[i], max:types[i])
        if a < min then
            return min;
        elseif a > max then
            return max;
        else
            return a;
        end
    end
    math["lerp" .. typenames[i]] = terra(a:types[i], b:types[i], t:types[i])
        return a + (b - a) * t;
    end
    math["smoothstep" .. typenames[i]] = terra(a:types[i])
        return a * a * (3 - 2 * a);
    end
    math["smootherstep" .. typenames[i]] = terra(a:types[i])
        return a * a * a * (a * (a * 6 - 15) + 10);
    end
end

--math.exports.addint8 = math.addint8
for i = 1, #typenames do
    math.exports["add" .. typenames[i]] = math["add" .. typenames[i]]
    math.exports["sub" .. typenames[i]] = math["sub" .. typenames[i]]
    math.exports["mul" .. typenames[i]] = math["mul" .. typenames[i]]
    math.exports["div" .. typenames[i]] = math["div" .. typenames[i]]
    math.exports["mod" .. typenames[i]] = math["mod" .. typenames[i]]
    math.exports["pow" .. typenames[i]] = math["pow" .. typenames[i]]
    math.exports["eq" .. typenames[i]] = math["eq" .. typenames[i]]
    math.exports["neg" .. typenames[i]] = math["neg" .. typenames[i]]
    math.exports["sin" .. typenames[i]] = math["sin" .. typenames[i]]
    math.exports["cos" .. typenames[i]] = math["cos" .. typenames[i]]
    math.exports["tan" .. typenames[i]] = math["tan" .. typenames[i]]
    math.exports["asin" .. typenames[i]] = math["asin" .. typenames[i]]
    math.exports["acos" .. typenames[i]] = math["acos" .. typenames[i]]
    math.exports["atan" .. typenames[i]] = math["atan" .. typenames[i]]
    math.exports["atan2" .. typenames[i]] = math["atan2" .. typenames[i]]
    math.exports["exp" .. typenames[i]] = math["exp" .. typenames[i]]
    math.exports["log" .. typenames[i]] = math["log" .. typenames[i]]
    math.exports["log10" .. typenames[i]] = math["log10" .. typenames[i]]
    math.exports["sqrt" .. typenames[i]] = math["sqrt" .. typenames[i]]
    math.exports["ceil" .. typenames[i]] = math["ceil" .. typenames[i]]
    math.exports["floor" .. typenames[i]] = math["floor" .. typenames[i]]
    math.exports["abs" .. typenames[i]] = math["abs" .. typenames[i]]
    math.exports["min" .. typenames[i]] = math["min" .. typenames[i]]
    math.exports["max" .. typenames[i]] = math["max" .. typenames[i]]
    math.exports["round" .. typenames[i]] = math["round" .. typenames[i]]
    math.exports["sign" .. typenames[i]] = math["sign" .. typenames[i]]
    math.exports["clamp" .. typenames[i]] = math["clamp" .. typenames[i]]
    math.exports["lerp" .. typenames[i]] = math["lerp" .. typenames[i]]
    math.exports["smoothstep" .. typenames[i]] = math["smoothstep" .. typenames[i]]
    math.exports["smootherstep" .. typenames[i]] = math["smootherstep" .. typenames[i]]
end

--math.pi = 3.14159265358979323846
--math.e = 2.7182818284590452354

return math