#include "graphics.h"
#include <commctrl.h>
#include "cube.h"

Object* objects[4];
Graphics* g = NULL;
DWORD lastTime = (DWORD)GetTickCount();
char statusBuffer[32];
DWORD updateStateTimer = 1000;

HWND statusBar;

const char* Help =
	"X/ctrl + X: 绕X轴旋转摄像机\n"
	"Y/ctrl + Y: 绕Y轴旋转摄像机\n"
	"Z/ctrl + Z: 绕Z轴旋转摄像机\n"
	"L/ctrl + L: 左右移动摄像机\n"
	"U/ctrl + U: 上下移动摄像机\n"
	"M/ctrl + M: 前后移动摄像机\n"
	"I: 重置摄像机";

#pragma comment(lib, "comctl32.lib")

void InitObjects() {
	/*Vertex vertices[] = {
	Vertex(-8, -5, -8),	// 0
	Vertex(0, 5, 0),	// 1
	Vertex(8, -5, -8),	// 2
	Vertex(8, -5, 8),	// 3
	Vertex(-8, -5, 8),	// 4
	};

	Triangle triangles[] {
	Triangle(0, 1, 2, color), Triangle(4, 1, 0, color),
	Triangle(2, 1, 3, color), Triangle(3, 1, 4, color),
	Triangle(0, 2, 3, color), Triangle(3, 4, 0, color),
	};
	*/

	Vertex vertices[] = {
		Vertex(-8, -5, -8, 0, 126 / 128.f), Vertex(0, 5, 0, 64 / 128.f, 0), Vertex(8, -5, -8, 126 / 128.f, 126 / 128.f),
		Vertex(-8, -5, 8, 0, 126 / 128.f), Vertex(0, 5, 0, 64 / 128.f, 0), Vertex(-8, -5, -8, 126 / 128.f, 126 / 128.f),
		Vertex(8, -5, -8, 0, 126 / 128.f), Vertex(0, 5, 0, 64 / 128.f, 0), Vertex(8, -5, 8, 126 / 128.f, 126 / 128.f),
		Vertex(8, -5, 8, 0, 126 / 128.f), Vertex(0, 5, 0, 64 / 128.f, 0), Vertex(-8, -5, 8, 126 / 128.f, 126 / 128.f),
		Vertex(-8, -5, -8, 0, 126 / 128.f), Vertex(8, -5, -8, 64 / 128.f, 0), Vertex(8, -5, 8, 126 / 128.f, 126 / 128.f),
		Vertex(8, -5, 8, 0, 126 / 128.f), Vertex(-8, -5, 8, 64 / 128.f, 0), Vertex(-8, -5, -8, 126 / 128.f, 126 / 128.f),
	};

	Texture crateTexture("bmp/crate.bmp");

	Triangle triangles[] = {
		Triangle(0, 1, 2, crateTexture), Triangle(3, 4, 5, crateTexture),
		Triangle(6, 7, 8, crateTexture), Triangle(9, 10, 11, crateTexture),
		Triangle(12, 13, 14, crateTexture), Triangle(15, 16, 17, crateTexture),
	};

	//Vector3 normal = TriangleSelfContained(v[20], v[22], v[23]).Normal();
	//
	// 	pyramid[0] = new Object(VertexContainer(v, v + _countof(v)),
	// 		IndexedTriangleContainer(cube, cube + _countof(cube)),
	//		5.f, 5, Vector3(-5, 20), Vector3(3.1515926f / 4, 3.1515926f / 4));

// 	pyramid[1] = new Object();// , 3.1515926f / 4));
// 	pyramid[1].Initialize(VertexContainer(v, v + _countof(v)),
// 						  IndexedTriangleContainer(cube, cube + _countof(cube)),
// 						  5.f, 1.f, Vector3(0, 4, -35), Vector3(0, 3.141592f / 4));
	objects[0] = new Cube(0.4f);

	/*pyramid[2] = new Object(VertexContainer(vertices, vertices + _countof(v)),
	IndexedTriangleContainer(triangles, triangles + _countof(triangles)),
	5.f, 2.f, Vector3(0, 3, 0), Vector3(0, 0, 0));// 3.141592F / 4));*/
}

