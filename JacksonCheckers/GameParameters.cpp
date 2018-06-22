#define _CRT_SECURE_NO_WARNINGS
#include "GameParameters.h"
#include <WinSock2.h>
using namespace sf;

bool GameParameters::invert_desk;
unsigned long long GameParameters::operations;

#define INF 1000000

bool GetBit(bit_type field, int number)
{
	register bit_type bit = (number == 0 ? 1 : 2);
	for (register char i = 1; i < number; i++) bit = bit << 1;
	register bit_type res = (field & bit);
	if (res == bit) return true;
	return false;
}
bit_type SetBit(bit_type field, int number, bool value)
{
	register bit_type bit = (number == 0 ? 1 : 2);
	for (register char i = 1; i < number; i++) bit = bit << 1;
	if (value)
	{
		field = field | bit;
	}
	else
	{
		bit = ~bit;
		field = field & bit;
	}
	return field;
}

inline void AddMove(MoveList * &list, Move move)
{
	list = new MoveList(move, list);
}
void DeleteMoveList(MoveList * &list)
{
	while (list != nullptr)
	{
		MoveList * p = list;
		list = list->next;
		delete p;
	}
}

////// Public functions
GameParameters::GameParameters()
{
	//Content
	tex_main_menu.loadFromFile("content/images/mainmenu.png");
	tex_singleplayer.loadFromFile("content/images/singleplayer.png");
	tex_galka.loadFromFile("content/images/galka.png");
	tex_snake.loadFromFile("content/images/snake.png");

	tex_white_shape.loadFromFile("content/images/white_shape.png");
	tex_white_queen.loadFromFile("content/images/white_queen.png");
	tex_black_shape.loadFromFile("content/images/black_shape.png");
	tex_black_queen.loadFromFile("content/images/black_queen.png");

	tex_gameground.loadFromFile("content/images/gameground.png");
	tex_selected.loadFromFile("content/images/selected.png");
	tex_light.loadFromFile("content/images/light.png");

	font.loadFromFile("content/font.TTF");

	b_Game = true;
	focus = true;
	gameState = st_Main_Menu;

	sp_main_menu.setTexture(tex_main_menu);
	sp_singleplayer.setTexture(tex_singleplayer);
	sp_galka.setTexture(tex_galka);
	sp_snake.setTexture(tex_snake);

	sp_gameground.setTexture(tex_gameground);
	sp_selected.setTexture(tex_selected);

	sp_galka.setPosition(Vector2f(522, 211));
	sp_snake.setPosition(Vector2f(169, 221));

	tx_diff.setFont(font);
	tx_diff.setFillColor(Color::Black);
	tx_diff.setPosition(Vector2f(370, 144));
	tx_diff.setString("6");

	tx_name1.setFont(font);
	tx_name1.setFillColor(Color::Black);
	tx_name1.setPosition(Vector2f(180, 287));
	tx_name1.setStyle(Text::Italic);
	tx_name1.setString("");

	tx_name2.setFont(font);
	tx_name2.setFillColor(Color::Black);
	tx_name2.setPosition(Vector2f(180, 380));
	tx_name2.setStyle(Text::Italic);
	tx_name2.setString("");

	tx_status.setFont(font);
	tx_status.setFillColor(Color::Black);
	tx_status.setPosition(Vector2f(781, 329));
	tx_status.setString("");

	tx_name1ingame.setFont(font);
	tx_name1ingame.setFillColor(Color::Black);
	tx_name1ingame.setPosition(Vector2f(814, 198));
	tx_name1ingame.setString("");

	tx_name2ingame.setFont(font);
	tx_name2ingame.setFillColor(Color::Black);
	tx_name2ingame.setPosition(Vector2f(814, 238));
	tx_name2ingame.setString("");

	name1 = "";
	name2 = "";
	ready = false;
	input = false;
	once = true;

	difficult = 6;
	player_color = true;
	winer = true;

	main_desk = Desk();
	selected = false;
	turn = t_Player1;
	memset(light, 0, 32);
	selected_cell = -1;
	player_list = nullptr;
	must_beat = false;
	game_over = false;
	invert_desk = false;

	thread_state = Waiting;
	thread_mode = AI;
}
void GameParameters::AllUpdate(Event &eve)
{
	if (eve.type == Event::GainedFocus) focus = true;
	else if (eve.type == Event::LostFocus) focus = false;

	if (eve.type == Event::Closed) { thread_state = Terminate; return; }
	switch (gameState)
	{
	case st_Main_Menu: { UpdateMainMenu(); } break;
	case st_Single_Player: { UpdateSinglePlayer(); } break;
	case st_Game_Ai: { UpdateGameAI(); } break;
	}
}
void GameParameters::AllDraw(RenderWindow &window)
{
	if (!b_Game) { window.close(); thread_state = Terminate; return; }
	Vector2f real_mouse_position_in_game = window.mapPixelToCoords(Vector2i(mouse.getPosition(window)));
	MouseX = int(real_mouse_position_in_game.x);
	MouseY = int(real_mouse_position_in_game.y);
	switch (gameState)
	{
	case st_Main_Menu: { DrawMainMenu(window); } break;
	case st_Single_Player: { DrawSinglePlayer(window); } break;
	default: { DrawGame(window); }
	}
}

