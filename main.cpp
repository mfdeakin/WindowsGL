
#include <Windows.h>
#include <gl/GL.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

struct {
	HWND wnd;
	HINSTANCE inst;
	HDC dvc;
	HGLRC renderc;
	bool run, draw;
	FILE *log;
	int wwidth, wheight;
} globals;

void initWnd(void);
void initGL(void);
void drawGL(void);
LRESULT CALLBACK wndProc(HWND, UINT, WPARAM, LPARAM);

INT WINAPI WinMain(HINSTANCE hCI, HINSTANCE hPI, LPSTR cmdstr, int cmdval)
{
	globals.inst = hCI;
	globals.log = fopen("gl4.log", "w");
	globals.draw = false;
	initWnd();
	if(globals.wnd)
		globals.run = true;
	while(globals.run) {
		MSG msg;
		if(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		if(globals.draw)
			drawGL();
	}
	fclose(globals.log);
	return 0;
}

void drawGL(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glFlush();
	SwapBuffers(globals.dvc);
}

LRESULT CALLBACK wndProc(HWND wnd, UINT msg, WPARAM wpm, LPARAM lpm)
{
	switch(msg) {
	case WM_CREATE:
		return 0;
	case WM_PAINT:
		return 0;
	case WM_SIZE:
		globals.wwidth = LOWORD(lpm);
		globals.wheight = HIWORD(lpm);
		glViewport(0, 0, globals.wwidth, globals.wheight);
		return 0;
	case WM_DESTROY:
		globals.run = false;
		wglMakeCurrent(globals.dvc, NULL);
		wglDeleteContext(globals.renderc);
		ReleaseDC(globals.wnd, globals.dvc);
		return 0;
	}
	return DefWindowProc(wnd, msg, wpm, lpm);
}

void initGL(void)
{
	globals.dvc = GetDC(globals.wnd);
	fprintf(globals.log, "Device Context: 0x%x\n", globals.dvc);
	PIXELFORMATDESCRIPTOR pfd;
	memset(&pfd, 0, sizeof(pfd));
	pfd.nSize = sizeof(pfd);
	pfd.dwFlags = PFD_DOUBLEBUFFER | PFD_SUPPORT_OPENGL | PFD_DRAW_TO_WINDOW;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cColorBits = 32;
	pfd.cDepthBits = 32;
	pfd.iLayerType = PFD_MAIN_PLANE;
	pfd.nVersion = 1;
	int pf = ChoosePixelFormat(globals.dvc, &pfd);
	if(!pf) {
		DWORD err = GetLastError();
		fprintf(globals.log, "ChoosePixelFormat Error: %d\n", err);
		MessageBox(globals.wnd, "Could not create pixel format descriptor", "GL4 Error", MB_OK);
		return;
	}
	fprintf(globals.log, "Pixelformat: %d\n", pf);
	if(!SetPixelFormat(globals.dvc, pf, &pfd)) {
		DWORD err = GetLastError();
		fprintf(globals.log, "SetPixelFormat Error: %d\n", err);
		MessageBox(globals.wnd, "Could not set pixel format descriptor", "GL4 Error", MB_OK);
		return;
	}
	globals.renderc = wglCreateContext(globals.dvc);
	if(!globals.renderc)
	{
		DWORD err = GetLastError();
		fprintf(globals.log, "wglCreateContext Error: %d\n", err);
		MessageBox(globals.wnd, "Could not create the descriptor", "GL4 Error", MB_OK);
		return;
	}
	fprintf(globals.log, "Render context: 0x%x\n", globals.renderc);
	if(!wglMakeCurrent(globals.dvc, globals.renderc))
	{
		DWORD err = GetLastError();
		fprintf(globals.log, "wglMakeCurrent Error: %d\n", err);
		MessageBox(globals.wnd, "Could not make the descriptor current", "GL4 Error", MB_OK);
		return;
	}
	glClearColor(1.0f, 0.0f, 0.0f, 1.0f);
	globals.draw = true;

    const char *renderer = (const char *)glGetString(GL_RENDERER); // get renderer string
    const char *version = (const char *)glGetString(GL_VERSION); // version as a string
	fprintf(globals.log, "renderer: %s\nversion: %s\n", renderer, version);
}

void initWnd(void)
{
	WNDCLASSEX wc;
	const char *classname = "myopengl4test";
	memset(&wc, 0, sizeof(wc));
	wc.cbClsExtra = 0;
	wc.cbSize = sizeof(wc);
	wc.cbWndExtra = 0;
	wc.hbrBackground = 0;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wc.hIconSm = wc.hIcon;
	wc.hInstance = globals.inst;
	wc.lpfnWndProc = wndProc;
	wc.lpszClassName = classname;
	wc.lpszMenuName = NULL;
	wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	RegisterClassEx(&wc);
	globals.wnd = CreateWindowEx(WS_EX_APPWINDOW | WS_EX_WINDOWEDGE,
		classname,
		"Open GL 4 Test",
		WS_OVERLAPPEDWINDOW,
		200, 200, 512, 512,
		NULL,
		NULL,
		globals.inst,
		NULL);
	initGL();
	ShowWindow(globals.wnd, true);
	UpdateWindow(globals.wnd);
}
