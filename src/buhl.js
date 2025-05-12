// 100% ai generated code, so we can see how easy buxu is to be transpiled to
// this is intended to be a minimalistic typeless c-like language that transpiles to buxu
// no operators and so, almost everything is a function call
// written in js because thats what ai is usually best at

// example syntax:
/* 
    a = 123;
    b = 456;
    c = i+(i*(5, i/(b, i-(999, 99))), b);
    i+(a, c);
    str = "hello world";
    str2 = "hello w\"orld2";
    char_a = ' ';
    print((get(0))(str));
    i+(1, str[a]);
    [index][byte][bit] = 0;
    [index][byte] = 123;
    [index] = 456;
    a = [index];
    a = [index][byte];
    a = [index][byte][bit];
    str2[0] = 105;
    str2[a][b] = 1;
    [index](a, b, c);
    a = [index](a, b, c);
    b = [index]([index2](a, b, c), d, e);
    return a;
*/// Manus: modified_parser_v5.js
// Starting from modified_parser_v4.js and adding control structures.
function clean(code) {
    return code
        .replace(/\/\/.*$/gm, "") // remove comentários
        .replace(/\s+/g, " ")     // colapsa espaços
        .trim();
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
        case "'": return "'".charCodeAt(0);
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

        // Modified: Only support "" for strings
        if (expr[index] === '"') {
            const openingChar = '"';
            const closingChar = '"';
            index++; // consume opening quote
            let str = "";
            while (index < expr.length) {
                if (expr[index] === '\\') {
                    index++;
                    if (index >= expr.length) break; // Unterminated string
                    let esc_ch = expr[index++];
                    switch (esc_ch) {
                        case 'n': str += '\n'; break;
                        case 't': str += '\t'; break;
                        case 'r': str += '\r'; break;
                        case '\\': str += '\\'; break;
                        case '"': str += '"'; break; // Escape for double quote
                        default: str += esc_ch; break;
                    }
                } else if (expr[index] === closingChar) {
                    index++; // consume closing quote
                    break;
                } else {
                    str += expr[index++];
                }
            }
            return `{${str}}`; // Keep Buxu-style output for parsed string content
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
            // else: error, malformed char literal
        }

        if (expr[index] === '`') {
            index++; // consume opening backtick
            let buxuCode = "";
            while (index < expr.length) {
                if (expr[index] === '\\') {
                    index++;
                    if (index >= expr.length) break; // Unterminated Buxu inline
                    let esc_ch = expr[index++];
                    if (esc_ch === '`') {
                        buxuCode += '`';
                    } else {
                        buxuCode += '\\' + esc_ch;
                    }
                } else if (expr[index] === '`') {
                    index++; // consume closing backtick
                    break;
                } else {
                    buxuCode += expr[index++];
                }
            }
            return buxuCode; // Return raw Buxu code
        }

        let start = index;
        while (index < expr.length && /[\w@.+\-*\/%]/.test(expr[index])) {
            index++;
        }
        let name = expr.slice(start, index);
        skipWhitespace();

        if (expr[index] === '[') {
            index++;
            let firstIndex = parseExpr(); // Recursive call, ensure it returns a string for concatenation
            skipWhitespace();
            if (expr[index] === ']') index++;

            skipWhitespace();
            if (expr[index] === '[') {
                index++;
                let secondIndex = parseExpr(); // Recursive call
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
            args.push(parseExpr()); // Recursive call
            skipWhitespace();
            if (expr[index] === ',') index++;
        }
        return args;
    }

    function parseExpr() {
        skipWhitespace();
        // Tratamento adicionado para '{'
        if (expr[index] === '{') {
            index++;
            while (index < expr.length && expr[index] !== '}') {
                index++;
            }
            if (index < expr.length && expr[index] === '}') {
                index++;
            }
            return "{}";
        }
        if (expr[index] === '[') {
            index++;
            let innerExpr = parseExpr();
            skipWhitespace();
            if (expr[index] === ']') index++;
            let node = `(get ${innerExpr})`;
            skipWhitespace();
            while (expr[index] === '(') {
                index++;
                let args = parseArguments();
                node = `(${node} ${args.join(' ')})`;
                skipWhitespace();
            }
            return node;
        }
        if (expr[index] === '(') {
            index++;
            let node = parseExpr();
            skipWhitespace();
            if (expr[index] === ')') index++;
            skipWhitespace();
            while (expr[index] === '(') {
                index++;
                let args = parseArguments();
                node = `(${node} ${args.join(' ')})`;
                skipWhitespace();
            }
            return node;
        }
        if (expr[index] === '"' || expr[index] === "'" || expr[index] === '`') {
            return parseToken();
        }
        if (/\d/.test(expr[index])) {
            let start = index;
            while (index < expr.length && /[\d]/.test(expr[index])) {
                index++;
            }
            return expr.slice(start, index);
        }
        if (/[\w@.+\-*\/%]/.test(expr[index])) {
            let node = parseToken();
            skipWhitespace();
            while (expr[index] === '(') {
                index++;
                let args = parseArguments();
                node = `(${node} ${args.join(' ')})`;
                skipWhitespace();
            }
            return node;
        }
        return "";
    }
    return parseExpr();
}

