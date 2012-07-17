#include "TestProject.h"

// Windows API
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

// C standard library includes
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>

// C++ standard library includes
#include <algorithm>
#include <vector>

// Renderer
#include <Pixel.h>
#include <MD2_Model.h>
#include <Renderer.h>

// Scene Graph
#include <SceneNode.h>
#include <ModelNode.h>
#include <TransformNode.h>
#include <RotatingNode.h>
#include <PulseNode.h>
#include <TranslatingNode.h>

// Demos
#include "Cube.h"
#include "Miku.h"
#include "Tunnel.h"
#include "Orbit.h"
#include "Sonic.h"

#define MAX_LOADSTRING 100

// Global Variables:
HWND hWnd;
a3d::Renderer rend(1, 250);
bool running = true;
bool resizing = false;
HINSTANCE hInst;								// current instance
TCHAR szTitle[MAX_LOADSTRING];					// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];			// The main window class name
bool keys[255];									// Input buffer
bool oldkeys[255];								// Old input buffer

// Demo text output
std::vector<std::string> screenText;			// Array of strings to be drawn to the screen
HFONT font;
int fontSize = 24;

// Graphical buffers
a3d::Pixel* bufferPixelData = 0;
HBITMAP buffer;

// Forward declarations of functions included in this code module:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);

// Window dimensions
int width = 1200;
int height = 720;

// Camera
a3d::Vertex4f pos(4, 0, 0, 80, 1);
a3d::Camera cam(pos, 0, 0, 0);

// Camera x and y rotation
float xrot = 0;
float yrot = 0;

a3d::Matrix4f projection;

