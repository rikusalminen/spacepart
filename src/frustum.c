#include <stddef.h>
#include <stdbool.h>

#include <spacepart/renderq.h>
#include <spacepart/octree.h>
#include <spacepart/scene.h>

typedef enum {
    FRUSTUM_OUTSIDE = 0,
    FRUSTUM_INTERSECTS,
    FRUSTUM_INSIDE
} frustum_cull_t;

static frustum_cull_t frustum_cull_aabb(
    const float * restrict frustum,
    const float * restrict aabb_min,
    const float * restrict aabb_max)
{
    bool intersects = false;

    for(const float *plane = frustum; plane != frustum + 6 * 4; plane += 4)
    {
        float min_dist =
            (plane[0] < 0.0 ? aabb_max[0] : aabb_min[0]) * plane[0] +
            (plane[1] < 0.0 ? aabb_max[1] : aabb_min[1]) * plane[1] +
            (plane[2] < 0.0 ? aabb_max[2] : aabb_min[2]) * plane[2] +
            -plane[3];
        float max_dist =
            (plane[0] < 0.0 ? aabb_min[0] : aabb_max[0]) * plane[0] +
            (plane[1] < 0.0 ? aabb_min[1] : aabb_max[1]) * plane[1] +
            (plane[2] < 0.0 ? aabb_min[2] : aabb_max[2]) * plane[2] +
            -plane[3];

        if(min_dist > 0 && max_dist > 0) return FRUSTUM_OUTSIDE;
        intersects = intersects || min_dist * max_dist < 0;
    }

    return intersects ? FRUSTUM_INTERSECTS : FRUSTUM_INSIDE;
}

static float frustum_cull_depth(
    const float * restrict frustum,
    const float * restrict aabb_min,
    const float * restrict aabb_max)
{
    const float *plane = frustum + 4 * 4; // near plane
    return -(
        (plane[0] < 0.0 ? aabb_min[0] : aabb_max[0]) * plane[0] +
        (plane[1] < 0.0 ? aabb_min[1] : aabb_max[1]) * plane[1] +
        (plane[2] < 0.0 ? aabb_min[2] : aabb_max[2]) * plane[2] +
        -plane[3]);
}

static renderq_node_t *frustum_cull_nodes(
    const float * restrict frustum,
    frustum_cull_t visibility,
    const scene_node_t *list,
    renderq_node_t **free_nodes)
{
    if(!list) return NULL;

    renderq_node_t *queue = NULL;
    const scene_node_t *scene_node = list;
    do
    {
        frustum_cull_t node_visibility =
            visibility == FRUSTUM_INTERSECTS ?
            frustum_cull_aabb(frustum, scene_node->aabb_min, scene_node->aabb_max) :
            visibility;

        if(node_visibility != FRUSTUM_OUTSIDE)
        {
            renderq_node_t *queue_node = *free_nodes;
            if(!queue_node) break;
            *free_nodes = renderq_detach(*free_nodes);

            queue_node->key = frustum_cull_depth(frustum, scene_node->aabb_min, scene_node->aabb_max);
            queue_node->scene_node = scene_node;
            queue_node->degree = 0;

            queue = renderq_join_siblings(queue, queue_node);
        }

        scene_node = scene_node->next;
    } while (scene_node != list);

    return renderq_compress(queue);
}

static renderq_node_t *frustum_cull_octree_node(
    const float * restrict frustum,
    frustum_cull_t visibility,
    int traversal_order,
    const octree_node_t *node,
    renderq_node_t **free_nodes)
{
    renderq_node_t *queue =
        renderq_join_siblings(
            frustum_cull_nodes(frustum, visibility, node->nodes, free_nodes),
            frustum_cull_nodes(frustum, visibility, node->child_nodes, free_nodes));

    for(int i = 0; i < 8; ++i)
    {
        int child = (traversal_order >> (i * 3)) & 0x3;
        octree_node_t *child_node = node->children[child];
        if(!child_node) continue;

        frustum_cull_t child_visibility =
            visibility == FRUSTUM_INTERSECTS ?
            frustum_cull_aabb(frustum, child_node->min, child_node->max) :
            visibility;

        if(child_visibility == FRUSTUM_OUTSIDE) continue;

        queue =
            renderq_join_siblings(
                queue,
                frustum_cull_octree_node(frustum, child_visibility, traversal_order, child_node, free_nodes));
    }

    return renderq_compress(queue);
}

static int frustum_traversal_order(const float *frustum)
{
#define ABS(x) ((x) < 0 ? -(x) : (x))
    const float *near_plane = frustum + 4*4;
    int major_axis =
        (ABS(near_plane[0]) >= ABS(near_plane[1]) && ABS(near_plane[0]) >= ABS(near_plane[2])) ? 0 :
        (ABS(near_plane[1]) >= ABS(near_plane[2])) ? 1 : 2;
    int middle_axis =
        (ABS(near_plane[(major_axis+1)%3]) >= ABS(near_plane[(major_axis+2)%3])) ?
        (major_axis+1)%3 : (major_axis+2)%3;
    int minor_axis =
        (ABS(near_plane[(major_axis+1)%3]) >= ABS(near_plane[(major_axis+2)%3])) ?
        (major_axis+2)%3 : (major_axis+1)%3;
#undef ABS

    int order = 0;
    for(int i = 0; i < 8; ++i)
        order |=
            (((((i & 4) != 0)^(near_plane[major_axis] < 0)) ? 4 : 0) |
            ((((i & 2) != 0)^(near_plane[middle_axis] < 0)) ? 2 : 0) |
            ((((i & 1) != 0)^(near_plane[minor_axis] < 0)) ? 1 : 0))
            << (i*3);

    return order;
}

renderq_node_t *frustum_cull_octree(
    const float * restrict frustum,
    const octree_node_t *root,
    renderq_node_t **free_nodes)
{
    frustum_cull_t visibility = frustum_cull_aabb(frustum, root->min, root->max);

    if(visibility == FRUSTUM_OUTSIDE)
        return frustum_cull_nodes(frustum, FRUSTUM_INTERSECTS, root->nodes, free_nodes);

    int traversal_order = frustum_traversal_order(frustum);
    return frustum_cull_octree_node(frustum, visibility, traversal_order, root, free_nodes);
}
