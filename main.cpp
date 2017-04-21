//#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp> //�������� ���������� ��� ������ � �������� � ������������
#include <iostream> //��� ������ � �����
#include <fstream> //��� ������ � �������
#include "forMcText.h" //���� ���������� �� ������ kychka-pc.ru/wiki/svobodnaya-baza-znanij-sfml/test/sistemnyj-modul-system-module/potoki-threads/sfsound-sfmusic
#include "algorithm.h" //���������� ���� ������� ��� ���������� ������������ ���� � ��������� (����� � ������) ��� �������� �����
#include <Windows.h>
#include "view.h"
#include <string.h>
using namespace std;
using namespace sf;
//GetSystemMetrics(0); //������ ������ 
//GetSystemMetrics(1); //������ ������

#define GLOBAL_W 2240
#define GLOBAL_H 1280
#define W_WIN 1366 //GetSystemMetrics (0) //������ ���� 700/ ����� ��������� 1366
#define H_WIN 768 //GetSystemMetrics(1) //������ ���� 500/ ����� ��������� 768
#define EDGE 20 //������ ����� ������
#define NUM_CELL_X 64
#define NUM_CELL_Y 32
#define INDENTATION_X (W_WIN - EDGE * NUM_CELL_X) / 2 //������
#define INDENTATION_Y (H_WIN - EDGE * NUM_CELL_Y) / 2 //������
#define NUM_H_LINE (NUM_CELL_Y + 1) //���������� �������������� ������, ������� ������� ����
#define NUM_V_LINE (NUM_CELL_X + 1) //���������� ������������ ������, ������� ������� ����
#define W_BUTTON 120
#define H_BUTTON 30
#define GLOB_IND_W (GLOBAL_W - NUM_CELL_X * EDGE) / 2
#define GLOB_IND_H (GLOBAL_H - NUM_CELL_Y * EDGE) / 2



enum StateList {menu, mode, admin, player, backToMenu, setting, exitt, reqPass}; //�������� ������������ ������� �������� �� ��������� ����
String AdOrPlMode = "PlayerMode"; //������ �������� ��� �������� ���� ���� (����� ��� �����)
Coordinate Start, Finish; //���������� ������ (������ ����� ��������) � ����� (���� ������ ������)
bool lvlComplete = false; //���������� �������� �� ������ �������
StateList state;
int CurrentLVL = 1;
bool PassEnter = false;
char Pass [30] = "";

class Body{
public:
	int x, y; //����������
	int w, h; //������, ������
	Texture texture; //��������
	Sprite sprite; //������
	String name; //���
public:
	Body (Image &image, String Name, int X, int Y, int W, int H){
		x = X; y = Y; w = W; h = H; name = Name;
		texture.loadFromImage (image); 
		sprite.setTexture (texture);
		sprite.setTextureRect (IntRect (0, 0, w, h));
		sprite.setPosition ((float) x, (float) y);
	}
};

class Wall : public Body{ //����� �����
public:
	bool drawThis; //�������� �� �����
public:
	Wall (Image &image, String Name, int X, int Y, int W, int H) : Body (image, Name, X, Y, W, H){ //����������� � ������
		drawThis = true; 
		if (name == "Wall") //����� �������
			sprite.setTextureRect (IntRect (0, 0, w, h));
		if (name == "Finish") //���� ���� ������ ����
			sprite.setTextureRect (IntRect (0, h, w, h));
		if (name == "Start") //������ ����� ����� ��������, ������� ��� �������� �������� ����, ��� � ����� ����� ��� ����� �������� � ��� ����������
			sprite.setTextureRect (IntRect (0, h * 2, w, h));
		if (name == "HelpWall") //��������������� "�����" ������� ���������� ���������� ����
			sprite.setTextureRect (IntRect (0, h * 3, w, h));
	}

	Wall (Image &image, int X, int Y, int W, int H) : Body (image, "Wall", X, Y, W, H){ //����������� ��� �����
		drawThis = true;
	}
};

