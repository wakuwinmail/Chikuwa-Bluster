#define _USE_MATH_DEFINES
#include "DxLib.h"
#include "math.h"
#define PSHOT_NUM 256
#define ESHOT_NUM 1024
#define ENEMY_NUM 8
#define PSHOT_SPEED 10
#define ESHOT_SPEED 3
#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600
#define LIFE_VALUE 50
#define FLAME_RATE 60
#define ROOT2 1.41421356
#define ROOT3 1.7320508075

class title {
	int logo, color, color2, w, h, font, font2;
	int haikei = 0;
	bool select = TRUE;
public:
	title(int a, int b, int c, int d) :logo(a), haikei(b), font(c), font2(d) { GetGraphSize(logo, &w, &h); }
	int start(int);
	int help(int);
};
int title::start(int) {
	int rot = 0;
	rot = GetMouseWheelRotVol();
	SetDrawScreen(DX_SCREEN_BACK);
	DrawExtendGraph(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, haikei, TRUE);
	DrawGraph(SCREEN_WIDTH / 2 - w / 2, 0, logo, TRUE);
	if (CheckHitKey(KEY_INPUT_UP) == 1)select = TRUE;
	if (CheckHitKey(KEY_INPUT_DOWN) == 1)select = FALSE;
	if (rot >= 1) select = TRUE;
	if (rot <= -1) select = FALSE;
	if (select) {
		color = GetColor(255, 0, 0); color2 = GetColor(0, 255, 0);
	}
	else {
		color = GetColor(0, 255, 0); color2 = GetColor(255, 0, 0);
	}
	DrawStringToHandle(SCREEN_WIDTH / 3, SCREEN_HEIGHT / 2, "すたーと", color, font);
	DrawStringToHandle(SCREEN_WIDTH / 3, SCREEN_HEIGHT / 2 + 50, "へるぷ", color2, font);
	if (CheckHitKey(KEY_INPUT_RETURN) == 1 || (GetMouseInput() & MOUSE_INPUT_LEFT) != 0) {
		WaitTimer(500);
		if (select)return 2;
		else return 1;
	}
	return 0;
}

int title::help(int) {
	SetDrawScreen(DX_SCREEN_BACK);
	DrawExtendGraph(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, haikei, TRUE);
	DrawGraph(SCREEN_WIDTH / 2 - w / 2, 0, logo, TRUE);
	color = GetColor(250, 60, 123); color2 = GetColor(0, 255, 0);
	DrawStringToHandle(SCREEN_WIDTH / 4, SCREEN_HEIGHT * 2 / 5, "敵から発射される弾を避けながら", color, font2);
	DrawStringToHandle(SCREEN_WIDTH / 4, SCREEN_HEIGHT * 2 / 5 + 40, "敵に弾を当てて攻撃しよう", color, font2);
	DrawStringToHandle(SCREEN_WIDTH / 4, SCREEN_HEIGHT * 2 / 5 + 80, "3回攻撃を受けるとゲームオーバーです", color, font2);
	DrawStringToHandle(SCREEN_WIDTH / 3, SCREEN_HEIGHT * 3 / 4, "戻る", color2, font);
	if (CheckHitKey(KEY_INPUT_RETURN) == 1 || (GetMouseInput() & MOUSE_INPUT_LEFT) != 0) {
		WaitTimer(500);
		return 0;
	}
	return 1;
}

struct SHOT
{
	bool flag;
	int x;
	int y;
	int vx;
	int vy;
	int ax;
	int ay;
	int gh;
	int width, height;
	int abilty;
};
struct LIFE
{
	bool live;
	int hp;
	int x;
	int y;
	int width, height;
	int ghandle;
	int vecx;
	int vecy;
};

