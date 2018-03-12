﻿// Tests.cpp: определяет точку входа для приложения.
//

#include "Miscellaneous/Dictionary.h"
#include "PlatformDependent/Direct2D.h"
#include "Streaming.h"
#include "UserInterface/BinaryLoader.h"
#include "UserInterface/ButtonControls.h"
#include "UserInterface/ControlBase.h"
#include "UserInterface/ControlClasses.h"
#include "UserInterface/GroupControls.h"
#include "UserInterface/ShapeBase.h"
#include "UserInterface/StaticControls.h"
#include "UserInterface/Templates.h"

#pragma warning(push, 0)

#include "Tests.h"
#include "stdafx.h"

#include <dwmapi.h>
#pragma comment(lib, "dwmapi.lib")

#include <d3d11_1.h>
#pragma comment(lib, "d3d11.lib")

#include "PlatformDependent/WindowStation.h"

#undef CreateWindow
#undef GetCurrentDirectory

using namespace Engine;
using namespace Engine::UI;

#define MAX_LOADSTRING 100

// Глобальные переменные:
HINSTANCE hInst; // текущий экземпляр
WCHAR szTitle[MAX_LOADSTRING] = L"AAAA"; // Текст строки заголовка
WCHAR szWindowClass[MAX_LOADSTRING] = L"HuiHUiHUI"; // имя класса главного окна
HWND Window;

// Отправить объявления функций, включенных в этот модуль кода:
ATOM MyRegisterClass(HINSTANCE hInstance);
BOOL InitInstance(HINSTANCE, Int32);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

Engine::Direct2D::D2DRenderDevice* Device;
Engine::UI::FrameShape* Shape;
Engine::UI::ITexture* Texture;

Engine::UI::HandleWindowStation* station = 0;

ID3D11Device* D3DDevice;
ID3D11DeviceContext* D3DDeviceContext;
IDXGIDevice1* DXGIDevice;
ID2D1Device* D2DDevice;
ID2D1DeviceContext* Target = 0;
IDXGISwapChain1* SwapChain = 0;

SafePointer<Engine::Streaming::TextWriter> conout;
SafePointer<Engine::UI::InterfaceTemplate> Template;

UI::IInversionEffectRenderingInfo* Inversion = 0;

ENGINE_PACKED_STRUCTURE struct TestPacked
{
	UInt8 foo;
	UInt32 bar;
	UInt16 foobar;
};
ENGINE_END_PACKED_STRUCTURE

