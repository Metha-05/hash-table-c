#ifndef HASH_H
#define HASH_H
// --------------------------------------------
typedef struct 
{
    char name[20];
    char surname[50];
    char num[15];

} buffer;
// --------------------------------------------
typedef struct entry_t
{
    char key[30];
    buffer *file;
    struct entry_t *next;
} entry_t;
// --------------------------------------------
typedef struct 
{
    entry_t **tab;
    int size;
    int capacity;
} hash_t;
// --------------------------------------------
hash_t *hash_create (int capacity);
void hash_push (hash_t **hash,buffer *new_buffer,char *key);
buffer *hash_get (hash_t *hash, char *key);
void hash_delete (hash_t *hash, char *key);
void hash_show (hash_t *hash);
void hash_destroy (hash_t **hash);
// --------------------------------------------
#endif