////// Functions ///
void GameParameters::DrawMainMenu(RenderWindow &window)
{
	window.draw(sp_main_menu);
}
void GameParameters::DrawSinglePlayer(RenderWindow &window)
{
	window.draw(sp_singleplayer);
	window.draw(sp_galka);
	window.draw(sp_snake);
	window.draw(tx_diff);
}
void GameParameters::DrawGame(RenderWindow &window)
{
	window.draw(sp_gameground);
	for (register int i = 0; i < (int)shapes.size(); ++i) window.draw(shapes[i]);
	for (register int i = 0; i < (int)lights.size(); ++i) window.draw(lights[i]);
	if (selected) window.draw(sp_selected);
	window.draw(tx_status);
	window.draw(tx_name1ingame);
	window.draw(tx_name2ingame);
}

void GameParameters::UpdateMainMenu()
{
	if (mouse.isButtonPressed(mouse.Left) && MouseInside(364, 200, 861, 300)) { gameState = st_Single_Player; thread_mode = AI; return; }
	if (mouse.isButtonPressed(mouse.Left) && MouseInside(364, 432, 861, 532)) { b_Game = false; return; }
}
void GameParameters::UpdateSinglePlayer()
{
	if (mouse.isButtonPressed(mouse.Left) && MouseInside(20, 20, 250, 129))
	{
		gameState = st_Main_Menu;
		difficult = 6;
		player_color = true;
		sp_snake.setPosition(Vector2f(169, 221));
		tx_diff.setString("6");
		name1 = "";
		name2 = "";
		return;
	}
	if (mouse.isButtonPressed(mouse.Left) && MouseInside(514, 202, 613, 302)) { player_color = true; }
	if (mouse.isButtonPressed(mouse.Left) && MouseInside(515, 328, 614, 427)) { player_color = false; }
	if (player_color) { sp_galka.setPosition(Vector2f(514, 202)); }
	else { sp_galka.setPosition(Vector2f(515, 328)); }
	if (mouse.isButtonPressed(mouse.Left) && MouseInside(160, 236, 209, 599))
	{
		sp_snake.setPosition(Vector2f(169, (float)MouseY - 15));
		for (int i = 0; i < 11; i++) if ((sp_snake.getPosition().y + 15 > 220 + 34 * i) && (sp_snake.getPosition().y + 15 < 220 + 34 * (i + 1))) difficult = i + 6;
	}
	tx_diff.setString(std::to_string(difficult));
	if (mouse.isButtonPressed(mouse.Left) && MouseInside(520, 481, 1100, 635))
	{
		if (player_color)
		{
			name1 = "Player";
			name2 = "AI";
		}
		else
		{
			name1 = "AI";
			name2 = "Player";
		}
		main_desk = Desk(player_color);
		memset(light, 0, 32);
		once = true;
		selected = false;
		gameState = st_Game_Ai;
		tx_name1ingame.setString(name1);
		tx_name2ingame.setString(name2);
		thread_mode = AI;
		invert_desk = !player_color;
		turn = (player_color ? t_Player1 : t_Player2);
	}
}

