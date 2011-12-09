#include <spacepart/scene.h>

scene_node_t *scene_join_nodes(scene_node_t *a, scene_node_t *b)
{
    if(!a) return b;
    if(!b) return a;

    b->prev->next = a;
    a->prev->next = b;

    scene_node_t *temp = a->prev;
    a->prev = b->prev;
    b->prev = temp;

    return a;
}
