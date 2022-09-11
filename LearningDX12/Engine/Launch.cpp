#pragma once
#include "EngineMinimal.h"
#include "EngineFactory.h"
#include "Debug/Log/SimpleLog.h"


int Init(FEngine* InEngine, HINSTANCE hInstance, HINSTANCE prevInstance, PSTR cmdLine, int showCmd)// �����ʼ��
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
void Tick(FEngine* InEngine)//������Ⱦ
{
	float DeltaTime = 0.03f;
	InEngine->Tick(DeltaTime);
}
int Exit(FEngine* InEngine)//�˳�����
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

//hInstance �����ʵ��
//prevInstance �ϴ�������ʵ��
//cmdLine ��������
//showcmd ��ǰ������ʲô��ʽ��ʾ  
	//SW_HIDE ���ش��ڲ��Ҽ�����һ������ 
	//SW_MINIMIZE ��С��ָ���Ĵ��ڣ����Ҽ�����ϵͳ���ֵĶ��㴰��
	//SW_RESTORE �����ʾ���ڡ���������Ѿ���С������󻯣�ϵͳ���Իָ���ԭ���ĳߴ��λ����ʾ���ڣ���SW_SHOWNORMAL��ͬ��
	//SW_SHOW ����һ�����ڲ���ԭ���ĳߴ��λ����ʾ����
	//SW_SHOWMAXIMIZED ����ڲ��������
	//SW_SHOWMINIMIZED ����ڲ�������С��
	//SW_SHOWMINNOACTIVE ��һ��������ʾΪͼ�ꡣ�����ά�ֻ״̬
	//SW_SHOWNA �Դ��ڵĵ�ǰ״̬��ʾ���ڡ�����ڱ��ֻ״̬
	//SW_SHOWNOACTIVATE �Դ��ڵ����һ�εĳߴ��λ����ʾ���ڡ��������
	//SW_SHOWNORMAL �����ʾ����
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance, PSTR cmdLine, int showCmd) // ��ڳ���
{
	Engine = FEngineFactory::CreateEngine();
	int ReturnValue = 0;
	if (Engine)
	{
		//��ʼ��
		Init(Engine, hInstance, prevInstance, cmdLine, showCmd);

		MSG EngineMsg = { 0 };

		//�����ȡ����Windows��Ϣ�����˳� ������Ⱦ
		while (EngineMsg.message != WM_QUIT)
		{
			//PM_NOREMOVE ��Ϣ���Ӷ����������
			//PM_REMOVE   ��Ϣ�Ӷ����������
			//PM_NOYIELD  �˱�־ʹϵͳ���ͷŵȴ����ó�����е��߳�
			// 
			//PM_QS_INPUT �������ͼ�����Ϣ��
			//PM_QS_PAINT ����ͼ��Ϣ��
			//PM_QS_POSTMESSAGE �������б����͵���Ϣ��������ʱ�����ȼ���
			//PM_QS_SENDMESSAGE �������з�����Ϣ��
			if (PeekMessage(&EngineMsg, 0, 0, 0, PM_REMOVE))//�Ƴ���Ϣ����
			{
				TranslateMessage(&EngineMsg);//����Ϣ������ַ���
				DispatchMessage(&EngineMsg);//������Ϣ������
			}
			else
			{
				Tick(Engine);//��Ⱦ
			}
		}

		ReturnValue = Exit(Engine); //Ԥ�˳� �˳�
	}
	else
	{
		ReturnValue = 1;
	}
	Engine_Log("[%i]Engine Has Exited .", ReturnValue);
	return ReturnValue;
}