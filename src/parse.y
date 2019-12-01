%{
#include <stdio.h>

#include "aaoss.h"
#include "disk.h"
#include "process.h"
%}

%start input

%union {
    size_t uint;
    char *string;
};

%token <string> A FORK EXIT WAIT
%token <string> DREQ DDONE
%token <string> SHOWR SHOWI SHOWM
%token <string> STRING
%token <uint> UINT

%%

input:
     | A UINT UINT { process_create($2, $3); }
     | FORK { process_fork(); }
     | EXIT { process_exit(); }
     | WAIT { process_wait(); }
     | DREQ UINT STRING { disk_request($2, $3); }
     | DDONE UINT { disk_done($2); }
     | SHOWR { show_processes(); }
     | SHOWI { show_disks(); }
     | SHOWM { show_memory(); }
     | error { fprintf("Parse error.\n"); print_prompt(); yyclearin; yyerrok; }
     ;
