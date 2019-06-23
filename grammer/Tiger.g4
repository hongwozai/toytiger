grammar Tiger;

exps
    : exp +
    | dec +
    ;

dec
    : tydec
    | vardec
    | fundec
    ;

tydec
    : 'type' Identifier '=' ty
    ;

ty
    : Identifier
    | '{' tyfields '}'
    | 'array' 'of' Identifier
    ;

tyfields
    :
    | Identifier ':' Identifier (',' Identifier ':' Identifier)*
    ;

vardec
    : 'var' Identifier ':=' exp
    | 'var' Identifier ':' Identifier ':=' exp
    ;

fundec
    : 'function' Identifier '(' tyfields ')' '=' exp
    | 'function' Identifier '(' tyfields ')' ':' Identifier '=' exp
    ;

lvalue
    : Identifier
    | lvalue '.' Identifier
    | lvalue '[' exp ']'
    ;

exp
    : Identifier
    | Number
    | String
    | 'nil'
    | '(' exp (';' exp)* ')'
    | Identifier '(' exp (',' exp)* ')'
    | Identifier '[' exp ']' 'of' exp
    | Identifier '{' Identifier '=' exp (',' Identifier '=' exp)* '}'
    | lvalue
    | lvalue ':=' exp
    | 'if' exp 'then' exp
    | 'if' exp 'then' exp 'else' exp
    | 'while' exp 'do' exp
    | 'for' Identifier ':=' exp 'to' exp 'do' exp
    | 'break'
    | 'let' dec* 'in' exp* 'end'
    | '-' exp
    | exp ('*'|'/') exp
    | exp ('+'|'-') exp
    | exp ('='|'<'|'>'|'>='|'<=') exp
    | exp '&' exp
    | exp '|' exp
    ;

// 词法部分
Identifier : [_A-Za-z][_A-Za-z0-9]+ ;

Number : [0-9]+ ;

WS : [ \t\n]+ -> skip ;

String : '"' Charseq? '"' ;

Comment : '/*' .*? '*/' -> skip ;
LineComment : '//' ~[\r\n]* -> skip ;

Charseq
    : ~["\\\r\n]
    | '\\\n'
    |  '\\' ['"?abfnrtv\\]
    ;