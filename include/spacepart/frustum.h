#ifndef SPACEPART_FRUSTUM_H
#define SPACEPART_FRUSTUM_H

#include <math.h>

struct renderq_node_t;
struct octree_node_t;

struct renderq_node_t *frustum_cull_octree(
    const float * restrict frustum,
    const struct octree_node_t *root,
    struct renderq_node_t **free_nodes);

void frustum_matrix(
    float *matrix,
    float left, float right,
    float bottom, float top,
    float znear, float zfar)
{
    matrix[0] = 2.0 * znear / (right - left);
    matrix[1] = 0.0;
    matrix[2] = 0.0;
    matrix[3] = 0.0;
    matrix[4] = 0.0;
    matrix[5] = 2.0 * znear / (top - bottom);
    matrix[6] = 0.0;
    matrix[7] = 0.0;
    matrix[8] = (right + left) / (right - left);
    matrix[9] = (top + bottom) / (top - bottom);
    matrix[10] = -(zfar + znear) / (zfar - znear);
    matrix[11] = -1.0;
    matrix[12] = 0.0;
    matrix[13] = 0.0;
    matrix[14] = (-2.0 * znear * zfar) / (zfar - znear);
    matrix[15] = 0.0;
}

void frustum_perspective(float *matrix, float fovy, float aspect, float znear, float zfar)
{
    float ymax = znear * tanf(fovy / 2.0);
    float xmax = ymax * aspect;
    frustum_matrix(matrix, -xmax, xmax, -ymax, ymax, znear, zfar);
}

void frustum_planes(float *frustum, const float *matrix)
{
    for(int i = 0; i < 6; ++i)
    {
        float *plane = frustum + i * 4;

        for(int j = 0; j < 4; ++j)
            plane[j] = matrix[3 + 4 * j] + matrix[(i / 2) + 4 * j] * (i & 1 ? 1.0 : -1.0);

        float mag = sqrtf(plane[0]*plane[0] + plane[1]*plane[1] + plane[2]*plane[2]);
        for(int j = 0; j < 4; ++j)
            plane[j] /= mag;
    }
}

#endif