class Button : public Body{ //����� ������
public:
	int value;
	bool drawThis, buttPressed, buttClick; //�������� �� ������, ������ �� ������ � �������� �� �� ������. ����- ��� ������ � ��������� ������ ����� ������ ���� �� ������
	mcText *text; //����� ������� ��������� �� ������
	StateList state; //������ ������ ����� ����� ����� ������ � ������� ��� ���������
public:
	Button (Image &image, String tmpT, String Name, Font &font, int X, int Y, int W, int H) : Body (image, Name, X, Y, W, H){ //����������� � ������
	    buttPressed = false; buttClick = false; 
		text = new mcText (&font); //������� ����� ������� ����� ������������ �� ������
		text -> changeSize (30); //������ ������
		text -> add (tmpT);
		float tmp = (float) tmpT.getSize (); //�������� ������ ������ � ��������
		tmp = x + 50 - (tmp / 2) * 12; //�������� ����� � ������ ������ (����� ��������, �.�. ���������� ������ ����, �� ������� ������ ���������� ����, � �� ������ ������)
		text -> setPosition ((float) tmp, (float) y - 5); //����������� ����� �� ������

		if (Name == "Go!" || Name == "Mode" || Name == "Setting" || Name == "Exit"){ //������ ������-����, ������������ ��� ������� ����
			drawThis = true; state = menu;
		}
		if (Name == "PlayerMode" || Name == "AdminMode" || Name == "BackToMenu"){ //������ ������-����� � ���� ������ ������ Mode
			drawThis = false; state = mode; 
		}
		if (name == "RequestPass"){
			drawThis = false; state = reqPass; text -> setPosition ((float) tmp - 20, (float) y - 5);
		}
		if (name == "EnterPass"){
			drawThis = false; state = reqPass; text -> setPosition ((float) x, (float) y - 5); //����������� ����� �� ������
			if (tmpT == "0")
				value = 0;
			if (tmpT == "1")
				value = 1;
			if (tmpT == "2")
				value = 2;
			if (tmpT == "3")
				value = 3;
			if (tmpT == "4")
				value = 4;
			if (tmpT == "5")
				value = 5;
			if (tmpT == "6")
				value = 6;
			if (tmpT == "7")
				value = 7;
			if (tmpT == "8")
				value = 8;
			if (tmpT == "9")
				value = 9;
		}
		if (Name == "BackToMenuAd" || Name == "OpenAd" || Name == "SaveAd"){ //������ ������-����� �� ����������� ����� � ������ �����
			drawThis = false; state = admin;
		}
		if (Name == "BackToMenuPl" || Name == "HelpPl"){ //���������� ������-����� �� ������ 
			drawThis = false; state = player;
		}
		if (Name == "lvl1Complete") //���� ������ ������� ����������, ���������� ������ ���������� �� ����
			drawThis = false;
	}

	void draw (RenderWindow &window){ //������� ��������� ������ � ������ ������� ����� ������ ������
		if (name != "RequestPass")
			window.draw (sprite);
		text -> draw (&window);
	}

	void checkCursor (Vector2f mousePosWin){ //������� �������� �� ������� ������ ��� ��������� ������� �� ������
		buttClick = false;
		if ((mousePosWin.x >= x) && (mousePosWin.x <= x + w) && (mousePosWin.y >= y) && (mousePosWin.y <= y + h)){ //���� ������ ���� ��������� �� ������
			if (Mouse::isButtonPressed (Mouse::Left)) //� ���� ������ �� ���
				buttPressed = true;
			else{
				if (buttPressed) //���� �� ������ �� ������ � ������ ���� ������, � ������ �� ������-������ �� �������� �� ���
					buttClick = true; 
				buttPressed = false;
			}
			sprite.setTextureRect (IntRect (0, h, w, h)); //���� ������� ������ �� �����, �� ������ ������ ��������
		}
		else{
			buttPressed = false; //���� ������ �� �� ���� �� ������ �������� ����
			sprite.setTextureRect (IntRect (0, 0, w, h));
		}

		if (buttClick && (name == "AdminMode" || name == "PlayerMode")) //���� �� � state = mode, ����� ������� ����� ����, ����� (��� �������������� � �������� ����) ��� ����� (������)
			AdOrPlMode = name; //���������� �������� ������� ����� ����

		if (name == AdOrPlMode) //� ���� ��� ���������� ������� ������ ��� ������ ������� � �������, �� ������ ������� (�������������� ����������)
			sprite.setTextureRect (IntRect (0, h * 2, w, h));
	}
};

class Player : public Body{
public:
	int tmpX, tmpY; //���������� ������� ������ ����� ���� �� ����� �������, ����� �������
	bool playerMove;
public:
	Player (Image &image, int X, int Y, int W, int H) : Body (image, "Player", X, Y, W, H){ //����������� ��� �����
	    tmpX = x; tmpY = y;
	    playerMove = false;
	}

	void update (bool **CoordWall){
		if (Keyboard::isKeyPressed (Keyboard::W) && !playerMove){
			if (y != GLOB_IND_H){
				tmpY = y - EDGE; playerMove = true;
			}
		}
		else
			if (Keyboard::isKeyPressed (Keyboard::S) && !playerMove){
				if (y != GLOB_IND_H + NUM_CELL_Y * EDGE){
					tmpY = y + EDGE; playerMove = true;
				}
			}
			else
				if (Keyboard::isKeyPressed (Keyboard::A) && !playerMove){
					if (x != GLOB_IND_W){
						tmpX = x - EDGE; playerMove = true;
					}
				}
				else
					if (Keyboard::isKeyPressed (Keyboard::D) && !playerMove){
						if (x != GLOB_IND_W + (NUM_CELL_X - 1) * EDGE){
							tmpX = x + EDGE; playerMove = true;
						}
					}
		if ((!CoordWall [(tmpX - GLOB_IND_W) / EDGE][(tmpY - GLOB_IND_H) / EDGE]) && playerMove){ //���������, ��� �� ����� �� ��� ����� ���� �� ����� �������
			if (x == tmpX && y == tmpY) //���� �� ������ ���� ���� ������, �� ����� �� ��������
				playerMove = false;
			else{
				if (x < tmpX)
					x += 2; //�������� ����� ���� ��������
				if (x > tmpX)
					x -= 2;
				if (y < tmpY)
					y += 2;
				if (y > tmpY) 
					y -= 2;
			}
		}
		else{
			tmpX = x; tmpY = y;
		}
		if (x == Finish.x && y == Finish.y) //���� �� �������� ������, �� ����� �������� ������, ����������������� �� ����
			lvlComplete = true;
		else
			lvlComplete = false; 
		sprite.setPosition ((float) x, (float) y); //������������� ������� ������
	}