class Player
{
	int death;
	int pw, ph, j, roop, picture;
	int mutekit = 120;
	bool life;
	bool mikata = TRUE;
	SHOT bullet[PSHOT_NUM];
	LIFE me[1];
public:
	Player(int);
	int move();
	SHOT* shoot(int b);
	int draw(int);
	int detecthit(SHOT* a);
};
Player::Player(int) {
	me[0].live = TRUE;
	me[0].x = 0;
	me[0].y = 0;
	me[0].ghandle = LoadGraph("プレイヤー.png");
	me[0].vecx = 1;
	me[0].vecy = 1;
	picture = LoadGraph("ちくわ.png");
	memset(bullet, 0, sizeof(bullet));
	int w, h;
	GetGraphSize(picture, &w, &h);
	GetGraphSize(me[0].ghandle, &pw, &ph);
	for (int i = 0; i < PSHOT_NUM; i++) {
		bullet[i].flag = FALSE;
		bullet[i].gh = picture;
		bullet[i].width = w;
		bullet[i].height = h;
		bullet[i].abilty = 0;
	}
	roop = 0;
}
int Player::move() {
		GetMousePoint(&me[0].x, &me[0].y);
		if (me[0].x <= 0) { me[0].x = 0; /*SetMousePoint(0, me[0].y);*/ }
		if (me[0].x >= SCREEN_WIDTH - 30) { me[0].x = SCREEN_WIDTH - 30; SetMousePoint(SCREEN_WIDTH - 20, me[0].y); }
		if (me[0].y <= 0) { me[0].y = 0; SetMousePoint(me[0].x, 0); }
		if (me[0].y >= SCREEN_HEIGHT - ph) { me[0].y = SCREEN_HEIGHT - ph; SetMousePoint(me[0].x, SCREEN_HEIGHT - ph); }
		//if (CheckHitKey(KEY_INPUT_ESCAPE) == 1)death = 0;
		return 0;
}
SHOT* Player::shoot(const int b) {//ちくわ発射用の関数
	int j = 1;
	if (CheckHitKey(KEY_INPUT_1) == 1)j = 1;
	if (me[0].live) {
		if (b - roop > 3) {
			int k = 0;
			for (int i = 0; i < PSHOT_NUM &&k<3; i++) {
				if (bullet[i].flag == false) {
					bullet[i].flag = TRUE;
					bullet[i].x = me[0].x + pw;
					bullet[i].y = me[0].y + ph / 2;
					bullet[i].ay = 98 / FLAME_RATE;//挙動が流石に草生える。なんで落ちたり落ちなかったりするのかわからん。小数周りの問題。丸め方を考えるかDOUBLE使え
					bullet[i].abilty = k;
					k++;
						switch (bullet[i].abilty) {
						case 0:
							bullet[i].vx = PSHOT_SPEED;
							bullet[i].vy = 0;
							break;
						case 1:
							bullet[i].vx = PSHOT_SPEED*cos(M_PI / 20);
							bullet[i].vy = int(PSHOT_SPEED*sin(M_PI / 20));
							break;
						case 2:
							bullet[i].vx = PSHOT_SPEED*cos(M_PI / 20);
							bullet[i].vy = -(int(PSHOT_SPEED*sin(M_PI / 20)));//カウント変数のミス。気を付けっましょい
							break;
						default:
							break;
						}
					}
			}
			roop = b;
		}
	}
	return bullet;
}

