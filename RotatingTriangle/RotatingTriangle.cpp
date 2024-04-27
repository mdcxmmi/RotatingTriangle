// RotatingTriangle.cpp : Определяет точку входа для приложения.
//

#include "framework.h"
#include "RotatingTriangle.h"
#include<math.h>

#define MAX_LOADSTRING 100

#define PI 3.14159265  
#define MARGINTOP 50
#define MARGINLEFT 50
#define MARGINRIGHT 50
#define MARGINBOTTOM 50

#define DEFAULTMAINPOINT_X = 500
#define DEFAULTMAINPOINT_Y = 500

#define backgroundcolor RGB(222,222,222)
#define  bordercolor RGB(0,0,0)

#define TriangleBackgroupndColor RGB(222,222,222)
#define TriangleBorderColor RGB(0,0,222)

#define TWISTSPEED = 100


HWND hWnd;

struct stCONFIG
{
    POINT topleft;
    POINT bottomright;
    RECT winrect;
    BOOL dirty;
} MoveBoxConfig;


struct TTriangle {
    POINT A;
    POINT B;
    POINT C;

    COLORREF color;
    COLORREF backgroundColor;
    UCHAR thickness;
    BOOL visible;
    BOOL dirty;
    double omega;
    double omegaA, omegaB, omegaC;
    double radA, radB, radC;
} Triangle;

enum TVector
{
    Left,
    Right
};


struct TMoving {
    TVector vector;
    UCHAR speed;
}Moving;


POINT MainPoint;


double changeAngile(double angile, double increment) {
    double sum = angile + increment;
    //    if (sum > 360) sum -= 360;
    //    if (sum < 360) sum += 360;

    return sum;
}


double GradToRadian(double grad) {
    return grad * PI / 180;
}

double RadianToGrad(double rad) {
    return rad * 180 / PI;
}


//Длина отрезка в декартовых координатах
double LineLength(POINT a, POINT b) {

    return sqrt(pow(a.x - b.x, 2) + pow(a.y - b.y, 2));
}


//Угол между линией образованной точкой вращения и вершиной и осью X
double BaseAngle(POINT B) {
    double omega;
    //Точка вращения - MainPoint, Вершина "B"
    //У них общая точка - MainPoint. Т.е.  один вектор - от точки MainPoint до "B"
    //Для вычисления угла, нужно получить 2 вектор, параллельный оси X. Отложим от MainPoint по оси X, длину первого вектора:  

    POINT A;
    //   A.x = MainPoint.x + LineLength(MainPoint, B);
    //   A.y = MainPoint.y;


       //Sector 1 - X>0 && Y>0  Точка B правее и выше
    if (MainPoint.x <= B.x && MainPoint.y >= B.y) {
        A.x = MainPoint.x + LineLength(MainPoint, B);
        A.y = MainPoint.y;

        omega = RadianToGrad(acos((A.x * A.y + B.x * B.y) / (sqrt(pow(A.x, 2) + pow(B.x, 2)) * sqrt(pow(A.y, 2) + pow(B.y, 2)))));
        return omega;
    }

    //Sector 2 - X<0 && Y>0   Точка B левее и выше MainPoint
    if (MainPoint.x >= B.x && MainPoint.y >= B.y) {
        A.x = MainPoint.x - LineLength(MainPoint, B);
        A.y = MainPoint.y;
        omega = RadianToGrad(acos((A.x * A.y + B.x * B.y) / (sqrt(pow(A.x, 2) + pow(B.x, 2)) * sqrt(pow(A.y, 2) + pow(B.y, 2)))));
        omega += 90;
        return omega;
    }

    //Sector 3 - X<0 && Y<0   Точка B левее  и ниже MainPoint
    if (MainPoint.x >= B.x && MainPoint.y <= B.y) {
        A.x = MainPoint.x - LineLength(MainPoint, B);
        A.y = MainPoint.y;
        omega = RadianToGrad(acos((A.x * A.y + B.x * B.y) / (sqrt(pow(A.x, 2) + pow(B.x, 2)) * sqrt(pow(A.y, 2) + pow(B.y, 2)))));
        omega += 180;
        return omega;
    }

    //Sector 4 - X<0 && Y<0   Точка B правее  и ниже MainPoint
    if (MainPoint.x <= B.x && MainPoint.y <= B.y) {
        A.x = MainPoint.x + LineLength(MainPoint, B);
        A.y = MainPoint.y;
        omega = RadianToGrad(acos((A.x * A.y + B.x * B.y) / (sqrt(pow(A.x, 2) + pow(B.x, 2)) * sqrt(pow(A.y, 2) + pow(B.y, 2)))));
        omega = 360 - omega;
        return omega;
    }


}