	void changeCoord (int x2, int y2){ //������� ����� ��� ����������� ������ � ������ ���������� (����� ��� �������� ������ �������)
		x = x2; y = y2; sprite.setPosition ((float) x, (float) y);
		tmpX = x; tmpY = y;
	}
};

class PlayerBackground : public Body{
public:
	bool drawThis;
public:
	PlayerBackground (Image &image, int X, int Y, int W, int H) : Body (image, "PlayerBackground", X, Y, W, H){
		drawThis = false;
		sprite.setOrigin ((float) w / 2, (float) h / 2);
	}

	void changeCoord (int x2, int y2){
		x = x2 + EDGE / 2; y = y2 + EDGE / 2;
		sprite.setPosition ((float) x, (float) y);
	}

	void changeCoord (int x2, int y2, bool fictiv){
		x = x2; y = y2;
		sprite.setPosition ((float) x, (float) y);
	}
};

class System{
public:
	static Image wallImage; //�������� ������� ����
	static int NumWall; //���������� ����
	static Wall *ArrWall [10000]; //������ ����
	static bool **CoordWall; //���������� ����
	static Wall *helpWall [10000];

	static int NumButton; //���������� ������
	static Button *button [70]; //������ ������
	static Font font; //�����

	static VertexArray lines;
	static Clock clock; //�����
	static Vector2i mousePosWin; //���������� ���� �����. ����
	static Vector2f posMouse; //���������� ���� �����. �����
	static Player *pl;
	static PlayerBackground *plBackground;
	static PlayerBackground *plBackground2;

	static RenderWindow *window;

