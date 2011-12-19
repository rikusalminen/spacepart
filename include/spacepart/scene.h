#ifndef SPACEPART_SCENE_H
#define SPACEPART_SCENE_H

struct octree_node_t;

typedef struct scene_node_t
{
    struct octree_node_t *octree_node;
    int octant;

    struct scene_node_t *next, *prev;

    float aabb_min[3], aabb_max[3];
    void *ptr;
} scene_node_t;

scene_node_t *scene_join_nodes(scene_node_t *a, scene_node_t *b);
scene_node_t *scene_node_detach(scene_node_t *node);

#endif