void GameParameters::UpdateGameAI()
{
	if (mouse.isButtonPressed(mouse.Left) && MouseInside(784, 24, 1170, 136))
	{
		gameState = st_Main_Menu;
		difficult = 6;
		player_color = true;
		sp_snake.setPosition(Vector2f(169, 221));
		tx_diff.setString("3");
		name1 = "";
		name2 = "";
		shapes.clear();
		lights.clear();
		once = false;
		turn = t_Player1;
		player_list = nullptr;
		tx_name1ingame.setString("");
		tx_name2ingame.setString("");
		tx_status.setString("");
		game_over = false;
		must_beat = false;
		search_time = 0;
		return;
	}
	if (!game_over)
	{
		std::string s = std::to_string(search_time);
		s.erase(4, s.length());
		if (turn == t_Player1)
		{
			if (player_color) tx_status.setString(name1 + "'s turn " + s + "s. " + std::to_string(operations));
			else tx_status.setString(name2 + "'s turn " + s + "s. " + std::to_string(operations));
			PlayerTurn(player_color, main_desk);
		}
		else
		{
			if (player_color) tx_status.setString(name2 + "'s turn... ");
			else  tx_status.setString(name1 + "'s turn... ");
			if (thread_state == Waiting) thread_state = Working;
			if (thread_state == Ready)
			{
				thread_state = Waiting;
				NextTurn();
			}
		}
	}
	else
	{
		thread_state = Waiting;
		if (turn == t_Player1)
		{
			tx_status.setString(name1 + " wins!!!");
		}
		else
		{
			tx_status.setString(name2 + " wins!!!");
		}
	}
	if (once)
	{
		if (turn == t_Player2) thread_state = Working;
		game_over = GameOver((turn == t_Player1 ? true : false));
		must_beat = MustBeat(main_desk, (turn == t_Player1 ? player_color : !player_color));
		ResetShapes();
		once = false;
	}
}

void GameParameters::ResetShapes()
{
	shapes.clear();
	lights.clear();
	for (register char i = 0; i < 32; i++)
	{
		if (GetBit(main_desk.exist, i))
		{
			if (GetBit(main_desk.color, i))
			{
				if (GetBit(main_desk.queen, i))
				{
					Sprite sp;
					sp.setTexture(tex_white_queen);
					sp.setPosition((Celltype(i) ? Vector2f((float)(100 + (i % 4) * 2 * 64), (float)(548 - (i / 8) * 2 * 64)) : Vector2f((float)(100 + (i % 4) * 2 * 64 + 64), (float)(548 - (i / 8) * 2 * 64 - 64))));
					shapes.push_back(sp);
				}
				else
				{
					Sprite sp;
					sp.setTexture(tex_white_shape);
					sp.setPosition((Celltype(i) ? Vector2f((float)(100 + (i % 4) * 2 * 64), (float)(548 - (i / 8) * 2 * 64)) : Vector2f((float)(100 + (i % 4) * 2 * 64 + 64), (float)(548 - (i / 8) * 2 * 64 - 64))));
					shapes.push_back(sp);
				}
			}
			else
			{
				if (GetBit(main_desk.queen, i))
				{
					Sprite sp;
					sp.setTexture(tex_black_queen);
					sp.setPosition((Celltype(i) ? Vector2f((float)(100 + (i % 4) * 2 * 64), (float)(548 - (i / 8) * 2 * 64)) : Vector2f((float)(100 + (i % 4) * 2 * 64 + 64), (float)(548 - (i / 8) * 2 * 64 - 64))));
					shapes.push_back(sp);
				}
				else
				{
					Sprite sp;
					sp.setTexture(tex_black_shape);
					sp.setPosition((Celltype(i) ? Vector2f((float)(100 + (i % 4) * 2 * 64), (float)(548 - (i / 8) * 2 * 64)) : Vector2f((float)(100 + (i % 4) * 2 * 64 + 64), (float)(548 - (i / 8) * 2 * 64 - 64))));
					shapes.push_back(sp);
				}
			}
		}
		if (light[i])
		{
			Sprite sp;
			sp.setTexture(tex_light);
			sp.setPosition((Celltype(i) ? Vector2f((float)(100 + (i % 4) * 2 * 64), (float)(548 - (i / 8) * 2 * 64)) : Vector2f((float)(100 + (i % 4) * 2 * 64 + 64), (float)(548 - (i / 8) * 2 * 64 - 64))));
			lights.push_back(sp);
		}
	}
}
bool GameParameters::GameOver(bool color)
{
	bool res = true;
	MoveList * temp = nullptr;
	for (register char i = 0; i < 32; i++)
	{
		if (GetBit(main_desk.exist, i) && (GetBit(main_desk.color, i) == color))
		{
			FindBeats(temp, main_desk, true, i);
			if (!temp) FindMoves(temp, main_desk, true, i);
			if (temp)
			{
				res = false;
				break;
			}
		}
	}
	if (res) { turn = t_Player2; DeleteMoveList(temp); return true; }
	res = true;
	for (register char i = 0; i < 32; i++)
	{
		if (GetBit(main_desk.exist, i) && (GetBit(main_desk.color, i) != color))
		{
			res = false;
			break;
		}
	}
	if (res) { turn = t_Player1; DeleteMoveList(temp); return true; }
	return false;
}
void GameParameters::PlayerTurn(bool color, Desk &current_desk)
{
	if (mouse.isButtonPressed(mouse.Left) && MouseInside(100, 100, 611, 611))
	{
		char cell = ClickCell();
		if (cell != -1)
		{
			if (selected)
			{
				if (GetBit(current_desk.exist, cell) && GetBit(current_desk.color, cell) == color)
				{
					ResetData();
					player_list = nullptr;
					FindBeats(player_list, current_desk, color, cell);
					if (!player_list && !must_beat) FindMoves(player_list, current_desk, color, cell);
					if (player_list) SelectCell(cell);
				}
				else if (light[cell])
				{
					Move m = MoveFromCell(player_list, cell);
					MakeMove(current_desk, m);
					DeleteMoveList(player_list);
					BecameQueen(current_desk);
					FindBeats(player_list, current_desk, color, m.next);
					if (!(m.must_beat) || !(player_list != nullptr)) NextTurn();
					ResetData();
				}
				else ResetData();
			}
			else
			{
				if (GetBit(current_desk.exist, cell) && GetBit(current_desk.color, cell) == color)
				{
					FindBeats(player_list, current_desk, color, cell);
					if (!player_list && !must_beat) FindMoves(player_list, current_desk, color, cell);
					if (player_list) SelectCell(cell);
				}
			}
		}
		else ResetData();
		once = true;
	}
	if (mouse.isButtonPressed(mouse.Right))
	{
		ResetData();
		once = true;
	}
}

