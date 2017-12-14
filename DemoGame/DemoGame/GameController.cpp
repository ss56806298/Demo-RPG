#include "GameController.h"
#include "resource.h"

GameController::GameController(DemoGame *demoGame) :
	m_pDemoGame(demoGame)
{
}

GameController::~GameController()
{

}

//初始化
void GameController::Initialize()
{
	//加载场景资源
	LoadSceneRes();

	//加载UI资源
	LoadUiRes();
}

//加载场景资源
void GameController::LoadSceneRes()
{
	m_pBM_Map_City = m_pDemoGame->LoadImageResource(MAP_CITY, L"MAP");

}

//加载UI资源
void GameController::LoadUiRes()
{
	m_pBM_Dialog_Frame = m_pDemoGame->LoadImageResource(DIALOG_FRAME, L"UI");
	m_pPN_Dialog_End_Arrow = m_pDemoGame->LoadImageResource(DIALOG_END_ARROW, L"UI");
}

void GameController::GameRunning()
{
	switch (GameMode) {
	case NoMode:
		GameStart();
		break;
	}
}

//游戏开始
void GameController::GameStart()
{
	GameMode = WaitMode;
	onRender();
	GameDelay(100);
	RenderTalk(L"新的冒险开始了", L"???", 1, 2, 0);
	RenderTalk(L"新的冒险开始了", L"???", 1, 2, 1);
	RenderTalk(L"新的冒险开始了", L"???", 1, 2, 0);
	RenderTalk(L"新的冒险开始了", L"???", 1, 3, 1);
	RenderTalk(L"新的冒险开始了", L"???", 1, 3, 0);
}

//剧情对话
void GameController::StoryTalk()
{

}

void GameController::GameDelay(long time)
{
	long new_time, old_time;
	new_time = old_time = GetTickCount();
	while (new_time - old_time < time) {
		new_time = GetTickCount();
		CheckSysMsg();
	}
	
}

void GameController::onRender()
{
	m_pDemoGame->RenderStart();
	//绘制地图
	D2D1_SIZE_F rtSize = m_pDemoGame->m_pRenderTarget->GetSize();
	/*D2D1_SIZE_U sizeU = m_pBM_Map_City->GetPixelSize();*/
	D2D1_RECT_F rectangle = D2D1::RectF(
		0.0f,
		0.0f,
		rtSize.width*1.0f,
		rtSize.height*1.0f
	);

	m_pDemoGame->m_pRenderTarget->DrawBitmap(m_pBM_Map_City, &rectangle, 1.0f);
	m_pDemoGame->RenderEnd();
}

