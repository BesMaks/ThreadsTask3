#include <cmath>
#include <windows.h>
#include <thread>
#include <mutex>
#include <math.h>
using namespace std;

mutex mtx;

const double overall_timeD = 500.0;
const int fps = 30;
const double overall_frames = 2000;
const int frame_timing = overall_frames / fps;

const double defender_speed = 8;
const double striker_horizontal_speed = 8;
const double striker_vertical_speed = striker_horizontal_speed * 2;
double ball_speed = 6;

bool ball_launched = 0;
const int player_size = 40;
const int ball_size = 20;

double defender_position_x = 300.0;
double defender_position_y = 300.0;
double striker_position_x = 1.0;
double striker_position_y = 250.0;
double goalkeeper_position_x = 720.0;
double goalkeeper_position_y = 350.0;
double ball_position_x = striker_position_x;
double ball_position_y = striker_position_y + ball_size * 3;

char key_code[] = "";
unsigned int key;
LONG CALLBACK WINAPI WndProc(HWND, UINT, WPARAM, LPARAM);
MSG lpMsg;

double goalPossibility() {
	return (double)striker_position_x / 800.0;
}
void moveStriker() {
	//double launchDistance = (double)(rand() % 450);
	double launchDistance = 450;
	for (int j = 0; j < overall_timeD; j++) {
		if (striker_position_x < launchDistance) {
			striker_position_x += striker_horizontal_speed;
			striker_position_y += striker_vertical_speed * sin(j / 3) * 3;
		}
		else {
			ball_launched = 1;
			break;
		}
		Sleep(frame_timing);
	}
}
void moveDefender() {
	for (int j = 0; j < overall_timeD; j++) {
		mtx.lock();
		switch (key)
		{
		case VK_LEFT:
			defender_position_x -= defender_speed;
			break;
		case VK_UP:
			defender_position_y -= defender_speed;
			break;
		case VK_RIGHT:
			defender_position_x += defender_speed;
			break;
		case VK_DOWN:
			defender_position_y += defender_speed;
			break;
		default:
			key = 2;
			break;
		}
		mtx.unlock();
		Sleep(frame_timing);
	}
}
void moveBall() {
	for (int j = 0; j < overall_timeD; j++) {
		if (!ball_launched) {
			ball_position_x = striker_position_x + (2 * player_size);
			ball_position_y = striker_position_y + (2 * player_size);
		}
		else {
			// прямая между точками (striker_position_x, striker_position_y) и приблизительной серединой ворот(790, 400)
			double x1 = (double)striker_position_x; double y1 = (double)striker_position_y;
			double x2 = 790.0; double y2 = 400.0;

			ball_position_x += ball_speed;
			ball_position_y = (y2 - y1) * (ball_position_x - x1) / (x2 - x1) + y1;
		}
		Sleep(frame_timing);
	}
}
void moveGoalkeeper() {
	double launchDistance = (double)(rand() % 450);
	for (int j = 0; j < overall_timeD; j++) {
		if (striker_position_x < launchDistance) {
			ball_launched = 0;
			striker_position_x += striker_horizontal_speed;
			striker_position_y += striker_vertical_speed * sin(j / 3) * 3;
		}
		else {
			ball_launched = 1;
			break;
		}
		Sleep(frame_timing);
	}
}

