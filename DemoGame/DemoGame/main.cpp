#include "DemoGame.h"
#include "GameController.h"

//构造
DemoGame::DemoGame() :
	m_hwnd(NULL),
	m_pDirect2dFactory(NULL),
	m_pRenderTarget(NULL),
	m_pLightSlateGrayBrush(NULL),
	m_pCornflowerBlueBrush(NULL)
{

}

//析构
DemoGame::~DemoGame()
{
	SafeRelease(&m_pDirect2dFactory);
	SafeRelease(&m_pRenderTarget);
	SafeRelease(&m_pLightSlateGrayBrush);
	SafeRelease(&m_pCornflowerBlueBrush);
}

//重新设置对象大小
void DemoGame::OnResize(UINT width, UINT height)
{
	if (m_pRenderTarget)
	{
		m_pRenderTarget->Resize(D2D1::SizeU(width, height));
	}
}

//Message Loop
void DemoGame::RunMessageLoop()
{
	MSG msg;
	while (TRUE) {
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
			//处理消息
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else 
		{
			m_pGameController->GameRunning();
		}
	}
}

//创建设备无关资源
LRESULT DemoGame::CreateDeviceIndependentResources()
{
	HRESULT hr = S_OK;

	//创建一个D2D工厂
	hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &m_pDirect2dFactory);

	//Create the COM imaging factory
	if (SUCCEEDED(hr))
	{
		hr = CoCreateInstance(
			CLSID_WICImagingFactory,
			NULL,
			CLSCTX_INPROC_SERVER,
			IID_PPV_ARGS(&m_pWicImagingFactory)
		);

	}

	//Create Text Format 
	if (SUCCEEDED(hr)) {
		hr = DWriteCreateFactory(
			DWRITE_FACTORY_TYPE_SHARED,
			__uuidof(IDWriteFactory),
			reinterpret_cast<IUnknown**>(&m_pDWriteFactory));
	}

	if (SUCCEEDED(hr)) {
		hr = m_pDWriteFactory->CreateTextFormat(L"微软雅黑", NULL, DWRITE_FONT_WEIGHT_REGULAR, \
			DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, 16.0f, L"chs", &m_pTextFormat);
	}

	return hr;
}

//创建设备相关资源
LRESULT DemoGame::CreateDeviceResources()
{
	HRESULT hr = S_OK;
	
	if (!m_pRenderTarget) {
		RECT rc;
		GetClientRect(m_hwnd, &rc);

		D2D1_SIZE_U size = D2D1::SizeU(rc.right	- rc.left, rc.bottom - rc.top);

		//创建一个D2D渲染对象
		hr = m_pDirect2dFactory->CreateHwndRenderTarget(
			D2D1::RenderTargetProperties(),
			D2D1::HwndRenderTargetProperties(m_hwnd, size),
			&m_pRenderTarget
		);

		if (SUCCEEDED(hr)) {
			//Create a gray brush
			hr = m_pRenderTarget->CreateSolidColorBrush(
				D2D1::ColorF(D2D1::ColorF::LightSlateGray),
				&m_pLightSlateGrayBrush
			);
		}

		if (SUCCEEDED(hr)) {
			//Create a blue brush
			hr = m_pRenderTarget->CreateSolidColorBrush(
				D2D1::ColorF(D2D1::ColorF::CornflowerBlue),
				&m_pCornflowerBlueBrush
			);
		}

	}

	return hr;
}

