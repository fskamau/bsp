#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#define ARENA_IMPLEMENTATION
#include "arena.h"
#include "bsp.h"

#include "raylib.h"

int _bsp_get_height(BSPNode *node);
void _bsp_set_area_recursive(BSPNode *node, Rect area, float spacing);
void _bsp_print_recursive(BSPNode *node, char *prefix, bool is_last);
void bsp_split_multi(BSPContext *ctx, BSPNode *node, int count, int *ratio, bool vertical, BSPNode **out_leaves);

BSPContext *bsp_init(float margin, size_t pool_size)
{
  Arena *arena = arena_create(pool_size);
  BSPContext *ctx = arena_alloc_or_die(arena, sizeof(BSPContext));
  ctx->pool = arena;
  ctx->margin = margin;
  ctx->root = arena_alloc_or_die(arena, sizeof(BSPNode));
  ctx->root->a = ctx->root->b = NULL;
  ctx->margin = margin;
  return ctx;
}

BSPNode *bsp_split(BSPContext *ctx, BSPNode *node, float split_ratio, bool vertical)
{
  assert(ctx != NULL);
  assert(node != NULL);
  assert(split_ratio >= 0.f && split_ratio <= 1.f && "Invalid split_ratio");
  assert(!node->a && !node->b && "Invalid | already split node");
  BSPNode *a, *b;
  node->split_ratio = split_ratio;
  node->vertical = vertical;

  a = arena_alloc_or_die(ctx->pool, sizeof(*a));
  *a = (BSPNode){0};
  node->a = a;

  b = arena_alloc_or_die(ctx->pool, sizeof(*b));
  ;
  *b = (BSPNode){0};
  node->b = b;

  return node;
}

void _bsp_split_multi_recursive(BSPContext *ctx, BSPNode *node, int count, int *weights, bool vertical, int total_weight, BSPNode **out_leaves)
{
  if (count < 2 || total_weight <= 0)
    return;
  float local_ratio = (float)weights[0] / (float)total_weight;
  bsp_split(ctx, node, local_ratio, vertical);
  if (out_leaves)
  {
    out_leaves[0] = node->a;
    out_leaves++;
  }
  if (count > 2)
  {
    _bsp_split_multi_recursive(ctx, node->b, count - 1, &weights[1], vertical, total_weight - weights[0], out_leaves);
  }
  else
  {
    if (out_leaves)
    {
      out_leaves[0] = node->b;
    }
  }
}

void bsp_split_multi(BSPContext *ctx, BSPNode *node, int count, int *ratio, bool vertical, BSPNode **out_leaves)
{
  if (!node || count < 2 || !ratio)
    return;
  int total_sum = 0;
  for (int i = 0; i < count; i++)
  {
    assert(ratio[i] > 0);
    total_sum += ratio[i];
  }

  _bsp_split_multi_recursive(ctx, node, count, ratio, vertical, total_sum, out_leaves);
}

void _bsp_print_recursive(BSPNode *node, char *prefix, bool is_last)
{
  if (!node)
    return;
  printf("%s", prefix);
  printf("%s", is_last ? "└── " : "├── ");
  if (node->a == NULL)
  {
    printf("[Leaf] [%.0f, %.0f, %.0f, %.0f]\n",
           node->area.x, node->area.y, node->area.w, node->area.h);
  }
  else
  {
    printf("[Parent.%s] | Ratio: %.2f]\n",
           node->vertical ? "Vert" : "Horz", node->split_ratio);
    char new_prefix[256];
    snprintf(new_prefix, sizeof(new_prefix), "%s%s", prefix, is_last ? "    " : "│   ");
    _bsp_print_recursive(node->a, new_prefix, false);
    _bsp_print_recursive(node->b, new_prefix, true);
  }
}

void bsp_print(BSPNode *node)
{
  if (!node)
    return;
  _bsp_print_recursive(node, "", true);
}

void _bsp_set_area_recursive(BSPNode *node, Rect area, float spacing)
{

  if (!node)
    return;
  node->area = area;
  if (node->a == NULL)
    return;
  Rect aa = {0}, ab = {0};

  if (node->vertical)
  {
    float split_x = area.w * node->split_ratio;
    float wa = split_x - spacing;
    float wb = (area.w - split_x) - spacing;

    if (wa > 0 && wb > 0)
    {
      aa = (Rect){area.x, area.y, wa, area.h};
      ab = (Rect){area.x + split_x + spacing, area.y, wb, area.h};
    }
  }
  else
  {
    float split_y = area.h * node->split_ratio;
    float ha = split_y - spacing;
    float hb = (area.h - split_y) - spacing;

    if (ha > 0 && hb > 0)
    {
      aa = (Rect){area.x, area.y, area.w, ha};
      ab = (Rect){area.x, area.y + split_y + spacing, area.w, hb};
    }
  }

  _bsp_set_area_recursive(node->a, aa, spacing);
  _bsp_set_area_recursive(node->b, ab, spacing);
}

void bsp_set_area(BSPContext *ctx, Rect total_area)
{
  assert(ctx);
  assert(ctx->root);
  _bsp_set_area_recursive(ctx->root, total_area, ctx->margin / 2.0f);
}
