#ifndef SPACEPART_OCTREE_H
#define SPACEPART_OCTREE_H

struct scene_node_t;

typedef struct octree_node_t
{
    struct octree_node_t *parent, *children[8];

    float min[3], max[3], mid[3];
    int num_nodes, num_child_nodes;
    struct scene_node_t *nodes, *child_nodes;
} octree_node_t;

void octree_add(octree_node_t *root_node, struct scene_node_t *scene_node);
void octree_remove(struct scene_node_t *scene_node);

extern int octree_allocs;

#endif