//加载图片资源
ID2D1Bitmap* DemoGame::LoadImageResource(int resNum, LPCWSTR resType)
{
	HRESULT hr = S_OK;

	hr = CreateDeviceResources();

	//imageResHandle = FindResource(
	//	NULL,             // This component.
	//	MAKEINTRESOURCE(resNum),   // Resource name.
	//	resType);

	imageResHandle = FindResource(
		NULL,             // This component.
		MAKEINTRESOURCE(resNum),   // Resource name.
		resType);

	hr = (imageResHandle ? S_OK : E_FAIL);

	// Load the resource to the HGLOBAL.
	if (SUCCEEDED(hr)) {
		imageResDataHandle = LoadResource(NULL, imageResHandle);
		hr = (imageResDataHandle ? S_OK : E_FAIL);
	}

	// Lock the resource to retrieve memory pointer.
	if (SUCCEEDED(hr)) {
		pImageFile = LockResource(imageResDataHandle);
		hr = (pImageFile ? S_OK : E_FAIL);
	}

	// Calculate the size.
	if (SUCCEEDED(hr)) {
		imageFileSize = SizeofResource(NULL, imageResHandle);
		hr = (imageFileSize ? S_OK : E_FAIL);
	}

	if (SUCCEEDED(hr)) {
		hr = m_pWicImagingFactory->CreateStream(&m_pWicStream);
	}

	//if (SUCCEEDED(hr)) {
	//	//create an IWICBitmapDecoder from an image file
	//	//hr = m_pWicImagingFactory->CreateDecoderFromFilename(
	//	//	L"E:/test.jpg",						// Image to be decoded
	//	//	NULL,								// Do not prefer a particular vendor
	//	//	GENERIC_READ,						// Desired read access to the file
	//	//	WICDecodeMetadataCacheOnDemand,		// Cache metadata when needed
	//	//	&m_pWicDecoder                      // Pointer to the decoder
	//	//);

	//}

	// Initialize the stream with the memory pointer and size.
	if (SUCCEEDED(hr)) {
		hr = m_pWicStream->InitializeFromMemory(
			reinterpret_cast<BYTE*>(pImageFile),
			imageFileSize);
	}

	if (SUCCEEDED(hr)) {
		hr = m_pWicImagingFactory->CreateDecoderFromStream(
			m_pWicStream,                   // The stream to use to create the decoder
			NULL,                          // Do not prefer a particular vendor
			WICDecodeMetadataCacheOnLoad,  // Cache metadata when needed
			&m_pWicDecoder);                   // Pointer to the decoder
	}

	// Retrieve the first bitmap frame.
	if (SUCCEEDED(hr))
	{
		hr = m_pWicDecoder->GetFrame(0, &m_pWicFrame);
	}

	//Convert the bitmap source to an 32bppPBGRA pixel format.
	if (SUCCEEDED(hr)) {
		SafeRelease(&m_pConvertedSourceBitMap);
		hr = m_pWicImagingFactory->CreateFormatConverter(&m_pConvertedSourceBitMap);
	}

	if (SUCCEEDED(hr)) {
		hr = m_pConvertedSourceBitMap->Initialize(
			m_pWicFrame,					 // Input bitmap to convert
			GUID_WICPixelFormat32bppPBGRA,   // Destination pixel format
			WICBitmapDitherTypeNone,         // Specified dither pattern
			NULL,                            // Specify a particular palette 
			0.f,                             // Alpha threshold
			WICBitmapPaletteTypeCustom       // Palette translation type
		);
	}

	//Create an ID2D1Bitmap object
	if (m_pConvertedSourceBitMap)
	{
		m_pRenderTarget->CreateBitmapFromWicBitmap(m_pConvertedSourceBitMap, NULL, &m_pTempID2DBitmap);
	}

	return m_pTempID2DBitmap;
}

//初始化
HRESULT DemoGame::Initialize()
{
	HRESULT hr;

	//设备无关资源
	hr = CreateDeviceIndependentResources();

	if (SUCCEEDED(hr)) {
		//设计窗口
		WNDCLASSEX wcex = { 0 };
		wcex.cbSize = sizeof(WNDCLASSEX);
		wcex.style = CS_DBLCLKS | CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
		wcex.lpfnWndProc = DemoGame::WndProc;
		wcex.cbClsExtra = 0;
		wcex.cbWndExtra = 0;
		wcex.hInstance = NULL;
		wcex.hIcon = NULL;
		wcex.hCursor = LoadCursor(NULL, IDI_APPLICATION);
		wcex.hbrBackground = NULL;
		wcex.lpszMenuName = NULL;
		wcex.lpszClassName = L"D2DDemoGame";

		//注册窗口
		RegisterClassEx(&wcex);

		FLOAT dpiX, dpiY;

		m_pDirect2dFactory->GetDesktopDpi(&dpiX, &dpiY);

		//创建窗口
		m_hwnd = CreateWindow(
			L"D2DDemoGame",
			L"Demo Game",
			WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_VISIBLE,
			CW_USEDEFAULT,
			CW_USEDEFAULT,
			static_cast<UINT>(ceil(800.f * dpiX / 96.f)),
			static_cast<UINT>(ceil(600.f * dpiY / 96.f)),
			NULL,
			NULL,
			NULL,
			this
		);

		hr = m_hwnd ? S_OK : E_FAIL;

		if (SUCCEEDED(hr))
		{
			ShowWindow(m_hwnd, SW_SHOWNORMAL);
			UpdateWindow(m_hwnd);
		}

		if (SUCCEEDED(hr))
		{
			ShowWindow(m_hwnd, SW_SHOWNORMAL);
			UpdateWindow(m_hwnd);
		}
	}

	//创建游戏控制器
	if (SUCCEEDED(hr))
	{
		m_pGameController = new GameController(this);
		m_pGameController->Initialize();
	}

	return hr;
}

