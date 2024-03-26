local io = {}
local c = terralib.includec('stdio.h')

io.printint8 = terra(a: int8)
    c.printf("%d\n", a)
end

io.printint16 = terra(a: int16)
    c.printf("%d\n", a)
end

io.printint32 = terra(a: int32)
    c.printf("%d\n", a)
end

io.printint64 = terra(a: int64)
    c.printf("%d\n", a)
end

io.printuint8 = terra(a: uint8)
    c.printf("%d\n", a)
end

io.printuint16 = terra(a: uint16)
    c.printf("%d\n", a)
end

io.printuint32 = terra(a: uint32)
    c.printf("%d\n", a)
end

io.printuint64 = terra(a: uint64)
    c.printf("%d\n", a)
end

io.printdouble = terra(a: double)
    c.printf("%f\n", a)
end

io.printfloat = terra(a: float)
    c.printf("%f\n", a)
end

io.print = terralib.overloadedfunction("print", {io.printint8, io.printint16, io.printint32, io.printint64, io.printuint8, io.printuint16, io.printuint32, io.printuint64, io.printdouble, io.printfloat})

io.exports = {printdouble = io.printdouble, printfloat = io.printfloat, printint8 = io.printint8, printint16 = io.printint16, printint32 = io.printint32, printint64 = io.printint64, printuint8 = io.printuint8, printuint16 = io.printuint16, printuint32 = io.printuint32, printuint64 = io.printuint64}

return io