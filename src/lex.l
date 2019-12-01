%option noyywrap
%option interactive

%{
#include "parse.tab.h"
#include "utilities.h"
%}

%%

[ \t\n ]+ ;

[0-9]+ { yylval.uint = parseInt(yytext); return UINT; }

"A" { return A; }
"fork" { return FORK; }
"exit" { return EXIT; }
"wait" { return WAIT; }
"d" { return DREQ; }
"D" { return DDONE; }
"S +r" { return SHOWR; }
"S +i" { return SHOWI; }
"S +m" { return SHOWM; }

.* { strcpy(yylval.string, yytext); return STRING; }

%%
