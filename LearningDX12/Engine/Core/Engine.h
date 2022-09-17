#pragma once
#include "CoreObject/CoreMinimalObject.h"

#if defined(_WIN32)  //windows
#include "WinMainCommandParameters.h"
#endif //0

class CEngine:public CCoreMinimalObject
{
public:
	CEngine();

	virtual int PreInit(
#if defined(_WIN32)
		FWinMainCommandParameters InParameters
#endif
	) = 0;//����Ԥ��ʼ��
	virtual int Init(
#if defined(_WIN32)
		FWinMainCommandParameters InParameters
#endif
	) = 0;//�����ʼ��
	virtual int PostInit() = 0;//������ʼ��

	virtual void Tick(float DeltaTime) {};//��Ⱦ

	virtual int PreExit() = 0;//Ԥ�˳�
	virtual int Exit() = 0;//�˳�
	virtual int PostExit() = 0;//�˳�����
};