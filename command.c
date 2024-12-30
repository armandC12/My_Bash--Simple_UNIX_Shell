#include <assert.h>
#include <string.h>
#include <glib.h>
#include <stdio.h>

#include "command.h"
#include "strextra.h"

#define redirin " < "
#define redirout " > "
#define wait_symbol " & "
#define pipe_symbol " | "
#define space_bar " "

/* Estrcutra del scommand_s */
struct scommand_s {
    GSList *my_command;
    char *redin;
    char *redout;
};

scommand scommand_new(void){
    scommand new = malloc(sizeof(struct scommand_s));
    assert(new != NULL);
    new->my_command = NULL;
    new->redin = NULL;
    new->redout = NULL;
    assert(new != NULL && scommand_is_empty(new) 
            && scommand_get_redir_in(new) == NULL 
            && scommand_get_redir_out(new) == NULL);
    return new;
}

scommand scommand_destroy(scommand self){
    assert(self != NULL);
    while (self->my_command != NULL){
        scommand_pop_front(self);
    }
    free(self->redin);
    self->redin = NULL;
    free(self->redout);
    self->redout = NULL;
    free(self);
    self = NULL;
    assert(self == NULL);
    return self;
}

void scommand_push_back(scommand self, char *argument){
    assert(self!=NULL && argument!=NULL);
    self->my_command = g_slist_append(self->my_command, argument);    
    assert(!scommand_is_empty(self));
}


void scommand_pop_front(scommand self){
    assert(self!=NULL && !scommand_is_empty(self));
    GSList *aux = self->my_command;
    self->my_command = g_slist_next(self->my_command);
    g_free(aux->data);
    self->my_command = g_slist_delete_link(self->my_command,aux);
    
}

void scommand_set_redir_in(scommand self, char * filename){
    assert(self != NULL);
    if(self->redin  != NULL) {
        char *aux = self->redout;
        self->redin = filename;
        free(aux);
        aux = NULL;
    }
    else {
        self->redin = filename;
    }
}
void scommand_set_redir_out(scommand self, char * filename){
    assert(self != NULL);
    if(self->redout  != NULL) {
        char *aux = self->redout;
        self->redout = filename;
        free(aux);
        aux = NULL;
    }
    else {
        self->redout = filename;
    }
}

bool scommand_is_empty(const scommand self){
    assert(self != NULL);
    bool result = self->my_command == NULL;
    return (result);
}

unsigned int scommand_length(const scommand self) {
    assert(self!=NULL);
    unsigned int length = g_slist_length(self->my_command);
    assert((g_slist_length(self->my_command) == 0) == scommand_is_empty(self));
    return length;
}

char * scommand_front(const scommand self){
    assert(self!=NULL && !scommand_is_empty(self));
    return g_slist_nth_data(self->my_command, 0);
}

char * scommand_get_redir_in(const scommand self){
    assert(self!=NULL);
    return self->redin;
}

char * scommand_get_redir_out(const scommand self){
    assert(self!=NULL);
    return self->redout;
}

char * scommand_to_string(const scommand self){
    assert(self!=NULL);
    char* result = NULL;    
    if(!scommand_is_empty(self)) {
        GSList *aux = self->my_command;
        result = strdup(g_slist_nth_data(self->my_command, 0));
        aux = g_slist_next(aux);  

        while(aux != NULL){
            result = merge_clean(result, space_bar);
            result = merge_clean(result, g_slist_nth_data(aux, 0));
            aux = g_slist_next(aux);
        }
        if (self->redout != NULL){
            result = merge_clean(result, redirout);
            result = merge_clean(result, self->redout);
        }
        if (self->redin != NULL){
            result = merge_clean(result, redirin);
            result = merge_clean(result, self->redin);
        }
        free(aux);
    }
    else{
        result = (char*) calloc(1, sizeof(char));

    }
    assert(scommand_is_empty(self) || scommand_get_redir_in(self)==NULL || 
           scommand_get_redir_out(self)==NULL || strlen(result)>0);
    return result;
}

/* Estrcutra del pipeline_s */
struct pipeline_s {
    GSList *my_pipeline;
    bool expect;
};


pipeline pipeline_new(void){
   pipeline new;
   new = malloc(sizeof(struct pipeline_s));
   assert(new != NULL);
   new->my_pipeline = NULL;
   new->expect = true;
   assert(new != NULL && pipeline_is_empty(new) && pipeline_get_wait(new));
   return new;
}

pipeline pipeline_destroy(pipeline self){
    assert(self != NULL);
    while (self->my_pipeline != NULL) {
        pipeline_pop_front(self);
    }
    self->my_pipeline = NULL;
    free(self);
    self = NULL;
    assert(self == NULL);
    return self;
} 

void pipeline_push_back(pipeline self, scommand sc){
    assert(self != NULL && sc != NULL);
    self->my_pipeline = g_slist_append(self->my_pipeline, sc);
    assert(!pipeline_is_empty(self));
}

void pipeline_pop_front(pipeline self){
    assert(self != NULL && !pipeline_is_empty(self));
    GSList *aux = self->my_pipeline;
    self->my_pipeline = g_slist_next(self->my_pipeline);
    aux->data = scommand_destroy(aux->data);
    g_free(aux->data);
    self->my_pipeline = g_slist_delete_link(aux,aux);
    
}

void pipeline_set_wait(pipeline self , const bool w){
    assert(self != NULL);
    self->expect = w;
}

bool pipeline_is_empty(const pipeline self){
    assert(self != NULL);
    bool empty = self->my_pipeline == NULL;
    return empty;
}

unsigned int pipeline_length(const pipeline self){
    assert(self != NULL);
    unsigned int length = g_slist_length(self->my_pipeline);
    assert((g_slist_length(self->my_pipeline)==0) == pipeline_is_empty(self));
    
    return length;
}

scommand pipeline_front(const pipeline self){
    assert(self != NULL && !pipeline_is_empty(self));
    scommand command = g_slist_nth_data(self->my_pipeline, 0);
    assert(command != NULL);
    return command;
}

bool pipeline_get_wait(const pipeline self){
    assert(self != NULL);
    return self->expect;
}

char * pipeline_to_string(const pipeline self){
    assert(self != NULL);
    char *pipeline_string = NULL;
    char *command_string = NULL;
    if(!pipeline_is_empty(self))
    {
        GSList *aux_pipe = self->my_pipeline;
        scommand command = g_slist_nth_data(aux_pipe, 0);
        pipeline_string = scommand_to_string(command);
        aux_pipe = g_slist_next(aux_pipe);
        while(aux_pipe != NULL)
        {
            pipeline_string = merge_clean(pipeline_string, pipe_symbol);            // agrego el pipe
            command = g_slist_nth_data(aux_pipe, 0);       
            command_string = scommand_to_string(command);                                 // extraigo los demas commands 
            pipeline_string = merge_clean(pipeline_string, command_string);    // agrego los demas commands
            aux_pipe = g_slist_next(aux_pipe);
            free(command_string);  
        }
        pipeline_string = merge_clean(pipeline_string, wait_symbol);                // agrego el & al final
        
    }
    else{
        pipeline_string = (char*)calloc(1, sizeof(char));
    }
    assert(pipeline_is_empty(self) || pipeline_get_wait(self) || strlen(pipeline_string)>0);
    return pipeline_string;
}