int Player::draw(int) {
	if (me[0].live)DrawGraph(me[0].x, me[0].y, me[0].ghandle, TRUE);
	else if (mutekit % 2 == 0) { DrawGraph(me[0].x, me[0].y, me[0].ghandle, TRUE); mutekit--; }
	else { mutekit--; }
	for (int i = 0; i < PSHOT_NUM; i++) {
		if (bullet[i].flag) {//果たしてDRAWで位置更新しても良いのだろうか？保留
			bullet[i].vy += bullet[i].ay;//下が正方向
			switch (bullet[i].abilty)
			{
			case 0:
				bullet[i].x += bullet[i].vx;
				bullet[i].y += bullet[i].vy;
				break;
			case 1:
				bullet[i].x += bullet[i].vx;
				bullet[i].y += bullet[i].vy;
				break;
			case 2:
				bullet[i].x += bullet[i].vx;
				bullet[i].y += bullet[i].vy;
				break;
			default:
				break;
			}
		}
		if (bullet[i].x > SCREEN_WIDTH + 20 || bullet[i].y > SCREEN_HEIGHT - bullet[0].height || bullet[i].y < 0) {
			bullet[i].flag = false; bullet[i].x = 0; bullet[i].y = 0;
		}
		if (bullet[i].flag) {
			DrawGraph(bullet[i].x - bullet[i].width / 2, bullet[i].y - bullet[i].height / 2, bullet[i].gh, TRUE);
		}
	}
	if (mutekit == 0) {
		me[0].live = TRUE;
		mutekit = 120;
	}
	//DrawFormatString(100, 0, GetColor(0, 100, 200), "%d", death);
	return 0;
}

int Player::detecthit(SHOT* a) {
	for (int i = 0; i < ESHOT_NUM; i++) {
		if (a[i].flag) {
			if (me[0].live && a[i].x <= me[0].x + pw * 4 / 7 && a[i].x + a[i].width >= me[0].x + pw * 3 / 7 && a[i].y <= me[0].y + ph * 3 / 7 && a[i].y + a[i].height >= me[0].y + ph * 4 / 7) {
				me[0].live = FALSE;
				a[i].flag = FALSE;
				return 1;
			}
		}
		if (CheckHitKey(KEY_INPUT_SPACE) == 1) {
			for (int i = 0; i < ESHOT_NUM; i++) {
				a[i].flag = FALSE;
			}
			PlayMovie("cutin.mp4", 1, DX_MOVIEPLAYTYPE_NORMAL);
		}
		/*for (int j = 0; j < PSHOT_NUM; j++) {
		for (int i = 0; i < ENEMY_NUM; i++) {
		if (bullet[j].flag&&a[i].flag&&a[i].x >= bullet[j].x&&a[i].x <= bullet[j].x + bullet[j].width&& a[i].y + a[i].height >= bullet[j].y&& a[i].y <= bullet[j].y + bullet[j].height) {
		bullet[j].flag = FALSE;
		a[i].flag = FALSE;
		}
		}
		}*/
	}
	return 0;
}

class Enemy {
	int j = 0;
	int enex, eney;
	int itemflag, ighandle, ix, iy;
	int hpghandle;
	int ew, eh;
	bool life;
	bool mikata = false;
	SHOT bullet[ESHOT_NUM];
	LIFE teki[ENEMY_NUM];
	bool live = TRUE;
	int roop = 0;
	int roop2 = 0;
	int score = 0;
public:
	Enemy(int a);
	void generate();
	SHOT* shoot(int b);
	int move(int d);
	int move2(int e);
	int move3(int);
	int draw(int c);
	int detecthit(SHOT* a);
};
Enemy::Enemy(int a) {
	int temp = a;
	GetGraphSize(temp, &ew, &eh);
	teki[0].ghandle = temp;
	for (int i = 0; i < ENEMY_NUM; i++) {
		teki[i].hp = 0;
		teki[i].live = TRUE;
		teki[i].hp += 10;
		teki[i].x = SCREEN_WIDTH - 15;
		teki[i].y = SCREEN_HEIGHT / 5 + (i - 1) * 50;
		teki[i].ghandle = teki[0].ghandle;
		teki[i].vecx = 1;
		teki[i].vecy = 1;
		GetGraphSize(temp, &teki[i].width, &teki[i].height);
	}
	memset(bullet, 0, sizeof(bullet));
	temp = LoadGraph("おでん2.png");
	int w, h;
	GetGraphSize(temp, &w, &h);
	for (int i = 0; i < ESHOT_NUM; i++) {
		bullet[i].flag = FALSE;
		bullet[i].gh = temp;
		bullet[i].width = w;
		bullet[i].height = h;
	}
	ighandle = LoadGraph("ぱわーあっぷ.png");
	itemflag = 0;
	hpghandle = LoadGraph("hpbar.png");
};
void Enemy::generate() {
	for (int i = 0; i < ENEMY_NUM; i++) {
		teki[i].live = TRUE;
		//teki[i].hp += 15;
		if (SCREEN_HEIGHT / 5 + (i - 1) * 50<SCREEN_HEIGHT) {
			teki[i].x = SCREEN_WIDTH - 15;
			teki[i].y = SCREEN_HEIGHT / 5 + (i - 1) * 80;
		}
		if (teki[i].x < 0 || teki[i].x > SCREEN_WIDTH || teki[i].y < 0 || teki[i].y + teki[i].height>SCREEN_HEIGHT)teki[i].hp = 0;
	}
}

