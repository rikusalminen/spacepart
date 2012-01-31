#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include <spacepart/scene.h>
#include <spacepart/octree.h>

static float rnd(float x)
{
    return rand() / (float)RAND_MAX * x;
}

int main()
{
    octree_node_t octree_root;
    memset(&octree_root, 0, sizeof(octree_node_t));

    const float world_size = 1000;
    for(int i = 0; i < 3; ++i)
    {
        octree_root.min[i] = -world_size;
        octree_root.max[i] = world_size;
        octree_root.mid[i] = (octree_root.min[i] + octree_root.max[i]) / 2.0;
    }

    int num_octree_nodes = 1024; // way too small, should test OOM
    octree_node_t *octree_nodes = calloc(num_octree_nodes, sizeof(octree_node_t));
    for(octree_node_t *node = octree_nodes+0; node != octree_nodes+num_octree_nodes; ++node)
        node->parent = node+1 == octree_nodes+num_octree_nodes ? NULL : node+1;
    octree_node_t *free_octree_nodes = octree_nodes+0;

    int num_nodes = 128*1024;
    scene_node_t *nodes = calloc(num_nodes, sizeof(scene_node_t));

    const float min_size = 1, max_size = 100;
    for(scene_node_t *node = nodes; node != nodes + num_nodes; ++node)
    {
        memset(node, 0, sizeof(scene_node_t));
        node->next = node->prev = node;

        for(int i = 0; i < 3; ++i)
        {
            float size = min_size + rnd(max_size - min_size);
            node->aabb_min[i] = rnd(2.0 * world_size - size) - world_size;
            node->aabb_max[i] = node->aabb_min[i] + size;
        }

        octree_add(&octree_root, node, &free_octree_nodes);
    }

    for(scene_node_t *node = nodes; node != nodes + num_nodes; ++node)
        octree_remove(node, &free_octree_nodes);

    free(nodes);
    free(octree_nodes);

    return 0;
}
