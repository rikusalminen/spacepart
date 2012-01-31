#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include <spacepart/scene.h>
#include <spacepart/renderq.h>
#include <spacepart/octree.h>
#include <spacepart/frustum.h>

static float rnd(float x)
{
    return rand() / (float)RAND_MAX * x;
}

int main()
{
    const float world_size = 1000.0;

    const float frustum[] = {
        -1.0, 0.0, 0.0, world_size,
        1.0, 0.0, 0.0, world_size,
        0.0, -1.0, 0.0, world_size,
        0.0, 1.0, 0.0, world_size,
        0.0, 0.0, -1.0, world_size,
        0.0, 0.0, 1.0, world_size,
    };

    octree_node_t octree_root;
    memset(&octree_root, 0, sizeof(octree_node_t));
    for(int i = 0; i < 3; ++i)
    {
        octree_root.min[i] = -world_size;
        octree_root.max[i] = world_size;
        octree_root.mid[i] = (octree_root.min[i] + octree_root.max[i])/2.0;
    }

    int num_octree_nodes = 8192;
    octree_node_t *octree_nodes = calloc(num_octree_nodes, sizeof(octree_node_t));
    for(octree_node_t *node = octree_nodes+0; node != octree_nodes+num_octree_nodes; ++node)
        node->parent = node+1 == octree_nodes+num_octree_nodes ? NULL : node+1;
    octree_node_t *free_octree_nodes = octree_nodes+0;

    int num_scene_nodes = 1024*32;
    scene_node_t *scene_nodes = calloc(num_scene_nodes, sizeof(scene_node_t));

    const float min_size = 1, max_size = 100;
    for(scene_node_t *scene_node = scene_nodes + 0; scene_node != scene_nodes + num_scene_nodes; ++scene_node)
    {
        scene_node->next = scene_node->prev = scene_node;

        for(int i = 0; i < 3; ++i)
        {
            float size = min_size + rnd(max_size - min_size);
            scene_node->aabb_min[i] = rnd(2.0 * world_size - size) - world_size;
            scene_node->aabb_max[i] = scene_node->aabb_min[i] + size;
        }

        octree_add(&octree_root, scene_node, &free_octree_nodes);
    }

    int num_queue_nodes = num_scene_nodes;
    renderq_node_t *queue_nodes = calloc(num_queue_nodes, sizeof(renderq_node_t));

    renderq_node_t *free_nodes = 0;
    for(renderq_node_t *queue_node = queue_nodes + 0; queue_node != queue_nodes + num_queue_nodes; ++queue_node)
    {
        queue_node->next = queue_node->prev = queue_node;
        free_nodes = renderq_join_siblings(free_nodes, queue_node);
    }

    renderq_node_t *queue = frustum_cull_octree(frustum, &octree_root, &free_nodes);

    float prev_key = -1e10;
    int count = 0;
    while(queue)
    {
        renderq_node_t *node = renderq_extract_min(&queue);

        ++count;

        assert(node->key >= prev_key);
        prev_key = node->key;

        free_nodes = renderq_join_siblings(free_nodes, node);
    }

    assert(count == num_queue_nodes);

    for(scene_node_t *scene_node = scene_nodes + 0; scene_node != scene_nodes + num_scene_nodes; ++scene_node)
        octree_remove(scene_node, &free_octree_nodes);

    free(queue_nodes);
    free(scene_nodes);
    free(octree_nodes);

    return 0;
}
