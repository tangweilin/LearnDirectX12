#include "WindowsMessageProcessing.h"

LRESULT CALLBACK EngineWindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch(msg)
    {
        case WM_CLOSE://�ر�
            PostQuitMessage(0);
            return 0;
    }
    //����Windows�Լ��������
    return DefWindowProc(hwnd, msg, wParam, lParam);
}