void GameController::RenderTalk(LPCWSTR text, LPCWSTR speaker, int left_chara, int right_chara, int focus)
{

	D2D1_SIZE_F rtSize = m_pDemoGame->m_pRenderTarget->GetSize();

	D2D1_RECT_F rectangle_dialog_frame = D2D1::RectF(
		0.0f,
		rtSize.height*0.65f,
		rtSize.width*1.0f,
		rtSize.height*1.0f
	);

	D2D1_RECT_F rectangle_dialog_speaker = D2D1::RectF(
		rtSize.width*0.05f,
		rtSize.height*0.665f,
		rtSize.width*0.2f,
		rtSize.height*0.70f
	);

	D2D1_RECT_F rectangle_dialog = D2D1::RectF(
		rtSize.width*0.05f,
		rtSize.height*0.75f,
		rtSize.width*0.95f,
		rtSize.height*0.95f
	);

	D2D1_RECT_F rectangle_dialog_end_arrow = D2D1::RectF(
		rtSize.width*0.92f,
		rtSize.height*0.93f,
		rtSize.width*0.95f,
		rtSize.height*0.95f
	);

	ID2D1Bitmap* left_chara_bitmap = NULL;
	ID2D1Bitmap* right_chara_bitmap = NULL;
	D2D1_RECT_F rectangle_left_chara;
	D2D1_RECT_F rectangle_right_chara;
	if (left_chara != 0) 
	{
		left_chara_bitmap = GetD2DBitmap(left_chara, 1);
		D2D1_SIZE_U sizeU = left_chara_bitmap->GetPixelSize();
		rectangle_left_chara = D2D1::RectF(
			rtSize.width * 0.25f - sizeU.width * 0.5f,
			rtSize.height * 0.3f - sizeU.height * 0.25f,
			rtSize.width * 0.25f + sizeU.width * 0.5f,
			rtSize.height * 0.3f + sizeU.height * 0.75f
		);
	}
	if (right_chara != 0) {
		right_chara_bitmap = GetD2DBitmap(right_chara, 1);
		D2D1_SIZE_U sizeU = right_chara_bitmap->GetPixelSize();
		rectangle_right_chara = D2D1::RectF(
			rtSize.width * 0.75f - sizeU.width * 0.5f,
			rtSize.height * 0.3f - sizeU.height * 0.25f,
			rtSize.width * 0.75f + sizeU.width * 0.5f,
			rtSize.height * 0.3f + sizeU.height * 0.75f
		);
	}

	//绘制对话
	int length = wcslen(text);
	wchar_t* now_word = new wchar_t[length + 1]();
	int byte_size = sizeof(wchar_t);
	int x = 1;
	int next_dialog = 0;
	long new_time, old_time;
	new_time = old_time = GetTickCount();
	bool dialog_end_arrow_flag = false;
	while (next_dialog != 2)
	{
		memcpy(now_word, text, x * byte_size);

		m_pDemoGame->RenderStart();
		//绘制背景
		RenderBackgourd();

		//焦点在左边的人
		if (focus == 0) {
			if (right_chara != 0) {
				m_pDemoGame->m_pRenderTarget->DrawBitmap(right_chara_bitmap, &rectangle_right_chara, 0.5f);
			}

			if (left_chara != 0) {
				m_pDemoGame->m_pRenderTarget->DrawBitmap(left_chara_bitmap, &rectangle_left_chara, 1.0f);
			}
		}
		else if (focus == 1)
		{
			if (left_chara != 0) {
				m_pDemoGame->m_pRenderTarget->DrawBitmap(left_chara_bitmap, &rectangle_left_chara, 0.5f);
			}

			if (right_chara != 0) {
				m_pDemoGame->m_pRenderTarget->DrawBitmap(right_chara_bitmap, &rectangle_right_chara, 1.0f);
			}

		}

		m_pDemoGame->m_pRenderTarget->DrawBitmap(m_pBM_Dialog_Frame, &rectangle_dialog_frame, 0.9f); //绘制对话框

		m_pDemoGame->m_pRenderTarget->DrawText(speaker, wcslen(speaker), m_pDemoGame->m_pTextFormat, rectangle_dialog_speaker, m_pDemoGame->m_pCornflowerBlueBrush); //说话人

		m_pDemoGame->m_pRenderTarget->DrawText(
			now_word,
			wcslen(now_word),
			m_pDemoGame->m_pTextFormat,
			rectangle_dialog,
			m_pDemoGame->m_pLightSlateGrayBrush
		);
		
		//对话结束
		if (x == length) {
			new_time = GetTickCount();
			if (new_time - old_time > 300) {
				old_time = new_time;
				dialog_end_arrow_flag = !dialog_end_arrow_flag;
			}

			if (dialog_end_arrow_flag) {
				m_pDemoGame->m_pRenderTarget->DrawBitmap(m_pPN_Dialog_End_Arrow, &rectangle_dialog_end_arrow, 1.0f); //绘制对话结束标志
			}

			CheckSysMsg();
			if (next_dialog == 0 && !GetAsyncKeyState(VK_SPACE)) next_dialog = 1;
			if (next_dialog == 1 && GetAsyncKeyState(VK_SPACE)) next_dialog = 2;
		}
		else 
		{
			GameDelay(TalkSpeed);
			x++;
		}

		m_pDemoGame->RenderEnd();
	}
	delete []now_word;
}

//绘制背景
void GameController::RenderBackgourd()
{
	//绘制地图
	D2D1_SIZE_F rtSize = m_pDemoGame->m_pRenderTarget->GetSize();
	/*D2D1_SIZE_U sizeU = m_pBM_Map_City->GetPixelSize();*/
	D2D1_RECT_F rectangle = D2D1::RectF(
		0.0f,
		0.0f,
		rtSize.width*1.0f,
		rtSize.height*1.0f
	);

	m_pDemoGame->m_pRenderTarget->DrawBitmap(m_pBM_Map_City, &rectangle, 1.0f);
}

//根据序号获取人物的D2D BITMAP
ID2D1Bitmap* GameController::GetD2DBitmap(int id, int type)
{
	switch (type)
	{
	case 1:
		//人物
		if (!pImageCharaArray[id - 1]) pImageCharaArray[id - 1] = m_pDemoGame->LoadImageResource( 2000 + id, L"CHARA");
		return	pImageCharaArray[id - 1];
		break;
	}
}

//检查系统消息
void GameController::CheckSysMsg()
{
	MSG msg;
	if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

}

//按下键后松开
void GameController::PressKey(DWORD Key, int x)
{
	if (x == 0)
	while (!GetAsyncKeyState(Key))
	{
		CheckSysMsg();
	}
	while (GetAsyncKeyState(Key))
	{
		CheckSysMsg();
	}
}