//render target
//void DemoGame::OnRender()
//{
//	HRESULT hr = S_OK;
//
//	hr = CreateDeviceResources();
//
//	if (SUCCEEDED(hr))
//	{
//		m_pRenderTarget->BeginDraw();
//
//		m_pGameController->onRender();
//		//D2D1_SIZE_F rtSize = m_pRenderTarget->GetSize();
//		//D2D1_SIZE_U sizeU = m_pD2DBMP_MoveSprite->GetPixelSize();
//		//D2D1_RECT_F rectangle3 = D2D1::RectF(
//		//	(rtSize.width - sizeU.width)*0.5f,
//		//	(rtSize.height - sizeU.height)*0.5f,
//		//	sizeU.width + (rtSize.width - sizeU.width)*0.5f,
//		//	sizeU.height + (rtSize.height - sizeU.height)*0.5f
//		//);
//		//m_pRenderTarget->DrawBitmap(m_pD2DBMP_MoveSprite, &rectangle3, 1.0f);
//
//		m_pRenderTarget->EndDraw();
//	}
//}

//render start
HRESULT DemoGame::RenderStart()
{
	HRESULT hr = S_OK;
	
	hr = CreateDeviceResources();
	
	if (SUCCEEDED(hr))
	{
		m_pRenderTarget->BeginDraw();
	}

	return hr;
}

void DemoGame::RenderEnd()
{
	m_pRenderTarget->EndDraw();
}

//游戏的过程函数
LRESULT CALLBACK DemoGame::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	LRESULT result = 0;

	if (message == WM_CREATE)
	{
		LPCREATESTRUCT pcs = (LPCREATESTRUCT)lParam;
		DemoGame *pDemoGame = (DemoGame *)pcs->lpCreateParams;

		::SetWindowLongPtrW(
			hWnd,
			GWLP_USERDATA,
			PtrToUlong(pDemoGame)
		);

		result = 1;
	}
	else
	{
		DemoGame *pDemoGame = reinterpret_cast<DemoGame *>(static_cast<LONG_PTR>(
			::GetWindowLongPtrW(
				hWnd,
				GWLP_USERDATA
			)));

		bool wasHandled = false;

		if (pDemoGame)
		{
			switch (message)
			{
			case WM_SIZE:
				{
					UINT width = LOWORD(lParam);
					UINT height = HIWORD(wParam);
					pDemoGame->OnResize(width, height);
				}
				result = 0;
				wasHandled = true;
				break;
			case WM_DESTROY:
				{
					PostQuitMessage(0);
				}
				result = 1;
				wasHandled = true;
				break;
			}
		}

		if (!wasHandled)
		{
			result = DefWindowProc(hWnd, message, wParam, lParam);
		}
	}
	return result;
}

//心脏
int WINAPI WinMain(
	HINSTANCE hInstance,
	HINSTANCE hPrevInstace,
	LPSTR lpCmdLine,
	int nCmdShow
)
{
	HeapSetInformation(NULL, HeapEnableTerminationOnCorruption, NULL, 0);

	if (SUCCEEDED(CoInitialize(NULL)))
	{
		DemoGame app;

		if (SUCCEEDED(app.Initialize())){
			app.RunMessageLoop();
		}
		CoUninitialize();
	}
	return 0;
	
}

//释放资源
void DemoGame::ReleaseObjs()
{
	//D2D
	SafeRelease(&m_pDirect2dFactory);
	SafeRelease(&m_pRenderTarget);
	SafeRelease(&m_pLightSlateGrayBrush);
	SafeRelease(&m_pCornflowerBlueBrush);

	//WIC
	SafeRelease(&m_pWicImagingFactory);
	SafeRelease(&m_pWicDecoder);
	SafeRelease(&m_pWicFrame);
	SafeRelease(&m_pConvertedSourceBitMap);
}