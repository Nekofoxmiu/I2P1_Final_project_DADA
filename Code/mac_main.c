#define ALLEGRO_NO_MAGIC_MAIN
#include "GameWindow.h"
int real_main(int argc, char **argv)
{
    srand(time(NULL)); // 使用當前時間作為隨機數生成器的種子
    Game *game = New_Game();
    game->execute(game);
    game->game_destroy(game);
}
int main(int argc, char **argv)
{
    return al_run_main(argc, argv, real_main);
}