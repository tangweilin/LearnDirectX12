#pragma once

struct FEngineRenderConfig
{
	FEngineRenderConfig();

	int ScreenWidth;//��Ļ��
	int ScreenHeight;//��Ļ��
	int RefreshRate;//ˢ��ʱ�䣨֡�ʣ�
	int SwapChainCount;//����������

	static FEngineRenderConfig* GetRenderConfig();
	static void Destroy();
private:
	static FEngineRenderConfig* RenderConfig;
};