/*
 *     This file is part of Boa.
 *
 *  Boa is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  Boa is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with Boa.  If not, see <https://www.gnu.org/licenses/>.
 */

%{
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "minic.h"
#include "y.tab.h"

char token_string[MAX_TOKEN_SIZE+1];
%}

digit       [0-9]
number      {digit}+
letter      [a-zA-Z]
identifier  {letter}+
newline     \n
whitespace  [ \t]+

%%

"="           { return ASSIGN; }
"=="          { return EQ; }
">"           { return GT; }
"<"           { return LT; }
">="          { return GE; }
"<="          { return LE; }
"!="          { return NE; }
"if"          { return IF; }
"then"        { return THEN; }
"else"        { return ELSE; }
"print"       { return PRINT; }
"int"         { return INT; }
"("           { return LPAREN; }
")"           { return RPAREN; }
"+"           { return PLUS; }
"-"           { return MINUS; }
"*"           { return TIMES; }
"/"           { return OVER; }
"{"           { return LBRACE; }
"}"           { return RBRACE; }
";"           { return SEMICOLON; }
","           { return COMMA; }
{number}      { return INTEGER; }
{identifier}  { return ID; }
{newline}     { yylineno++; return NEWLINE}
{whitespace}  { /* do nothing */; }
.             { return ERROR; }

%%

int get_token(FILE *source_file) {
    static int first_time = true;
    int currentToken;
    if (first_time) {
        first_time = false;
        yylineno++;
        yyin = source_file;
        yyout = stdout;
    }
    currentToken = yylex();
    strncpy(token_string, yytext, MAX_TOKEN_SIZE);
    return currentToken;
}
