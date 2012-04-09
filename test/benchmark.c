#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#include <spacepart/spacepart.h>
#include <spacepart/octree.h>
#include <spacepart/renderq.h>
#include <spacepart/frustum.h>

const float world_size = 1000.0;
const float min_size = 1.0, max_size = 100.0;

static void move_node(spacepart_node_t *node)
{
    for(int i = 0; i < 3; ++i)
    {
        float size = min_size + drand48() * (max_size - min_size);
        node->aabb_min[i] = drand48() * (2.0 * world_size - size) - world_size;
        node->aabb_max[i] = node->aabb_min[i] + size;
    }
}

static uint64_t timespec2uint64(const struct timespec* ts)
{
    const uint64_t second = 1000000000ULL;
    return ts->tv_sec * second + ts->tv_nsec;
}

static uint64_t timespec_diff(const struct timespec *ts1, const struct timespec *ts2)
{
    return timespec2uint64(ts2) - timespec2uint64(ts1);
}

static int run_benchmarks(
    octree_node_t *octree_root,
    octree_node_t **free_octree_nodes,
    const int num_octree_nodes,
    spacepart_node_t *spacepart_nodes,
    const int num_spacepart_nodes,
    renderq_node_t **free_renderq_nodes,
    const int num_renderq_nodes
    )
{
    struct timespec timestamps[6];
    int stamp = 0;

    clock_gettime(CLOCK_MONOTONIC, &timestamps[stamp]);
    stamp++;

    for(spacepart_node_t *node = spacepart_nodes+0; node != spacepart_nodes+num_spacepart_nodes; ++node)
        octree_add(octree_root, node, free_octree_nodes);

    clock_gettime(CLOCK_MONOTONIC, &timestamps[stamp]);
    stamp++;

    for(spacepart_node_t *node = spacepart_nodes+0; node != spacepart_nodes+num_spacepart_nodes; ++node)
    {
        octree_remove(node, free_octree_nodes);
        move_node(node);
        octree_add(octree_root, node, free_octree_nodes);
    }

    clock_gettime(CLOCK_MONOTONIC, &timestamps[stamp]);
    stamp++;

    const float frustum[] = {
        -1.0, 0.0, 0.0, world_size,
        1.0, 0.0, 0.0, world_size,
        0.0, -1.0, 0.0, world_size,
        0.0, 1.0, 0.0, world_size,
        0.0, 0.0, -1.0, world_size,
        0.0, 0.0, 1.0, world_size,
    };

    renderq_node_t *renderq = frustum_cull_octree(frustum, octree_root, free_renderq_nodes);

    clock_gettime(CLOCK_MONOTONIC, &timestamps[stamp]);
    stamp++;

    while(renderq)
    {
        renderq_node_t *node = renderq_extract_min(&renderq);
        *free_renderq_nodes = renderq_join_siblings(*free_renderq_nodes, node);
    }

    clock_gettime(CLOCK_MONOTONIC, &timestamps[stamp]);
    stamp++;

    for(spacepart_node_t *node = spacepart_nodes+0; node != spacepart_nodes+num_spacepart_nodes; ++node)
        octree_remove(node, free_octree_nodes);

    clock_gettime(CLOCK_MONOTONIC, &timestamps[stamp]);
    stamp++;

    printf("%8d\t", num_spacepart_nodes);
    for(int s = 0; s < stamp-1; ++s)
        printf("%10lu\t", timespec_diff(&timestamps[s], &timestamps[s+1]));

    printf("%10lu\n", timespec_diff(&timestamps[0], &timestamps[stamp-1]));

    return 0;
}

static int benchmark(int num_octree_nodes, int num_spacepart_nodes, int num_renderq_nodes)
{

    octree_node_t *octree_nodes = calloc(num_octree_nodes, sizeof(octree_node_t));
    spacepart_node_t *spacepart_nodes = calloc(num_spacepart_nodes, sizeof(spacepart_node_t));
    renderq_node_t *renderq_nodes = calloc(num_renderq_nodes, sizeof(renderq_node_t));

    octree_node_t *octree_root = octree_nodes+0;
    for(int i = 0; i < 3; ++i)
    {
        octree_root->min[i] = -world_size;
        octree_root->max[i] = world_size;
        octree_root->mid[i] = (octree_root->min[i] + octree_root->max[i])/2.0;
    }

    for(octree_node_t *node = octree_nodes+1; node != octree_nodes+num_octree_nodes; ++node)
        node->parent = node+1 == octree_nodes+num_octree_nodes ? NULL : node+1;
    octree_node_t *free_octree_nodes = octree_nodes+1;

    for(spacepart_node_t *node = spacepart_nodes+0; node != spacepart_nodes+num_spacepart_nodes; ++node)
    {
        node->next = node->prev = node;
        move_node(node);

    }

    renderq_node_t *free_renderq_nodes = NULL;
    for(renderq_node_t *node = renderq_nodes+0; node != renderq_nodes+num_renderq_nodes; ++node)
    {
        node->next = node->prev = node;
        free_renderq_nodes = renderq_join_siblings(free_renderq_nodes, node);
    }

    int err = 0;

    const char *titles[] = { "nodes", "add", "mutate", "cull", "flatten", "remove" };
    for(int s = 0; s < sizeof(titles)/sizeof(*titles); ++s)
        printf("%s\t", titles[s]);
    printf("\n");

    for(int i = 0; i < 100; ++i)
    {
        err |= run_benchmarks(octree_root, &free_octree_nodes, num_octree_nodes, spacepart_nodes, (i+1)*num_spacepart_nodes/100, &free_renderq_nodes, num_renderq_nodes);
    }

    free(octree_nodes);
    free(spacepart_nodes);
    free(renderq_nodes);

    return err;
}

int main()
{
    srand48(time(NULL));
    return benchmark(1 << 7, 1 << 20, 1 << 20);
}
