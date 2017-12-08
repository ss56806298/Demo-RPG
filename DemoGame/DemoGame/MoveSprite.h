#pragma once

#include "CommonHeader.h"

class MoveSprite
{
public:
	MoveSprite();
	~MoveSprite();

	//初始化
	LRESULT Initialize();
private:
	//绘制
	void onRender();
};