SHOT* Enemy::shoot(int b) {
	//if (CheckHitKey(KEY_INPUT_RETURN) == 1 || CheckHitKey(KEY_INPUT_SPACE) == 1 || (GetMouseInput() & MOUSE_INPUT_LEFT) != 0) {
	int l=0;
	for (int i=0;i<ENEMY_NUM;i++) {
		if (teki[i].live)l++;
	}
	int j = 1;
	int h = GetRand(ENEMY_NUM-1);
	if (CheckHitKey(KEY_INPUT_1) == 1)j = 1;
	if (b - roop > 2) {
		if (teki[h].live) {
			for (int i = 0; i < ESHOT_NUM; i++) {
				if (bullet[i].flag == false) {
					switch (j) {
					case 0:
						bullet[i].flag = TRUE;
						bullet[i].x = teki[h].x + ew;
						bullet[i].y = teki[h].y + eh / 2;
						bullet[i].abilty = 0;
						break;
					case 1:
						for (int k = 0; k < 3; k++) {
							for (; ; i++) {
								if (i >= ESHOT_NUM)i = 0;
								if (!bullet[i].flag)break;//ここフリーズの危険性あり
							}
							bullet[i].flag = TRUE;
							bullet[i].x = teki[h].x;
							bullet[i].y = teki[h].y + eh / 2;
							bullet[i].abilty = k;
							i++;
						}
						break;
					default:
						break;
					}
					i = ESHOT_NUM;
					roop = b;
				}
			}
		}
	}

	for (int i = 0; i < ESHOT_NUM; i++) {
		if (bullet[i].flag) {
			if (bullet[i].x <-10 || bullet[i].y<0 || bullet[i].y + bullet[i].height>SCREEN_HEIGHT+10)bullet[i].flag = false;
		}
	}
	return bullet;
};

int Enemy::move(int d) {
	for (int i = 0; i < ENEMY_NUM; i++) {
		switch (d % 3) {
		case 0:
			teki[i].y += GetRand(10);
			break;
		case 1:
			teki[i].y -= GetRand(10);
			break;
		case 2:
			break;
		default:
			break;
		}
		if (teki[i].x < 0)teki[i].x = SCREEN_WIDTH - ew;
		if (teki[i].x < SCREEN_WIDTH)teki[i].x = SCREEN_WIDTH - ew;
		if (teki[i].y < 0)teki[i].y = 0;
		if (teki[i].y + teki[i].height>SCREEN_HEIGHT)teki[i].y = 0;
		if (teki[i].y < 0)teki[i].y = SCREEN_HEIGHT - eh;
	}
	return 0;
}

