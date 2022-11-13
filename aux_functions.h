#include <string.h>
#include <stdlib.h>
#define BOOK_NAME 43
#define WORD_SIZE 21
#include <stdio.h>

typedef struct htable_bucket
{
    void *borrowed;
    void *data;
    struct htable_bucket *next;
} hnode;

typedef struct hash_table
{
    hnode **bucket_vector;
    int B;
    int E;
} htable;

int check_empty(hnode h_node)
{
    unsigned char check;
    memcpy(&check, h_node.data, 1);
    if (check == 255)
    {
        return 1;
    }
    return 0;
}

unsigned int hash_fct(char *str)
{
    unsigned int hash = 5831;
    unsigned char c;
    char *initial;
    initial = str;
    while (*str != 0)
    {
        c = *str;
        hash = ((hash << 5) + hash) + c;
        str = str + 1;
    }
    str = initial;
    return hash;
}

void init_node(hnode *temp)
{
    unsigned char c = 255;
    temp->data = malloc(1);
    temp->borrowed = malloc(1);
    unsigned char can_borrow = 0;
    memcpy(temp->borrowed, &can_borrow, 1);
    memcpy(temp->data, &c, 1);
    temp->next = NULL;
}

void init_htable(htable *dictionary, int B)
{
    dictionary->B = B;
    dictionary->bucket_vector = (hnode **)malloc(dictionary->B * sizeof(hnode *));
    dictionary->E = 0;
    int i = 0;
    for (i = 0; i < dictionary->B; i++)
    {
        dictionary->bucket_vector[i] = (hnode *)malloc(sizeof(hnode));
        init_node(dictionary->bucket_vector[i]);
    }
}

void free_node(hnode *temp)
{
    free(temp->data);
    free(temp->borrowed);
    free(temp);
}

void free_contains_vector(hnode **vector, int size)
{
    hnode *temp, *prev;
    int i = 0;
    for (i = 0; i < size; i++)
    {
        temp = vector[i];
        while (temp != NULL)
        {
            prev = temp;
            temp = temp->next;
            free_node(prev);
        }
    }
}

void free_htable(htable *dictionary)
{

    free_contains_vector(dictionary->bucket_vector, dictionary->B);
    free(dictionary->bucket_vector);
    free(dictionary);
}

void separate_1argv(char *command, char *book_name)
{
    char *start = command;
    unsigned char c = 34;
    command = strchr(command, c);
    command = command + 1;
    int size_book_name = 0;
    char *command2;
    command2 = strchr(command, c);
    // de adaugat la comand un 1 sau 2?
    size_book_name = command2 - command;
    unsigned char row_terminator = 0;
    memcpy(book_name, command, size_book_name);
    memcpy(book_name + size_book_name, &row_terminator, 1);
    command = start;
}

void separate_string_number(char *command, char *book_name, int *number_def)
{
    char *start = command;
    unsigned char c = 34;
    separate_1argv(command, book_name);
    command = strchr(command, c);
    command = command + 1;
    command = strchr(command, c);
    command = command + 2;
    char *end;
    *number_def = (int)strtol(command, &end, 10);
    command = start;
}

void separate_two_strings(char *command, char *book_name, char *def_key)
{
    char *start = command;
    separate_1argv(command, book_name);
    char *token;
    token = strtok(command, "\"");
    token = strtok(NULL, "\"");
    token = strtok(NULL, "\"");
    token = token + 1;
    memcpy(def_key, token, WORD_SIZE);
    command = start;
}

int my_strlen(char *book_name)
{
    int result = 0;
    char *start;
    start = book_name;
    while (*book_name != '\0')
    {
        result = result + 1;
        book_name = book_name + 1;
    }
    book_name = start;
    // result = result - 2;
    //  scap de " "
    return result;
}

void printme_the_book(char *data)
{
    char *book_name;
    book_name = (char *)malloc(BOOK_NAME);
    memcpy(book_name, data, BOOK_NAME);
    printf("Name:");
    printf("%s ", book_name);
    printf("Rating:");
    float extractt;
    memcpy(&extractt, data + BOOK_NAME, 4);
    printf("%0.3f ", extractt);
    memcpy(&extractt, data + BOOK_NAME + 4, 4);
    printf("Purchases:");
    printf("%d\n", (int)extractt);
    free(book_name);
}

int compare(char *first_chunk, char *second_chunk)
{
    int extractt_first, extractt_second;
    memcpy(&extractt_first, first_chunk + BOOK_NAME, sizeof(float));
    memcpy(&extractt_second, second_chunk + BOOK_NAME, sizeof(float));

    if (extractt_first > extractt_second)
    {
        return 1;
    }

    if (extractt_first < extractt_second)
    {
        return -1;
    }
    return memcmp(first_chunk, second_chunk, BOOK_NAME);
}

