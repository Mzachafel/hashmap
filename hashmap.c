#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_ARR_SIZE 256

struct node {
	char *key;
	int value;
	struct node *next;
};

struct hashmap {
	struct node **nodes;
	int size;
	int (*hashfunc)(const char *);
};

struct hashmap *hashmap_new(void)
{
	struct hashmap *hm = (struct hashmap *) malloc(sizeof(struct hashmap));
	if (hm == NULL) { perror("hashmap_new: hm: "); return NULL; }

	hm->nodes = (struct node **) calloc(MAX_ARR_SIZE, sizeof(struct node *));
	if (hm->nodes == NULL) { perror("hashmap_new: hm->nodes: "); return NULL; }
	hm->size = MAX_ARR_SIZE;
	hm->hashfunc = NULL;

	return hm;
}

void node_destroy(struct node *this)
{
	if (this->next != NULL)
		node_destroy(this->next);
	free(this->key);
	free(this);
}

void hashmap_destroy(struct hashmap *this)
{
	if (this == NULL) { fprintf(stderr, "hashmap_destroy: hashmap is not initialized\n"); return; }

	for (int i = 0; i < this->size; i++)
		if (this->nodes[i] != NULL)
			node_destroy(this->nodes[i]);
	free(this->nodes);
	free(this);
}

struct hashmap *hashmap_set_hashfunc(struct hashmap *this, int (*hashfunc)(const char *))
{
	if (this == NULL) { fprintf(stderr, "hashmap_set_hashfunc: hashmap is not initialized\n"); return NULL; }

	this->hashfunc = hashfunc;

	return this;
}

struct hashmap *hashmap_put(struct hashmap *this, const char *key, int value)
{
	if (this == NULL) { fprintf(stderr, "hashmap_put: hashmap is not initialized\n"); return NULL; }
	if (key == NULL) { fprintf(stderr, "hashmap_put: key is empty\n"); return NULL; }

	int index = this->hashfunc(key) % MAX_ARR_SIZE;
	if (index < 0) index *= -1;
	struct node *tmp;
	if (this->nodes[index] == NULL) {
		tmp = this->nodes[index] = (struct node *) malloc(sizeof(struct node));
		if (tmp == NULL) { perror("hashmap_put: tmp: "); return NULL; }
	} else {
		tmp = this->nodes[index];
		while (tmp->next != NULL)
			tmp = tmp->next;
		tmp->next = (struct node *) malloc(sizeof(struct node));
		if (tmp->next == NULL) { perror("hashmap_put: tmp->next: "); return NULL; }
		tmp = tmp->next;
	}
	tmp->key = strdup(key);
	tmp->value = value;
	tmp->next = NULL;

	return this;
}

int hashmap_get(struct hashmap *this, const char *key)
{
	if (this == NULL) { fprintf(stderr, "hashmap_get: hashmap is not initialized\n"); return 0; }
	if (key == NULL) { fprintf(stderr, "hashmap_get: key is empty\n"); return 0; }

	int index = this->hashfunc(key) % MAX_ARR_SIZE;
	if (index < 0) index *= -1;
	struct node *tmp = this->nodes[index];
	if (tmp != NULL) {
		while (tmp != NULL) {
			if (!strcmp(tmp->key, key))
				return tmp->value;
			tmp = tmp->next;
		}
		fprintf(stderr, "hashmap_get: no entry with given key\n"); return 0;
	} else { fprintf(stderr, "hashmap_get: no entry with given key\n"); return 0; }
}

struct hashmap *hashmap_clear(struct hashmap *this)
{
	if (this == NULL) { fprintf(stderr, "hashmap_clear: hashmap is not initialized\n"); return NULL; }

	for (int i = 0; i < this->size; i++)
		if (this->nodes[i] != NULL) {
			node_destroy(this->nodes[i]);
			this->nodes[i] = NULL;
		}

	return this;
}

struct hashmap_iterator {
	struct node *nd;
	int index;
};

struct hashmap_iterator *hashmap_first(struct hashmap *this)
{
	if (this == NULL) { fprintf(stderr, "hashmap_first: hashmap is not initialized\n"); return NULL; }

	int index = 0;
	while (index < this->size && this->nodes[index] == NULL)
		index++;
	struct hashmap_iterator *i = (struct hashmap_iterator *) malloc(sizeof(struct hashmap_iterator));
	if (i == NULL) { perror("hashmap_first: i: "); return NULL; }
	i->index = index;
	i->nd = (index == this->size) ?	NULL : this->nodes[index];
	return i;
}

int hashmap_done(struct hashmap *this, struct hashmap_iterator *i)
{
	if (this == NULL) { fprintf(stderr, "hashmap_done: hashmap is not initialized\n"); return -1; }
	if (i == NULL) { fprintf(stderr, "hashmap_done: iterator is not initialized\n"); return -1; }

	return this->size == i->index;
}

struct hashmap_iterator *hashmap_next(struct hashmap *this, struct hashmap_iterator *i)
{
	if (this == NULL) { fprintf(stderr, "hashmap_done: hashmap is not initialized\n"); return NULL; }
	if (i == NULL) { fprintf(stderr, "hashmap_done: iterator is not initialized\n"); return NULL; }

	if (i->nd->next != NULL)
		i->nd = i->nd->next;
	else {
		i->index++;
		while (i->index < this->size && this->nodes[i->index] == NULL)
			i->index++;
		if (i->index == this->size)
			return i;
		i->nd = this->nodes[i->index];
	}

	return i;
}

const char *hashmap_key(struct hashmap_iterator *i)
{
	return i->nd->key;
}

int hashmap_value(struct hashmap_iterator *i)
{
	return i->nd->value;
}
