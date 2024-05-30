#include "GameWindow.h"
#include <time.h>
#include <stdlib.h>

int main(int argc, char **argv)
{
	srand(time(NULL)); // 使用當前時間作為隨機數生成器的種子
	Game *game = New_Game();
	game->execute(game);
	game->game_destroy(game);
	return 0;
}