void RenderStriker(HDC hdc) {

	HBRUSH hBrush = CreateSolidBrush(RGB(0, 150, 200));
	HPEN hpen = CreatePen(PS_SOLID, 0, RGB(0, 0, 0));

	SelectObject(hdc, hBrush);
	SelectObject(hdc, hpen);

	Ellipse(hdc, striker_position_x, striker_position_y, 2 * player_size + striker_position_x, 2 * player_size + striker_position_y);

	DeleteObject(hBrush);
	DeleteObject(hpen);
}
void RenderDefender(HDC hdc) {

	HBRUSH hBrush = CreateSolidBrush(RGB(200, 0, 200));
	HPEN hpen = CreatePen(PS_SOLID, 0, RGB(0, 0, 0));

	SelectObject(hdc, hBrush);
	SelectObject(hdc, hpen);

	Ellipse(hdc, defender_position_x, defender_position_y, 2 * player_size + defender_position_x, 2 * player_size + defender_position_y);

	DeleteObject(hBrush);
	DeleteObject(hpen);
}
void RenderGoalkeeper(HDC hdc) {
	HBRUSH hBrush = CreateSolidBrush(RGB(50, 200, 200));
	HPEN hpen = CreatePen(PS_SOLID, 0, RGB(0, 0, 0));

	SelectObject(hdc, hBrush);
	SelectObject(hdc, hpen);

	Ellipse(hdc, goalkeeper_position_x, goalkeeper_position_y, 2 * player_size + goalkeeper_position_x, 2 * player_size + goalkeeper_position_y);

	DeleteObject(hBrush);
	DeleteObject(hpen);
}
void RenderGoal(HDC hdc) {
	HBRUSH hBrush = CreateSolidBrush(RGB(0, 0, 0));
	HPEN hpen = CreatePen(PS_NULL, 0, RGB(0, 0, 0));

	SelectObject(hdc, hBrush);
	SelectObject(hdc, hpen);

	Rectangle(hdc, 700, 180, 800, 220);
	Rectangle(hdc, 700, 580, 800, 620);

	//черта пенальти
	Rectangle(hdc, 500, -1, 520, 801);

	DeleteObject(hBrush);
	DeleteObject(hpen);
}
void RenderBall(HDC hdc) {
	HBRUSH hBrush = CreateSolidBrush(RGB(100, 100, 100));
	HPEN hpen = CreatePen(PS_SOLID, 0, RGB(0, 0, 0));

	SelectObject(hdc, hBrush);
	SelectObject(hdc, hpen);

	Ellipse(hdc, ball_position_x, ball_position_y, ball_position_x + 2 * ball_size, ball_position_y + 2 * ball_size);

	DeleteObject(hBrush);
	DeleteObject(hpen);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
	LPSTR lpCmdLine, int nCmdShow) {
	WNDCLASS w;
	memset(&w, 0, sizeof(WNDCLASS));

	w.style = CS_HREDRAW | CS_VREDRAW;
	w.lpfnWndProc = WndProc;
	w.hInstance = hInstance;
	w.hbrBackground = CreateSolidBrush(0x00009F7F);
	w.lpszClassName = L"My Class";

	RegisterClass(&w);

	HWND hwnd =
		CreateWindow(L"My Class", L"Футбол", WS_OVERLAPPEDWINDOW,
			370, 15, 800, 800, NULL, NULL, hInstance, NULL);

	ShowWindow(hwnd, nCmdShow);
	srand(time(NULL));
	thread defenderTH(moveDefender);
	thread strikerTH(moveStriker);
	thread ballTH(moveBall);

	defenderTH.detach();
	strikerTH.detach();
	ballTH.detach();

	for (int i = 0; i < overall_timeD; i++) {
		InvalidateRect(hwnd, 0, 1);
		UpdateWindow(hwnd);
		Sleep(frame_timing);

		GetMessage(&lpMsg, NULL, 0, 0);
		TranslateMessage(&lpMsg);
		DispatchMessage(&lpMsg);
		
	}
	Sleep(frame_timing * 40);
}

LONG WINAPI WndProc(HWND hwnd, UINT Message, WPARAM wparam, LPARAM lparam) {
	HDC hdc;
	PAINTSTRUCT ps;
	HPEN hpen;

	switch (Message) {
	case WM_PAINT:
		hdc = BeginPaint(hwnd, &ps);
		RenderGoal(hdc);
		RenderDefender(hdc);
		RenderStriker(hdc);
		RenderBall(hdc);
		RenderGoalkeeper(hdc);
		EndPaint(hwnd, &ps);
		break;
	case WM_KEYDOWN:
		key = wparam;
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hwnd, Message, wparam, lparam);
	}

	return 0;
}