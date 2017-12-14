#pragma once

#include "DemoGame.h"

enum GameMode
{
	NoMode = 0,
	WaitMode = 1,
	TalkMode = 2,
	TalkingMode = 3,
	PaintMode = 100
};

class GameController
{
public:
	GameController(DemoGame *m_pDemoGame);
	~GameController();

	//游戏状态
	GameMode GameMode = NoMode;	//0为未运行

	//游戏每一帧最低时间
	int GameFrames = 60;	//默认每秒60次
	//对话文字播放速度 ms
	int TalkSpeed = 100;
	

	//当前场景
	int CurrentScene = 0;	//当前场景编号

	//游戏设备信息等
	DemoGame *m_pDemoGame;

	//初始化
	void Initialize();

	//绘制
	void onRender();

	//游戏运行
	void GameRunning();
	//游戏开始
	void GameStart();
	//游戏延时
	void GameDelay(long time);
	//剧情对话
	void StoryTalk();
	//绘制对话
	void RenderTalk(LPCWSTR text, LPCWSTR speaker, int left_cha, int right_cha, int focus);	//focus 0左边 1右边
	//绘制背景
	void RenderBackgourd();
	//检查系统消息
	void CheckSysMsg();
	//按下键后松开
	void PressKey(DWORD Key, int x);
private:
	ID2D1Bitmap *m_pBM_Map_City = NULL;	//传奇之城

	ID2D1Bitmap *m_pBM_Dialog_Frame = NULL; //对话框
	ID2D1Bitmap *m_pPN_Dialog_End_Arrow = NULL;	//对话结束标志

	//人物
	ID2D1Bitmap* pImageCharaArray[10] = {};

	//根据序号获取人物的D2DBITMAP
	ID2D1Bitmap* GetD2DBitmap(int id, int type);

	//加载场景图片
	void LoadSceneRes();

	//加载UI资源
	void LoadUiRes();

};
