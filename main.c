#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <time.h>
#include "htable.h"
#define WORD_SIZE 21
#define BOOK_NAME 43

int main()
{
    //clock_t start, end;
    //start = clock();
    htable *dictionary,*users;
    dictionary=(htable*)malloc(sizeof(htable));
    users=(htable*)malloc(sizeof(htable));
    init_htable(dictionary, 5);
    init_htable(users,5);
    char *command;
    command = (char *)malloc(20+BOOK_NAME+5);
    fgets(command,20 + BOOK_NAME + 5,stdin);
    int number_def;
    char *name;
    
    char *def_key=(char*)malloc(WORD_SIZE);
    char *def_val;
    while (memcmp(command, "EXIT", 4) != 0)
    {
        
        if (memcmp(command, "ADD_BOOK", 8) == 0)
        {
            name = (char *)calloc(BOOK_NAME,1);
            separate_string_number(command,name,&number_def);
            ADD_BOOK(dictionary, name, number_def);
            free(name);
            goto lala;
        }
        if(memcmp(command,"GET_BOOK",8)==0)
        {
           name = (char *)calloc(BOOK_NAME,1);
            separate_1argv(command,name);
            GET_BOOK(dictionary,name);
            free(name);
            goto lala;
        }
        if(memcmp(command,"RMV_BOOK",8)==0)
        {
            name = (char *)calloc(BOOK_NAME,1);
            separate_1argv(command,name);
            RMV_BOOK(dictionary,name);
            free(name);
            goto lala;
        }
        if(memcmp(command,"GET_DEF",7)==0)
        {
            name = (char *)calloc(BOOK_NAME,1);
            separate_two_strings(command,name,def_key);
            GET_DEF(dictionary,name,def_key);
            free(name);
            goto lala;
        }
        if(memcmp(command,"ADD_DEF",7)==0)
        {
            name = (char *)calloc(BOOK_NAME,1);
            separate_two_strings(command,name,def_key);
            def_val=strtok(def_key," ");
            def_val=strtok(NULL," ");
            ADD_DEF(dictionary,name,def_key,def_val);
            free(name);
            goto lala;
         }
         if(memcmp(command,"RMV_DEF",7)==0)
         {
            name = (char *)calloc(BOOK_NAME,1);
            separate_two_strings(command,name,def_key);
            RMV_DEF(dictionary,name,def_key);
            free(name);
            goto lala;
         }
         if(memcmp(command,"ADD_USER",8)==0)
         {
            name = (char *)calloc(BOOK_NAME,1);
            char *token;
            token=strtok(command," ");
            token=strtok(NULL, " ");
            memcpy(name,token,WORD_SIZE);
            ADD_USER(users,name);
            free(name);
            goto lala;
         }
    lala:
        free(command);
        command = (char *)malloc(20+BOOK_NAME+5);
        fgets(command,20 + BOOK_NAME + 5,stdin);
    }



    TOP_BOOKS(dictionary,BOOK_NAME+8);
    TOP_USERS();
    free(def_key);
    free_htable(dictionary);
    free_htable(users);
    free(command);
    return 0;
    // end = clock();
    // double cpu_time_used = ((double)(end - start)) / CLOCKS_PER_SEC;
    // printf("\n\n%f\n", cpu_time_used);
}