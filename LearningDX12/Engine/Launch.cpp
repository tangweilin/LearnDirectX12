#pragma once
#include "EngineMinimal.h"
#include "EngineFactory.h"
#include "Debug/Log/SimpleLog.h"


int Init(FEngine* InEngine, HINSTANCE hInstance, HINSTANCE prevInstance, PSTR cmdLine, int showCmd)// 引擎初始化
{
#if defined(_WIN32)
	FWinMainCommandParameters WinMainParameters(hInstance, prevInstance, cmdLine, showCmd);
#endif 

	int ReturnValue = InEngine->PreInit(
#if defined(_WIN32)
		WinMainParameters
#endif 
	);
	if (ReturnValue != 0)
	{
		Engine_Log_Error("[%i]Engine Pre Initialization Error, Check And Initialization Problem .", ReturnValue);
		return ReturnValue;
	}

	ReturnValue = InEngine->Init(
#if defined(_WIN32)
		WinMainParameters
#endif 
	);

	if (ReturnValue != 0)
	{
		Engine_Log_Error("[%i]Engine Initialization Error, Please Check The Initialization Problem .", ReturnValue);
		return ReturnValue;
	}
	ReturnValue = InEngine->PostInit();

	if (ReturnValue != 0)
	{
		Engine_Log_Error("[%i]Engine Post Initialization Error, Please Check The Initialization Problem .", ReturnValue);
		return ReturnValue;
	}
	return ReturnValue;
}
void Tick(FEngine* InEngine)//引擎渲染
{
	float DeltaTime = 0.03f;
	InEngine->Tick(DeltaTime);
}
int Exit(FEngine* InEngine)//退出引擎
{
	int ReturnValue = InEngine->PreExit();
	if (ReturnValue != 0)
	{
		Engine_Log_Error("[%i]Engine Pre Exit Failed .", ReturnValue);
		return ReturnValue;
	}

	ReturnValue = InEngine->Exit();
	if (ReturnValue != 0)
	{
		Engine_Log_Error("[%i]Engine Exit Failed .", ReturnValue);
		return ReturnValue;
	}

	ReturnValue = InEngine->PostExit();
	if (ReturnValue != 0)
	{
		Engine_Log_Error("[%i]Engine Post Exit Failed .", ReturnValue);
		return ReturnValue;
	}
	ReturnValue = 0;
	return ReturnValue;
}

FEngine* Engine = NULL;

//hInstance 自身的实例
//prevInstance 上次启动的实例
//cmdLine 传递命令
//showcmd 当前窗口以什么方式显示  
	//SW_HIDE 隐藏窗口并且激活另一个窗口 
	//SW_MINIMIZE 最小化指定的窗口，并且激活在系统表种的顶层窗口
	//SW_RESTORE 激活并显示窗口。如果窗口已经最小化或最大化，系统将以恢复到原来的尺寸和位置显示窗口（于SW_SHOWNORMAL相同）
	//SW_SHOW 激活一个窗口并以原来的尺寸和位置显示窗口
	//SW_SHOWMAXIMIZED 激活窗口并将其最大化
	//SW_SHOWMINIMIZED 激活窗口并将其最小化
	//SW_SHOWMINNOACTIVE 将一个窗口显示为图标。激活窗口维持活动状态
	//SW_SHOWNA 以窗口的当前状态显示窗口。激活窗口保持活动状态
	//SW_SHOWNOACTIVATE 以窗口的最近一次的尺寸和位置显示窗口。不激活窗口
	//SW_SHOWNORMAL 激活并显示窗口
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance, PSTR cmdLine, int showCmd) // 入口程序
{
	Engine = FEngineFactory::CreateEngine();
	int ReturnValue = 0;
	if (Engine)
	{
		//初始化
		Init(Engine, hInstance, prevInstance, cmdLine, showCmd);

		MSG EngineMsg = { 0 };

		//如果获取到的Windows消息不是退出 进行渲染
		while (EngineMsg.message != WM_QUIT)
		{
			//PM_NOREMOVE 消息不从队列里除掉。
			//PM_REMOVE   消息从队列里除掉。
			//PM_NOYIELD  此标志使系统不释放等待调用程序空闲的线程
			// 
			//PM_QS_INPUT 处理鼠标和键盘消息。
			//PM_QS_PAINT 处理画图消息。
			//PM_QS_POSTMESSAGE 处理所有被寄送的消息，包括计时器和热键。
			//PM_QS_SENDMESSAGE 处理所有发送消息。
			if (PeekMessage(&EngineMsg, 0, 0, 0, PM_REMOVE))//移出消息队列
			{
				TranslateMessage(&EngineMsg);//把消息翻译成字符串
				DispatchMessage(&EngineMsg);//发送消息至窗口
			}
			else
			{
				Tick(Engine);//渲染
			}
		}

		ReturnValue = Exit(Engine); //预退出 退出
	}
	else
	{
		ReturnValue = 1;
	}
	Engine_Log("[%i]Engine Has Exited .", ReturnValue);
	return ReturnValue;
}