void Move() {
    if (Moving.vector == Left) {
        Triangle.omega = changeAngile(Triangle.omega, Moving.speed);
    }
    else {
        Triangle.omega = changeAngile(Triangle.omega, -Moving.speed);
    }
}




void InitMovingBox() {
    RECT R;

    GetClientRect(hWnd, &R);

    if (R.top != MoveBoxConfig.winrect.top || R.left != MoveBoxConfig.winrect.left ||
        R.right != MoveBoxConfig.winrect.right || R.bottom != MoveBoxConfig.winrect.bottom)
    {
        MoveBoxConfig.dirty = true;
        MoveBoxConfig.topleft.x = MARGINLEFT;
        MoveBoxConfig.topleft.y = MARGINTOP;
        MoveBoxConfig.bottomright.x = R.right - MARGINRIGHT;
        MoveBoxConfig.bottomright.y = R.bottom - MARGINBOTTOM;

        MoveBoxConfig.winrect.top = R.top;
        MoveBoxConfig.winrect.left = R.left;
        MoveBoxConfig.winrect.right = R.right;
        MoveBoxConfig.winrect.bottom = R.bottom;
    }

}


void DrawMovingBox(HDC dc) {
    if (MoveBoxConfig.dirty) {
        HBRUSH bkg = CreateSolidBrush(backgroundcolor);
        HBRUSH bOld = (HBRUSH)SelectObject(dc, bkg);
        HPEN borderpen = CreatePen(PS_SOLID, 1, bordercolor);
        HPEN pOld = (HPEN)SelectObject(dc, borderpen);

        Rectangle(dc, MoveBoxConfig.topleft.x, MoveBoxConfig.topleft.y, MoveBoxConfig.bottomright.x, MoveBoxConfig.bottomright.y);
        SelectObject(dc, bOld);
        SelectObject(dc, pOld);

        DeleteObject(bkg);
        DeleteObject(borderpen);
        MoveBoxConfig.dirty = false;
    }
}


void InitTriangle() {
    POINT Centre;

    Triangle.color = TriangleBorderColor;
    Triangle.backgroundColor = TriangleBackgroupndColor;
    Triangle.thickness = 3;
    Triangle.visible = true;


    Centre.x = (MoveBoxConfig.topleft.x + MoveBoxConfig.bottomright.x) / 2;
    Centre.y = (MoveBoxConfig.topleft.y + MoveBoxConfig.bottomright.y) / 2;

    Triangle.A.x = Centre.x - 200;
    Triangle.A.y = Centre.y - 50;

    Triangle.B.x = Centre.x + 200;
    Triangle.B.y = Centre.y + 50;

    Triangle.C.x = Centre.x - 60;
    Triangle.C.y = Centre.y + 150;


    Triangle.radA = LineLength(MainPoint, Triangle.A);
    Triangle.radB = LineLength(MainPoint, Triangle.B);
    Triangle.radC = LineLength(MainPoint, Triangle.C);

    Triangle.omegaA = BaseAngle(Triangle.A);
    Triangle.omegaB = BaseAngle(Triangle.B);
    Triangle.omegaC = BaseAngle(Triangle.C);

    Triangle.omega = 0;
}