int Enemy::move2(int e) {
	for (int i = 0; i < ENEMY_NUM; i++) {
		if (e % (i + 1) == 0)teki[i].x -= 5;
		switch (e % 4) {
		case 0:
			teki[i].y += GetRand(20);
			break;
		case 1:
			teki[i].y -= GetRand(20);
			break;
		default:
			break;
		}
		if (teki[i].x < 0)teki[i].x = SCREEN_WIDTH - ew;
		if (teki[i].x > SCREEN_WIDTH)teki[i].x = SCREEN_WIDTH - ew;
		if (teki[i].y + teki[i].height>SCREEN_HEIGHT)teki[i].y = 0;
		if (teki[i].y < 0)teki[i].y = SCREEN_HEIGHT - eh;
	}
	return 0;
}

int Enemy::move3(int) {
	for (int i = 0; i < ENEMY_NUM; i++) {
		if (TRUE) {
			teki[i].x -= (GetRand(5)) * teki[i].vecx;
			teki[i].y -= (GetRand(5)) * teki[i].vecy;
		}
		if (teki[i].x < 0)teki[i].vecx = -1;
		if (teki[i].x > SCREEN_WIDTH)teki[i].vecx = 1;
		if (teki[i].y < 0)teki[i].vecy = -1;
		if (teki[i].y + teki[i].height>SCREEN_HEIGHT)teki[i].vecy = 1;
	}
	return 0;
}

int Enemy::draw(int c) {
	score = c;
	teki[0].hp = 0;
	teki[0].live = FALSE;
	for (int i = 0; i < ENEMY_NUM; i++) {
		if (SCREEN_HEIGHT / 5 + (i - 1) * 50<SCREEN_HEIGHT) {
			teki[i].x = SCREEN_WIDTH - 15;
			teki[i].y = SCREEN_HEIGHT / 5 + (i - 2) * 80;
		}
		if (teki[i].hp < 1)teki[i].live = FALSE;
		if (teki[i].live) {
			DrawGraph(teki[i].x, teki[i].y, teki[i].ghandle, TRUE);
		}
		if (teki[i].x < 0 || teki[i].x > SCREEN_WIDTH || teki[i].y < 0 || teki[i].y + teki[i].height>SCREEN_HEIGHT)teki[i].hp = 0;
		for (int j = 0; j < teki[i].hp; j++) { DrawGraph(teki[i].x + j, teki[i].y - 5, hpghandle, TRUE); }
		if (itemflag == i)DrawGraph(ix -= ESHOT_SPEED, iy, ighandle, TRUE);
		if (ix < 0)itemflag = FALSE;
	}
	for (int i = 0; i < ESHOT_NUM; i++) {
		if (bullet[i].flag) {
			switch (bullet[i].abilty)
			{
			case 0:
				bullet[i].x -= ESHOT_SPEED;
				DrawGraph(bullet[i].x - bullet[i].width / 2, bullet[i].y - bullet[i].height / 2, bullet[i].gh, TRUE); break;
			case 1:
				bullet[i].x -= ESHOT_SPEED*ROOT3 / 2; bullet[i].y += ESHOT_SPEED / 2;
				DrawRotaGraph(bullet[i].x - bullet[i].width / 2, bullet[i].y - bullet[i].height / 2, 1, 0, bullet[i].gh, TRUE); break;
			case 2:
				bullet[i].x -= ESHOT_SPEED*ROOT3 / 2; bullet[i].y -= ESHOT_SPEED / 2;
				DrawRotaGraph(bullet[i].x - bullet[i].width / 2, bullet[i].y - bullet[i].height / 2, 1, 0, bullet[i].gh, TRUE); break;
			default:
				break;
			}
		}
	}
	/*for (int i = 0; i < ENEMY_NUM; i++) {
	if (teki[i].live)break;
	if (i == ENEMY_NUM - 1) generate();
	}*/
	for (int i = 0; i < ENEMY_NUM; i++) {
		if (teki[i].live)break;
		if (i == ENEMY_NUM - 1) return 1;
	}
	if (CheckHitKey(KEY_INPUT_G) == 1)generate();
	return 0;
};
int Enemy::detecthit(SHOT *a) {
	for (int i = 0; i < PSHOT_NUM; i++) {
		if (a[i].flag) {
			for (int j = 0; j < ENEMY_NUM; j++) {
				if (teki[j].live) {
					if (a[i].x + a[i].width >= teki[j].x&&a[i].x <= teki[j].x + ew&& a[i].y + a[i].height >= teki[j].y&& a[i].y <= teki[j].y + eh) {
						teki[j].hp--;
						a[i].flag = FALSE;
						if (GetRand(100) < 0) { itemflag = j; ix = teki[j].x; iy = teki[j].y; }
						return 1;
					}
				}
			}
		}
	}
	if (CheckHitKey(KEY_INPUT_V) == 1)return 10;
	return 0;
}

