/* BSD License
 * Copyright (C) by Intel Crop.
 * Author: Louis Zhuang <louis.zhuang@linux.intel.com>
 */

#ifndef LIST_H
#define LIST_H

struct list_head {
	struct list_head *prev, *next;
};

static inline void list_init(struct list_head *entry)
{
	entry->next 	= entry;
	entry->prev	= entry;
}

static inline void list_add3(struct list_head *entry, 
			     struct list_head *prev, 
			     struct list_head *next)
{
	entry->next	= next;
	prev->next	= entry;
	entry->prev	= prev;
	next->prev	= entry;
}

static inline void list_add(struct list_head *entry,
			    struct list_head *head)
{
	list_add3(entry, head, head->next);
}

static inline void list_add_tail(struct list_head *entry,
				 struct list_head *head)
{
	list_add3(entry, head->prev, head);
}

static inline void list_del2(struct list_head *prev,
			     struct list_head *next)
{
	prev->next 	= next;
	next->prev	= prev;
}

static inline void list_del(struct list_head *entry)
{
	list_del2(entry->prev, entry->next);
}

static inline int list_empty(struct list_head *entry)
{
	return (entry->next == entry);
}

static inline struct list_head *list_first(struct list_head *head)
{
	if ( head->next!=head ) 
		return head->next;
	else
		return NULL;
}

static inline struct list_head *list_last(struct list_head *head)
{
	if ( head->prev!=head )
		return head->prev;
	else
		return NULL;
}

#define list_container(entry, type, member) ({ \
		typeof(((type *)0)->member) *ptr = entry; \
		const size_t off = (size_t)&(((type *)0)->member); \
		(type *)((char *)ptr - off); })

#define list_for_each(pos, head) \
		for(pos = (head)->next; pos != (head); pos = pos->next)
#define list_for_each_safe(pos, n, head) \
		for(pos = (head)->next, n = pos->next; pos != head; \
				pos = n, n = pos->next)
#endif//LIST_H