void InitWindow(HINSTANCE instance, HWND hwnd) {
	INITCOMMONCONTROLSEX icex;
	icex.dwSize = sizeof(icex);
	icex.dwICC = ICC_BAR_CLASSES;
	InitCommonControlsEx(&icex);
	
	statusBar = CreateWindow(STATUSCLASSNAME, TEXT("Ready"), WS_CHILD | WS_VISIBLE | SBARS_SIZEGRIP,
							 0, 0, 0, 0, hwnd, (HMENU)0x101, instance, NULL);

	int width = -1;
	SendMessage(statusBar, SB_SETPARTS, 1, (LPARAM)&width);
	RECT status;
	GetWindowRect(statusBar, &status);

	RECT window, client;
	int borderWidth, borderHeight;

	GetWindowRect(hwnd, &window);
	GetClientRect(hwnd, &client);
	borderWidth = (window.right - window.left) - (client.right - client.left);
	borderHeight = (window.bottom - window.top) - (client.bottom - client.top) + (status.bottom - status.top);

	SetWindowPos(hwnd, 0, 0, 0, borderWidth + 600, borderHeight + 600, SWP_NOMOVE | SWP_NOZORDER);
	MoveWindow(statusBar, 0, 0, 0, 0, TRUE);

	GetClientRect(hwnd, &client);
	client.bottom -= (status.bottom - status.top);
	g = new Graphics(client);

	for (size_t i = 0; i < sizeof(objects) / sizeof(objects[0]); ++i) {
		if (objects[i] != NULL)
			g->AddObject(objects[i]);
	}
}

void Initialize(HINSTANCE instance, HWND hwnd) {
	InitObjects();
	InitWindow(instance, hwnd);
}

void SetStatus(const char* msg) {
	SendMessage(statusBar, SB_SETTEXT, 0, (WPARAM)msg);
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	switch (msg) {
	case WM_CREATE:
		Initialize(((LPCREATESTRUCT)lParam)->hInstance, hwnd);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;

 	case WM_ERASEBKGND:
 		return TRUE;

	case WM_KEYDOWN:
		if (wParam == VK_ESCAPE) {
			DestroyWindow(hwnd);
			break;
		}
		if (wParam == VK_F1) {
			MessageBox(hwnd, Help, "help", MB_OK | MB_ICONINFORMATION);
			break;
		}

		g->SendKeyboardCommand(int((GetKeyState(VK_CONTROL) & 0x80) != 0) * 32 + wParam);
		break;

	case WM_MOUSEWHEEL:
		g->SendKeyboardCommand(int((HIWORD(wParam) & 0x80) == 0) * 32 + 'M');
		break;
	}

	return DefWindowProc(hwnd, msg, wParam, lParam);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd) {
	WNDCLASS wc;
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = (WNDPROC)WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInstance;
	wc.hIcon = LoadIcon(0, IDI_APPLICATION);
	wc.hCursor = LoadCursor(0, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wc.lpszMenuName = 0;
	wc.lpszClassName = "3D-App";

	RegisterClass(&wc);

	HWND hwnd = CreateWindow(wc.lpszClassName, "3D-App", WS_EX_TOPMOST | WS_SYSMENU,
		0, 0, 200, 200, 0, 0, hInstance, 0);

	ShowWindow(hwnd, SW_SHOW);
	UpdateWindow(hwnd);

	MSG msg;
	ZeroMemory(&msg, sizeof(msg));

	for (; msg.message != WM_QUIT;) {
		if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else if(g != NULL) {
			DWORD currentTime = GetTickCount();
			g->Update(hwnd, (currentTime - lastTime) / 1000.f);
			lastTime = currentTime;

			DWORD time2 = GetTickCount() - currentTime;
			updateStateTimer += time2;
			if (updateStateTimer >= 1000) {
				updateStateTimer = 0;
				_snprintf_s(statusBuffer, sizeof(statusBuffer), "%u ms(%.2f fps)", time2, 1000.f / time2);
				SetStatus(statusBuffer);
			}
		}
	}

	return msg.wParam;
}