////// Subfunctions ///
inline bool GameParameters::MouseInside(int first_x, int first_y, int second_x, int second_y)
{
	return ((MouseX >= first_x) && (MouseX <= second_x) && (MouseY >= first_y) && (MouseY <= second_y) && focus);
}
inline void GameParameters::ResetData()
{
	selected = false;
	DeleteMoveList(player_list);
	memset(light, 0, 32);
	selected_cell = -1;
}
inline bool GameParameters::Celltype(char n)
{
	return ((n / 4) % 2 == 0);
}
char GameParameters::ClickCell()
{
	for (register char i = 0; i < 32; i++)
	{
		Vector2f temp = (Celltype(i) ? Vector2f((float)(100 + (i % 4) * 2 * 64), (float)(548 - (i / 8) * 2 * 64)) : Vector2f((float)(100 + (i % 4) * 2 * 64 + 64), (float)(548 - (i / 8) * 2 * 64 - 64)));
		if (MouseInside((int)temp.x, (int)temp.y, (int)temp.x + 64, (int)temp.y + 64)) return i;
	}
	return -1;
}
inline void GameParameters::SelectCell(char cell)
{
	selected = true;
	selected_cell = cell;
	sp_selected.setPosition((Celltype(cell) ? Vector2f((float)(100 + (cell % 4) * 2 * 64), (float)(548 - (cell / 8) * 2 * 64)) : Vector2f((float)(100 + (cell % 4) * 2 * 64 + 64), (float)(548 - (cell / 8) * 2 * 64 - 64))));
	MoveList * p = player_list;
	while (p)
	{
		light[p->move.next] = true;
		p = p->next;
	}
}
inline void GameParameters::NextTurn()
{
	if (thread_mode == AI && name1 != "AI" && name2 != "AI") InvertDesk(main_desk);
	if (turn == t_Player1) turn = t_Player2;
	else turn = t_Player1;
}
Move GameParameters::MoveFromCell(MoveList * list, char cell)
{
	while (list)
	{
		if (list->move.next == cell) break;
		list = list->next;
	}
	return list->move;
}
void GameParameters::FindMoves(MoveList * &list, Desk current_desk, bool color, char cell)
{
	if (GetBit(current_desk.queen, cell)) // Если дамка
	{
		bool b = Celltype(cell);
		char k = cell;
		if ((k % 8) != 0 && (k < 28)) // Влево-вверх
			while (true)
			{
				k += (b ? 3 : 4);
				b = !b;
				if (!GetBit(current_desk.exist, k)) AddMove(list, Move(cell, k, 0, 0));
				else break;
				if ((k > 27) || ((k % 8) == 0)) break;
			}

		b = Celltype(cell);
		k = cell;
		if (((k + 1) % 8) != 0 && (k < 28)) // Вправо-вверх
			while (true)
			{
				k += (b ? 4 : 5);
				b = !b;
				if (!GetBit(current_desk.exist, k)) AddMove(list, Move(cell, k, 0, 0));
				else break;
				if ((k > 27) || (((k + 1) % 8) == 0)) break;
			}

		b = Celltype(cell);
		k = cell;
		if (((k + 1) % 8) != 0 && (k > 3)) // Вправо-вниз
			while (true)
			{
				k += (b ? -4 : -3);
				b = !b;
				if (!GetBit(current_desk.exist, k)) AddMove(list, Move(cell, k, 0, 0));
				else break;
				if ((k < 4) || (((k + 1) % 8) == 0)) break;
			}

		b = Celltype(cell);
		k = cell;
		if ((k % 8) != 0 && (k > 3)) // Влево-вниз
			while (true)
			{
				k += (b ? -5 : -4);
				b = !b;
				if (!GetBit(current_desk.exist, k)) AddMove(list, Move(cell, k, 0, 0));
				else break;
				if ((k < 4) || ((k % 8) == 0)) break;
			}
	}
	else // Если шашка
	{
		if (!invert_desk)
			if (color) // Если белая шашка
			{
				if ((cell % 8) != 0) // Налево
				{
					char k = (Celltype(cell) ? cell + 3 : cell + 4);
					if (!GetBit(current_desk.exist, k)) AddMove(list, Move(cell, k, 0, 0));
				}
				if (((cell + 1) % 8) != 0) // Направо
				{
					char k = (Celltype(cell) ? cell + 4 : cell + 5);
					if (!GetBit(current_desk.exist, k)) AddMove(list, Move(cell, k, 0, 0));
				}
			}
			else // Если черная шашка
			{
				if ((cell % 8) != 0) // Налево
				{
					char k = (Celltype(cell) ? cell - 5 : cell - 4);
					if (!GetBit(current_desk.exist, k)) AddMove(list, Move(cell, k, 0, 0));
				}
				if (((cell + 1) % 8) != 0) // Направо
				{
					char k = (Celltype(cell) ? cell - 4 : cell - 3);
					if (!GetBit(current_desk.exist, k)) AddMove(list, Move(cell, k, 0, 0));
				}
			}
		if (invert_desk)
			if (!color) // Если белая шашка
			{
				if ((cell % 8) != 0) // Налево
				{
					char k = (Celltype(cell) ? cell + 3 : cell + 4);
					if (!GetBit(current_desk.exist, k)) AddMove(list, Move(cell, k, 0, 0));
				}
				if (((cell + 1) % 8) != 0) // Направо
				{
					char k = (Celltype(cell) ? cell + 4 : cell + 5);
					if (!GetBit(current_desk.exist, k)) AddMove(list, Move(cell, k, 0, 0));
				}
			}
			else // Если черная шашка
			{
				if ((cell % 8) != 0) // Налево
				{
					char k = (Celltype(cell) ? cell - 5 : cell - 4);
					if (!GetBit(current_desk.exist, k)) AddMove(list, Move(cell, k, 0, 0));
				}
				if (((cell + 1) % 8) != 0) // Направо
				{
					char k = (Celltype(cell) ? cell - 4 : cell - 3);
					if (!GetBit(current_desk.exist, k)) AddMove(list, Move(cell, k, 0, 0));
				}
			}
	}
}
void GameParameters::FindBeats(MoveList * &list, Desk current_desk, bool color, char cell)
{
	if (GetBit(current_desk.queen, cell)) // Дамка
	{
		// Влево-вверх
		bool b = Celltype(cell);
		bool found = false;
		char k = cell;
		char kill = -1;
		if ((k % 8) != 0 && (k < 28))
			while (true)
			{
				k += (b ? 3 : 4);
				b = !b;
				if (GetBit(current_desk.exist, k))
				{
					if (!GetBit(current_desk.color, k) == color)
					{
						found = true;
						kill = k;
						break;
					}
					else break;
				}
				if ((k > 23) || ((k % 4) == 0)) break;
			}
		if ((k % 8) != 0 && (k < 28))
			while (found)
			{
				k += (b ? 3 : 4);
				b = !b;
				if (!GetBit(current_desk.exist, k)) AddMove(list, Move(cell, k, kill, 1));
				else break;
				if ((k > 27) || ((k % 8) == 0)) break;
			}

		// Вправо-вверх
		b = Celltype(cell);
		found = false;
		k = cell;
		kill = -1;
		if (((k + 1) % 8) != 0 && (k < 28))
			while (true)
			{
				k += (b ? 4 : 5);
				b = !b;
				if (GetBit(current_desk.exist, k))
				{
					if (!GetBit(current_desk.color, k) == color)
					{
						found = true;
						kill = k;
						break;
					}
					else break;
				}
				if ((k > 23) || (((k + 1) % 4) == 0)) break;
			}
		if (((k + 1) % 8) != 0 && (k < 28))
			while (found)
			{
				k += (b ? 4 : 5);
				b = !b;
				if (!GetBit(current_desk.exist, k)) AddMove(list, Move(cell, k, kill, 1));
				else break;
				if ((k > 27) || (((k + 1) % 8) == 0)) break;
			}

		// Вправо-вниз
		b = Celltype(cell);
		found = false;
		k = cell;
		kill = -1;
		if (((k + 1) % 8) != 0 && (k > 3))
			while (true)
			{
				k += (b ? -4 : -3);
				b = !b;
				if (GetBit(current_desk.exist, k))
				{
					if (!GetBit(current_desk.color, k) == color)
					{
						found = true;
						kill = k;
						break;
					}
					else break;
				}
				if ((k < 8) || (((k + 1) % 4) == 0)) break;
			}
		if (((k + 1) % 8) != 0 && (k > 3)) // Вправо-вниз
			while (found)
			{
				k += (b ? -4 : -3);
				b = !b;
				if (!GetBit(current_desk.exist, k)) AddMove(list, Move(cell, k, kill, 1));
				else break;
				if ((k < 4) || (((k + 1) % 8) == 0)) break;
			}

		// Влево-вниз
		b = Celltype(cell);
		found = false;
		k = cell;
		kill = -1;
		if ((k % 8) != 0 && (k > 3))
			while (true)
			{
				k += (b ? -5 : -4);
				b = !b;
				if (GetBit(current_desk.exist, k))
				{
					if (!GetBit(current_desk.color, k) == color)
					{
						found = true;
						kill = k;
						break;
					}
					else break;
				}
				if ((k < 8) || ((k % 4) == 0)) break;
			}
		if ((k % 8) != 0 && (k > 3)) // Влево-вниз
			while (found)
			{
				k += (b ? -5 : -4);
				b = !b;
				if (!GetBit(current_desk.exist, k)) AddMove(list, Move(cell, k, kill, 1));
				else break;
				if ((k < 4) || ((k % 8) == 0)) break;
			}
	}
	else // Шашка
	{
		if (cell < 24) // верхняя граница
		{
			if ((cell % 4) != 0) // Налево
			{
				char k = (Celltype(cell) ? cell + 3 : cell + 4);
				if (GetBit(current_desk.exist, k) && GetBit(current_desk.color, k) != color && !GetBit(current_desk.exist, cell + 7)) AddMove(list, Move(cell, cell + 7, k, 1));
			}
			if (((cell + 1) % 4) != 0) // Направо
			{
				char k = (Celltype(cell) ? cell + 4 : cell + 5);
				if (GetBit(current_desk.exist, k) && GetBit(current_desk.color, k) != color && !GetBit(current_desk.exist, cell + 9)) AddMove(list, Move(cell, cell + 9, k, 1));
			}
		}
		if (cell > 7) // нижняя граница
		{
			if ((cell % 4) != 0) // Налево
			{
				char k = (Celltype(cell) ? cell - 5 : cell - 4);
				if (GetBit(current_desk.exist, k) && GetBit(current_desk.color, k) != color && !GetBit(current_desk.exist, cell - 9)) AddMove(list, Move(cell, cell - 9, k, 1));
			}
			if (((cell + 1) % 4) != 0) // Направо
			{
				char k = (Celltype(cell) ? cell - 4 : cell - 3);
				if (GetBit(current_desk.exist, k) && GetBit(current_desk.color, k) != color && !GetBit(current_desk.exist, cell - 7)) AddMove(list, Move(cell, cell - 7, k, 1));
			}
		}
	}
}
void GameParameters::MakeMove(Desk &current_desk, Move move)
{
	current_desk.exist = SetBit(current_desk.exist, move.next, 1);
	current_desk.color = SetBit(current_desk.color, move.next, GetBit(current_desk.color, move.prev));
	current_desk.queen = SetBit(current_desk.queen, move.next, GetBit(current_desk.queen, move.prev));

	current_desk.exist = SetBit(current_desk.exist, move.prev, 0);
	current_desk.color = SetBit(current_desk.color, move.prev, 0);
	current_desk.queen = SetBit(current_desk.queen, move.prev, 0);

	if (move.must_beat)
	{
		current_desk.exist = SetBit(current_desk.exist, move.beat, 0);
		current_desk.color = SetBit(current_desk.color, move.beat, 0);
		current_desk.queen = SetBit(current_desk.queen, move.beat, 0);
	}
}
void GameParameters::BecameQueen(Desk &current_desk)
{
	if (invert_desk)
	{
		for (register char i = 0; i < 4; i++)
		{
			if (GetBit(current_desk.exist, i) && GetBit(current_desk.color, i))   current_desk.queen = SetBit(current_desk.queen, i, 1);
			if (GetBit(current_desk.exist, i + 28) && !GetBit(current_desk.color, i + 28))   current_desk.queen = SetBit(current_desk.queen, i + 28, 1);
		}
	}
	else
	{
		for (register char i = 0; i < 4; i++)
		{
			if (GetBit(current_desk.exist, i) && !GetBit(current_desk.color, i))   current_desk.queen = SetBit(current_desk.queen, i, 1);
			if (GetBit(current_desk.exist, i + 28) && GetBit(current_desk.color, i + 28))   current_desk.queen = SetBit(current_desk.queen, i + 28, 1);
		}
	}
}
bool GameParameters::MustBeat(Desk &current_desk, bool color)
{
	MoveList * list = nullptr;
	for (register char i = 0; i < 32; i++)
	{
		if (GetBit(current_desk.exist, i) && (GetBit(current_desk.color, i)) == color)
		{
			FindBeats(list, current_desk, color, i);
			if (list) { DeleteMoveList(list); return true; }
		}
	}
	return false;
}
void GameParameters::InvertDesk(Desk &current_desk)
{
	Desk desk;
	for (register char i = 0; i < 32; i++)
	{
		desk.exist = SetBit(desk.exist, i, GetBit(main_desk.exist, 31 - i));
		desk.color = SetBit(desk.color, i, GetBit(main_desk.color, 31 - i));
		desk.queen = SetBit(desk.queen, i, GetBit(main_desk.queen, 31 - i));
	}
	current_desk = desk;
	invert_desk = !invert_desk;
}

