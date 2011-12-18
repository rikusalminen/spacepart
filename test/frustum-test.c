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

    octree_node_t octree_root = { 0 };
    for(int i = 0; i < 3; ++i)
    {
        octree_root.min[i] = -world_size;
        octree_root.max[i] = world_size;
        octree_root.mid[i] = (octree_root.min[i] + octree_root.max[i])/2.0;
    }

    int num_scene_nodes = 1024*32;
    scene_node_t scene_nodes[num_scene_nodes];
    memset(scene_nodes, 0, sizeof(scene_nodes));

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

        octree_add(&octree_root, scene_node);
    }

    int num_queue_nodes = num_scene_nodes;
    renderq_node_t queue_nodes[num_queue_nodes];
    memset(queue_nodes, 0, sizeof(queue_nodes));

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

        printf("queue node: %4.1f\tmin: (%3.1f, %3.1f, %3.1f)\tmax: (%3.1f, %3.1f, %3.1f)\n",
            node->key,
            node->scene_node->aabb_min[0], node->scene_node->aabb_min[1], node->scene_node->aabb_min[2],
            node->scene_node->aabb_max[0], node->scene_node->aabb_max[1], node->scene_node->aabb_max[2]
            );

        ++count;

        assert(node->key >= prev_key);
        prev_key = node->key;

        free_nodes = renderq_join_siblings(free_nodes, node);
    }

    assert(count == num_queue_nodes);

    printf("%d nodes in queue\n", count);

    for(scene_node_t *scene_node = scene_nodes + 0; scene_node != scene_nodes + num_scene_nodes; ++scene_node)
        octree_remove(scene_node);

    return 0;
}