	static float timer;
	static float time;

	
public:
	void initializeButton (){
		font.loadFromFile ("modeka.otf");

		Image buttonImage; //�������� ������� ����
		buttonImage.loadFromFile ("button.png");

		NumButton = 0;
		button [NumButton++] = new Button (buttonImage, "Go!", "Go!", font, GLOBAL_W / 2 - 120 / 2, GLOB_IND_H + EDGE * 7, 120, 30);
		button [NumButton++] = new Button (buttonImage, "Mode", "Mode", font, GLOBAL_W / 2 - 120 / 2, GLOB_IND_H + EDGE * 7 + 50, 120, 30);
		button [NumButton++] = new Button (buttonImage, "Setting", "Setting", font, GLOBAL_W / 2 - 120 / 2, GLOB_IND_H + EDGE * 7 + 100, 120, 30);
		button [NumButton++] = new Button (buttonImage, "Exit", "Exit", font, GLOBAL_W / 2 - 120 / 2, GLOB_IND_H + EDGE * 7 + 150, 120, 30);

		button [NumButton++] = new Button (buttonImage, "Player", "PlayerMode", font, GLOBAL_W / 2 - 120 / 2, GLOB_IND_H + EDGE * 7, 120, 30);
		button [NumButton++] = new Button (buttonImage, "Admin", "AdminMode", font, GLOBAL_W / 2 - 120 / 2, GLOB_IND_H + EDGE * 7 + 50, 120, 30);
		button [NumButton++] = new Button (buttonImage, "Back", "BackToMenu", font, GLOBAL_W / 2 - 120 / 2, GLOB_IND_H + EDGE * 7 + 100, 120, 30);
		button [NumButton++] = new Button (buttonImage, "Enter 4 numbers", "RequestPass", font, GLOBAL_W / 2 - 120 / 2, GLOB_IND_H + EDGE * 7 + 150, 120, 30);

		button [NumButton++] = new Button (buttonImage, "Back", "BackToMenuAd", font, GLOB_IND_W - (W_WIN - NUM_CELL_X * EDGE) / 2 + (W_WIN - 3 * 120) / 4, ((H_WIN - NUM_CELL_Y * EDGE) / 2 - 30) / 2 + GLOB_IND_H + NUM_CELL_Y * EDGE, 120, 30);
		button [NumButton++] = new Button (buttonImage, "Open", "OpenAd", font, GLOB_IND_W - (W_WIN - NUM_CELL_X * EDGE) / 2 + (W_WIN - 3 * 120) / 2 + 120, ((H_WIN - NUM_CELL_Y * EDGE) / 2 - 30) / 2 + GLOB_IND_H + NUM_CELL_Y * EDGE, 120, 30);
		button [NumButton++] = new Button (buttonImage, "Save", "SaveAd", font, GLOB_IND_W - (W_WIN - NUM_CELL_X * EDGE) / 2 + 3 * (W_WIN - 3 * 120) / 4 + 120 * 2, ((H_WIN - NUM_CELL_Y * EDGE) / 2 - 30) / 2 + GLOB_IND_H + NUM_CELL_Y * EDGE, 120, 30);

		button [NumButton++] = new Button (buttonImage, "Back", "BackToMenuPl", font, GLOB_IND_W - (W_WIN - NUM_CELL_X * EDGE) / 2 + (W_WIN - 2 * 120) / 3, ((H_WIN - NUM_CELL_Y * EDGE) / 2 - 30) / 2 + GLOB_IND_H + NUM_CELL_Y * EDGE, 120, 30);
		button [NumButton++] = new Button (buttonImage, "Help", "HelpPl", font, GLOB_IND_W - (W_WIN - NUM_CELL_X * EDGE) / 2 + 2 * (W_WIN - 2 * 120) / 3 + 120, ((H_WIN - NUM_CELL_Y * EDGE) / 2 - 30) / 2 + GLOB_IND_H + NUM_CELL_Y * EDGE, 120, 30);

		button [NumButton++] = new Button (buttonImage, "0", "EnterPass", font, GLOBAL_W / 2 - 120 / 2, GLOB_IND_H + EDGE * 7 + 200, 24, 30);
		button [NumButton++] = new Button (buttonImage, "1", "EnterPass", font, GLOBAL_W / 2 - 120 / 2 + 24, GLOB_IND_H + EDGE * 7 + 200, 24, 30);
		button [NumButton++] = new Button (buttonImage, "2", "EnterPass", font, GLOBAL_W / 2 - 120 / 2 + 48, GLOB_IND_H + EDGE * 7 + 200, 24, 30);
		button [NumButton++] = new Button (buttonImage, "3", "EnterPass", font, GLOBAL_W / 2 - 120 / 2 + 72, GLOB_IND_H + EDGE * 7 + 200, 24, 30);
		button [NumButton++] = new Button (buttonImage, "4", "EnterPass", font, GLOBAL_W / 2 - 120 / 2 + 96, GLOB_IND_H + EDGE * 7 + 200, 24, 30);
		button [NumButton++] = new Button (buttonImage, "5", "EnterPass", font, GLOBAL_W / 2 - 120 / 2, GLOB_IND_H + EDGE * 7 + 250, 24, 30);
		button [NumButton++] = new Button (buttonImage, "6", "EnterPass", font, GLOBAL_W / 2 - 120 / 2 + 24, GLOB_IND_H + EDGE * 7 + 250, 24, 30);
		button [NumButton++] = new Button (buttonImage, "7", "EnterPass", font, GLOBAL_W / 2 - 120 / 2 + 48, GLOB_IND_H + EDGE * 7 + 250, 24, 30);
		button [NumButton++] = new Button (buttonImage, "8", "EnterPass", font, GLOBAL_W / 2 - 120 / 2 + 72, GLOB_IND_H + EDGE * 7 + 250, 24, 30);
		button [NumButton++] = new Button (buttonImage, "9", "EnterPass", font, GLOBAL_W / 2 - 120 / 2 + 96, GLOB_IND_H + EDGE * 7 + 250, 24, 30);

		button [NumButton++] = new Button (buttonImage, "End lvl", "lvlComplete", font, GLOBAL_W / 2 - 120 / 2, GLOB_IND_H - 30 - ((H_WIN - NUM_CELL_Y * EDGE) / 2 - 30) / 2, 120, 30);
	}

	void initializeLine (){
		lines = VertexArray (Lines, (NUM_H_LINE + NUM_V_LINE + 2) * 2); //������ �����
		int i = 0; //i-������� ����� ���������� � ������
		for (; i < NUM_V_LINE * 2; i += 2){ //�������� ������������ �����
			lines [i].position = Vector2f ((float) GLOB_IND_W + i * EDGE / 2, (float) GLOB_IND_H);
			lines [i + 1].position = Vector2f ((float) GLOB_IND_W + i * EDGE / 2, (float) GLOB_IND_H + NUM_CELL_Y * EDGE);
		}
		for (int k = 0; i < (NUM_H_LINE + NUM_V_LINE) * 2; i += 2, k += 2){ //�������� �������������� �����
			lines [i].position = Vector2f ((float) GLOB_IND_W, (float) EDGE * k / 2 + GLOB_IND_H);
			lines [i + 1].position = Vector2f ((float) GLOB_IND_W + NUM_CELL_X * EDGE, (float) EDGE * k / 2 + GLOB_IND_H);
		}
	}

