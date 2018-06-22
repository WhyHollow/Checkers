#include <SFML/Graphics.hpp>
#include <vector>
#include <thread>
#include <string>
#include <Windows.h>
#include "GameParameters.h"
using namespace sf;

void StartThread(GameParameters &g)
{
	g.ThreadFunc();
}
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
	RenderWindow window(VideoMode(1200, 700), "CheckerMaster v2.0");
	GameParameters game = GameParameters();
	std::thread SecondThread(StartThread, std::ref(game));
	while (window.isOpen())
	{
		Event eve;
		while (window.pollEvent(eve))
		{
			if (eve.type == Event::Closed)
				window.close();
		}
		game.AllUpdate(eve);
		window.clear();
		game.AllDraw(window);
		window.display();
	}
	if (SecondThread.joinable()) SecondThread.join();
	return 0;
}