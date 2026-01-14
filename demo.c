#include "raylib.h"
#include "bsp.h"
#include <time.h>
#include <stdlib.h>

#define POOL_SIZE 1024*16

void bsp_draw(BSPNode *node);

int main() {
  srand(time(0));

  SetTraceLogLevel(LOG_ERROR);
  SetConfigFlags(FLAG_WINDOW_RESIZABLE);  
  InitWindow(1280, 720, "BSP demo");
  SetExitKey(KEY_Q);
  SetTargetFPS(10);

  
  BSPContext* ctx= bsp_init(2,POOL_SIZE);
  bsp_set_area(ctx,(Rect){0,0,GetScreenWidth(),GetScreenHeight()});

  BSPNode* node=ctx->root;


  node=bsp_split(ctx,ctx->root,0.5,true);
  bsp_split_multi(ctx,node->a,3,(int[3]){1,1,1},false,NULL);
  BSPNode* nodes[32]={0};
  bsp_split_multi(ctx,node->b,4,(int[4]){1,1,1,2},true,nodes);
  bsp_split_multi(ctx,nodes[3],4,(int[4]){1,1,1,2},false,nodes);
    
  bsp_print(ctx->root);
  bsp_set_area(ctx, (Rect){0, 0, (float)GetScreenWidth(), (float)GetScreenHeight()});
  while (!WindowShouldClose()) {
    if (IsWindowResized()) {
      bsp_set_area(ctx, (Rect){0, 0, (float)GetScreenWidth(), (float)GetScreenHeight()});
    }

    BeginDrawing();
    ClearBackground(DARKBLUE);
    bsp_draw(ctx->root);
    EndDrawing();
  }

  CloseWindow();
  arena_free(ctx->pool);
  return 0;
}


void bsp_draw(BSPNode *node)
{
    if (!node) return;
    if (node->a == NULL) {
      DrawRectangleRec( RECT2RECTANGLE(node->area),Fade(YELLOW,0.9));
        DrawRectangleLinesEx(RECT2RECTANGLE(node->area), 2.0f, DARKBLUE);       
    } 
    else {
        bsp_draw(node->a);
        bsp_draw(node->b);
    }
}