	void initialize (){
		Start.x = GLOB_IND_H; Start.y = GLOB_IND_H + NUM_CELL_Y * EDGE; //�������������� ��������� �����
		Finish.x = GLOB_IND_W + NUM_CELL_X * EDGE; Finish.y = GLOB_IND_H; //�������������� �����

		state = menu; //�������������� ���������

		Image playerImage; //������� ������� ������
		playerImage.loadFromFile ("player.png");
		pl  = new Player (playerImage, Start.x, Start.y, EDGE, EDGE); //�������� ������� �����

		timer = 0;

		Image backgroundImage;
		backgroundImage.loadFromFile ("background.png");
		plBackground = new PlayerBackground (backgroundImage, 0, 0, 2560, 1280);

		Image backgroundImage2;
		backgroundImage2.loadFromFile ("background2.png");
		plBackground2 = new PlayerBackground (backgroundImage2, 0, 0, GLOBAL_W, GLOBAL_H);
		plBackground2 -> changeCoord (GLOBAL_W / 2, GLOBAL_H / 2, 0);
	}

	void initializeWall (){
		wallImage.loadFromFile ("wall.png");
	
		NumWall = 0; //���������� ����
		CoordWall = new bool* [NUM_CELL_X];
		for (int i = 0; i < NUM_CELL_X; i++){
			CoordWall [i] = new bool [NUM_CELL_Y];
			for (int j = 0; j < NUM_CELL_Y; j++)
				CoordWall [i][j] = false;
		}
	}

	void draw (){
		window [0].setView (view); //��������� ������
		window [0].clear (Color (40, 36, 62));
		if (state == admin || state == player){
			window[0].draw (lines); //����� ������ �����
			for (int j = 0; j < NumWall; j++) //����� �����
				if (ArrWall [j] -> drawThis)
					window [0].draw (ArrWall [j] -> sprite);
		}
		if (plBackground -> drawThis) //���
			window -> draw (plBackground -> sprite);
		window -> draw (plBackground2 -> sprite); //���
		if (state == admin || state == player){
			for (int j = 0; j < NumWall; j++) //����� �����
				if (ArrWall [j] -> name == "Finish" && ArrWall [j] -> drawThis)
					window [0].draw (ArrWall [j] -> sprite);
		}
		for (int i = 0; i < NumButton; i++) //����� ������
			if (button [i] -> drawThis)
				button [i] -> draw (window[0]);
		if (state == player){
			window [0].draw (pl [0].sprite); //����� ������
			for (int i = 0; i < NumAnsw; i++) //����� ��������������� �����
				window[0].draw (helpWall [i] -> sprite);
		}
		if (state == reqPass)
			for (int i = 0; i < NumButton; i++)
				if (button [i] -> name == "RequestPass" || button [i] -> name == "EnterPass")
					button [i] -> draw (*window);
			
		window [0].display ();
	}

	void createWalls (){
	int tmpX, tmpY, tmpX2, tmpY2;
	int tmp; bool wallDeleted = false;
	if (Mouse::isButtonPressed (Mouse::Left)){
		//cout << posMouse.x << " " << posMouse.y << endl;
		//cout << GLOB_IND_W << " " << GLOB_IND_H << " " << endl;
		if ((posMouse.x >= GLOB_IND_W) && (posMouse.x <= GLOB_IND_W + NUM_CELL_X * EDGE) && (posMouse.y >= GLOB_IND_H) && (posMouse.y <= GLOB_IND_H + NUM_CELL_Y * EDGE)){
			if (timer > 200){
				timer = 0;	
				tmpX = (int) posMouse.x; tmp = tmpX % EDGE; tmpX -= tmp; 
				tmpY = (int) posMouse.y; tmp = tmpY % EDGE; tmpY -= tmp; 
				tmpX2 = (tmpX - GLOB_IND_W) / EDGE; tmpY2 = (tmpY - GLOB_IND_H) / EDGE;
				if (CoordWall [tmpX2][tmpY2]){ //��������� �� ������� �����, ���� ���� �� ����� ���������
					for (int i = 0; i < NumWall; i++)
						if (ArrWall [i] -> x == tmpX && ArrWall [i] -> y == tmpY){
							ArrWall [i] -> drawThis = false;
							CoordWall [tmpX2][tmpY2] = false;
							wallDeleted = true;
							break;
						}
				}
				if(Keyboard::isKeyPressed (Keyboard::LControl)){
					for (int i = 0; i < NumWall; i++)
					if (ArrWall [i] -> name == "Start"){
						ArrWall [i] -> drawThis = false;
						CoordWall [(ArrWall [i] -> x - GLOB_IND_W) / EDGE][(ArrWall [i] -> y - GLOB_IND_H) / EDGE] = false;
					}
					ArrWall [NumWall++] = new Wall (wallImage, "Start", tmpX, tmpY, EDGE, EDGE);
					Start.x = tmpX; Start.y = tmpY;
				}
				else
					if (Keyboard::isKeyPressed (Keyboard::LShift)){
						for (int i = 0; i < NumWall; i++)
							if (ArrWall [i] -> name == "Finish"){
								ArrWall [i] -> drawThis = false;
								CoordWall [(ArrWall [i] -> x - GLOB_IND_W) / EDGE][(ArrWall [i] -> y - GLOB_IND_H) / EDGE] = false;
							}
							ArrWall [NumWall++] = new Wall (wallImage, "Finish", tmpX, tmpY, EDGE, EDGE);
							Finish.x = tmpX; Finish.y = tmpY;
					}
					else{
						if (!CoordWall [tmpX2][tmpY2] && !wallDeleted){
							bool tmpB = true;
							for (int i = 0; i < NumWall; i++){
								if (ArrWall [i] -> x == tmpX && ArrWall [i] -> y == tmpY){
									ArrWall [i] -> drawThis = true;
									tmpB = false;
								}
							}
							if (tmpB)
								ArrWall [NumWall++] = new Wall (wallImage, "Wall", tmpX, tmpY, EDGE, EDGE);
							CoordWall [tmpX2][tmpY2] = true;
						}
					}
			}
		}
	}
}