Int32 APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ Int32 nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	CoInitializeEx(0, COINIT::COINIT_APARTMENTTHREADED);
	SetProcessDPIAware();

	AllocConsole();
	SetConsoleTitleW(L"ui tests");
	SafePointer<Engine::Streaming::FileStream> constream = gd_new Engine::Streaming::FileStream(Engine::IO::GetStandartOutput());
	conout.operator=(gd_new Engine::Streaming::TextWriter(constream));

	//(*conout) << IO::GetCurrentDirectory() << IO::NewLineChar;

	Engine::Direct2D::InitializeFactory();

	// TODO: разместите код здесь.

	// Инициализация глобальных строк
	// LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	// LoadStringW(hInstance, IDC_TESTS, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Выполнить инициализацию приложения:
	if (!InitInstance(hInstance, nCmdShow))
	{
		return FALSE;
	}

	HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_TESTS));

	MSG msg;

	// Starting D3D
	{
		D3D_FEATURE_LEVEL lvl[] = { D3D_FEATURE_LEVEL_11_1, D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_1, D3D_FEATURE_LEVEL_10_0, D3D_FEATURE_LEVEL_9_3, D3D_FEATURE_LEVEL_9_2,
			D3D_FEATURE_LEVEL_9_1 };
		D3D_FEATURE_LEVEL selected;

		D3D11CreateDevice(0, D3D_DRIVER_TYPE_HARDWARE, 0, D3D11_CREATE_DEVICE_BGRA_SUPPORT, lvl, 7, D3D11_SDK_VERSION, &D3DDevice, &selected, &D3DDeviceContext);
		D3DDevice->QueryInterface(__uuidof(IDXGIDevice1), (void**)&DXGIDevice);
		Direct2D::D2DFactory->CreateDevice(DXGIDevice, &D2DDevice);
		D2DDevice->CreateDeviceContext(D2D1_DEVICE_CONTEXT_OPTIONS_NONE, &Target);

		DXGI_SWAP_CHAIN_DESC1 scd;
		RtlZeroMemory(&scd, sizeof(scd));
		scd.Width = 0;
		scd.Height = 0;
		scd.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
		scd.Stereo = false;
		scd.SampleDesc.Count = 1;
		scd.SampleDesc.Quality = 0;
		scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		scd.BufferCount = 2;
		scd.Scaling = DXGI_SCALING_NONE;
		scd.SwapEffect = DXGI_SWAP_EFFECT_SEQUENTIAL;
		scd.Flags = 0;
		scd.AlphaMode = DXGI_ALPHA_MODE_IGNORE;
		scd.Scaling = DXGI_SCALING_STRETCH;

		SafePointer<IDXGIAdapter> Adapter;
		DXGIDevice->GetAdapter(Adapter.GetPtr());
		SafePointer<IDXGIFactory2> Factory;
		Adapter->GetParent(IID_PPV_ARGS(Factory.GetPtr()));
		HRESULT r = Factory->CreateSwapChainForHwnd(D3DDevice, ::Window, &scd, 0, 0, &SwapChain);
		DXGIDevice->SetMaximumFrameLatency(1);

		D2D1_BITMAP_PROPERTIES1 props = D2D1::BitmapProperties1(
		    D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_CANNOT_DRAW, D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_IGNORE), 0.0f, 0.0f);
		SafePointer<IDXGISurface> Surface;
		SwapChain->GetBuffer(0, IID_PPV_ARGS(Surface.GetPtr()));
		SafePointer<ID2D1Bitmap1> Bitmap;
		Target->CreateBitmapFromDxgiSurface(Surface, props, Bitmap.GetPtr());
		Target->SetTarget(Bitmap);
	}
	Device = gd_new Engine::Direct2D::D2DRenderDevice(Target);
	Inversion = Device->CreateInversionEffectRenderingInfo();
	{
		Array<UI::GradientPoint> ps;
		ps << UI::GradientPoint(UI::Color(0, 255, 0), 0.0);
		ps << UI::GradientPoint(UI::Color(0, 0, 255), 1.0);
		::Shape = gd_new FrameShape(UI::Rectangle(100, 100, Coordinate::Right(), 600), FrameShape::FrameRenderMode::Layering, 0.5);
		auto s2 = gd_new BarShape(UI::Rectangle(100, 100, Coordinate(-100, 0.0, 1.0), 300), ps, 3.141592 / 6.0);
		::Shape->Children.InsertLast(s2);
		s2->Release();
		s2 = gd_new BarShape(UI::Rectangle(0, -50, 200, 200), UI::Color(255, 0, 255));
		::Shape->Children.InsertLast(s2);
		s2->Release();

		{
			UI::Zoom = 2.0;
			::Template.operator=(gd_new Engine::UI::InterfaceTemplate());
			{
				SafePointer<Streaming::Stream> Source = gd_new Streaming::FileStream(L"H:\\VirtualUI\\Tests\\test.eui", Streaming::AccessRead, Streaming::OpenExisting);
				struct _loader : public IResourceLoader
				{
					virtual ITexture* LoadTexture(Streaming::Stream* Source) override
					{
						return Device->LoadTexture(Source);
					}
					virtual ITexture* LoadTexture(const string& Name) override
					{
						SafePointer<Streaming::Stream> Source = gd_new Streaming::FileStream(Name, Streaming::AccessRead, Streaming::OpenExisting);
						return Device->LoadTexture(Source);
					}
					virtual UI::IFont* LoadFont(const string& FaceName, Int32 Height, Int32 Weight, bool IsItalic, bool IsUnderline, bool IsStrikeout) override
					{
						return Device->LoadFont(FaceName, Height, Weight, IsItalic, IsUnderline, IsStrikeout);
					}
					virtual void ReloadTexture(ITexture* Texture, Streaming::Stream* Source) override
					{
						Texture->Reload(Device, Source);
					}
					virtual void ReloadTexture(ITexture* Texture, const string& Name) override
					{
						SafePointer<Streaming::Stream> Source = gd_new Streaming::FileStream(Name, Streaming::AccessRead, Streaming::OpenExisting);
						Texture->Reload(Device, Source);
					}
					virtual void ReloadFont(UI::IFont* Font) override
					{
						Font->Reload(Device);
					}
				} loader;
				Engine::UI::Loader::LoadUserInterfaceFromBinary(*::Template, Source, &loader, 0);
			}
			station = gd_new HandleWindowStation(::Window);
			station->SetRenderingDevice(Device);
			auto Main = station->GetDesktop();
			SendMessageW(::Window, WM_SIZE, 0, 0);
			auto Back = gd_new Template::TextureShape;
			Back->From = Rectangle::Entire();
			Back->RenderMode = TextureShape::TextureRenderMode::Fit;
			Back->Position = Rectangle::Entire();
			Back->Texture = ::Template->Texture[L"Wallpaper"];
			Main->Background = Back;

			auto Group = station->CreateWindow<UI::Controls::ControlGroup>(0);
			Group->Background = ::Template->Application[L"Waffle"];
			Group->SetRectangle(UI::Rectangle(0, 0, Coordinate(0, 150.0, 0.0), Coordinate::Bottom()));

			auto New = station->CreateWindow<UI::Controls::Button>(Group, ::Template->Dialog[L"Test"]->Children[2]);
			New->SetRectangle(UI::Rectangle(10, 50, Coordinate(-10, 0.0, 1.0), Coordinate(0, 28.0, 0.0) + 50));
			New->ID = 101;
			auto New2 = station->CreateWindow<UI::Controls::Button>(Group, ::Template->Dialog[L"Test"]->Children[2]);
			New2->SetRectangle(UI::Rectangle(10, Coordinate(0, 28.0, 0.0) + 60, Coordinate(-10, 0.0, 1.0), Coordinate(0, 56.0, 0.0) + 60));
			New2->SetText(L"xyu");
			New2->ID = 102;
			auto New3 = station->CreateWindow<UI::Controls::Button>(Group, ::Template->Dialog[L"Test"]->Children[2]);
			New3->SetRectangle(UI::Rectangle(30, 10, 80, 200));
			New3->SetText(L"3");
			New3->ID = 103;
			New3->SetOrder(Window::DepthOrder::MoveDown);
		}
	}

	// Цикл основного сообщения:
	while (GetMessage(&msg, nullptr, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (Int32)msg.wParam;
}

ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEXW wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_TESTS));
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = 0;
	wcex.lpszMenuName = 0;
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassExW(&wcex);
}

