#include <stddef.h>
#include <stdint.h>

#include <spacepart/renderq.h>

renderq_node_t *renderq_join_siblings(renderq_node_t *a, renderq_node_t *b)
{
    if(!a) return b;
    if(!b) return a;

    b->prev->next = a;
    a->prev->next = b;

    renderq_node_t *temp = a->prev;
    a->prev = b->prev;
    b->prev = temp;

    return a->key <= b->key ? a : b;
}

renderq_node_t *renderq_detach(renderq_node_t *node)
{
    renderq_node_t *next = node->next != node ? node->next : NULL;
    renderq_node_t *child = node->child;

    node->prev->next = node->next;
    node->next->prev = node->prev;

    node->next = node->prev = node;
    node->child = NULL;
    node->degree = -1;

    return renderq_join_siblings(next, child);
}

static renderq_node_t *renderq_combine(renderq_node_t *a, renderq_node_t *b)
{
    renderq_node_t *parent = a->key <= b->key ? a : b;
    renderq_node_t *child = a->key <= b->key ? b : a;

    parent->child = renderq_join_siblings(parent->child, child);
    parent->degree += 1;

    return parent;
}


static int highest_1_bit(uint64_t word)
{
    int low = 0, high = 64;
    while(low != high)
    {
        int mid = low + (high - low) / 2;
        uint64_t mask = ~(0ull) << mid;
        if(word & mask) low = mid+1;
        else high = mid;
    }

    return low-1;
}

static void renderq_degree_bounds(renderq_node_t *root, int *min_deg, int *max_deg)
{
    uint64_t degree_mask = 0;
    int minimum = 64;
    renderq_node_t *node = root;

    do
    {
        degree_mask += 1 << node->degree;
        minimum = minimum < node->degree ? minimum : node->degree;
        node = node->next;
    } while(node != root);

    *max_deg = highest_1_bit(degree_mask);
    *min_deg = minimum;
}

renderq_node_t *renderq_compress(renderq_node_t *root)
{
    if(!root) return NULL;

    int min_degree, max_degree;
    renderq_degree_bounds(root, &min_degree, &max_degree);

    renderq_node_t *nodes_of_degree[max_degree - min_degree + 1];
    for(int deg = min_degree; deg <= max_degree; ++deg)
        nodes_of_degree[deg-min_degree] = NULL;

    renderq_node_t *node = root;
    do
    {
        renderq_node_t *next = node->next;
        node->next = node->prev = node;

        while(nodes_of_degree[node->degree - min_degree])
        {
            renderq_node_t *other = nodes_of_degree[node->degree - min_degree];
            nodes_of_degree[node->degree - min_degree] = NULL;
            node = renderq_combine(node, other);
        }

        nodes_of_degree[node->degree - min_degree] = node;

        node = next;
    } while(node != root);

    renderq_node_t *new_root = 0;
    for(int deg = min_degree; deg <= max_degree; ++deg)
        new_root = renderq_join_siblings(new_root, nodes_of_degree[deg-min_degree]);

    return new_root;
}

renderq_node_t *renderq_extract_min(renderq_node_t **root)
{
    renderq_node_t *node = *root;
    *root = renderq_compress(renderq_detach(node));
    return node;
}
