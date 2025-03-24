#include "Common\pch.h"
#include "Gfw/GameFramework.h"
// 콘솔 창 생성 함수
void CreateConsoleWindow()
{
    AllocConsole();
    FILE* pCout;
    freopen_s(&pCout, "CONOUT$", "w", stdout);
    FILE* pCerr;
    freopen_s(&pCerr, "CONOUT$", "w", stderr);
    FILE* pCin;
    freopen_s(&pCin, "CONIN$", "r", stdin);
    std::ios::sync_with_stdio();
}

int APIENTRY wWinMain(
    _In_        HINSTANCE hInstance,
    _In_opt_    HINSTANCE hPrevInstance,
    _In_        LPWSTR    lpCmdLine,
    _In_        int       nCmdShow)
{
    CreateConsoleWindow();
    CGameFramework D3D;
    D3D.InitGame(hInstance, nCmdShow, 800, 600);
  
    return D3D.Run();
}
