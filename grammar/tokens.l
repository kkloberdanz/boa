/*
 *     This file is part of boa.
 *
 *  boa is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  boa is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with boa.  If not, see <https://www.gnu.org/licenses/>.
 */

%{
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#include <compiler/ast.h>
#include <util/util.h>
#include <y.tab.h>

int fileno(FILE *stream);
%}

%option noyywrap

digit       [0-9]
number      (-?){digit}+
letter      [a-zA-Z]
identifier  ({letter})({letter}|{number}|_)*
newline     \n
whitespace  [ \t]+
string      \"(\\.|[^"\\])*\"|\'(\\.|[^"\\])*\'
comment     (\*(.*\n)*.*\*)
single_line_comment #(.*)

%%

"="           { return ASSIGN; }
"=="          { return EQ; }
">"           { return GT; }
"<"           { return LT; }
">="          { return GE; }
"<="          { return LE; }
"!="          { return NE; }
"=>"          { return FAT_ARROW; }
"if"          { return IF; }
"then"        { return THEN; }
"->"          { return SLIM_ARROW; }
"else"        { return ELSE; }
"("           { return LPAREN; }
")"           { return RPAREN; }
"+"           { return PLUS; }
"-"           { return MINUS; }
"*"           { return TIMES; }
"/"           { return OVER; }
"%"           { return PERCENT; }
"^"           { return CARROT; }
"{"           { return LBRACE; }
"}"           { return RBRACE; }
"["           { return LBRACK; }
"]"           { return RBRACK; }
";"           { return SEMICOLON; }
":"           { return COLON; }
","           { return COMMA; }
"\""          { return QUOTE; }
{number}      { return INTEGER; }
{identifier}  { return ID; }
{newline}     { yylineno++; return NEWLINE; }
{whitespace}  { /* do nothing */; }
{string}      { return STRING; }
{comment}     { /* do nothing */; }
{single_line_comment}     { /* do nothing */ }
.             { return ERROR; }

%%

int get_token(FILE *source_file) {
    static bool first_time = true;
    int current_token;

    /* suppress warnings of unused symbols autogenerated from flex */
    UNUSED(input);
    UNUSED(yyunput);

    if (first_time) {
        first_time = false;
        yyin = source_file;
        yyout = stdout;
    }
    current_token = yylex();
    strncpy(token_string, yytext, MAX_TOKEN_SIZE);
    return current_token;
}

void yyerror(const char *msg) {
    fprintf(stderr,"Error | Line: %d\n%s\n", yylineno, msg);
}
