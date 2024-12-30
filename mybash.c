#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "command.h"
#include <unistd.h>
#include "execute.h"
#include "parser.h"
#include "parsing.h"
#include "builtin.h"

#define MAX_HOST_NAME 40
static void show_prompt(void) {
    //dir
    char *current_dir = getcwd(NULL, 0);
    //host
    char *host_name = calloc(MAX_HOST_NAME, sizeof(char));
    gethostname(host_name, MAX_HOST_NAME);
    //user
    char *user = getlogin();
    //print user@host: ~/<path>$ 
    printf ("%s@%s :%s$ ", user, host_name, current_dir);
    
    fflush (stdout);
    free(current_dir);
    free(host_name);
}

int main(int argc, char *argv[]) {
    pipeline pipe;
    Parser input;
    bool quit = false;

    input = parser_new(stdin);
    while (!quit) {
        show_prompt();
        pipe = parse_pipeline(input);

        if (pipe != NULL && !pipeline_is_empty(pipe)){
            execute_pipeline(pipe);
            pipe = pipeline_destroy(pipe);
        }
    }
    parser_destroy(input);
    input = NULL;
    return EXIT_SUCCESS;
}