void GameParameters::ThreadFunc()
{
	while (true)
	{
		if (thread_state == Terminate) return; ///////////
		if (thread_mode == AI)
		{
			if (thread_state == Working) ///////////
			{
				start_time = clock();
				AITurn(!player_color, main_desk);
				once = true;
				end_time = clock();
				search_time = end_time - start_time;
				search_time /= (double)CLK_TCK;
			}
		}
		sleep(milliseconds(100));
	}
}
void GameParameters::AITurn(bool color, Desk &current_desk)
{
	////// Обнулили счетчик
	operations = 1;
	////// 

	////// Создаем все ходы
	MoveList * list = nullptr;
	for (register char i = 0; i < 32; i++)
	{
		if (GetBit(current_desk.exist, i) && (GetBit(current_desk.color, i) == color))
			FindBeats(list, current_desk, color, i);
	}
	if (list == nullptr)
	{
		for (register char i = 0; i < 32; i++)
		{
			if (GetBit(current_desk.exist, i) && GetBit(current_desk.color, i) == color)
				FindMoves(list, current_desk, color, i);
		}
	}
	if (list == nullptr) return;
	//////

	////// Если только один ход
	if (list->next == nullptr)
	{
		MakeMove(main_desk, list->move);
		BecameQueen(main_desk);
		MoveList * p = nullptr;
		if (list->move.must_beat) FindBeats(p, current_desk, color, list->move.next);
		if (p)
		{
			DeleteMoveList(p);
		}
		else thread_state = Ready;
		DeleteMoveList(list);
		sleep(milliseconds(100));
		return;
	}
	//////

	////// Объявляем рабочие переменные
	MoveList * iter = list;
	Move best_move;
	int alpha = -INF;
	int beta = INF;
	////// 
	
	//////////////////////////////////////////////////////////////////////////////////////////////
	auto func = [](int &val, bool color, Desk &current_desk, char depht, int alpha, int beta, bool FLAG)
	{
		if (FLAG) val = AIRecurrent(color, current_desk, depht, alpha, beta);
		else val = -AIRecurrent(!color, current_desk, depht, -beta, -alpha);
	};

	size_t move_cnt = 0;
	while (iter) { move_cnt++; iter = iter->next; }
	iter = list;
	Desk *all_variant = new Desk[move_cnt];
	int *all_values = new int[move_cnt];

	std::thread **threads = new std::thread*[move_cnt];
	for (size_t i = 0; i < move_cnt; i++)
	{
		threads[i] = nullptr;
	}

	for (size_t i = 0; i < move_cnt; i++)
	{
		all_variant[i] = current_desk;
		all_values[i] = -INF;
		MakeMove(all_variant[i], iter->move);
		BecameQueen(all_variant[i]);

		MoveList * p = nullptr;
		if (iter->move.must_beat) FindBeats(p, all_variant[i], color, iter->move.next);

		if (p)
		{
			threads[i] = new std::thread(func, std::ref(all_values[i]), color, std::ref(all_variant[i]), difficult - 1, alpha, beta, true);
			DeleteMoveList(p);
		}
		else threads[i] = new std::thread(func, std::ref(all_values[i]), color, std::ref(all_variant[i]), difficult - 1, alpha, beta, false);

		iter = iter->next;
	}

	for (size_t i = 0; i < move_cnt; i++)
	{
		if (threads[i] != nullptr && threads[i]->joinable()) 
			threads[i]->join();
	}

	for (size_t i = 0; i < move_cnt; i++)
		if (threads[i]) delete threads[i];
	delete[] threads;

	size_t ind = 0;
	for (size_t i = 0; i < move_cnt; i++)
		if (all_values[i] > all_values[ind]) ind = i;

	current_desk = all_variant[ind];

	iter = list;
	for (size_t i = 0; i < ind; i++) iter = iter->next;
	best_move = iter->move;

	delete[] all_variant;
	delete[] all_values;
	DeleteMoveList(list);

	////// Проверяем продолжение хода
	if (best_move.must_beat) FindBeats(list, current_desk, color, best_move.next);
	if (list) DeleteMoveList(list);
	else thread_state = Ready;
	sleep(milliseconds(100));
	//////
}
int GameParameters::AIRecurrent(bool color, Desk current_desk, char depht, int alpha, int beta)
{
	////// Счетчик
	operations++;
	//////

	////// Оценить последний ход
	if (depht <= 0) return Evaluate(current_desk, color);
	//////

	////// Создать все ходы
	MoveList * list = nullptr;
	for (register char i = 0; i < 32; i++)
	{
		if (GetBit(current_desk.exist, i) && GetBit(current_desk.color, i) == color)
			FindBeats(list, current_desk, color, i);
	}
	if (list == nullptr)
	{
		for (register char i = 0; i < 32; i++)
		{
			if (GetBit(current_desk.exist, i) && GetBit(current_desk.color, i) == color)
				FindMoves(list, current_desk, color, i);
		}
	}
	if (list == nullptr) return Evaluate(current_desk, color) - depht;
	//////

	////// Объявить рабочие переменные
	MoveList * iter = list;
	//////

	////// Запуск цикла перебора
	while (iter && (alpha < beta))
	{
		////// Создать доску с новым ходом
		Desk temp_desk = current_desk;
		MakeMove(temp_desk, iter->move);
		BecameQueen(temp_desk);
		//////

		////// Проверка наличия продолжения хода
		int temp = -INF;
		MoveList * p = nullptr;
		if (iter->move.must_beat) FindBeats(p, temp_desk, color, iter->move.next);
		//////

		////// Проверка хода рекурсивной функцией
		if (p)
		{
			temp = AIRecurrent(color, temp_desk, depht - 1, alpha, beta);
			DeleteMoveList(p);
		}
		else temp = -AIRecurrent(!color, temp_desk, depht - 1, -beta, -alpha);
		//////

		////// Сравнивание хода
		if (temp > alpha) alpha = temp;
		//////

		////// Итерация
		iter = iter->next;
		//////
	}
	DeleteMoveList(list);

	////// Возврат оценки
	return alpha;
}
int GameParameters::Evaluate(Desk &current_desk, bool color)
{
	int score1 = 0;
	int score2 = 0;
	for (register char i = 0; i < 32; i++)
	{
		if (GetBit(current_desk.exist, i))
		{
			if (GetBit(current_desk.color, i) == color)
			{
				score1 += (GetBit(current_desk.queen, i) ? 30 : 10);

			}
			else
			{
				score2 -= (GetBit(current_desk.queen, i) ? 30 : 10);
			}
		}
	}
	if (score1 == 0) return -INF;
	if (score2 == 0) return INF;
	return score1 + score2;
}