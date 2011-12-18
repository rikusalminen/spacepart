#ifndef SPACEPART_FRUSTUM_H
#define SPACEPART_FRUSTUM_H

struct renderq_node_t;
struct octree_node_t;

struct renderq_node_t *frustum_cull_octree(
    const float * restrict frustum,
    const struct octree_node_t *root,
    struct renderq_node_t **free_nodes);

#endif
