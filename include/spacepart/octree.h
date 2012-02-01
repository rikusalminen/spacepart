#ifndef SPACEPART_OCTREE_H
#define SPACEPART_OCTREE_H

struct spacepart_node_t;

typedef struct octree_node_t
{
    struct octree_node_t *parent, *children[8];

    float min[3], max[3], mid[3];
    int num_nodes, num_child_nodes;
    struct spacepart_node_t *nodes, *child_nodes;
} octree_node_t;

void octree_add(octree_node_t *root_node, struct spacepart_node_t *spacepart_node, octree_node_t **free_nodes);
void octree_remove(struct spacepart_node_t *spacepart_node, octree_node_t **free_nodes);

#endif
