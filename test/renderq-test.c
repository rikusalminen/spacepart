#include <assert.h>
#include <stdlib.h>

#include <spacepart/renderq.h>

int main()
{
    int num_nodes = 1024;
    renderq_node_t nodes[num_nodes];

    for(renderq_node_t *node = nodes+0; node != nodes+num_nodes; ++node)
    {
        node->child = NULL;
        node->next = node->prev = node;
        node->degree = 0;
        node->key = rand() % 1000;
    }

    renderq_node_t *root = NULL;

    for(renderq_node_t *node = nodes+0; node != nodes+num_nodes; ++node)
        root = renderq_join_siblings(root, node);

    root = renderq_compress(root);

    int count = 0;
    float previous = -1;
    while(root)
    {
        renderq_node_t *node = renderq_extract_min(&root);
        count += 1;
        assert(node->key >= previous);
        previous = node->key;
    }

    assert(count == num_nodes);

    return 0;
}
