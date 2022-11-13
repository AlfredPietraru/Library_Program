#define WORD_SIZE 21
#define BOOK_NAME 43
#define MAX_RATING 9999
#include "aux_functions.h"

// void one_node_each_bucket()

void exchange_one_empty(hnode **destination, hnode *source)
{
    int number_def;
    memcpy(&number_def, source->data, 4);
    free((*destination)->data);
    (*destination)->data = malloc(12 + BOOK_NAME + 2 * WORD_SIZE * number_def);
    unsigned char to_borrow;
    memcpy(&to_borrow,source->borrowed,1);
    memcpy((*destination)->borrowed,&to_borrow,1);
    memcpy((*destination)->data, source->data, 12 + BOOK_NAME + 2 * WORD_SIZE * number_def);
    free(source->data);
    source->data = malloc(1);
    unsigned char c=255;
    memcpy(source->data, &c, 1);
}

int which_bucket(hnode *start,int new_B)
{
    int finish;
    char *name;
    name = (char *)malloc(BOOK_NAME);
    memcpy(name, start->data + 4, BOOK_NAME);
    finish = hash_fct(name) % new_B;
    free(name);
    return finish;
}
//de regandit functia asta 
void where_send_bucket(htable *dictionary, hnode **new_vector, int index)
{
    int new_index;
    hnode *temp;
    hnode *new_temp;
    temp = dictionary->bucket_vector[index];
    if (check_empty(*temp) == 1)
    { // the node from dictionary is empty
        return;
    }
    while (temp != NULL)
    {
         // node from dictionary is not empty and is a single node
            new_index = which_bucket(temp,2 *dictionary->B);
            if (check_empty(*new_vector[new_index]) == 1)
            { // destination is empty
                exchange_one_empty(&new_vector[index], temp);
                new_vector[index]->next=NULL;
            }
            else
            {
                // destination is not empty
                new_temp = new_vector[new_index];
                while (new_temp->next != NULL)
                {
                    new_temp = new_temp->next;
                }
                hnode *new_node;
                new_node = (hnode *)malloc(sizeof(hnode));
                init_node(new_node);
                exchange_one_empty(&new_node, temp);
                new_temp->next = new_node;
                new_node->next = NULL;
            }
        
        temp = temp->next;
    }
}

void resize(htable *dictionary, int new_B)
{
    hnode **new_vector;
    new_vector = (hnode **)malloc(new_B * sizeof(hnode *));
    int i = 0;
    for (i = 0; i < new_B; i++)
    {
        new_vector[i] = (hnode *)malloc(sizeof(hnode));
        init_node(new_vector[i]);
    }
    
    for (i = 0; i < dictionary->B; i++)
    {
        where_send_bucket(dictionary, new_vector, i);
    }
    free_contains_vector(dictionary->bucket_vector,dictionary->B);
    free(dictionary->bucket_vector);
    dictionary->bucket_vector=new_vector;
    dictionary->B=new_B;
}

void ADD_BOOK(htable *dictionary, char *book_name, int number_def)
{
    char *value;
    value = (char *)malloc(WORD_SIZE * number_def * 2 + BOOK_NAME + 12);
    memcpy(value, &number_def, 4);
    memcpy(value + 4, book_name, BOOK_NAME);
    int i = 10, cursor = BOOK_NAME + 12;
    memcpy(value + BOOK_NAME + 4, &i, sizeof(float));
    memcpy(value + BOOK_NAME + 8, &i, sizeof(int));

    char key_word_pair[WORD_SIZE * 2];
    for (i = 0; i < number_def; i++)
    {
        fgets(key_word_pair, WORD_SIZE * 2, stdin);
        memcpy(value + cursor, key_word_pair, WORD_SIZE * 2);
        cursor = cursor + WORD_SIZE * 2;
    }
    put(dictionary, value, number_def);

    if (dictionary->E > dictionary->B)
    {
        resize(dictionary, 2 * dictionary->B);
    }    
    free(value);
}

void GET_BOOK(htable *dictionary, char *book_name)
{
    hnode *output;
    output = get(dictionary, book_name);
    if (output == NULL)
    {
        printf("The book is not in the library.\n");
        return;
    }
    printme_the_book((char *)output->data + 4);
}

void RMV_BOOK(htable *dictionary, char *book_name)
{
    int i = 255, string_size;
    int index = hash_fct(book_name) % dictionary->B;
    hnode *temp = dictionary->bucket_vector[index];

    if (check_empty(*temp) == 1)
    {
        printf("The book is not in the library.\n");
        return;
    }
    // if there is one element in bucket and that one is to be deleted
    if (temp->next == NULL)
    {
        //verify if is actually the book 
        if(memcmp(book_name,temp->data+4,BOOK_NAME)==0)
        {
        free(temp->data);
        temp->data = malloc(1);
        memcpy(temp->data, &i, 1);
        dictionary->E = dictionary->E - 1;
        return;
        }
        else
        {
            printf("The book is not in the library.\n");
            return;
        }
    }

    string_size = my_strlen(book_name) - 2;
    // if there is more than one element and we do not know which one to delete
    hnode *prev;
    prev = temp;
    while (temp != NULL)
    { // traverse the bucket
        if (memcmp(temp->data + 4, book_name, string_size) == 0)
        {
            // element to be deleted is the first and the bucket has other elements
            if (temp == dictionary->bucket_vector[index])
            {
                dictionary->bucket_vector[index] = temp->next;
                free(temp->data);
                free(temp);
                dictionary->E = dictionary->E - 1;
                return;
            }

            // element to be deleted is the last one in bucket
            if (temp->next == NULL)
            {
                while (prev->next!=temp)
                {
                    prev = prev->next;
                }
                // free(temp->borrowed);
                // free(temp->data);
                // free(temp);
                free_node(temp);
                prev->next = NULL;
                dictionary->E = dictionary->E - 1;
                return;
            }

            // element is between the first and the last one
            while (prev != temp)
            {
                prev = prev->next;
            }
            prev->next = temp->next;
            // free(temp->data);
            // free(temp);
            free_node(temp);
            dictionary->E = dictionary->E - 1;
            return;
        }

        temp = temp->next;
    }

    if (temp == NULL)
    {
        printf("The book is not in the library.\n");
        return;
    }
}

