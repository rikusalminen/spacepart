#ifndef SPACEPART_SPACEPART_H
#define SPACEPART_SPACEPART_H

struct octree_node_t;

typedef struct spacepart_node_t
{
    struct octree_node_t *octree_node;
    int octant;

    struct spacepart_node_t *next, *prev;

    float aabb_min[3], aabb_max[3];
    void *ptr;
} spacepart_node_t;

spacepart_node_t *spacepart_join_nodes(spacepart_node_t *a, spacepart_node_t *b);
spacepart_node_t *spacepart_node_detach(spacepart_node_t *node);

#endif