	void saveFile (){
		cout << "Enter name of file which you want save:" << endl;
		char tmpC [50];
		cin >> tmpC;
		ofstream outF (tmpC);
		int tmp = 0;
		for (int i = 0; i < NumWall; i++){
			if (ArrWall [i] -> drawThis)
				tmp++;
		}
		outF << tmp << endl;
		for (int i = 0; i < NumWall; i++){
			if (ArrWall [i] -> drawThis){
				if (ArrWall [i] -> name == "Wall")
					outF << ArrWall [i] -> x << " " << ArrWall [i] -> y << " Wall" << endl;
				if (ArrWall [i] -> name == "Start")
					outF << ArrWall [i] -> x << " " << ArrWall [i] -> y << " Start" << endl;
				if (ArrWall [i] -> name == "Finish")
					outF << ArrWall [i] -> x << " " << ArrWall [i] -> y << " Finish" << endl;
			}
		}
	}

	void openFile (){
		int tmpX, tmpY;
		char tmpC [50];
		if (NumWall != 0){
			for (int i = 0; i < NumWall; i++)
				ArrWall [i] -> ~Wall ();
		}
		cout << "Enter name of file which you want open:" << endl;
		cin >> tmpC;
		ifstream inF (tmpC);

		//inF >> Start.x >> Start.y >> Finish.x >> Finish.y ;
		inF >> NumWall;
		for (int i = 0; i < NumWall; i++){
			inF >> tmpX >> tmpY >> tmpC;
			if (strcmp (tmpC, "Wall") == 0)
				ArrWall [i] = new Wall (wallImage, "Wall", tmpX, tmpY, EDGE, EDGE);
			if (strcmp (tmpC, "Start") == 0){
				Start.x = tmpX; Start.y = tmpY;
				ArrWall [i] = new Wall (wallImage, "Start", tmpX, tmpY, EDGE, EDGE);
			}
			if (strcmp (tmpC, "Finish") == 0){
				Finish.x = tmpX; Finish.y = tmpY;
				ArrWall [i] = new Wall (wallImage, "Finish", tmpX, tmpY, EDGE, EDGE);
			}
			if (strcmp (tmpC, "Start") != 0){
				ArrWall [i] -> drawThis = true;
				CoordWall [(tmpX - GLOB_IND_W) / EDGE][(tmpY - GLOB_IND_H) / EDGE] = true;
				if (strcmp (tmpC, "Finish") == 0)
					CoordWall [(tmpX - GLOB_IND_W) / EDGE][(tmpY - GLOB_IND_H) / EDGE] = false;
			}
		}
	}

	void openSpecificFile (char *nameFile){
		int tmpX, tmpY;
		char tmpC [40];
		if (NumWall != 0){
			for (int i = 0; i < NumWall; i++)
				ArrWall [i] -> ~Wall ();
		}
		for (int i = 0; i < NUM_CELL_X; i++)
			for (int j = 0; j < NUM_CELL_Y; j++)
				CoordWall [i][j] = false;
		NumAnsw = 0;
		ifstream inF (nameFile);
		inF >> NumWall;
		for (int i = 0; i < NumWall; i++){
			inF >> tmpX >> tmpY >> tmpC;
			if (strcmp (tmpC, "Wall") == 0)
				ArrWall [i] = new Wall (wallImage, "Wall", tmpX, tmpY, EDGE, EDGE);
			if (strcmp (tmpC, "Start") == 0){
				Start.x = tmpX; Start.y = tmpY;
				ArrWall [i] = new Wall (wallImage, "Start", tmpX, tmpY, EDGE, EDGE);
				ArrWall [i] -> drawThis = false;
				CoordWall [(tmpX - GLOB_IND_W) / EDGE][(tmpY - GLOB_IND_H) / EDGE] = false;
			}
			if (strcmp (tmpC, "Finish") == 0){
				Finish.x = tmpX; Finish.y = tmpY;
				ArrWall [i] = new Wall (wallImage, "Finish", tmpX, tmpY, EDGE, EDGE);
			}
			if (strcmp (tmpC, "Start") != 0){
				ArrWall [i] -> drawThis = true;
				CoordWall [(tmpX - GLOB_IND_W) / EDGE][(tmpY - GLOB_IND_H) / EDGE] = true;
				if (strcmp (tmpC, "Finish") == 0)
					CoordWall [(tmpX - GLOB_IND_W) / EDGE][(tmpY - GLOB_IND_H) / EDGE] = false;
			}
		}
	}
};