void DrawTriangle(HDC dc) {
    RECT ClearBox;
    POINT PoligonPoints[4];
    //  if (!Circle.dirty) return;

    HBRUSH BKG = CreateSolidBrush(Triangle.backgroundColor);
    HBRUSH bOld = (HBRUSH)SelectObject(dc, BKG);
    HPEN pen = CreatePen(PS_SOLID, Triangle.thickness, Triangle.color);
    HPEN pOld = (HPEN)SelectObject(dc, pen);

    PoligonPoints[0].x = MainPoint.x + Triangle.radA * cos(GradToRadian(Triangle.omegaA + Triangle.omega));
    PoligonPoints[1].x = MainPoint.x + Triangle.radB * cos(GradToRadian(Triangle.omegaB + Triangle.omega));
    PoligonPoints[2].x = MainPoint.x + Triangle.radC * cos(GradToRadian(Triangle.omegaC + Triangle.omega));

    PoligonPoints[0].y = MainPoint.y + Triangle.radA * sin(GradToRadian(Triangle.omegaA + Triangle.omega));
    PoligonPoints[1].y = MainPoint.y + Triangle.radB * sin(GradToRadian(Triangle.omegaB + Triangle.omega));
    PoligonPoints[2].y = MainPoint.y + Triangle.radC * sin(GradToRadian(Triangle.omegaC + Triangle.omega));

    PoligonPoints[3] = PoligonPoints[0];

    if (Triangle.visible)
    {

        Polygon(dc, PoligonPoints, 4);

        Triangle.dirty = false;

        //            InvalidateRect(hWnd, &ClearBox, true);
    }

    SelectObject(dc, bOld);
    SelectObject(dc, pOld);
    DeleteObject(BKG);
    DeleteObject(pen);
}



void DrawSpicy(HDC dc) {
    RECT ClearBox;
    POINT PoligonPoints[4];
    HBRUSH BKG = CreateSolidBrush(Triangle.backgroundColor);
    HBRUSH bOld = (HBRUSH)SelectObject(dc, BKG);
    HPEN pen = CreatePen(PS_DOT, 1, RGB(10, 10, 10));
    HPEN pOld = (HPEN)SelectObject(dc, pen);

    PoligonPoints[0].x = MainPoint.x + Triangle.radA * cos(GradToRadian(Triangle.omegaA + Triangle.omega));
    PoligonPoints[1].x = MainPoint.x + Triangle.radB * cos(GradToRadian(Triangle.omegaB + Triangle.omega));
    PoligonPoints[2].x = MainPoint.x + Triangle.radC * cos(GradToRadian(Triangle.omegaC + Triangle.omega));

    PoligonPoints[0].y = MainPoint.y + Triangle.radA * sin(GradToRadian(Triangle.omegaA + Triangle.omega));
    PoligonPoints[1].y = MainPoint.y + Triangle.radB * sin(GradToRadian(Triangle.omegaB + Triangle.omega));
    PoligonPoints[2].y = MainPoint.y + Triangle.radC * sin(GradToRadian(Triangle.omegaC + Triangle.omega));

    //   PoligonPoints[3] = PoligonPoints[0];

    if (Triangle.visible)
    {

        Ellipse(dc, MainPoint.x - 2, MainPoint.y - 2, MainPoint.x + 2, MainPoint.y + 2);

        MoveToEx(dc, MainPoint.x, MainPoint.y, NULL);
        LineTo(dc, PoligonPoints[0].x, PoligonPoints[0].y);

        MoveToEx(dc, MainPoint.x, MainPoint.y, NULL);
        LineTo(dc, PoligonPoints[1].x, PoligonPoints[1].y);

        MoveToEx(dc, MainPoint.x, MainPoint.y, NULL);
        LineTo(dc, PoligonPoints[2].x, PoligonPoints[2].y);


        // Triangle.dirty = false;

         //            InvalidateRect(hWnd, &ClearBox, true);
    }

    SelectObject(dc, bOld);
    SelectObject(dc, pOld);
    DeleteObject(BKG);
    DeleteObject(pen);
}





