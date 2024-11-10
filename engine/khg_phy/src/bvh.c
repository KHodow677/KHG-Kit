/*

  This file is a part of the Nova Physics Engine
  project and distributed under the MIT license.

  Copyright © Kadir Aksoy
  https://github.com/kadir014/nova-physics

*/

#include "khg_phy/bvh.h"
#include "khg_phy/body.h"
#include "khg_phy/collision.h"
#include <math.h>


/**
 * @file bvh.c
 * 
 * @brief Bounding Volume Hierarchy Tree implementation.
 */


phy_bvh_node *phy_bvh_node_new(bool is_leaf, phy_array *bodies) {
    phy_bvh_node *node = malloc(sizeof(phy_bvh_node));
    if (!node) return NULL;

    node->is_leaf = is_leaf;
    node->left = NULL;
    node->right = NULL;
    node->bodies = bodies;

    return node;
}

void phy_bvh_node_free(phy_bvh_node *node) {
    if (!node) return;

    phy_array_free(node->bodies);

    if (!node->is_leaf) {
        phy_bvh_node_free(node->left);
        phy_bvh_node_free(node->right);
    }

    free(node);
}

void phy_bvh_node_build_aabb(phy_bvh_node *node) {
    if (!node) return;
    if (!node->bodies) return;

    if (node->bodies->size > 0) {
        node->aabb = (phy_aabb){ INFINITY, INFINITY, -INFINITY, -INFINITY };

        for (size_t i = 0; i < node->bodies->size; i++) {
            phy_rigid_body *body = node->bodies->data[i];
            phy_aabb aabb = phy_rigid_body_get_aabb(body);

            node->aabb.min_x = fminf(node->aabb.min_x, aabb.min_x);
            node->aabb.min_y = fminf(node->aabb.min_y, aabb.min_y);
            node->aabb.max_x = fmaxf(node->aabb.max_x, aabb.max_x);
            node->aabb.max_y = fmaxf(node->aabb.max_y, aabb.max_y);
        }
    }
}

void phy_bvh_node_subdivide(phy_bvh_node *node) {
    if (!node) return;
    if (node->is_leaf) return;

    float width = node->aabb.max_x - node->aabb.min_x;
    float height = node->aabb.max_y - node->aabb.min_y;

    phy_array *lefts = phy_array_new();
    phy_array *rights = phy_array_new();

    // Current splitting method is midway trough the longest axis

    if (width > height) {
        float split = 0.0;
        for (size_t i = 0; i < node->bodies->size; i++) {
            phy_rigid_body *body = node->bodies->data[i];
            split += body->bvh_median_x;
        }
        split /= (float)node->bodies->size;

        for (size_t i = 0; i < node->bodies->size; i++) {
            phy_rigid_body *body = node->bodies->data[i];
            float c = body->bvh_median_x;

            if (c <= split)
                phy_array_add(lefts, body);
            else
                phy_array_add(rights, body);
        }
    }
    else {
        float split = 0.0;
        for (size_t i = 0; i < node->bodies->size; i++) {
            phy_rigid_body *body = node->bodies->data[i];;
            split += body->bvh_median_y;
        }
        split /= (float)node->bodies->size;

        for (size_t i = 0; i < node->bodies->size; i++) {
            phy_rigid_body *body = node->bodies->data[i];
            float c = body->bvh_median_y;

            if (c <= split)
                phy_array_add(lefts, body);
            else
                phy_array_add(rights, body);
        }
    }

    // Do not split if one of the sides is empty
    if ((lefts->size == 0) || (rights->size == 0)) {
        node->is_leaf = true;
        phy_array_free(lefts);
        phy_array_free(rights);
        return;
    }

    bool left_leaf = lefts->size <= PHY_BVH_LEAF_THRESHOLD;
    bool right_leaf = rights->size <= PHY_BVH_LEAF_THRESHOLD;

    node->left = phy_bvh_node_new(left_leaf, lefts);
    node->right = phy_bvh_node_new(right_leaf, rights);
    if (!node->left || !node->right) return;

    phy_bvh_node_build_aabb(node->left);
    phy_bvh_node_build_aabb(node->right);

    phy_bvh_node_subdivide(node->left);
    phy_bvh_node_subdivide(node->right);
}

void phy_bvh_node_collide(phy_bvh_node *node, phy_aabb aabb, phy_array *collided) {
    if (!node) return;

    if (!phy_collide_aabb_x_aabb(node->aabb, aabb)) return;

    if (node->is_leaf) {
        for (size_t i = 0; i < node->bodies->size; i++) {
            phy_array_add(collided, node->bodies->data[i]);
        }
    }
    else {
        phy_bvh_node_collide(node->left, aabb, collided);
        phy_bvh_node_collide(node->right, aabb, collided);
    }
}

size_t phy_bvh_node_size(phy_bvh_node *node) {
    if (!node) return 0;
    if (node->is_leaf) return 0;
    else {
        size_t a = phy_bvh_node_size(node->left);
        size_t b = phy_bvh_node_size(node->right);
        return 1 + a + b;
    }
}


phy_bvh_node *phy_bvh_tree_new(phy_array *bodies) {
    phy_bvh_node *root = phy_bvh_node_new(false, bodies);

    phy_bvh_node_build_aabb(root);
    phy_bvh_node_subdivide(root);

    return root;
}

void phy_bvh_tree_free(phy_bvh_node *root) {
    phy_bvh_node_free(root->left);
    phy_bvh_node_free(root->right);

    free(root);
}
