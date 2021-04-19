#ifndef HASHMAP_H_INCLUDED
#define HASHMAP_H_INCLUDED

#include <stddef.h>

struct hashmap;

extern struct hashmap *hashmap_new(void);
extern void hashmap_destroy(struct hashmap *this);

extern struct hashmap *hashmap_set_hashfunc(struct hashmap *this, size_t (f)(const char *key));

extern struct hashmap *hashmap_put(struct hashmap *this, const char *key, int value);

extern int hashmap_get(struct hashmap *this, const char *key);

extern struct hashmap *hashmap_clear(struct hashmap *this);

/*  Iteration
 *   */
struct hashmap_iterator;

extern struct hashmap_iterator *hashmap_first(struct hashmap *this);
extern int hashmap_done(struct hashmap *this, struct hashmap_iterator *i);

extern struct hashmap_iterator *hashmap_next(struct hashmap *this, struct hashmap_iterator *i);

extern const char *hashmap_key(struct hashmap_iterator *i);
extern int hashmap_value(struct hashmap_iterator *i);

#endif
