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

    int num_nodes = 128*1024;
    scene_node_t nodes[num_nodes];

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

        octree_add(&octree_root, node);
    }

    printf("%d allocs\n", octree_allocs);

    for(scene_node_t *node = nodes; node != nodes + num_nodes; ++node)
        octree_remove(node);

    printf("%d allocs\n", octree_allocs);
    assert(octree_allocs == 0);

    return 0;
}
