#ifndef __HASH_H__
#define __HASH_H__

/*
 * hash.h - Hash table mapping a string key to a pointer to a buffer.
 *          Collisions are handled by separate chaining.
 *
 * Ownership rules (apply to every function below):
 *   - The table stores the buffer POINTER. It never copies a buffer and
 *     never frees one.
 *   - The caller must keep each buffer valid as long as it is in the table,
 *     and frees it itself (also after hash_delete / hash_destroy).
 *   - Keys ARE copied into the table. A key is at most 29 bytes
 *     (the 30th byte is the terminating '\0'). Bytes, not characters:
 *     an accented letter takes 2 bytes in UTF-8.
 */

// --------------------------------------------

/* Data stored for one entry (example payload: a phone book record). */
typedef struct 
{
    char name[20];
    char surname[50];
    char num[15];

} buffer;

// --------------------------------------------

/* One node of a bucket's linked list. Internal: do not touch directly. */
typedef struct entry_t
{
    char key[30];
    buffer *file;
    struct entry_t *next;
} entry_t;

// --------------------------------------------

/*
 * The table itself. Internal layout: use the functions below instead of
 * reading or writing the fields, except size and capacity (read only).
 *   tab      : array of 'capacity' bucket heads
 *   size     : number of entries currently stored
 *   capacity : number of buckets
 */
typedef struct 
{
    entry_t **tab;
    int size;
    int capacity;
} hash_t;

// --------------------------------------------

/*
 * hash_create - Create an empty table with 'capacity' buckets.
 *
 * Returns the new table, or NULL if capacity <= 0 or allocation fails
 * (an "ERROR" line may be printed on stdout in the second case).
 * Release it with hash_destroy.
 */
hash_t *hash_create (int capacity);

/*
 * hash_push - Insert new_buffer under key.
 *
 * hash       : address of the caller's table pointer. It is a pointer to a
 *              pointer because the table may be rebuilt (see Growth).
 * new_buffer : stored by address, not copied (see ownership rules).
 * key        : copied. Compared byte by byte, case-sensitive.
 *
 * Does nothing, silently, if: an argument is NULL, the key already exists
 * (the existing value is kept, not replaced), the key is 30 bytes or
 * longer, or an allocation fails. The caller cannot tell these cases apart,
 * because the function returns nothing. "ERROR" may be printed on stdout.
 *
 * Growth: when size / capacity reaches 0.75 after an insertion, the table
 * is rebuilt with twice the capacity and *hash is replaced by the new one.
 * Any other copy of the old hash_t pointer becomes invalid. If the rebuild
 * fails, the table stays valid with its old capacity (the new entry is in).
 */
void hash_push (hash_t **hash,buffer *new_buffer,char *key);

/*
 * hash_get - Look up the buffer stored under key.
 *
 * Returns the stored pointer (not a copy), or NULL if the key is absent or
 * an argument is NULL.
 */
buffer *hash_get (hash_t *hash, char *key);

/*
 * hash_delete - Remove the entry stored under key.
 *
 * Frees the entry but NOT the buffer it pointed to: the caller still owns
 * it. Does nothing if the key is absent or an argument is NULL.
 */
void hash_delete (hash_t *hash, char *key);

/*
 * hash_show - Debug helper. Prints size and capacity, then for each entry
 * its key, the buffer's name / surname / num, and its bucket index (stdout).
 *
 * Reads every stored buffer, so they must all still be valid.
 * Does nothing if hash is NULL.
 */
void hash_show (hash_t *hash);

/*
 * hash_destroy - Free the table and all its entries, then set *hash to NULL.
 *
 * Buffers are NOT freed (see ownership rules). Safe to call with NULL or
 * with a pointer that is already NULL.
 */
void hash_destroy (hash_t **hash);

// --------------------------------------------
#endif
