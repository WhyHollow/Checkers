#pragma once
#include <SFML/Graphics.hpp>
#include <thread>
using namespace sf;
typedef unsigned int bit_type;

enum GameState
{
	st_Main_Menu, st_Single_Player, st_Game_Ai
};
enum Turn
{
	t_Player1, t_Player2
};
enum ThreadState
{
	Waiting, Working, Ready, Terminate, Error
};
enum ThreadMode
{
	AI
};

struct Desk
{
	bit_type exist : 32;
	bit_type color : 32;
	bit_type queen : 32;
	Desk() : exist(0), color(0), queen(0) {};
	Desk(bool a) : exist(4293922815), color((a) ? 4095 : 4293918720), queen(0) {}; // 1/true - white, 0/false - black
};
struct Move
{
	bit_type prev : 5;
	bit_type next : 5;
	bit_type beat : 5;
	bit_type must_beat : 1;
	Move() : prev(0), next(0), beat(0), must_beat(0) {};
	Move(bit_type _prev, bit_type _next, bit_type _beat, bit_type _must_beat) : prev(_prev), next(_next), beat(_beat), must_beat(_must_beat) {};
};
struct MoveList
{
	Move move;
	MoveList * next;
	MoveList() : move(Move()), next(nullptr) {};
	MoveList(Move _move, MoveList * _next) : move(_move), next(_next) {};
};

bool GetBit(bit_type field, int number);
bit_type SetBit(bit_type field, int number, bool value);

void AddMove(MoveList * &list, Move move);
void DeleteMoveList(MoveList * &list);

class GameParameters
{
	///Content
	Texture tex_main_menu;
	Texture tex_singleplayer;
	Texture tex_galka;
	Texture tex_snake;
	Texture tex_white_shape;
	Texture tex_white_queen;
	Texture tex_black_shape;
	Texture tex_black_queen;
	Texture tex_gameground;
	Texture tex_selected;
	Texture tex_light;
	Font font;

	Sprite sp_main_menu;
	Sprite sp_singleplayer;
	Sprite sp_galka;
	Sprite sp_snake;
	Sprite sp_gameground;
	Sprite sp_selected;

	bool b_Game;
	bool focus;
	GameState gameState;
	Mouse mouse;
	int MouseX;
	int MouseY;

	Text tx_diff;
	std::string name1;
	std::string name2;
	Text tx_name1;
	Text tx_name2;
	bool input;
	bool once;
	bool ready;
	Text tx_name1ingame;
	Text tx_name2ingame;
	Text tx_status;

	std::vector<Sprite> shapes;
	std::vector<Sprite> lights;
	bool light[32];

	int difficult;
	bool player_color;
	bool winer;

	Desk main_desk;
	Turn turn;

	bool selected;
	int selected_cell;
	MoveList * player_list;

	bool game_over;
	bool must_beat;

	static bool invert_desk;
	static unsigned long long operations;

	// AI fields
	ThreadState thread_state;
	ThreadMode thread_mode;

	double start_time;
	double end_time;
	double search_time;

	////// Functions ///
	void DrawMainMenu(RenderWindow &window);
	void DrawSinglePlayer(RenderWindow &window);
	void DrawGame(RenderWindow &window);

	void UpdateMainMenu();
	void UpdateSinglePlayer();

	void UpdateGameAI();

	void ResetShapes();
	bool GameOver(bool color);
	void PlayerTurn(bool color, Desk &current_desk);

	void AITurn(bool color, Desk &current_desk);
	static int AIRecurrent(bool color, Desk current_desk, char depht, int alpha, int beta);
	static int Evaluate(Desk &current_desk, bool color);

	////// Subfunctions ///
	inline bool MouseInside(int first_x, int first_y, int second_x, int second_y);
	inline void ResetData();
	static inline bool Celltype(char n);
	char ClickCell();
	inline void SelectCell(char cell);
	inline void NextTurn();
	Move MoveFromCell(MoveList * list, char cell);
	static void FindMoves(MoveList * &list, Desk current_desk, bool color, char cell);
	static void FindBeats(MoveList * &list, Desk current_desk, bool color, char cell);
	static void MakeMove(Desk &current_desk, Move move);
	static void BecameQueen(Desk &current_desk);
	bool MustBeat(Desk &current_desk, bool color);
	void InvertDesk(Desk &current_desk);

public:
	GameParameters();
	void AllUpdate(Event &eve);
	void AllDraw(RenderWindow &window);
	void ThreadFunc();
};