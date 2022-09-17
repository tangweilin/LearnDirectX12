#pragma once

#if defined(_WIN32)
#include "../../Core/Engine.h"
#include "../../Rendering/Enigne/Core/RenderingEngine.h"

class CDirectXRenderingEngine;

class CWindowsEngine :public CEngine
{
	friend class IRenderingInterface;
public:
	CWindowsEngine();
	~CWindowsEngine();
	//预初始化
	virtual int PreInit(FWinMainCommandParameters InParameters);
	//初始化
	virtual int Init(FWinMainCommandParameters InParameters);
	//结束初始化
	virtual int PostInit();
	//tick渲染行为
	virtual void Tick(float DeltaTime);
	//预退出
	virtual int PreExit();
	//退出
	virtual int Exit();
	//结束退出
	virtual int PostExit();
	//获得DX渲染引擎
	CDirectXRenderingEngine* GetRenderingEngine() { return RenderingEngine; }
public:
	//初始化Windows
	bool InitWindows(FWinMainCommandParameters InParameters);

protected:
	HWND MainWindowsHandle;//主windows句柄

protected:
	CDirectXRenderingEngine* RenderingEngine; //DX渲染引擎
};
#endif