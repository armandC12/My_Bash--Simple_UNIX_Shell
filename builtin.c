#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <errno.h>
#include <assert.h>
#include <string.h>
#include "tests/syscall_mock.h"

#include "command.h"
#include "builtin.h"
#include "strextra.h"

char *slash = "/";

bool builtin_is_internal(scommand cmd) {
    assert(cmd != NULL);
    bool result = false;
    char *cmd_internal = scommand_front(cmd);

    if (strcmp(cmd_internal, "cd") == 0)
    {

        result = true;
    }
    else if (strcmp(cmd_internal, "help") == 0)
    {
        
        result = true;
    }
    else if (strcmp(cmd_internal, "exit") == 0)
    {
        
        result = true;
    }

    return result;
}

bool builtin_alone(pipeline p) {
    assert(p != NULL);
    bool result = pipeline_length(p) == 1 && builtin_is_internal(pipeline_front(p));

    return result;
}

void builtin_run(scommand cmd){
    assert(builtin_is_internal(cmd)); 
    char *cmd_internal = strdup(scommand_front(cmd));
    
    scommand_pop_front(cmd);
    int check;
    char *path;
    //--------CD-----------
    if (strcmp(cmd_internal, "cd") == 0)
    {   if (!scommand_is_empty(cmd)){
            path = strdup(scommand_front(cmd));

        }else{
            path = strdup("/");
        }
        check = chdir(path);
        if (check != 0)
        {
            perror("commmand failed");
        }
        free(path);
        
    }
    //--------HELP-----------
    else if (strcmp(cmd_internal, "help") == 0)
    {
        printf("\nThe best shell 2022\n comando implementados: cd, help, exit\n\n");
    }
    //--------EXIT------------
    else if (strcmp(cmd_internal, "exit") == 0)
    {
        exit(EXIT_SUCCESS);
    }
}