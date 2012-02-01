#include <stddef.h>

#include <spacepart/spacepart.h>

spacepart_node_t *spacepart_join_nodes(spacepart_node_t *a, spacepart_node_t *b)
{
    if(!a) return b;
    if(!b) return a;

    b->prev->next = a;
    a->prev->next = b;

    spacepart_node_t *temp = a->prev;
    a->prev = b->prev;
    b->prev = temp;

    return a;
}

spacepart_node_t *spacepart_node_detach(spacepart_node_t *node)
{
    spacepart_node_t *next = node->next != node ? node->next : NULL;

    node->prev->next = node->next;
    node->next->prev = node->prev;

    node->next = node->prev = node;
    return next;
}
