/*
 * Implementation of the hash table type.
 *
 * Author : Stephen Smalley, <sds@epoch.ncsc.mil>
 *          Paul Merrill, <napalminc@gmail.com>
 */
#include <stdlib.h>
#include <string.h>
#include "hashtab.h"

struct hashtab *hashtab_create(unsigned (*hash_value)(struct hashtab *h, void *key),
			       int (*keycmp)(struct hashtab *h, void *key1, void *key2),
			       unsigned size)
{
	struct hashtab *p;
	unsigned i;

	p = malloc(sizeof(*p));

	p->size = size;
	p->nel = 0;
	p->hash_value = hash_value;
	p->keycmp = keycmp;
	p->htable = malloc(sizeof(struct hashtab_node*) * size);

	for (i = 0; i < size; i++)
		p->htable[i] = NULL;

	return p;
}

int hashtab_insert(struct hashtab *h, void *key, void *datum)
{
	unsigned hvalue;
	struct hashtab_node *prev, *cur, *newnode;

	hvalue = h->hash_value(h, key);
	prev = NULL;
	cur = h->htable[hvalue];
	while (cur && h->keycmp(h, key, cur->key) > 0) {
		prev = cur;
		cur = cur->next;
	}

	if (cur && (h->keycmp(h, key, cur->key) == 0))
		return -1;

	newnode = malloc(sizeof(*newnode));
	newnode->key = key;
	newnode->datum = datum;
	if (prev) {
		newnode->next = prev->next;
		prev->next = newnode;
	}
	else {
		newnode->next = h->htable[hvalue];
		h->htable[hvalue] = newnode;
	}

	h->nel++;
	return 0;
}

int hashtab_remove(struct hashtab *h, void *key)
{
	unsigned hvalue;
	struct hashtab_node *prev, *cur;

	hvalue = h->hash_value(h, key);
	prev = NULL;
	cur = h->htable[hvalue];
	while (cur && h->keycmp(h, key, cur->key) > 0) {
		prev = cur;
		cur = cur->next;
	}

	if (!cur || (h->keycmp(h, key, cur->key) != 0))
		return -1;

	if (prev)
		prev->next = cur->next;
	else
		h->htable[hvalue] = NULL;
	free(cur);

	h->nel--;
	return 0;
}

void *hashtab_search(struct hashtab *h, void *key)
{
	unsigned hvalue;
	struct hashtab_node *cur;

	if (!h)
		return NULL;

	hvalue = h->hash_value(h, key);
	cur = h->htable[hvalue];
	while (cur && h->keycmp(h, key, cur->key) > 0)
		cur = cur->next;

	if (cur == NULL || (h->keycmp(h, key, cur->key) != 0))
		return NULL;

	return cur->datum;
}

void hashtab_destroy(struct hashtab *h)
{
	unsigned i;
	struct hashtab_node *cur, *temp;

	if (!h)
		return;

	for (i = 0; i < h->size; i++) {
		cur = h->htable[i];
		while (cur) {
			temp = cur;
			cur = cur->next;
			free(temp);
		}
		h->htable[i] = NULL;
	}

	free(h->htable);
	h->htable = NULL;

	free(h);
}

void hashtab_stat(struct hashtab *h, struct hashtab_info *info)
{
	unsigned i, chain_len, slots_used, max_chain_len;
	struct hashtab_node *cur;

	slots_used = 0;
	max_chain_len = 0;
	for (slots_used = max_chain_len = i = 0; i < h->size; i++) {
		cur = h->htable[i];
		if (cur) {
			slots_used++;
			chain_len = 0;
			while (cur) {
				chain_len++;
				cur = cur->next;
			}

			if (chain_len > max_chain_len)
				max_chain_len = chain_len;
		}
	}

	info->slots_used = slots_used;
	info->max_chain_len = max_chain_len;
}

unsigned str_hash(struct hashtab *h, void *data)
{
	// FNV-1a hash algorithm.
	// see http://www.isthe.com/chongo/tech/comp/fnv/

	char *str = data;
	unsigned i = 2166136261;
	for (i; *str; str++) {
		i ^= *str;
		i *= 16777619;
	}
	return i % h->size;
}

int strcmp_hash(struct hashtab *h, void *a, void *b)
{
	return strcmp(a, b);
}
