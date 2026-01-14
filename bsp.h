/*
 * R_BSP.h - A Lightweight Binary Space Partitioning (BSP) Layout Manager like bspwm
 */

#ifndef R_BSP_H
#define R_BSP_H

#include <stdbool.h>
#include <stddef.h>
#include "arena.h"

/* --------------------------------------------------------------------------
 * Macros & Utilities
 * -------------------------------------------------------------------------- */

#define RECT_UNWRAP(r) (r).x, (r).y, (r).w, (r).h
#define RECT2RECTANGLE(rect) \
  (Rectangle) { RECT_UNWRAP(rect) }

/* --------------------------------------------------------------------------
 * Data Structures
 * -------------------------------------------------------------------------- */

typedef struct Rect
{
  float x, y, w, h;
} Rect;

typedef struct BSPNode BSPNode;

typedef struct
{
  Rect area;
} Axes;

struct BSPNode
{
  Rect area;         /**< Physical bounds calculated during bsp_set_area */
  BSPNode *a;        /**< Left or Top child */
  BSPNode *b;        /**< Right or Bottom child */
  float split_ratio; /**< 0.0 to 1.0 (relative to parent size) */
  bool vertical;     /**< True for Left|Right, False for Top/Bottom */
  void *user_data;   /**< Generic pointer for user-defined data (e.g., Plot objects) */
};

typedef struct
{
  float margin;  /**< Uniform spacing between nodes and screen borders */
  BSPNode *root; /**< Root of the layout tree */
  Arena *pool;   /**< Arena memory for fast, zero-fragmentation node allocation */
} BSPContext;

/* --------------------------------------------------------------------------
 * Public API
 * -------------------------------------------------------------------------- */

/**
 * Initializes the BSP system and allocates an initial root node.
 * @param margin Space (in pixels) between elements.
 * @param pool_size Bytes to allocate for the internal Arena pool.
 */
BSPContext *bsp_init(float margin, size_t pool_size);

/**
 * Performs a single split on a leaf node.
 * @return The parent node that was just split.
 */
BSPNode *bsp_split(BSPContext *ctx, BSPNode *node, float split_ratio, bool vertical);

/**
 * Divides a node into multiple segments using integer weights (e.g., {1, 2, 1}).
 * * @param count Number of segments to create.
 * @param weights Array of relative proportions.
 * @param vertical If true, splits into columns; if false, splits into rows.
 * @param out_leaves (Optional) An array of size [count] to store pointers to the resulting leaf nodes.
 */
void bsp_split_multi(BSPContext *ctx, BSPNode *node, int count, int *weights, bool vertical, BSPNode **out_leaves);

/**
 * Recursively calculates the physical Rect areas for the entire tree.
 * Call this whenever the window resizes or the tree structure changes.
 * returns whether any dimension has 0
 */
void bsp_set_area(BSPContext *ctx, Rect total_area);

/**
 * Debug utility to print the tree structure to the console.
 */
void bsp_print(BSPNode *node);

#endif // R_BSP_H
