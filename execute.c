#include <assert.h>
#include <stdlib.h>
#include <sys/types.h> 
#include <sys/wait.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

#include "tests/syscall_mock.h"
#include "builtin.h"
#include "command.h"
#include "execute.h"

void execute_pipeline(pipeline apipe){
    assert(apipe != NULL);
    if (!pipeline_is_empty(apipe)){
        scommand cmd = pipeline_front(apipe); 
        int execvp_result;
        //COMANDO INTERNO -------------------------------------------
        if (builtin_alone(apipe)){
            builtin_run(cmd);
        }
        else {
        //armo vector de argumentos
            char **argvt = build_argv(cmd);

        //-------- PIPELINE CON UN SCOMMAND NO INTERNO ---------------------------------------
            if (pipeline_length(apipe) == 1) {
                pid_t pid = fork();
                if (pid < 0){
                    fprintf(stderr, "error en fork\n");
                    _exit(EXIT_FAILURE);
                }
                //------ HIJO ÚNICO ---------
                if (pid == 0){
                    set_redin(cmd);
                    set_redout(cmd);
                    execvp_result = execvp(argvt[0], argvt);
                    if (execvp_result < 0){
                        fprintf(stderr, "error en execvp\ncompruebe si es un programa válido\n");
                        _exit(EXIT_FAILURE);
                    }
                //------- PADRE -----------
                }else {
                bool wait_pipe = pipeline_get_wait(apipe);
                if (wait_pipe){
                    wait(NULL);
                }
            }
            //----------PIPELINE CON 2 SCOMMAND---------------------------------------------
            }else if (pipeline_length(apipe) == 2){
                int p[2];
                pipe(p);
                pid_t pid1 = fork();
                
                //------ HIJO 1 -------
                if (pid1 < 0){
                    fprintf(stderr, "error en fork\n");
                    _exit(EXIT_FAILURE);
                }else if (pid1 == 0){
                    // apunto stdout a la punta de escritura del pipe (p[1])
                    dup2(p[1], 1);
                    close(p[1]);
                    close(p[0]);
                    //redirecciones
                    set_redin(cmd);
                    set_redout(cmd);
                    //execevp
                    execvp_result = execvp(argvt[0], argvt);
                    if (execvp_result < 0){
                        fprintf(stderr, "Falla en el execvp, no se puede ejecutar el comando, intente help\n");
                        _exit(EXIT_FAILURE);
                    }
                    //------ PADRE -------
                }else {
                    pipeline_pop_front(apipe);
                    cmd = pipeline_front(apipe);
                    argvt = destroy_argv(argvt);
                    argvt = build_argv(cmd);
                    pid_t pid2 = fork();
                    //------ HIJO 2 -------
                    if (pid2 == 0){
                        // apunto stdin a la punta de lectura
                        dup2(p[0],0);
                        close(p[0]);
                        close(p[1]);
                        set_redin(cmd);
                        set_redout(cmd);
                        execvp_result = execvp(argvt[0], argvt);
                        if (execvp_result < 0){
                            fprintf(stderr, "Falla en el execvp, no se puede ejecutar el comando, intente help\n");
                            _exit(EXIT_FAILURE);
                        }
                    //------- PADRE ---------
                    }else{
                        close(p[1]);
                        close(p[0]);
                        bool wait_pipe = pipeline_get_wait(apipe);
                        // ¿espero?
                        if (wait_pipe){
                            wait(NULL);
                            wait(NULL);
                        }
                        argvt = destroy_argv(argvt);
                    }
                }          

            }else {
                fprintf(stderr, "Este bash no admite pipelines de mas de dos comandos \n");
            }        
        }
    }
}     


//arma vector de argumentos para un scommand
char **build_argv(scommand cmd){
    unsigned int cmd_length = scommand_length(cmd);
    char **argvt = calloc(cmd_length +1u, sizeof(char *));
        for (unsigned int arg = 0u ; arg < cmd_length; arg++){
            argvt[arg] = (char *)strdup(scommand_front(cmd));
            scommand_pop_front(cmd);
        }
        argvt[cmd_length] = NULL;
    return argvt;
}

char **destroy_argv(char **argvt){
    unsigned int i = 0u;
    while (argvt[i] != NULL){
        free(argvt[i]);
        ++i;
    }
    free(argvt);
    argvt = NULL;
    return argvt;
}

void set_redin(scommand cmd){
    if (scommand_get_redir_in(cmd )!= NULL){
        char *path = strdup(scommand_get_redir_in(cmd));
        int fd = open(path, O_RDONLY, S_IRWXU | S_IRWXG);
        if (fd < 0){
            fprintf(stderr, "Bash: el archivo %s no existe\n",
                scommand_get_redir_in(cmd));
            _exit(EXIT_FAILURE);
                    
        }else {
            dup2(fd,0);
        }
        free(path);
        close(fd);
    }
}

void set_redout(scommand cmd){
    if (scommand_get_redir_out(cmd) != NULL){
        char *path = strdup(scommand_get_redir_out(cmd));
        int fd = open(path, O_WRONLY | O_CREAT, S_IRWXU | S_IRWXG);
        if (fd < 0){
            fprintf(stderr, "Error en la redirección de salida\n");
            _exit(EXIT_FAILURE);
                    
        }else{
                    dup2(fd,1);
        }
        free(path);
        close(fd);
    }            
}