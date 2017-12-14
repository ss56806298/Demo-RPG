#pragma once

#include "CommonHeader.h"

template <class Interface>
inline void SafeRelease(
	Interface **ppInterfaceToRelease
)
{
	if (*ppInterfaceToRelease != NULL) {
		(*ppInterfaceToRelease)->Release();

		(*ppInterfaceToRelease) = NULL;
	}
}

class GameController;

class DemoGame
{
public:
	DemoGame();
	~DemoGame();


	//初始化
	HRESULT Initialize();

	//消息循环
	void RunMessageLoop();

	//加载图片资源
	ID2D1Bitmap* LoadImageResource(int resNum, LPCWSTR resType);

	//绘制
	HRESULT RenderStart();
	void RenderEnd();

	//渲染目标
	ID2D1HwndRenderTarget *m_pRenderTarget;

	//字
	IDWriteTextFormat *m_pTextFormat;

	//刷
	ID2D1SolidColorBrush *m_pLightSlateGrayBrush;
	ID2D1SolidColorBrush *m_pCornflowerBlueBrush;
private:
	//游戏控制器
	GameController *m_pGameController;

	//重新设置窗口大小
	void OnResize(UINT width, UINT height);

	//设备无关资源
	HRESULT CreateDeviceIndependentResources();

	//设备相关资源
	HRESULT CreateDeviceResources();

	//窗口过程函数
	static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

	//释放资源
	void ReleaseObjs();

	//D2D
	HWND m_hwnd;
	ID2D1Factory *m_pDirect2dFactory;
	IDWriteFactory *m_pDWriteFactory = NULL;

	//WIC
	IWICImagingFactory *m_pWicImagingFactory = NULL;
	HRSRC imageResHandle = NULL;
	HGLOBAL imageResDataHandle = NULL;
	void *pImageFile = NULL;
	DWORD imageFileSize = 0;
	IWICStream *m_pWicStream = NULL;
	IWICBitmapDecoder *m_pWicDecoder = NULL;
	IWICBitmapFrameDecode *m_pWicFrame = NULL;
	IWICFormatConverter *m_pConvertedSourceBitMap = NULL;
	ID2D1Bitmap *m_pTempID2DBitmap = NULL;

};