class Enemy2 :public Enemy {
public:
	Enemy2(int a) :Enemy(a) {}
};


/*class Lvup
{
int score,lifep,shotp;
public:
Lvup(){}
int result();
};
int Lvup::result() {

}*/

class Gameover {
	int font;
	int select = 1;
public:
	Gameover(int a);
	void end(int c);
	int clear(int d, int e, int f);
	int resurrect(int& b);
};
Gameover::Gameover(int) {
	font = CreateFontToHandle(NULL, 40, 3);
	ClearDrawScreen();
};
void Gameover::end(int c) {
	SetDrawScreen(DX_SCREEN_BACK);
	ClearDrawScreen();
	DrawFormatStringToHandle(SCREEN_WIDTH / 3, SCREEN_HEIGHT / 4, GetColor(255, 0, 0), font, "GAME OVER");
	DrawFormatStringToHandle(SCREEN_WIDTH / 3, SCREEN_HEIGHT / 3, GetColor(255, 255, 255), font, "スコア：%d", c);
	ScreenFlip();
	WaitTimer(2000);
};
int Gameover::resurrect(int& score) {
	SetDrawScreen(DX_SCREEN_BACK);
	ClearDrawScreen();
	for (; ProcessMessage() == 0;) {
		int rot = 0;
		rot = GetMouseWheelRotVol();
		DrawFormatStringToHandle(SCREEN_WIDTH / 3, SCREEN_HEIGHT / 3, GetColor(255, 255, 255), font, "スコア：%d", score);
		DrawFormatStringToHandle(SCREEN_WIDTH / 3, SCREEN_HEIGHT / 5, GetColor(255, 255, 255), font, "%d点で復活しますか？", LIFE_VALUE*2);
		if (select)DrawFormatStringToHandle(SCREEN_WIDTH / 3, SCREEN_HEIGHT / 1.5, GetColor(255, 255, 255), font, "はい");
		else DrawFormatStringToHandle(SCREEN_WIDTH / 3, SCREEN_HEIGHT / 1.5, GetColor(255, 255, 255), font, "いいえ");
		if (CheckHitKey(KEY_INPUT_UP) == 1)select = TRUE;
		if (CheckHitKey(KEY_INPUT_DOWN) == 1)select = FALSE;
		if (rot >= 1) select = TRUE;
		if (rot <= -1) select = FALSE;
		if (CheckHitKey(KEY_INPUT_RETURN) == 1 || (GetMouseInput() & MOUSE_INPUT_LEFT) != 0 && select == 1)break;
		if (CheckHitKey(KEY_INPUT_RETURN) == 1 || (GetMouseInput() & MOUSE_INPUT_LEFT) != 0 && select == 0)return 0;
		ScreenFlip();
		ClearDrawScreen();
	}
	ScreenFlip();
	score -= LIFE_VALUE*2;
	ClearDrawScreen();
	return 1;
}
int Gameover::clear(int d, int e, int f) {
	int tbonus = 0;
	if ((30 * FLAME_RATE - f) > 0)tbonus = 30 * FLAME_RATE - f;
	SetDrawScreen(DX_SCREEN_BACK);
	ClearDrawScreen();
	DrawFormatStringToHandle(SCREEN_WIDTH / 3, SCREEN_HEIGHT / 4, GetColor(255, 0, 255), font, "GAME CLEAR");
	DrawFormatStringToHandle(SCREEN_WIDTH / 3, SCREEN_HEIGHT / 3, GetColor(255, 255, 255), font, "スコア：%d", d + LIFE_VALUE*e + tbonus);
	DrawFormatStringToHandle(SCREEN_WIDTH / 3, SCREEN_HEIGHT / 2, GetColor(255, 255, 255), font, "Rを押してもう一度");
	ScreenFlip();
	WaitKey();
	if (CheckHitKey(KEY_INPUT_R) == 1)return 1;
	else return 0;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
	LPSTR lpCmdLine, int nCmdShow)
{
	SetGraphMode(SCREEN_WIDTH, SCREEN_HEIGHT, 32);
	ChangeWindowMode(TRUE);
	if (DxLib_Init() == -1)		// ＤＸライブラリ初期化処理
	{
		return -1;			// エラーが起きたら直ちに終了
	}
	int logo, seaback, mountback, font = 0, font2 = 0, life, score, eg, eg2, i = 0;
	//int *x, *y;
	//int w, h;
	int j = 0;
	SHOT *p = NULL;
	SHOT *e = NULL;
	//SetUse3DFlag(FALSE);
	SetDrawScreen(DX_SCREEN_BACK);
	SetWindowSizeChangeEnableFlag(TRUE);
	SetMouseDispFlag(FALSE);
	font = CreateFontToHandle(NULL, 40, 3, -1);
	font2 = CreateFontToHandle(NULL, 30, 3, -1);
	logo = LoadGraph("title.png");
	seaback = LoadGraph("うみ.png");
	mountback = LoadGraph("けいこく.jpg");
	eg = LoadGraph("かまぼこ.png");
	//eg2 = LoadGraph("さつまあげ.png");
retry:

	Player sakana(1);
	Enemy teki(eg);
	//Enemy2 teki2(eg2);
	title taitoru(logo, seaback, font, font2);
	Gameover owata(0);
	life = 3;
	score = 0;
	for (i = 0; i < 2 && ProcessMessage() == 0;) {
		ClearDrawScreen();

		if (i == 0) {
			i = taitoru.start(font);
		}
		else {
			i = taitoru.help(font);
		}
		WaitTimer(1000 / FLAME_RATE);
		ScreenFlip();
	}
	for (i = 0;; i++) {
		//if (ProcessMessage() == 0 && GetMovieStateToGraph(mountback) != 1) { SeekMovieToGraph(mountback, FLAME_RATE);  PlayMovieToGraph(mountback); }
		ClearDrawScreen();
		DrawExtendGraph(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, mountback, FALSE);
		p = sakana.shoot(i);
		e = teki.shoot(i);
		sakana.draw(0);
		sakana.move();
		life -= sakana.detecthit(e);
		//teki.move3(i);
		if (teki.draw(score) == 1) { if(owata.clear(score, life, i)==1)goto retry; else break; }
		score += teki.detecthit(p);
		//teki.draw(score);
		//if (i>600) { owata.clear(score); break; }
		DrawFormatString(10, 0, GetColor(100, 0, 200), "%d", life);
		DrawFormatString(30, 0, GetColor(0, 0, 0), "%d", score);
		DrawFormatString(200, 0, GetColor(200, 200, 200), "%d", i);
		if (CheckHitKey(KEY_INPUT_ESCAPE) == 1)break;
		if (life <= 0) {
			WaitTimer(500);
			if (score >= LIFE_VALUE) {
				if (owata.resurrect(score) == 0) { owata.end(score); break; }
				else life++;
			}
			else { owata.end(score); break; }
		}
		ScreenFlip();
	}
	DxLib_End();	// ＤＸライブラリ使用の終了処理

	return 0;	// ソフトの終了 
}