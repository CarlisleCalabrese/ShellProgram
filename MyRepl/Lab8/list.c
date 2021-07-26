/*
 * list.c
 * David Owen
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAXCHARS 64

typedef struct list_t
{
    int i;
    char *c;
    struct list_t *next;
} list_t;

void add_element(list_t **list, int i, char *c)
{
    list_t *e;

    if (*list == NULL)  // New empty list.
    {
        *list = (list_t *) malloc(sizeof(list_t));
        (*list)->i = i;
        (*list)->c = strndup(c, MAXCHARS);
        (*list)->next = NULL;
    }
    else  // List with at least one element.
    {
        // Loop through elements, so that e is left
        // pointing to the last one in the list.
        for (e = *list; e->next != NULL; e = e->next)
            ; // (Do nothing.)

        e->next = (list_t *) malloc(sizeof(list_t));
        e = e->next;
        e->i = i;
        e->c = strndup(c, MAXCHARS);
        e->next = NULL;
    }
}

int main(int argc, char **argv)
{
    list_t *list = NULL, *e;

    add_element(&list, 97, "ninety-seven");
    add_element(&list, 86, "eighty-six");
    add_element(&list, 75, "seventy-five");
    add_element(&list, 64, "sixty-four");
    add_element(&list, 53, "fifty-three");

    for (e = list; e != NULL; e = e->next)
        printf("%d, %s\n", e->i, e->c);

    return 0;
}