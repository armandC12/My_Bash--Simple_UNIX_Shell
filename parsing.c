#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>
#include <string.h>

#include "parsing.h"
#include "parser.h"
#include "command.h"

static scommand parse_scommand(Parser p) {
    /* Devuelve NULL cuando hay un error de parseo */
    
    scommand my_command = scommand_new();  
    arg_kind_t type;
    char *arg = NULL;
    arg = parser_next_argument(p, &type);
    bool err = false;
    
    while (arg != NULL && !err){
        parser_skip_blanks(p);
        
            switch (type)
            {
            case ARG_NORMAL:
                scommand_push_back(my_command, arg);
                break;

            case ARG_INPUT:
                if ((strncmp(arg, "<",1) == 0) || (strncmp(arg, ">",1) == 0))
                {
                    fprintf(stderr, "Syntax Error\n");
                    err = true;
                } else {
                    scommand_set_redir_in(my_command,arg);
                }
                break;
            
            case ARG_OUTPUT:
                if ((strncmp(arg, "<",1) == 0) || (strncmp(arg, ">",1) == 0))
                {
                    fprintf(stderr, "Syntax Error\n");
                    err = true;
                } else {
                    scommand_set_redir_out(my_command,arg);
                }
                break;
            
            default:
                printf("Orden no encontrada, fallo en parse_scommand\n");
                exit(EXIT_FAILURE);
            }
            arg = parser_next_argument(p, &type);

    }
    //Si algo falla y no se leyó un scommand, libero recursos
    if (scommand_length(my_command) == 0 || err){
        my_command = scommand_destroy(my_command);
    }
    return my_command;
}

pipeline parse_pipeline(Parser p) {
    assert(p!=NULL && !parser_at_eof(p));

    pipeline result = pipeline_new();
    scommand cmd = NULL;
    bool error = false, another_pipe=true;

    cmd = parse_scommand(p);
    error = (cmd==NULL); /* Comando inválido al empezar */
    while (another_pipe && !error) 
    {    
        pipeline_push_back(result,cmd);  //se agrega un comando por detras
        parser_op_pipe(p,&another_pipe); //leer un pipe y consume si lo hay
        cmd = parse_scommand(p);         //Nos devuelve el command armado
        error = (cmd==NULL);
    }
    
    /* Opcionalmente un OP_BACKGROUND al final */
    
    // Consume los espacios vacios a continuacion, se detiene si hay al algo raro
    parser_skip_blanks(p);

    // Detecta si hay un & y se guarda es resultado en result
    bool is_background;
    parser_op_background(p, &is_background);  //detecta si hay un &
    pipeline_set_wait(result,!is_background); //de lo anterior se guarda si hay que esperar o no
    

    // Consume todos los caracteres hasta un final de linea \n y tambien a ese \n.
    bool garbage;
    parser_garbage(p, &garbage);
    if (garbage)
    {
        fprintf(stderr, "Syntax error, no se reconoce el comando\n");
    }

    // Si result es vacio, hay error o garbage entonces lo destruimos
    if (pipeline_is_empty(result) || garbage)
    {
        result = pipeline_destroy(result);
    }
    
    /* Tolerancia a espacios posteriores */
    /* Consumir todo lo que hay inclusive el \n */
    /* Si hubo error, hacemos cleanup */
    return result; 
}