BOOL InitInstance(HINSTANCE hInstance, Int32 nCmdShow)
{
	hInst = hInstance; // Сохранить дескриптор экземпляра в глобальной переменной

	HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

	::Window = hWnd;

	if (!hWnd)
	{
		return FALSE;
	}

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	return TRUE;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_COMMAND:
	{
		Int32 wmId = LOWORD(wParam);
		// Разобрать выбор в меню:
		switch (wmId)
		{
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
	}
	break;
	case WM_PAINT:
	{
		RECT Rect;
		GetClientRect(hWnd, &Rect);
		HDC hdc = GetDC(hWnd);
		// FillRect(hdc, &Rect, (HBRUSH) GetStockObject(LTGRAY_BRUSH));

		// ValidateRect(hWnd, 0);
		if (Target)
		{
			Target->SetDpi(96.0f, 96.0f);
			Target->BeginDraw();
			Device->SetTimerValue(GetTimerValue());
			if (station)
				station->Render();
			Target->EndDraw();
			SwapChain->Present(1, 0);
		}

		ReleaseDC(hWnd, hdc);
	}
	break;
	case WM_SIZE:
	{
		RECT Rect;
		GetClientRect(hWnd, &Rect);
		if (SwapChain)
		{
			Target->SetTarget(0);
			HRESULT r = SwapChain->ResizeBuffers(0, 0, 0, DXGI_FORMAT_UNKNOWN, 0);
			D2D1_BITMAP_PROPERTIES1 props = D2D1::BitmapProperties1(
			    D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_CANNOT_DRAW, D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_IGNORE), 0.0f, 0.0f);
			SafePointer<IDXGISurface> Surface;
			SwapChain->GetBuffer(0, IID_PPV_ARGS(Surface.GetPtr()));
			SafePointer<ID2D1Bitmap1> Bitmap;
			Target->CreateBitmapFromDxgiSurface(Surface, props, Bitmap.GetPtr());
			Target->SetTarget(Bitmap);
		}
		if (station)
			station->ProcessWindowEvents(message, wParam, lParam);
	}
	break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		if (station)
			return station->ProcessWindowEvents(message, wParam, lParam);
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

#pragma warning(pop)
