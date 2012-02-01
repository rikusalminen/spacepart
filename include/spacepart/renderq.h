#ifndef SPACEPART_RENDERQ_H
#define SPACEPART_RENDERQ_H

struct spacepart_node_t;

typedef struct renderq_node_t
{
    struct renderq_node_t *child, *next, *prev;
    float key;
    int degree;
    const struct spacepart_node_t *spacepart_node;
} renderq_node_t;

renderq_node_t *renderq_join_siblings(renderq_node_t *a, renderq_node_t *b);
renderq_node_t *renderq_detach(renderq_node_t *node);
renderq_node_t *renderq_compress(renderq_node_t *root);
renderq_node_t *renderq_extract_min(renderq_node_t **root);

#endif