Wall* System::helpWall [10000];
int System::NumWall; //���������� ����
Wall* System::ArrWall [10000]; //������ ����
bool** System::CoordWall; //���������� ����
int System::NumButton; //���������� ������
Button* System::button [70]; //������ ������
Font System::font; //�����
Clock System::clock; //�����
Vector2i System::mousePosWin; //���������� ���� �����. ����
Vector2f System::posMouse; //���������� ���� �����. �����
RenderWindow* System::window;
VertexArray System::lines;
Image System::wallImage; //�������� ������� ����
Player* System::pl;
PlayerBackground* System::plBackground;
PlayerBackground* System::plBackground2;
float System::timer;
float System::time;

System Basic;

class Menu : public System{
public:
	void update (){
		switch (state){
		case admin:///////////////////////////////////////////////////////
			createWalls ();
			for (int i = 0; i < NumButton; i++)
				if (button [i] -> drawThis){
					button [i] -> checkCursor (posMouse);
					if (button [i] -> buttClick && button [i] -> name == "SaveAd")
						saveFile ();
					if (button [i] -> buttClick && button [i] -> name == "OpenAd")
						openFile ();
					if (button [i] -> buttClick && button [i] -> name == "BackToMenuAd"){
						state = menu;
						for (int i = 0; i < NumButton; i++)
							if (button [i] -> state == menu)
								button [i] -> drawThis = true;
							else
								button [i] -> drawThis = false;
					}
				}
				break;
				
		case player:////////////////////////////////////////////////////////////
			timer += time;
			pl [0].update (CoordWall);
			plBackground -> changeCoord (pl [0]. x, pl [0].y);
			for (int i = 0; i < NumButton; i++)
				if (button [i] -> drawThis){
					button [i] -> checkCursor (posMouse);
					if (button [i] -> buttClick && button [i] -> name == "BackToMenuPl"){
						plBackground -> drawThis = false;
						timer = 0;
						NumAnsw = 0;
						state = menu;
						for (int i = 0; i < NumButton; i++)
							if (button [i] -> state == menu)
								button [i] -> drawThis = true;
							else
								button [i] -> drawThis = false;
					}
					if (button [i] -> buttClick && button [i] -> name == "lvlComplete"){
						if (CurrentLVL < 2){
							CurrentLVL++;
							char tmpC [30], *tmpC2;
							tmpC2 = _itoa (CurrentLVL, tmpC, 10);
							char nameFile [30] = "lvl";
							strcat (nameFile, tmpC2);
							strcat (nameFile, ".txt");
							cout << nameFile << endl;
							openSpecificFile (nameFile);
							pl [0].changeCoord (Start.x, Start.y);
						}
					}
					if (button [i] -> buttClick && button [i] -> name == "HelpPl"){
						if (timer > 2000){ //20 000 = 5 ������ ��������� �������, 40 000=15, 80 000=30
							timer = 0;
							Coordinate fn, sizeMap, st;
							sizeMap.x = NUM_CELL_Y; ///////////////////////////////////��, ����� �Ѩ ���������, ��� ������������������ ��� � �����!!!
							sizeMap.y = NUM_CELL_X;
							st.x = (pl [0].y - GLOB_IND_H) / EDGE;
							st.y = (pl [0].x - GLOB_IND_W) / EDGE;
							fn.x = (Finish.y - GLOB_IND_H) / EDGE;
							fn.y = (Finish.x - GLOB_IND_W) / EDGE;
							outputSearch (CoordWall, st, fn, sizeMap);

							cout << endl << NumAnsw << endl;
							int tmp = NumAnsw;
							NumAnsw = NumAnsw / 7; 
							int j;
							//cout << NumAnsw << endl;
							for (int i = 0; i < NumAnsw; i++){
								j = tmp - i - 1;
								//cout << j << " ";
								helpWall [i] = new Wall (wallImage, "HelpWall", Arr [j].y * EDGE + GLOB_IND_W, Arr [j].x * EDGE + GLOB_IND_H, EDGE, EDGE);
							}
						}
					}
				}
					
				if (lvlComplete){
					for (int i = 0; i < NumButton; i++)
						if (button [i] -> name == "lvlComplete")
							button [NumButton - 1] -> drawThis = true;
				}
				else{
					for (int i = 0; i < NumButton; i++)
						if (button [i] -> name == "lvlComplete")
							button [NumButton - 1] -> drawThis = false;
				}
				break;

		case menu:////////////////////////////////////////////////////////////////
			button [NumButton - 1] -> drawThis = false; 
			for (int i = 0; i < NumButton; i++)
				if (button [i] -> drawThis){
					button [i] -> checkCursor (posMouse);
					if (button [i] -> buttClick && button [i] -> name == "Mode"){
						state = mode; 
						for (int i = 0; i < NumButton; i++)
							if (button [i] -> state == mode)
								button [i] -> drawThis = true;
							else
								button [i] -> drawThis = false;
					}
					if (button [i] -> buttClick && button [i] -> name == "Go!"){
						if (AdOrPlMode == "AdminMode"){
							openSpecificFile ("empty.txt");
							state = admin; 
							for (int i = 0; i < NumButton; i++)
								if (button [i] -> state == admin)
									button [i] -> drawThis = true;
								else
									button [i] -> drawThis = false;
						}
						if (AdOrPlMode == "PlayerMode"){
							state = player;
							char nameFile [30];
							strcpy_s (nameFile, "lvl1.txt");
							openSpecificFile ("lvl1.txt");
							pl [0].changeCoord (Start.x, Start.y);
							plBackground -> drawThis = true;
							plBackground -> changeCoord (Start.x, Start.y);
							for (int i = 0; i < NumButton; i++)
								if (button [i] -> state == player)
									button [i] -> drawThis = true;
								else
									button [i] -> drawThis = false;
						}
					}
					if (button [i] -> buttClick && button [i] -> name == "Exit")
						state = exitt;
				}
				break;
		case mode:///////////////////////////////////////////////////////////////////////
			for (int i = 0; i < NumButton; i++)
				if (button [i] -> drawThis){
					button [i] -> checkCursor (posMouse);
					if (button [i] -> buttClick && button [i] -> name == "BackToMenu"){
						state = menu;
						for (int i = 0; i < NumButton; i++)
							if (button [i] -> state == menu)
								button [i] -> drawThis = true;
							else
								button [i] -> drawThis = false;
					}
				}
            if (AdOrPlMode == "AdminMode" && !PassEnter)
				state = reqPass;
				break;
		case reqPass://////////////////////////////////////////////////////////////////
			cout << Pass << endl;
			if (strlen (Pass) == 4 && !PassEnter)
				if (strcmp (Pass, "4329") == 0){
					state = mode; PassEnter = true;
					AdOrPlMode = "AdminMode"; strcpy (Pass, "");
				}
				else{
					state = mode; PassEnter = false;
					AdOrPlMode = "PlayerMode"; strcpy (Pass, "");
				}

			for (int i = 0; i < NumButton; i++)
				if (button [i] -> name == "RequestPass" || button [i] -> name == "EnterPass"){
					button [i] -> checkCursor (posMouse);
					if (button [i] -> buttClick && button [i] -> name == "EnterPass"){
						if (button [i] -> value == 0)
							strcat (Pass, "0");
						if (button [i] -> value == 1)
							strcat (Pass, "1");
						if (button [i] -> value == 2)
							strcat (Pass, "2");
						if (button [i] -> value == 3)
							strcat (Pass, "3");
						if (button [i] -> value == 4)
							strcat (Pass, "4");
						if (button [i] -> value == 5)
							strcat (Pass, "5");
						if (button [i] -> value == 6)
							strcat (Pass, "6");
						if (button [i] -> value == 7)
							strcat (Pass, "7");
						if (button [i] -> value == 8)
							strcat (Pass, "8");
						if (button [i] -> value == 9)
							strcat (Pass, "9");
					}
				}
			break;
		case exitt://///////////////////////////////////////////////////////////////////
			window[0].close ();
			break;
		}
	}
};

