#ifndef SPIDER_H
#define SPIDER_H

#define URL_MAX_LENGTH 2048 // https://www.sitemaps.org/protocol.html [This value must be less than 2048 characters.]

typedef struct _list {
    char *data;
    struct _list *next;
} list;

void append_list(list *head, list *new) {
    list *lptr = head;
    for(;lptr->next; lptr = lptr->next);
    lptr->next = new;
}

void backtrack(char *alphabet, char *prev_str, int index, list *result, int n, CURL *curl);

#endif /* SPIDER_H*/