int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	a3d::md2::MD2_Model model = a3d::md2::MD2_Model();
	bool loaded = model.loadModel("cube.md2");
	model.setAnimation(8, -1);

	// Create font to use for output
	font = CreateFont(fontSize, 0, 0, 0, FW_DONTCARE, FALSE, FALSE, FALSE, DEFAULT_CHARSET,
				OUT_OUTLINE_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, FF_MODERN, 0);

	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	int time = GetTickCount();

	MSG msg;
	HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_TESTPROJECT));

	// Initialize global strings
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_TESTPROJECT, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Perform application initialization:
	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}

	int fps = 0;
	int frameCount = 0;
	long startTime = GetTickCount();
	wchar_t title[255];

	// Set up projection matrix
	projection = a3d::Matrix4f::createPerspective(-1, 1, -1, 1, 1, 250.0f, (float)width / (float)height);

	rend.setMatrixMode(a3d::MatrixModes::PROJECTION);
	rend.pushMatrix();
	rend.loadIdentity();
	rend.transform(projection);

	// Add demos to playlist
	std::vector<Demos::Demo*> list;

	// Set up demos
	Demos::Demo* demo;

	// Wireframe, no culling
	demo = new Demos::Cube(rend, cam, time, 5000, a3d::MaterialTypes::WIREFRAME, a3d::ShadingTypes::NONE, a3d::CullingTypes::NONE);
	demo->addMessage("Material mode: Wireframe");
	demo->addMessage("Shading mode:  None");
	demo->addMessage("Culling mode:  None");
	list.push_back(demo);

	// Wireframe, front-face culling
	demo = new Demos::Cube(rend, cam, time, 5000, a3d::MaterialTypes::WIREFRAME, a3d::ShadingTypes::NONE, a3d::CullingTypes::FRONT);
	demo->addMessage("Material mode: Wireframe");
	demo->addMessage("Shading mode:  None");
	demo->addMessage("Culling mode:  Front");
	list.push_back(demo);

	// Wireframe, back-face culling
	demo = new Demos::Cube(rend, cam, time, 5000, a3d::MaterialTypes::WIREFRAME, a3d::ShadingTypes::NONE, a3d::CullingTypes::BACK);
	demo->addMessage("Material mode: Wireframe");
	demo->addMessage("Shading mode:  None");
	demo->addMessage("Culling mode:  Back");
	list.push_back(demo);

	// Solid, flat shading
	demo = new Demos::Cube(rend, cam, time, 5000, a3d::MaterialTypes::SOLID, a3d::ShadingTypes::FLAT, a3d::CullingTypes::BACK);
	demo->addMessage("Material mode: Solid");
	demo->addMessage("Shading mode:  Flat");
	demo->addMessage("Culling mode:  Back");
	demo->addLight(new a3d::AmbientLight(0.25f));
	demo->addLight(new a3d::DirectionalLight(a3d::Vector(0, 0, -1), a3d::Colour(0.5f, 0.5f, 0.5f)));
	list.push_back(demo);

	// Solid, smooth shading
	demo = new Demos::Cube(rend, cam, time, 5000, a3d::MaterialTypes::SOLID, a3d::ShadingTypes::SMOOTH, a3d::CullingTypes::BACK);
	demo->addMessage("Material mode: Solid");
	demo->addMessage("Shading mode:  Smooth");
	demo->addMessage("Culling mode:  Back");
	demo->addLight(new a3d::AmbientLight(0.25f));
	demo->addLight(new a3d::DirectionalLight(a3d::Vector(0, 0, -1), a3d::Colour(0.5f, 0.5f, 0.5f)));
	list.push_back(demo);

	// Solid, phong shading
	demo = new Demos::Cube(rend, cam, time, 5000, a3d::MaterialTypes::SOLID, a3d::ShadingTypes::PHONG, a3d::CullingTypes::BACK);
	demo->addMessage("Material mode: Solid");
	demo->addMessage("Shading mode:  Phong");
	demo->addMessage("Culling mode:  Back");
	demo->addLight(new a3d::AmbientLight(0.25f));
	demo->addLight(new a3d::DirectionalLight(a3d::Vector(0, 0, -1), a3d::Colour(0.5f, 0.5f, 0.5f)));
	list.push_back(demo);

	// Textured, brightly lit
	demo = new Demos::Cube(rend, cam, time, 5000, a3d::MaterialTypes::TEXTURED, a3d::ShadingTypes::FLAT, a3d::CullingTypes::BACK);
	demo->addMessage("Material mode: Textured");
	demo->addMessage("Shading mode:  Flat");
	demo->addMessage("Culling mode:  Back");
	demo->addMessage("");
	demo->addMessage("Perspective-correct textures");
	demo->addLight(new a3d::AmbientLight(0.35f));
	demo->addLight(new a3d::DirectionalLight(a3d::Vector(0, 0, -1), a3d::Colour(0.7f, 0.7f, 0.7f)));
	list.push_back(demo);

	// Miku
	// Solid, phong
	demo = new Demos::Miku(rend, cam, time, 5000, a3d::MaterialTypes::SOLID, a3d::ShadingTypes::PHONG, a3d::CullingTypes::BACK);
	demo->addMessage("Material mode: Solid");
	demo->addMessage("Shading mode:  Phong");
	demo->addMessage("Culling mode:  Back");
	demo->addLight(new a3d::AmbientLight(0.15f));
	demo->addLight(new a3d::DirectionalLight(a3d::Vector(0, 0, -1), a3d::Colour(1.0f, 1.0f, 1.0f)));
	list.push_back(demo);

	// Textured, phong, directional light
	demo = new Demos::Miku(rend, cam, time, 5000, a3d::MaterialTypes::TEXTURED, a3d::ShadingTypes::PHONG, a3d::CullingTypes::BACK);
	demo->addMessage("Material mode: Textured");
	demo->addMessage("Shading mode:  Phong");
	demo->addMessage("Culling mode:  Back");
	demo->addMessage("");
	demo->addMessage("Perspective-correct textures");
	demo->addMessage("");
	demo->addMessage("Lights:");
	demo->addMessage("Directional (white)");
	demo->addLight(new a3d::DirectionalLight(a3d::Vector(-1, 0, 0).getNormalised(), a3d::Colour(0.85f, 0.85f, 0.85f)));
	list.push_back(demo);

	// Textured, phong, directional + point light
	demo = new Demos::Miku(rend, cam, time, 5000, a3d::MaterialTypes::TEXTURED, a3d::ShadingTypes::PHONG, a3d::CullingTypes::BACK);
	demo->addMessage("Material mode: Textured");
	demo->addMessage("Shading mode:  Phong");
	demo->addMessage("Culling mode:  Back");
	demo->addMessage("");
	demo->addMessage("Perspective-correct textures");
	demo->addMessage("");
	demo->addMessage("Lights:");
	demo->addMessage("Directional (white)");
	demo->addMessage("Point (pink)");
	demo->addLight(new a3d::DirectionalLight(a3d::Vector(-1, 0, 0).getNormalised(), a3d::Colour(0.85f, 0.85f, 0.85f)));
	demo->addLight(new a3d::PointLight(a3d::Vector(15, 15, -80), a3d::Colour(0.5f, 0, 0.5f)));
	list.push_back(demo);

	// Textured, phong, directional + point + spotlight
	demo = new Demos::Miku(rend, cam, time, 10000, a3d::MaterialTypes::TEXTURED, a3d::ShadingTypes::PHONG, a3d::CullingTypes::BACK);
	demo->addMessage("Material mode: Textured");
	demo->addMessage("Shading mode:  Phong");
	demo->addMessage("Culling mode:  Back");
	demo->addMessage("");
	demo->addMessage("Perspective-correct textures");
	demo->addMessage("");
	demo->addMessage("Lights:");
	demo->addMessage("Directional (white)");
	demo->addMessage("Point (pink)");
	demo->addMessage("Spotlight (blue)");
	demo->addLight(new a3d::DirectionalLight(a3d::Vector(-1, 0, 0).getNormalised(), a3d::Colour(0.85f, 0.85f, 0.85f)));
	demo->addLight(new a3d::PointLight(a3d::Vector(15, 15, -80), a3d::Colour(0.7f, 0, 0.7f)));
	demo->addLight(new a3d::Spotlight(a3d::Vector(0, 0, 0), a3d::Vector(0, 0, -1).getNormalised(), a3d::Colour(0, 0.75f, 0.75f), PI / 2.0f, 64));
	list.push_back(demo);

	// Scenegraph recursion, camera movement
	demo = new Demos::Tunnel(rend, cam, time, 15000, a3d::MaterialTypes::TEXTURED, a3d::ShadingTypes::SMOOTH, a3d::CullingTypes::BACK);
	demo->addMessage("Material mode: Solid");
	demo->addMessage("Shading mode:  Smooth");
	demo->addMessage("Culling mode:  Back");
	demo->addMessage("");
	demo->addMessage("Camera movement");
	demo->addMessage("Recursive scene graph");
	demo->addLight(new a3d::AmbientLight(0.25f));
	demo->addLight(new a3d::DirectionalLight(a3d::Vector(0, 0, -1), a3d::Colour(0.5f, 0.5f, 0.5f)));
	list.push_back(demo);

	// A scene with an object orbiting another to show off the scene graph
	demo = new Demos::Sonic(rend, cam, time, 15000, a3d::MaterialTypes::TEXTURED, a3d::ShadingTypes::SMOOTH, a3d::CullingTypes::BACK);
	demo->addMessage("Material mode: Textured");
	demo->addMessage("Shading mode:  Smooth");
	demo->addMessage("Culling mode:  Back");
	demo->addMessage("");
	demo->addMessage("Perspective-correct textures");
	demo->addMessage("Scene graph");
	demo->addMessage("Animation");
	demo->addLight(new a3d::AmbientLight(0.25f));
	demo->addLight(new a3d::DirectionalLight(a3d::Vector(0, 0, -1), a3d::Colour(0.5f, 0.5f, 0.5f)));
	demo->addLight(new a3d::Spotlight(a3d::Vector(0, 50, 0), a3d::Vector(0, -1, 0).getNormalised(), a3d::Colour(1.0f, 1.0f, 1.0f), PI/3, 1));
	list.push_back(demo);

	// A scene with an object orbiting another to show off the scene graph
	demo = new Demos::Orbit(rend, cam, time, 20000, a3d::MaterialTypes::TEXTURED, a3d::ShadingTypes::SMOOTH, a3d::CullingTypes::BACK);
	demo->addMessage("Material mode: Textured");
	demo->addMessage("Shading mode:  Smooth");
	demo->addMessage("Culling mode:  Back");
	demo->addMessage("");
	demo->addMessage("Perspective-correct textures");
	demo->addMessage("Scene graph");
	demo->addMessage("");
	demo->addMessage("Lights:");
	demo->addMessage("Point (orange)");
	demo->addLight(new a3d::PointLight(a3d::Vector(0, 0, -10), a3d::Colour(1.0f, 0.7f, 0)));
	list.push_back(demo);

	unsigned int currentDemo = 0;
	screenText = list[currentDemo]->getDemoText();

	// Main message loop
	while (running)
	{			
		// Handle all messages
		while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}

		if (resizing)
			continue;
		
		// On pressing left
		if (keys[VK_LEFT] && !oldkeys[VK_LEFT])
		{
			if (currentDemo > 0)
				currentDemo--;
			else
				currentDemo = list.size() - 1;

			list[currentDemo]->reset(time);
		}

		// On pressing right
		if (keys[VK_RIGHT] && !oldkeys[VK_RIGHT])
		{
			if (currentDemo < list.size() - 1)
				currentDemo++;
			else
				currentDemo = 0;

			list[currentDemo]->reset(time);
		}
		
		// Clear screen
		rend.beginScene(a3d::Pixel(0, 0, 0, 0));

		// Set up view matrix
		rend.setMatrixMode(a3d::MatrixModes::VIEW);
		rend.loadIdentity();
		
		// Transform it to the camera
		rend.transform(a3d::Matrix4f::createTranslation(-cam.getPosition()(0, 0), -cam.getPosition()(1, 0), -cam.getPosition()(2, 0)));
		
		// If there's at least one demo
		if (list.size() > 0)
		{
			// Draw current demo
			rend.setMatrixMode(a3d::MatrixModes::WORLD);
			rend.pushMatrix();
				rend.loadIdentity();

				// Transform MD2 model to be the regular way up
				rend.transform(a3d::Matrix4f::createRotationX(-PI / 2.0f));
				rend.transform(a3d::Matrix4f::createRotationY(-PI / 2.0f));

				list[currentDemo]->draw(time);

				screenText = list[currentDemo]->getDemoText();
			rend.popMatrix();

			if (list[currentDemo]->dead())
			{
				if (currentDemo < list.size() - 1)
					currentDemo++;
				else
					currentDemo = 0;

				list[currentDemo]->reset(time);

				screenText = list[currentDemo]->getDemoText();
			}
		}

		frameCount++;

		long dt = GetTickCount() - startTime;
		if (dt >= 1000)
		{
			fps = frameCount;
			swprintf_s(title, L"%d", fps);
			startTime = GetTickCount();
			frameCount = 0;
			
			SetWindowText(hWnd, title);
		}

		time = GetTickCount();

		// Make copy of this frame's input state for next frame
		for (int i = 0; i < 255; ++i)
		{
			oldkeys[i] = keys[i];
		}

		// Redraw window
		InvalidateRect(hWnd, NULL, false);
	}

	// Clean up matrix stack
	rend.setMatrixMode(a3d::MatrixModes::PROJECTION);
	rend.popMatrix();

	DeleteObject(font);

	return (int) msg.wParam;
}

