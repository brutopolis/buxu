function clean(code) {
    return code
        .replace(/\/\/.*$/gm, "") // remove comentários
        .replace(/\s+/g, " ")     // colapsa espaços
        .trim();
}

function unescapeString(str) {
    return str
        .replace(/\\n/g, "\n")
        .replace(/\\t/g, "\t")
        .replace(/\\r/g, "\r")
        .replace(/\\"/g, '"')
        .replace(/\\\\/g, '\\');
}

function parseCharLiteral(charLiteral) {
    if (charLiteral.length === 1) {
        return charLiteral.charCodeAt(0);
    }
    const ch = charLiteral[1];
    switch (ch) {
        case 'n': return '\n'.charCodeAt(0);
        case 't': return '\t'.charCodeAt(0);
        case 'r': return '\r'.charCodeAt(0);
        case '\'': return '\''.charCodeAt(0);
        case '\\': return '\\'.charCodeAt(0);
        default: return ch.charCodeAt(0);
    }
}

function parseExpression(expr) {
    let index = 0;

    function skipWhitespace() {
        while (expr[index] === ' ') index++;
    }

    function parseToken() {
        skipWhitespace();

        if (expr[index] === '"') {
            index++;
            let str = "";
            while (index < expr.length) {
                if (expr[index] === '\\') {
                    index++;
                    if (index >= expr.length) break;
                    let ch = expr[index++];
                    switch (ch) {
                        case 'n': str += '\n'; break;
                        case 't': str += '\t'; break;
                        case 'r': str += '\r'; break;
                        case '"': str += '"'; break;
                        case '\\': str += '\\'; break;
                        default: str += ch; break;
                    }
                } else if (expr[index] === '"') {
                    index++;
                    break;
                } else {
                    str += expr[index++];
                }
            }
            return `{${str}}`;
        }

        if (expr[index] === "'") {
            index++;
            let char = '';
            if (expr[index] === '\\') {
                index++;
                char = '\\' + expr[index++];
            } else {
                char = expr[index++];
            }
            if (expr[index] === "'") {
                index++;
                return parseCharLiteral(char).toString();
            }
        }

        // array access
        let start = index;
        while (index < expr.length && /[\w@]/.test(expr[index])) index++;
        let name = expr.slice(start, index);
        skipWhitespace();

        if (expr[index] === '[') {
            index++; // skip first [
            let firstIndex = parseExpr();
            skipWhitespace();
            if (expr[index] === ']') index++;

            skipWhitespace();
            if (expr[index] === '[') {
                index++; // skip second [
                let secondIndex = parseExpr();
                skipWhitespace();
                if (expr[index] === ']') index++;
                return `(bit.get ${name} ${firstIndex} ${secondIndex})`;
            }

            return `(byte.get ${name} ${firstIndex})`;
        }

        return name;
    }

    function parseArguments() {
        let args = [];
        while (index < expr.length) {
            skipWhitespace();
            if (expr[index] === ')') {
                index++;
                break;
            }
            args.push(parseExpr());
            skipWhitespace();
            if (expr[index] === ',') index++;
        }
        return args;
    }

    function parseExpr() {
        skipWhitespace();

        if (expr[index] === '"' || expr[index] === "'") {
            return parseToken();
        }

        if (/[\w@]/.test(expr[index])) {
            let name = parseToken();
            skipWhitespace();
            if (expr[index] === '(') {
                index++;
                let args = parseArguments();
                return `(${name} ${args.join(' ')})`;
            } else {
                return name;
            }
        }

        if (/\d/.test(expr[index])) {
            let start = index;
            while (index < expr.length && /\d/.test(expr[index])) index++;
            return expr.slice(start, index);
        }

        return "";
    }

    return parseExpr();
}

function compile_declaration(line, output) 
{
    // bit set: name[index1][index2] = value
    let bitSetMatch = line.match(/^(\w+)\[(.+?)\]\[(.+?)\]\s*=\s*(.+)$/);
    if (bitSetMatch) {
        let [, name, i1, i2, valueExpr] = bitSetMatch;
        let bit1 = parseExpression(i1);
        let bit2 = parseExpression(i2);
        let value = parseExpression(valueExpr);
        output.push(`ignore (bit.set ${bit1} ${bit2} ${value});`);
        return true;
    }
    
    // array assignment: name[index] = value
    let arraySetMatch = line.match(/^(\w+)\[(.+?)\]\s*=\s*(.+)$/);
    if (arraySetMatch) {
        let [, array, indexExpr, valueExpr] = arraySetMatch;
        let index = parseExpression(indexExpr);
        let value = parseExpression(valueExpr);
        output.push(`ignore (byte.set ${array} ${index} ${value});`);
        return true;
    }

    // variable = something
    let assignMatch = line.match(/^([\w@]+)\s*=\s*(.+)$/);
    if (assignMatch) {
        let [, varname, expr] = assignMatch;

        let strLiteral = expr.match(/^"((?:\\.|[^"])*)"?$/);
        if (strLiteral) {
            let raw = strLiteral[1];
            let unescaped = unescapeString(raw);
            output.push(`ignore {${unescaped}} @${varname};`);
            return true;
        }

        let charLiteral = expr.match(/^'(\\?.)'$/);
        if (charLiteral) {
            let ch = charLiteral[1];
            let code = parseCharLiteral(ch);
            output.push(`ignore ${code} @${varname};`);
            return true;
        }

        let parsed = parseExpression(expr);
        output.push(`ignore ${parsed} @${varname};`);
        return true;
    }

    // function call
    let isFunctionCall = line.match(/^\w+\s*\(/);
    if (isFunctionCall) {
        let parsed = parseExpression(line);
        output.push(`ignore ${parsed};`);
        return true;
    }

    let directAssign = line.match(/^([\d]+)\s*@([\w@]+)$/);
    if (directAssign) {
        let [, val, name] = directAssign;
        output.push(`${val} @${name};`);
        return true;
    }

    return false;
}

function compile_return(line, output) {
    let match = line.match(/^return\s+(.+);?$/);
    if (match) {
        let parsed = parseExpression(match[1]);
        output.push(`return ${parsed};`);
        return true;
    }
    return false;
}

function transpile(code) {
    const output = [
        "load {std};",
        "load {io};"
    ];

    code = clean(code);
    for (let line of code.split(/;\s*/)) {
        line = line.trim();
        if (!line) continue;

        let ok = compile_declaration(line, output) || compile_return(line, output);
        if (!ok) {
            output.push(`// não reconhecido: ${line}`);
        }
    }

    return output.join("\n");
}

// execução direta
if (require.main === module) {
    const fs = require("fs");
    const file = process.argv[2];
    if (!file) {
        console.error("uso: node ibr.js entrada.c");
        process.exit(1);
    }
    const code = fs.readFileSync(file, "utf-8");
    console.log(transpile(code));
}

module.exports = { transpile };