function compile_declaration(line, output) {
    let bitSetMatch = line.match(/^([\w@.+\-*\/%]+)\[(.+?)\]\[(.+?)\]\s*=\s*(.+)$/);
    if (bitSetMatch) {
        let [, name, i1, i2, valueExpr] = bitSetMatch;
        let bit1 = parseExpression(i1);
        let bit2 = parseExpression(i2);
        let value = parseExpression(valueExpr);
        output.push(`ignore (bit.set ${name} ${bit1} ${bit2} ${value});`);
        return true;
    }
    let arraySetMatch = line.match(/^([\w@.+\-*\/%]+)\[(.+?)\]\s*=\s*(.+)$/);
    if (arraySetMatch) {
        let [, array, indexExpr, valueExpr] = arraySetMatch;
        let indexVal = parseExpression(indexExpr);
        let value = parseExpression(valueExpr);
        output.push(`ignore (byte.set ${array} ${indexVal} ${value});`);
        return true;
    }
    let bracketAssign = line.match(/^\[(.+?)\](?:\s*\[(.+?)\])?(?:\s*\[(.+?)\])?\s*=\s*(.+)$/);
    if (bracketAssign) {
        let [, i1, i2, i3, valueExpr] = bracketAssign;
        let v1 = parseExpression(i1);
        let value = parseExpression(valueExpr);
        if (i2 && i3) {
            let v2 = parseExpression(i2);
            let v3 = parseExpression(i3);
            output.push(`ignore (bit.set ${v1} ${v2} ${v3} ${value});`);
        } else if (i2) {
            let v2 = parseExpression(i2);
            output.push(`ignore (byte.set ${v1} ${v2} ${value});`);
        } else {
            output.push(`ignore (set ${v1} ${value});`);
        }
        return true;
    }
    let bracketCall = line.match(/^\[(.+?)\]\s*\((.*)\)$/);
    if (bracketCall) {
        let [, exprIn, argsStr] = bracketCall;
        let caller = parseExpression(exprIn);
        let argsList = [];
        if (argsStr.trim() !== "") {
            let currentArg = "";
            let depth = 0;
            for (let i = 0; i < argsStr.length; i++) {
                let ch = argsStr[i];
                if (ch === '(') depth++;
                else if (ch === ')') depth--;
                else if (ch === ',' && depth === 0) {
                    argsList.push(parseExpression(currentArg.trim()));
                    currentArg = "";
                    continue;
                }
                currentArg += ch;
            }
            if (currentArg.trim()) argsList.push(parseExpression(currentArg.trim()));
        }
        output.push(`ignore ((get ${caller}) ${argsList.join(' ')})`);
        return true;
    }
    let assignBracketMatch = line.match(/^([\w@.+\-*\/%]+)\s*=\s*\[(.+?)\](?:\s*\[(.+?)\])?(?:\s*\[(.+?)\])?(?:\s*\((.*)\))?$/);
    if (assignBracketMatch) {
        let [, varname, i1, i2, i3, maybeArgsStr] = assignBracketMatch;
        let v1 = parseExpression(i1);
        let v2 = i2 ? parseExpression(i2) : null;
        let v3 = i3 ? parseExpression(i3) : null;
        if (maybeArgsStr !== undefined) {
            let argsList = [];
            if (maybeArgsStr.trim() !== "") {
                let currentArg = "";
                let depth = 0;
                for (let i = 0; i < maybeArgsStr.length; i++) {
                    let ch = maybeArgsStr[i];
                    if (ch === '(') depth++;
                    else if (ch === ')') depth--;
                    else if (ch === ',' && depth === 0) {
                        argsList.push(parseExpression(currentArg.trim()));
                        currentArg = "";
                        continue;
                    }
                    currentArg += ch;
                }
                if (currentArg.trim()) argsList.push(parseExpression(currentArg.trim()));
            }
            output.push(`ignore ((get ${v1}) ${argsList.join(' ')}) @${varname};`);
            return true;
        }
        if (i3) {
            output.push(`ignore (bit.get ${v1} ${v2} ${v3}) @${varname};`);
            return true;
        }
        if (i2) {
            output.push(`ignore (byte.get ${v1} ${v2}) @${varname};`);
            return true;
        }
        output.push(`ignore (get ${v1}) @${varname};`);
        return true;
    }
    let assignMatch = line.match(/^([\w@.+\-*\/%]+)\s*=\s*(.+)$/);
    if (assignMatch) {
        let [, varname, expr] = assignMatch;
        let parsed = parseExpression(expr);
        output.push(`ignore ${parsed} @${varname};`);
        return true;
    }
    let isFunctionCall = line.match(/^[\w@.+\-*\/%]+\s*\(/);
    if (isFunctionCall) {
        let parsed = parseExpression(line);
        output.push(`ignore ${parsed};`);
        return true;
    }
    let directAssign = line.match(/^([\d]+)\s*@([\w@.+\-*\/%]+)$/);
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

function transpile(code_content) {
    const output = [
        "load {std};",
        "load {io};",
        "load {bit};",
        "load {byte};",
    ];
    let cleaned_code = clean(code_content);
    for (let line_statement of cleaned_code.split(/;\s*/)) {
        line_statement = line_statement.trim();
        if (!line_statement) continue;

        if (line_statement.startsWith('`') && line_statement.endsWith('`')) {
            let buxuContent = line_statement.slice(1, -1);
            buxuContent = buxuContent.replace(/\\`/g, '`');
            if (buxuContent.trim().length > 0 && !buxuContent.trim().endsWith(';')) {
                 buxuContent += ';';
            }
            output.push(buxuContent);
            continue;
        }

        let ok = compile_declaration(line_statement, output) || compile_return(line_statement, output);
        if (!ok) {
            output.push(`// não reconhecido: ${line_statement}`);
        }
    }
    return output.join("\n");
}

if (require.main === module) {
    const fs = require("fs");
    const file = process.argv[2];
    if (!file) {
        console.error("uso: node parser_double_quotes_only.js entrada.c_like");
        process.exit(1);
    }
    const code_content_fs = fs.readFileSync(file, "utf-8");
    console.log(transpile(code_content_fs));
}

module.exports = { transpile };


