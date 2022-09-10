#pragma once

struct FEngineRenderConfig
{
	FEngineRenderConfig();

	int ScreenWidth;//屏幕宽
	int ScreenHeight;//屏幕高
	int RefreshRate;//刷新时间（帧率）
	int SwapChainCount;//交换链数量

	static FEngineRenderConfig* GetRenderConfig();
	static void Destroy();
private:
	static FEngineRenderConfig* RenderConfig;
};