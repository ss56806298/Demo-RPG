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

class DemoGame
{
public:
	DemoGame();
	~DemoGame();

	//初始化
	HRESULT Initialize();

	//消息循环
	void RunMessageLoop();

private:
	//加载图片资源
	void LoadImageResources();

	//绘制
	void OnRender();

	//重新设置窗口大小
	void OnResize(UINT width, UINT height);

	//设备无关资源
	HRESULT CreateDeviceIndependentResources();

	//设备相关资源
	HRESULT CreateDeviceResources();

	//窗口过程函数
	static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

	HWND m_hwnd;
	ID2D1Factory *m_pDirect2dFactory;
	ID2D1HwndRenderTarget *m_pRenderTarget;
	ID2D1SolidColorBrush *m_pLightSlateGrayBrush;
	ID2D1SolidColorBrush *m_pCornflowerBlueBrush;

	//WIC
	IWICImagingFactory *m_pWicImagingFactory = NULL;
	IWICBitmapDecoder *m_pWicDecoder = NULL;
	IWICBitmapFrameDecode *m_pWicFrame = NULL;
	IWICFormatConverter *m_pConvertedSourceBitMap = NULL;
	ID2D1Bitmap *m_pD2DBMP_MoveSprite = NULL;
};