hnode *get(htable *dictionary, char *book_name)
{
    int size_book_name = my_strlen(book_name) - 2;
    hnode *output;
    int index = hash_fct(book_name) % dictionary->B;
    hnode *temp;
    temp = dictionary->bucket_vector[index];
    while (temp != NULL)
    {
        if (check_empty(*temp) == 0 &&
            memcmp(book_name, temp->data + 4, size_book_name) == 0)
        {
            output = temp;
            return output;
        }
        temp = temp->next;
    }
    return NULL;
}

void put(htable *dictionary, void *value, int number_def)
{
    char *book_name;
    book_name=(char*)malloc(BOOK_NAME);
    memcpy(book_name, value + 4, BOOK_NAME);
    int index = hash_fct(book_name) % dictionary->B;
    unsigned char can_borrow = 1;

    hnode *temp;
    temp = dictionary->bucket_vector[index];
    if (check_empty(*temp) == 1)
    {
        free(temp->data);
        memcpy(temp->borrowed, &can_borrow, 1);
        temp->data = malloc(WORD_SIZE * 2 * number_def + BOOK_NAME + 12);
        memcpy(temp->data, value, WORD_SIZE * 2 * number_def + BOOK_NAME + 12);
        dictionary->E = dictionary->E + 1;
        free(book_name);
        return;
    }
    while (temp->next != NULL)
    { // adapteaza aceeasi carte la noile valori
        if (memcmp(book_name, temp->data, BOOK_NAME) == 0)
        {
            memcpy(temp->borrowed, &can_borrow, 1);
            temp->data = realloc(temp->data, WORD_SIZE * 2 * number_def + BOOK_NAME + 12);
            memcpy(temp->data, value, WORD_SIZE * 2 * number_def + BOOK_NAME + 12);
            free(book_name);
            return;
        }
        temp = temp->next;
    }
    hnode *new;
    new = (hnode *)malloc(sizeof(hnode));
    new->next = NULL;
    new->data = malloc(WORD_SIZE * 2 * number_def + BOOK_NAME + 12);
    memcpy(new->data, value, WORD_SIZE * 2 * number_def + BOOK_NAME + 12);
    new->borrowed = malloc(1);
    memcpy(new->borrowed, &can_borrow, 1);
    dictionary->E = dictionary->E + 1;
    temp->next = new;
    free(book_name);
}

char *make_zip(htable *dictionary, int chunk_size)
{ // chunk_size= BOOK_NAME + 8 for any book ; BOOK_NAME + 4 for users
    char *all_vector;
    all_vector = (char *)malloc(chunk_size * dictionary->E);
    hnode *temp;
    int i = 0, contor = 0;
    for (i = 0; i < dictionary->B; i++)
    { //nu se face corect adaugarea;
        temp = dictionary->bucket_vector[i];
        while (temp != NULL)
        {
            if (check_empty(*temp) == 0)
            {
                memcpy(all_vector + contor, temp->data + 4, chunk_size);
                contor = contor + chunk_size;
            }
            temp = temp->next;
        }
    }
    return all_vector;
}

char *sort_zip(htable *dictionary, int chunk_size)
{ // de continuat cu asta si de rezolvat partea de selection sort;
    char *extractt_zip = make_zip(dictionary, chunk_size);
    char auxx1[BOOK_NAME], auxx2[BOOK_NAME];
    int i, j, cursor1 = 0, cursor2 = chunk_size;
    for (i = 0; i < dictionary->E - 1; i++)
    {
        for (j = i + 1; j < dictionary->E; j++)
        {
            if (compare(extractt_zip + cursor1, extractt_zip + cursor2) == -1)
            {
                memcpy(auxx1, extractt_zip + cursor1, chunk_size);
                memcpy(auxx2, extractt_zip + cursor2, chunk_size);
                // makes two copies
                memcpy(extractt_zip + cursor2, auxx1, chunk_size);
                memcpy(extractt_zip + cursor1, auxx2, chunk_size);
                // makes the copying
            }
            cursor2 = cursor2 + chunk_size;
        }
        cursor1 = cursor1 + chunk_size;
        cursor2 = cursor1 + chunk_size;
    }
    return extractt_zip;
}

hnode *find_node(htable *dictionary, char *book_name)
{
    int index = hash_fct(book_name) % dictionary->B;
    hnode *temp = dictionary->bucket_vector[index];
    if (check_empty(*temp) == 0)
    {
        while (temp != NULL)
        { // my_strlen(book_name)-2
            if (memcmp(temp->data + 4, book_name, my_strlen(book_name) - 2) == 0)
            {
                break;
            }
            temp = temp->next;
        }
        return temp;
    }
    return NULL;
}

char *find_def(char *in_string, char *def_key)
{
    int cursor = BOOK_NAME + 12;
    int number_def;
    memcpy(&number_def, in_string, 4);
    int i = 0;
    int amount_to_check = my_strlen(def_key);
    for (i = 0; i < number_def; i++)
    {
        if (memcmp(in_string + cursor, def_key, amount_to_check - 1) == 0)
        {
            char *ressult;
            cursor = cursor + amount_to_check;
            ressult = in_string + cursor;
            return ressult;
        }
        cursor = cursor + 2 * WORD_SIZE;
    }
    return NULL;
}
