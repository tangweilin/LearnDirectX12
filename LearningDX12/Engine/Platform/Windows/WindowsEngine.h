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
	//Ԥ��ʼ��
	virtual int PreInit(FWinMainCommandParameters InParameters);
	//��ʼ��
	virtual int Init(FWinMainCommandParameters InParameters);
	//������ʼ��
	virtual int PostInit();
	//tick��Ⱦ��Ϊ
	virtual void Tick(float DeltaTime);
	//Ԥ�˳�
	virtual int PreExit();
	//�˳�
	virtual int Exit();
	//�����˳�
	virtual int PostExit();
	//���DX��Ⱦ����
	CDirectXRenderingEngine* GetRenderingEngine() { return RenderingEngine; }
public:
	//��ʼ��Windows
	bool InitWindows(FWinMainCommandParameters InParameters);

protected:
	HWND MainWindowsHandle;//��windows���

protected:
	CDirectXRenderingEngine* RenderingEngine; //DX��Ⱦ����
};
#endif