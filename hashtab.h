/*
 * A hash table (hashtab) maintains associations between
 * key values and datum values.  The type of the key values
 * and the type of the datum values is arbitrary.  The
 * functions for hash computation and key comparison are
 * provided by the creator of the table.
 *
 * Author : Stephen Smalley, <sds@epoch.ncsc.mil>
 *          Paul Merrill, <napalminc@gmail.com>
 */
#ifndef _SS_HASHTAB_H_
#define _SS_HASHTAB_H_

#define HASHTAB_MAX_NODES	0xffffffff

struct hashtab_node {
	void *key;
	void *datum;
	struct hashtab_node *next;
};

struct hashtab {
	struct hashtab_node **htable;	/* hash table */
	unsigned size;			/* number of slots in hash table */
	unsigned nel;			/* number of elements in hash table */
	unsigned (*hash_value)(struct hashtab *h, void *key);
					/* hash function */
	int (*keycmp)(struct hashtab *h, void *key1, void *key2);
					/* key comparison function */
};

struct hashtab_info {
	unsigned slots_used;
	unsigned max_chain_len;
};

/*
 * Creates a new hash table with the specified characteristics.
 *
 * Returns NULL if insufficent space is available or
 * the new hash table otherwise.
 */
struct hashtab *hashtab_create(unsigned (*hash_value)(struct hashtab *h, void *key),
			       int (*keycmp)(struct hashtab *h, void *key1, void *key2),
			       unsigned size);

/*
 * Inserts the specified (key, datum) pair into the specified hash table.
 *
 * Returns -1 there is already an entry with the same key or
 * 0 otherwise.
 */
int hashtab_insert(struct hashtab *h, void *k, void *d);

/*
 * Removes the matching (key, datum) pair from the specified hash table.
 *
 * Returns -1 if the key was not found or
 * 0 otherwise.
 */
int hashtab_remove(struct hashtab *h, void *key);

/*
 * Searches for the entry with the specified key in the hash table.
 *
 * Returns NULL if no entry has the specified key or
 * the datum of the entry otherwise.
 */
void *hashtab_search(struct hashtab *h, void *k);

/*
 * Destroys the specified hash table.
 */
void hashtab_destroy(struct hashtab *h);

/* Fill info with some hash table statistics */
void hashtab_stat(struct hashtab *h, struct hashtab_info *info);

/* hash_value and hashcmp functions for char*s */
unsigned str_hash(struct hashtab *h, void *data);
int strcmp_hash(struct hashtab *h, void *a, void *b);

#endif