Menu GameMenu;

int main (){
	view.reset (FloatRect (0, 0, W_WIN, H_WIN)); //�������� ������
	setCoordinateForView (GLOBAL_W / 2, GLOBAL_H / 2); //������� ������ � �������
	cout << NUM_H_LINE << "-horizont " << NUM_V_LINE << "-vertical" << endl;
	cout << W_WIN << "-W_WIN " << H_WIN << "-H_WIN"<<endl;
	Basic.window = new RenderWindow (VideoMode (W_WIN, H_WIN), "LABYRINTH PRO"/*, Style::Fullscreen*/); //�������� ����
	Basic.initialize ();
	Basic.initializeButton ();
	Basic.initializeLine ();
	Basic.initializeWall ();

	while (Basic.window[0].isOpen ()){
		Basic.time = (float) Basic.clock.getElapsedTime ().asMicroseconds(); //����� ������ ��� �����������
		Basic.clock.restart ();
		Basic.time = Basic.time / 800;
		Basic.timer += Basic.time;

		Basic.mousePosWin = Mouse::getPosition (Basic.window[0]); //���������� ���� �����. ����
		Basic.posMouse = Basic.window[0].mapPixelToCoords (Basic.mousePosWin); //���������� ���� �����. �����

		Event event; //���������� �������� ���� !����� ��� ����������� �������� ����
		while (Basic.window[0].pollEvent (event)){
			if ((event.type == Event::Closed) || Keyboard::isKeyPressed (Keyboard::Escape)) //������� ���� ����� � ��������
				Basic.window[0].close (); 
		}		
		GameMenu.update ();
		Basic.draw ();
	}
	return 0;
}