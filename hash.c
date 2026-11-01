#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "hash.h"

static void hash_resize(hash_t ** h);

// ----------------------------------------------------------------------------------

static unsigned int hash_index (char *key,int capacity)
{
    unsigned int index = 0;
    int size = strlen(key);
    for (int i = 0; i < size; i++)
    {
        index += (i + 1) * (255 & key[i]);
    }

    return index % capacity;
}

// ----------------------------------------------------------------------------------

static entry_t **entry_init (int capacity)
{
    entry_t **tab = malloc(sizeof(entry_t *) * capacity);

    if (tab == NULL)
    {
        printf("ERROR\n");
        return NULL;
    }

    for (int i = 0; i < capacity; i++)
    {
        tab[i] = NULL;
    }
    
    return tab;
}

// ----------------------------------------------------------------------------------

hash_t *hash_create (int capacity)
{
    if (capacity <= 0) return NULL;
        
    hash_t *new_hash_tab = malloc(sizeof(hash_t));
    if (new_hash_tab == NULL)
    {
        printf("ERROR\n");
        return NULL;
    }

    new_hash_tab->capacity = capacity;
    new_hash_tab->tab = entry_init(new_hash_tab->capacity);

    if (new_hash_tab->tab == NULL)
    {
        free(new_hash_tab);
        printf("ERROR\n");
        return NULL;
    }

    new_hash_tab->size = 0;

    return new_hash_tab;
}

// ----------------------------------------------------------------------------------

static entry_t *create_entry (buffer *add_buffer,char *key)
{
    entry_t *new_entry = malloc(sizeof(entry_t));
    if (new_entry == NULL || add_buffer == NULL || key == NULL)
    {
        if (new_entry != NULL) free(new_entry);
        printf("ERROR\n");
        return NULL;
    }

    if (strlen(key) >= sizeof(new_entry->key))
    {
        free(new_entry);
        printf("ERROR\n");
        return NULL;
    }
    strcpy(new_entry->key,key);
    new_entry->file = add_buffer;
    new_entry->next = NULL;

    return new_entry;
}

// ----------------------------------------------------------------------------------

void hash_push (hash_t **hash,buffer *new_buffer,char *key)
{
    if (hash == NULL || *hash == NULL || new_buffer == NULL || key == NULL)
        return;

    if (hash_get(*hash, key) != NULL)
        return;

    int index = hash_index(key, (*hash)->capacity);
    
    entry_t *new_entry = create_entry(new_buffer, key);

    if (new_entry == NULL)
    {
        printf("ERROR\n");
        return;
    }

    new_entry->next = (*hash)->tab[index];

    (*hash)->tab[index] = new_entry;

        (*hash)->size++;
    if ((float)(*hash)->size / (*hash)->capacity >= 0.75)    
        hash_resize(hash);
}

// ----------------------------------------------------------------------------------

buffer *hash_get (hash_t *hash, char *key)
{
    if (hash == NULL || hash->size == 0 || key == NULL)
        return NULL;

    int index = hash_index(key, hash->capacity);
    
    entry_t *entry = hash->tab[index];

    if (strlen(key) >= sizeof(entry->key))
        return NULL;

    while (entry != NULL)
    {
        if (strcmp(entry->key,key) == 0)
            return entry->file;

        entry = entry->next;
    }
    return NULL;
}

// ----------------------------------------------------------------------------------

void hash_delete (hash_t *hash, char *key)
{
    if (hash == NULL || hash->size == 0 || key == NULL)
        return;

    int index = hash_index(key, hash->capacity);
    
    entry_t **entry = &hash->tab[index];

    while (*entry != NULL)
    {
        if (!strcmp((*entry)->key, key))
        {
            entry_t *tmp = (*entry);

            *entry = (*entry)->next;

            free(tmp);

            hash->size--;

            return;
        }
        entry = &(*entry)->next;
    }
}

// ----------------------------------------------------------------------------------

static int hash_push_resize (hash_t *hash,buffer *new_buffer,char *key)
{
    if (hash == NULL || new_buffer == NULL || key == NULL)
        return 0;
    
    int index = hash_index(key, hash->capacity);

    entry_t *new_entry = create_entry(new_buffer, key);

    if (new_entry == NULL)
        return 0;
    
    new_entry->next = hash->tab[index];

    hash->tab[index] = new_entry;

    hash->size++;

    return 1;
}

// ----------------------------------------------------------------------------------

static int hash_copy_entry (entry_t *e, hash_t *hash)
{
    if(hash == NULL) return 0;
       
    if (e == NULL) return 1;

    if (!hash_push_resize(hash, e->file, e->key)) return 0;
    
    return hash_copy_entry(e->next, hash);
}

// ----------------------------------------------------------------------------------

static void hash_resize (hash_t **hash)
{
    int capacity = (*hash)->capacity * 2;

    hash_t *new_hash = hash_create(capacity);
    if (new_hash == NULL)
        return;
    
    int copy_is_correct = 1;
        
    for (int i = 0; i < (*hash)->capacity; i++)
        copy_is_correct = copy_is_correct && hash_copy_entry((*hash)->tab[i], new_hash);
    
    if (copy_is_correct)
    {    
        hash_destroy(hash);    
        *hash = new_hash;
    }   
    else hash_destroy (&new_hash);
}

// ----------------------------------------------------------------------------------

static void entry_destroy (entry_t *entry)
{
    while (entry != NULL)
    {
        entry_t *tmp_entry = entry;
        entry = entry->next;
        free(tmp_entry);
    }
}

// ----------------------------------------------------------------------------------

void hash_destroy (hash_t **hash)
{
    if (hash == NULL || *hash == NULL) return;
    hash_t *tmp = *hash;
    *hash = NULL;

    for (int i = 0; i < tmp->capacity; i++)
        if (tmp->tab[i] != NULL)
            entry_destroy(tmp->tab[i]);
    free(tmp->tab);    
    free(tmp);        
}

// ----------------------------------------------------------------------------------

static void entry_show (entry_t *entry, int capacity)
{
    while (entry != NULL)
    {
        printf("key : %s.\n", entry->key);
        buffer *e = entry->file;
        printf("name : %s, surname : %s.\nnum = %s\n",e->name, e->surname, e->num);
        printf("index dans la table : %d.\n", hash_index(entry->key, capacity));
        entry = entry->next;
    }
}

// ----------------------------------------------------------------------------------

void hash_show (hash_t *hash)
{
    if (hash == NULL) return;

    printf("size = %d, capacity = %d\n", hash->size, hash->capacity);
    for (int i = 0; i < hash->capacity; i++)
        if (hash->tab[i] != NULL)
            entry_show(hash->tab[i], hash->capacity);
}