grammar WASM;

ID:		[a-z]+;
NL:		'\n' -> skip;
SPACE:	' '+ -> skip;
WS:		[\t\r\n]+ -> skip;
NUMBER:	[0-9]+;
L_BRACE:	'{';
R_BRACE:	'}';
L_BRACKET:	'[';
R_BRACKET:	']';
L_ANGLE: '<';
R_ANGLE: '>';
L_PAREN: '(';
R_PAREN: ')';
SEMI:	';';
COLON:	':';
COMMA:	',';
EQ:		'=';
AT:		'@';
AND:	'&';
NOT:	'~';
ADD:	'+';
SUB:	'-';
INTO:	'->';
OR:		'|';
MULT:	'*';

STRING:	'"' ('\\"' | ~'"')* '"' { setText(getText().substring(1, s.length - 1).replace("\\\\\"", "\"")); };
CHAR:	'\'' (~('\'' | '\\') | '\\\'' | '\\\\') '\'';

TEMP_REG:	'$t' ([0-9a-f] | '1' [0-9a]);
SAVED_REG:	'$s' ([0-9a-f] | '1' [0-9a]);
ARG_REG:	'$a' [0-9a-f];
RETURN_REG:	'$r' [0-9a-f];
RD: '$rd';
RS: '$rs';
RT: '$rt';

HEX_PREFIX: '0x';
BIN_PREFIX: '0b';
HEX: HEX_PREFIX [0-9a-f]+;
BINARY: BIN_PREFIX [01]+;
hex: HEX;
binary: BINARY;

immediate: hex | binary | NUMBER;
register: TEMP_REG | SAVED_REG | ARG_REG | RETURN_REG; // | RD | RS | RT;

op_add: register ADD register INTO register;
op_addi: register ADD immediate INTO register;
op_sub: register SUB register INTO register;
op_subi: register SUB immediate INTO register;
op_mult: register MULT register INTO register;
op_multi: register MULT immediate INTO register;

op_and: register AND register INTO register;
op_andi: register AND immediate INTO register;
op_nand: register NOT AND register INTO register;
op_nandi: register NOT AND immediate INTO register;

op_or: register OR register INTO register;
op_ori: register OR immediate INTO register;
op_nor: register NOT OR register INTO register;
op_nori: register NOT OR immediate INTO register;

op_xor: register 'x' register INTO register;
op_xori: register 'x' immediate INTO register;
op_xnor: register NOT 'x' register INTO register;
op_xnori: register NOT 'x' immediate INTO register;

op_not: NOT register INTO register;

op_lui: 'lui:' immediate INTO register;
op_mfhi: '%hi' INTO register;
op_mflo: '%lo' INTO register;

op: op_and | op_andi | op_or | op_ori | op_nor | op_nori | op_xor | op_xori | op_xnor | op_xnori |
    op_not | op_lui | op_mfhi | op_mflo;

statement: L_BRACKET op R_BRACKET SEMI?;
program: statement+;