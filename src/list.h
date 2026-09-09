#ifndef LIST_H
#define LIST_H

/* macros inspired by bsd list */

#define LIST_FOREACH(var, head) \
	for ((var) = (head);    \
	    (var);              \
	    (var) = (var)->next)

#define LIST_LAST(var, head) do {    \
	(var) = (head);              \
	while ((var) && (var)->next) \
		(var) = (var)->next; \
} while (0)

#define LIST_FOREACH_SAFE(var, head, tvar)  \
	for ((var) = (head);                \
	    (var) && ((tvar) = (var)->next, 1); \
	    (var) = (tvar))

#endif
