/* Ejecuta comandos simples y pipelines.
 * No toca ningún comando interno.
 */

#ifndef EXECUTE_H
#define EXECUTE_H

#include "command.h"


void execute_pipeline(pipeline apipe);
/*
 * Ejecuta un pipeline, identificando comandos internos, forkeando, y
 *   redirigiendo la entrada y salida. puede modificar `apipe' en el proceso
 *   de ejecución.
 *   apipe: pipeline a ejecutar
 * Requires: apipe!=NULL
 */

// Crea y devuelve vector de argumentos para un dado scommand
char **build_argv(scommand cmd);
void set_redout(scommand cmd);
void set_redin(scommand cmd);
char **destroy_argv(char **argvt);

#endif /* EXECUTE_H */
