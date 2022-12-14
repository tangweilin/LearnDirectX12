#include "WindowsMessageProcessing.h"

LRESULT CALLBACK EngineWindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch(msg)
    {
        case WM_CLOSE://关闭
            PostQuitMessage(0);
            return 0;
    }
    //交给Windows自己处理完成
    return DefWindowProc(hwnd, msg, wParam, lParam);
}