void ClearTriangle(HDC dc)
{
    HBRUSH bkg = CreateSolidBrush(backgroundcolor);
    HBRUSH bOld = (HBRUSH)SelectObject(dc, bkg);
    HPEN pen = CreatePen(PS_SOLID, Triangle.thickness, backgroundcolor);
    HPEN pOld = (HPEN)SelectObject(dc, pen);

    POINT PoligonPoints[4];


    PoligonPoints[0] = Triangle.A;
    PoligonPoints[1] = Triangle.B;
    PoligonPoints[2] = Triangle.C;
    PoligonPoints[3] = Triangle.A;


    if (Triangle.visible)
    {
        Polygon(dc, PoligonPoints, 4);
    }
    SelectObject(dc, bOld);
    SelectObject(dc, pOld);
    DeleteObject(bkg);
    DeleteObject(pen);
}






#define MAX_LOADSTRING 100

// Глобальные переменные:
HINSTANCE hInst;                                // текущий экземпляр
WCHAR szTitle[MAX_LOADSTRING];                  // Текст строки заголовка
WCHAR szWindowClass[MAX_LOADSTRING];            // имя класса главного окна

// Отправить объявления функций, включенных в этот модуль кода:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: Разместите код здесь.

    // Инициализация глобальных строк
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_ROTATINGTRIANGLE, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Выполнить инициализацию приложения:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_ROTATINGTRIANGLE));

    MSG msg;

    // Цикл основного сообщения:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}



//
//  ФУНКЦИЯ: MyRegisterClass()
//
//  ЦЕЛЬ: Регистрирует класс окна.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ROTATINGTRIANGLE));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_ROTATINGTRIANGLE);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   ФУНКЦИЯ: InitInstance(HINSTANCE, int)
//
//   ЦЕЛЬ: Сохраняет маркер экземпляра и создает главное окно
//
//   КОММЕНТАРИИ:
//
//        В этой функции маркер экземпляра сохраняется в глобальной переменной, а также
//        создается и выводится главное окно программы.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // Сохранить маркер экземпляра в глобальной переменной

   hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }


   InitMovingBox();
   MainPoint.x = 620; // DEFAULTMAINPOINT_X;
   MainPoint.y = 400; // DEFAULTMAINPOINT_Y;
   Moving.vector = Left;
   Moving.speed = 1;
   InitTriangle();
   SetTimer(hWnd,             // handle to main window 
       NULL,            // timer identifier 
       100,                 // 100-milisecond interval 
       (TIMERPROC)NULL);


   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  ФУНКЦИЯ: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  ЦЕЛЬ: Обрабатывает сообщения в главном окне.
//
//  WM_COMMAND  - обработать меню приложения
//  WM_PAINT    - Отрисовка главного окна
//  WM_DESTROY  - отправить сообщение о выходе и вернуться
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_COMMAND:
    {
        int wmId = LOWORD(wParam);
        // Разобрать выбор в меню:
        switch (wmId)
        {
        case IDM_ABOUT:
            DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
            break;
        case IDM_EXIT:
            DestroyWindow(hWnd);
            break;
        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
        }
    }
    break;
    case WM_TIMER:
        Move();
        InvalidateRect(hWnd, NULL, true);
        MoveBoxConfig.dirty = true;
        return 0;
    case WM_KEYDOWN:
        switch (wParam)
        {
        case VK_LEFT:
            Moving.vector = Left;
            Move();
            InvalidateRect(hWnd, NULL, true);
            MoveBoxConfig.dirty = true;

            break;

        case VK_RIGHT:
            Moving.vector = Left;
            Move();
            InvalidateRect(hWnd, NULL, true);
            MoveBoxConfig.dirty = true;

            break;

        case  VK_ESCAPE:
        {
            DestroyWindow(hWnd);
            break;
        }
        break;

        }
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);
        // TODO: Добавьте сюда любой код прорисовки, использующий HDC...

        InitMovingBox();
        DrawMovingBox(hdc);
        DrawSpicy(hdc);
        //           ClearCircle(hdc);
        //           ResizeCircle();
        //UpdateTriangle();
        DrawTriangle(hdc);



        EndPaint(hWnd, &ps);
    }
    break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}
// Обработчик сообщений для окна "О программе".
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}