void GET_DEF(htable *dictionary, char *book_name, char *def_key)
{

    hnode *temp;
    temp = find_node(dictionary, book_name);

    if (temp == NULL)
    {
        printf("The book is not in the library.\n");
        return;
    }

    char *result = find_def(temp->data, def_key);
    if (result == NULL)
    {
        printf("The definition is not in the book.\n");
        return;
    }
    printf("%s", result);
}

void ADD_DEF(htable *dictionary, char *book_name, char *def_key, char *def_val)
{
    hnode *temp;
    temp = find_node(dictionary, book_name);
    if (temp == NULL)
    {
        printf("The book is not in the library.\n");
        return;
    }
    char *start_point = find_def((char *)temp->data, def_key);
    // the definition does not exist and i will add it
    if (start_point == NULL)
    {
        int number_def;
        memcpy(&number_def, temp->data, 4);
        temp->data = realloc(temp->data, 12 + BOOK_NAME + (number_def + 1) * 2 * WORD_SIZE);
        int cursor = 12 + BOOK_NAME + number_def * 2 * WORD_SIZE;
        memcpy(temp->data + cursor, def_key, my_strlen(def_key));
        cursor = cursor + 1 + my_strlen(def_key);
        memcpy(temp->data + cursor, def_val, my_strlen(def_val));
        cursor = cursor + my_strlen(def_val);
        number_def = number_def + 1;
        memcpy(temp->data, &number_def, 4);
        unsigned char str_end = 0;
        memcpy(temp->data + cursor, &str_end, 1);
        return;
    }

    // the definition exist and i will update it
    if (start_point != NULL)
    {
        int cursor = my_strlen(def_key) - 1;
        memcpy(start_point + cursor, def_val, my_strlen(def_val) - 2);
        return;
    }
}

void RMV_DEF(htable *dictionary, char *book_name, char *def_key)
{
    hnode *temp;
    temp = find_node(dictionary, book_name);
    if (temp == NULL)
    {
        printf("The book is not in the library.\n");
        return;
    }
    char *result = find_def(temp->data, def_key);
    if (result == NULL)
    {
        printf("The definition is not in the book.\n");
        return;
    }
    int number_def;
    memcpy(&number_def, temp->data, 4);
    int cursor = result - (char *)temp->data;
    int init_size = number_def * 2 * WORD_SIZE + 12 + BOOK_NAME;
    // if the word is inside temp->data and is not he last one
    if (init_size != cursor + 2 * WORD_SIZE)
    {
        cursor = cursor - strlen(def_key);
        memcpy(temp->data + cursor, temp->data + cursor + 2 * WORD_SIZE, init_size - cursor - 2 * WORD_SIZE);
        number_def = number_def - 1;
        memcpy(temp->data, &number_def, 4);
        return;
    }
    int clearer = 0;
    int i = 0;
    for (i = 0; i < 4; i++)
    {
        memcpy(temp->data + cursor, &clearer, 4);
        cursor = cursor + 4;
    }
    number_def = number_def - 1;
    memcpy(temp->data, &number_def, 4);
}
// interested in learning to resize a htable
void ADD_USER(htable *users, char *name)
{
    // 1- the book is in the library, and you can take it
    // 2 -the book is registered in the library but can't be taken
    int index = hash_fct(name) % users->B;
    hnode *temp;
    temp = users->bucket_vector[index];
    if (check_empty(*temp) == 1)
    {
        free(temp->data);
        temp->data = malloc(WORD_SIZE);
        memcpy(temp->data, name, WORD_SIZE);
        users->E = users->E + 1;
    }
    else
    {
        while (temp->next != NULL)
        {
            if (check_empty(*temp) == 1)
            {
                break;
            }
            if (memcmp(temp->data, name, my_strlen(name) - 1) == 0)
            {
                printf("User is already registered.\n");
                return;
            }
            temp = temp->next;
        }
        hnode *new = (hnode *)malloc(sizeof(hnode));
        new->data = malloc(WORD_SIZE);
        new->borrowed = malloc(1);
        memcpy(new->data, name, WORD_SIZE);
        new->next = NULL;
        temp->next = new;
        users->E = users->E + 1;
    }
}

// chunk_size -field dimension BOOK_NAME + sizeof(float)+ sizeof(int) for any book
void TOP_BOOKS(htable *dictionary, int chunk_size)
{
    char *extractt_zip;
    extractt_zip = sort_zip(dictionary, chunk_size);
    int i = 0, cursor = 0;
    printf("Books ranking:\n");
    for (i = 0; i < dictionary->E; i++)
    {
        printf("%d. ", i + 1);
        printme_the_book(extractt_zip + cursor);
        cursor = cursor + chunk_size;
    }
    free(extractt_zip);
}

void TOP_USERS()
{
    printf("Users ranking:\n");
}