//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
//  COMMENTS:
//
//    This function and its usage are only necessary if you want this code
//    to be compatible with Win32 systems prior to the 'RegisterClassEx'
//    function that was added to Windows 95. It is important to call this function
//    so that the application will get 'well formed' small icons associated
//    with it.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_TESTPROJECT));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_TESTPROJECT);
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	// Store instance handle in our global variable
	hInst = hInstance;

	hWnd = CreateWindowEx(0, szWindowClass, szTitle, WS_OVERLAPPEDWINDOW, CW_USEDEFAULT,
		CW_USEDEFAULT, width, height, NULL, NULL, hInstance, NULL);

	if (!hWnd)
	{
		return FALSE;
	}

	// Register raw input for keyboard/mouse
	RAWINPUTDEVICE Rid[1];
        
	// adds HID mouse and also ignores legacy mouse messages
	Rid[0].usUsagePage = 0x01; 
	Rid[0].usUsage = 0x02; 
	Rid[0].dwFlags = 0;   
	Rid[0].hwndTarget = 0;

	//if (RegisterRawInputDevices(Rid, 1, sizeof(Rid[0])) == FALSE) {
	//	// Error - use getLastError() if you want an undecipherable number
	//	MessageBoxW(hWnd, L"There was an problem registering input devices.",
	//		L"Error", MB_OK | MB_ICONEXCLAMATION);
	//}

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND	- process the application menu
//  WM_PAINT	- Paint the main window
//  WM_DESTROY	- post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;

	switch (message)
	{
	case WM_COMMAND:
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		// Parse the menu selections:
		switch (wmId)
		{
		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	case WM_DESTROY:
		running = false;
		PostQuitMessage(0);
		break;
	case WM_INPUT:
		{
			UINT dwSize = 40;
			static BYTE lpb[40];
    
			GetRawInputData((HRAWINPUT)lParam, RID_INPUT, 
							lpb, &dwSize, sizeof(RAWINPUTHEADER));
    
			RAWINPUT* raw = (RAWINPUT*)lpb;
    
			if (raw->header.dwType == RIM_TYPEMOUSE) 
			{
				int xPosRelative = raw->data.mouse.lLastX;
				int yPosRelative = raw->data.mouse.lLastY;
				
				xrot += yPosRelative / 200.0f;
				yrot += -xPosRelative / 200.0f;

				if (xrot < -PI/4)
					xrot = -PI/4;
				if (xrot > PI/4)
					xrot = PI/4;
				if (yrot < -PI/4)
					yrot = -PI/4;
				if (yrot > PI/4)
					yrot = PI/4;

				cam.setRotation(xrot, yrot, 0);
			}
		}
		break;
	case WM_KEYDOWN:
		oldkeys[wParam] = keys[wParam];
		keys[wParam] = true;
		break;
	case WM_KEYUP:
		oldkeys[wParam] = keys[wParam];
		keys[wParam] = false;
		break;
	case WM_SIZE:
		{
			width = LOWORD(lParam);
			height = HIWORD(lParam);

			// Correct projection matrix for aspect ratio
			projection = a3d::Matrix4f::createPerspective(-1, 1, -1, 1, 1, 250.0f, (float)width / (float)height);

			rend.setMatrixMode(a3d::MatrixModes::PROJECTION);
			rend.pushMatrix();
			rend.loadIdentity();
			rend.transform(projection);

			if (width > 0 && height > 0)
			{
				// Initialise buffer
				BITMAPINFO bitmapInfo;
				bitmapInfo.bmiHeader.biWidth = width;
				bitmapInfo.bmiHeader.biHeight = -height;
				bitmapInfo.bmiHeader.biBitCount = 32;
				bitmapInfo.bmiHeader.biCompression = BI_RGB;
				bitmapInfo.bmiHeader.biPlanes = 1;
				bitmapInfo.bmiHeader.biSize = sizeof(bitmapInfo.bmiHeader);
						
				DeleteObject(buffer);

				buffer = CreateDIBSection(NULL, &bitmapInfo, DIB_RGB_COLORS, (void**)(&bufferPixelData), NULL, 0);
				rend.setTarget(bufferPixelData, width, height);
			}
		}
		break;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		{
			// Blit pixel buffer
			HDC bDC = CreateCompatibleDC(hdc);
			HGDIOBJ old = SelectObject(bDC, buffer);

			// Draw text
			SetBkMode(bDC, TRANSPARENT);
			SetTextColor(bDC, RGB(255, 255, 255));
            SelectObject(bDC, font);
			for (unsigned int i = 0; i < screenText.size(); ++i)
			{
				TextOutA(bDC, 50, 50 + fontSize * i, screenText[i].c_str(), screenText[i].length());
			}
			
			BitBlt(hdc, 0, 0, width, height, bDC, 0, 0, SRCCOPY);

			// Clean up device contexts
			SelectObject(bDC, old);
			DeleteDC(bDC);
		}
		EndPaint(hWnd, &ps);
		break;
	case WM_ERASEBKGND:
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}
