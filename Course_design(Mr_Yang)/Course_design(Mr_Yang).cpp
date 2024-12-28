#define _CRT_NONSTDC_NO_DEPRECATE
#define _CRT_SECURE_NO_WARNINGS
#include<stdio.h>
#include<graphics.h>
#include<string.h>
#include<conio.h>
#include<stdlib.h>
#include <time.h>
#pragma comment(lib, "imm32.lib")

#define Max 100    //max Space
#define MAX_USERNAME_LENGTH 20
#define MAX_PASSWORD_LENGTH 20
#define MAX_PASSWORD_BAK_LENGTH 20
#define MaxQSize 20						//QueryMax
#define MaxHB 20						//Ticketnumber

//------------------------------ColorDefine---------------------------------------//
const COLORREF BACKGROUND_COLOR = RGB(255, 255, 255);  //白色背景色
const COLORREF TITLE_COLOR = RGB(31, 41, 55);         // 深蓝灰色标题  
const COLORREF TEXT_COLOR = RGB(55, 65, 81);          // 深灰色文本  
const COLORREF BUTTON_COLOR = RGB(59, 130, 246);      // 蓝色按钮  
const COLORREF BUTTON_TEXT_COLOR = RGB(0, 0, 0);      // 黑色按钮文本  
const COLORREF INPUT_BG_COLOR = RGB(255, 255, 255);   // 白色输入框背景  
const COLORREF INPUT_BORDER_COLOR = RGB(209, 213, 219); // 灰色输入框边框  
const COLORREF SUCCESS_COLOR = RGB(34, 197, 94);      // 绿色成功提示  
const COLORREF ERROR_COLOR = RGB(239, 68, 68);        // 红色错误提示

//------------------------------ENUM---------------------------------------//
typedef enum UIState
{
    LoginMenu,
    RegisterMenu,
    MainPage,
    WeekdayTicket,
    QueryTicket
}UIState;

//-------------------------------Data--------------------------------------//
const int Base1 = 13;           //进制数
const int Mod1 = 1e7 + 19;      //取余
int hash1[Max];                 //哈希表
UIState currentState;

bool cursorVisible = true;      //是否闪烁
int cursorPos = 0;              //点击位置
DWORD lastCursorTime = GetTickCount();      //获取当前系统以来的毫秒数
char username[MAX_USERNAME_LENGTH + 1] = "";
char password[MAX_PASSWORD_LENGTH + 1] = "";
char password_bak[MAX_PASSWORD_BAK_LENGTH + 1] = "";
char flightnumber[MAX_PASSWORD_LENGTH + 1] = "";

//-------------------------------global_Page----------------------------//
int flag = 0;
int currentPage = 1;
int itemsPerPage = 6;
int weekday = -1;
bool exitInnerLoop = false;  // 新增标志变量，用于跳出内层循环
bool canPass = false;				 // 是否可以购买
//------------------------------Clock---------------------------------------//			//新增
char currentTimeStr[64] = { 0 };
time_t now = time(&now);
struct tm* localTime = localtime(&now), lastlocalTime = *localtime(&now);
//-------------------------------struct--------------------------------------//
//-------------------------------UserData------------------------------------//
typedef struct User
{
    char UserName[Max];
    int hashName;
    char UserPassword[Max];
    int hashPassword;
}User;

typedef struct UserData
{
    User data;
    struct UserData* list;
}UserData;
//--------------------------------Plane---------------------------------------//
typedef struct hangbandata {
    int num;      // 航班号
    int weekday;    // 星期几
    int qfweek;		//起飞星期
    int qfhour;		//起飞小时
    int qfmin;		//起飞分钟
    int jlweek;		//降落星期
    int jlhour;		//降落小时
    int jlmin;		//降落分钟
    int island;		//冗余，占位符
    int where;  // 1为荆门飞北京 2为北京飞荆门 3为荆门飞武汉，4为武汉飞荆门
    int piaoshu;
    int zhuangtai; //状态,1为即将起飞，2为未起飞，默认为0
} *Data;

typedef struct TreeNode {
    Data data;
    struct TreeNode* left;
    struct TreeNode* right;
    int ltag;
    int rtag;
} *Node;

typedef struct  ArrayList {
    Data* array;   //实现顺序表的底层数组
    int capacity;   //表示底层数组的容量
    int size;
} *List;
//-----------------------------------BuyTicket---------------------------------//
typedef struct TicketData
{
    int num;
}TicketData;

typedef struct Booking//订票结构体
{
    TicketData T[MaxQSize];
    int Len;
}Booking;

typedef struct Alternate//候补结构体
{
    int* base;//链表的顺序代表用户的标识
    int front;
    int rear;
    int Len;
}Alternate;

typedef struct AltArray
{
    Alternate Q[MaxHB];//保存每个
    int len;
}AltArray;
//----------------------------Debug-------------------------------------//
Node mainNode = NULL;
//------------------------------Function---------------------------------------//
void DrawGraph_Login();					                //Login Menu
void DrawGraph_Register();                              //Register Menu
void MouseClick_Login(int x, int y, UserData* D, Booking* Book, AltArray* Alt);	    //Processing mouse onclick
void MouseClick_Register(int x, int y, UserData* D);    //Processing mouse onclick
void KeyBoardOnClick_Login(char key, UserData* D, Booking* Book, AltArray* Alt);      //Processing keyborad onclick
void KeyBoardOnClick_Register(char key, UserData* D);
void Clo();
int Convert(char* ch);
//-----------------------------State_menchiene-------------------------------//
void SwitchState(UserData* D, Node xian, Booking* Book, AltArray* Alt);                   //Menu state
void LoginState(UserData* D, Booking* Book, AltArray* Alt);
void RegisterState(UserData* D);
void MainPageState(UserData* D, Booking* Book, AltArray* Alt, Node xian);
//-----------------------------UserData----------------------------------------//
void ReadUser(UserData* D);          //Read User Data
int UserMatching(UserData* D);       //Match Passage and UserName
int Encrypt(char* ch);               //Hash
void SaveUser(UserData* D);
void CinUser(UserData* B);
void AddUser(UserData* D);
//----------------------------Clock------------------------------------------//					//新增
void GetCurrenttime();
void UpdateCurrentTime();
void ShowTime();
//----------------------------MainPage------------------------------------------//
void MainPageState(UserData* D, Booking* Book, AltArray* Alt, Node xian);
void DrawGraph_MainPage(Node xian);                           //To Drawing MainPage
void MouseClick_MainPage(int x, int y, UserData* D, Booking* Book, AltArray* Alt);
void DrawInputBox(int x, int y, int width, int height, const TCHAR* label, const TCHAR* value);			//新增
void menuready(Node tree, int y);
//---------------------------WeekdayTicket-------------------------------------//
void WeekdayTicketState(UserData* D, Node xian, Booking* Book, AltArray* Alt);
void DrawGraph_WeekdayTicket(Node xian, Booking* Book, AltArray* Alt);
void MouseClick_WeekdayTicket(int x, int y, UserData* D, Node xian, Booking* Book, AltArray* Alt);
void weekseeinmenu(int x, int y, const char* from, const char* to, struct tm time1, struct tm time2, const char* text);
void weeksee(int x, int y, const char* from, const char* to, struct tm time1, struct tm time2, const char* text, int flightID, int ticketNum);
void displaytoday(Node tree, int weekday, int y, int page);
int buybuybuy(Node xian, int weekday, int choice, int page);
//---------------------------Binary_Tree---------------------------------------//
Node createNode(Data data);
void preorderThreaded(Node root);
Node insert(Node root, Data data);
void inorder(Node root);
Node copyNode(Node root);
Node copytree(Node root);
Node importDataFromFile(const char* filename);
Node finddatabynum(Node tree, int num);
//----------------------------Buy_Ticket---------------------------------------//
char* FindUserId(UserData* D, int id);
void InitQ(Alternate* Q);
void EnQueue(Alternate* Q, int e);
int DeQueue(Alternate* Q, int* e);
int GetHead(Alternate Q);
void QuitQueue(Alternate* Q, int id);
void PrintQueue(Alternate Q);
void ReadAlt(AltArray* Alt);
void SaveAlt(AltArray* Alt);
void AddQueu(AltArray* Alt, int num, int id);
void ReadData(Booking* Book, char* ch);
void SaveData(Booking* Book, char* ch);
void SaveDatatotree(Node xian);
void AddDataBook(Booking* Book, int num);			      //Add Ticket
void DelDataBook(UserData* D, Booking* Book, int num, AltArray* Alt, Node xian);  //Delete Ticket
void PrinT(Booking* Alt);
char* FindUserId(UserData* D, int id);
//---------------------------Query_Ticket--------------------------------------//
void QueryTicketState(Booking* book, Node tree, UserData* D, AltArray* Alt);
void DrawGraph_QueryTicket(Booking* book);
void MouseClick_QueryTicket(int x, int y, Booking* book, Node tree, UserData* D, AltArray* Alt);
void KeyBoardOnClick_QueryTicket(char key, Booking* book);

int Partition(int stu[], int low, int high);
void QSort(int stu[], int low, int high);
void fun2(int stu[], int n);
int FindDataBook(Booking* Book, int num);
void QueryTicket_Data(Node tree, int y);
void QuitTicket(int x, int y, const char* from, const char* to, struct tm time1, struct tm time2, const char* text);
//-------------------------------Function--------------------------------------//


void main()
{
    //Init User Data
    UserData D;
    Booking Book;
    AltArray Alt;
    D.list = NULL;
    ReadUser(&D);
    //Init Plane Data
    Node tree = NULL;
    tree = importDataFromFile("a.txt");

    currentState = LoginMenu;
    //Init Graph
    initgraph(1080, 640, NOCLOSE);
    setbkcolor(RGB(65, 165, 238));
    setbkmode(TRANSPARENT);
    cleardevice();

    Node xian = copytree(tree);
    preorderThreaded(xian);
    while (1)
    {
        SwitchState(&D, xian, &Book, &Alt);
        Sleep(10);
    }
}

//----------------------------------Graph-------------------------------------//
//更新当前时间函数																		//新增
void UpdateCurrentTime()
{
    GetCurrenttime();
    DrawInputBox(100, 600, 150, 25, "当前时间:", currentTimeStr);
}

// 获取当前时间的函数	
void GetCurrenttime()															//新增
{
    // 格式化时间字符串  
    _snprintf_s(currentTimeStr, 64, _TRUNCATE, "%04d/%02d/%02d %02d:%02d:%02d",
        lastlocalTime.tm_year + 1900,  //年份（从1900开始）  
        lastlocalTime.tm_mon + 1,      //月份（0-11）  
        lastlocalTime.tm_mday,         //日  
        lastlocalTime.tm_hour,         //小时  
        lastlocalTime.tm_min,          //分钟
        lastlocalTime.tm_sec          // 秒钟  
    );
}

//以一秒为间隔绘制系统时钟
void ShowTime()																		//新增
{
    now = time(&now);
    localTime = localtime(&now);
    UpdateCurrentTime();
    //printf("%d\t%d\n",localTime->tm_sec, lastlocalTime.tm_sec);
    if (localTime->tm_sec - lastlocalTime.tm_sec >= 1 || lastlocalTime.tm_sec == 59)
    {
        lastlocalTime = *localTime;
        UpdateCurrentTime();
    }
}

// 绘制输入框函数  
void DrawInputBox(int x, int y, int width, int height, const TCHAR* label, const TCHAR* value) {		//新增
    // 绘制标签  
    settextcolor(TEXT_COLOR);
    settextstyle(20, 0, _T("微软雅黑"));
    outtextxy(x - textwidth(label) - 10, y + (height - textheight(label)) / 2, label);

    // 绘制输入框  
    setfillcolor(INPUT_BG_COLOR);
    setlinecolor(INPUT_BORDER_COLOR);
    fillrectangle(x, y, x + width, y + height);

    // 绘制输入的文本  
    if (value && _tcslen(value) > 0) {
        settextcolor(TEXT_COLOR);
        outtextxy(x + 5, y + (height - textheight(value)) / 2, value);
    }
}

void DrawGraph_Login()
{
    BeginBatchDraw();
    setbkcolor(RGB(65, 165, 238));
    cleardevice();

    setfillcolor(RGB(255, 255, 255));
    fillrectangle(0, 214, 1080, 640);

    IMAGE img;			//初始化logo
    loadimage(&img, "./logo.jpg", 90, 90);
    putimage(213, 80, &img);
    //以一秒为间隔绘制系统时钟													//新增
    ShowTime();
    settextcolor(RGB(255, 255, 255));
    char arr[] = "荆门爱飞客订票系统";
    settextstyle(90, 20, "华文新魏");
    outtextxy(213 + 90, 85, arr);
    //绘制输入框
    settextstyle(0, 0, "华文新魏");
    settextcolor(BLACK);
    setlinecolor(RGB(102, 102, 102));
    fillrectangle(350, 250, 700, 250 + 30);
    outtextxy(250, 250, "用户名:");
    fillrectangle(350, 350, 700, 350 + 30);
    outtextxy(250, 350, "密码:");
    //登入与退出
    setfillcolor(RGB(42, 194, 212));
    settextcolor(RGB(255, 255, 255));
    fillrectangle(400, 450, 400 + 250, 450 + 50);
    char tmp[] = "登出";
    int wight = (250 - textwidth(tmp)) / 2;
    int height = (50 - textheight(tmp)) / 2;
    outtextxy(400 + wight, 450 + height, "登入");
    setfillcolor(RGB(255, 255, 255));
    settextcolor(RED);
    fillrectangle(400, 550, 400 + 250, 550 + 50);
    outtextxy(400 + wight, 550 + height, "退出");
    //绘制用户注册
    settextcolor(RGB(65, 165, 238));
    outtextxy(630, 390, "注册账号");

    //绘制用户输入内容
    settextcolor(BLACK);
    outtextxy(355, 255, username);
    TCHAR passwordDisplay[MAX_PASSWORD_LENGTH + 1] = "";
    for (int i = 0; i < strlen(password); i++) {
        passwordDisplay[i] = '*';
    }
    passwordDisplay[strlen(password)] = '\0';
    outtextxy(355, 355, passwordDisplay);

    if (cursorVisible)
    {
        if (cursorPos == 0)
        {
            line(360 + textwidth(username), 250, 360 + textwidth(username), 280);
        }
        else
        {
            line(360 + textwidth(password), 350, 360 + textwidth(password), 380);
        }
    }
    EndBatchDraw();
}

void DrawGraph_Register()
{
    BeginBatchDraw();
    setbkcolor(RGB(255, 255, 255));
    cleardevice();
    setfillcolor(RGB(65, 165, 238));
    fillrectangle(0, 0, 1080, 124);

    setfillcolor(RGB(255, 255, 255));
    setlinecolor(RGB(0, 0, 0));
    settextcolor(RGB(0, 0, 0));
    outtextxy(270, 210, "用户名: ");
    fillrectangle(350, 200, 350 + 350, 200 + 30);
    outtextxy(270, 290, "密码: ");
    fillrectangle(350, 280, 350 + 350, 280 + 30);
    outtextxy(270, 370, "确认密码: ");
    fillrectangle(350, 360, 350 + 350, 360 + 30);
    //绘制文字输入
    settextcolor(BLACK);
    outtextxy(355, 205, username);
    outtextxy(355, 285, password);
    outtextxy(355, 365, password_bak);
    //以一秒为间隔绘制系统时钟													//新增
    ShowTime();
    if (cursorVisible)
    {
        if (cursorPos == 0)
        {
            line(360 + textwidth(username), 200, 360 + textwidth(username), 230);
        }
        else if (cursorPos == 1)
        {
            line(360 + textwidth(password), 280, 360 + textwidth(password), 310);
        }
        else
        {
            line(360 + textwidth(password_bak), 360, 360 + textwidth(password_bak), 390);
        }
    }
    //注册与返回
    setfillcolor(RGB(42, 194, 212));
    settextcolor(RGB(255, 255, 255));
    fillrectangle(400, 450, 400 + 250, 450 + 50);
    char tmp[] = "注册";
    int wight = (250 - textwidth(tmp)) / 2;
    int height = (50 - textheight(tmp)) / 2;
    outtextxy(400 + wight, 450 + height, "注册");
    setfillcolor(RGB(255, 255, 255));
    settextcolor(RED);
    fillrectangle(400, 550, 400 + 250, 550 + 50);
    outtextxy(400 + wight, 550 + height, "返回");
    EndBatchDraw();
}
void DrawGraph_MainPage(Node xian)
{
    setbkcolor(RGB(255, 255, 255));
    BeginBatchDraw();
    cleardevice();

    //主界面绘制
    settextcolor(BLACK);
    settextstyle(50, 0, "华文新魏");
    outtextxy(300, 50, "荆门爱飞客运航班管理");
    setfillcolor(RGB(65, 165, 238));
    fillrectangle(0, 100, 1080, 640);

    settextstyle(0, 0, "黑体");
    setfillcolor(RGB(255, 255, 255));
    fillroundrect(30, 150, 220, 200, 10, 10);
    char tmp[] = "查看本周航班信息";
    int nwidth = (190 - textwidth(tmp)) / 2;
    int nheight = (50 - textheight(tmp)) / 2;
    outtextxy(30 + nwidth, 150 + nheight, "查看本周航班信息");
    fillroundrect(30, 240, 220, 290, 10, 10);
    outtextxy(30 + nwidth, 240 + nheight, "查看本人机票");
    fillroundrect(30, 330, 220, 380, 10, 10);
    outtextxy(30 + nwidth, 330 + nheight, "退出登录");
    //以一秒为间隔绘制系统时钟													//新增
    ShowTime();
    //fillroundrect(30, 420, 220, 470, 10, 10);
    //outtextxy(30 + nwidth, 420 + nheight, "查看本人机票");
    //fillroundrect(30, 510, 220, 560, 10, 10);
    //outtextxy(30 + nwidth, 510 + nheight, "退出登录");
    menuready(xian, 150);
    setfillstyle(PS_SOLID, 5);
    setlinecolor(RGB(54, 54, 54));
    line(280, 140, 280, 600);
    FlushBatchDraw();
}
void DrawGraph_WeekdayTicket(Node xian, Booking* Book, AltArray* Alt)
{
    BeginBatchDraw();
    cleardevice();

    //绘制星期按钮
    char ch[] = "星期一";
    int nwidth = (190 - textwidth(ch)) / 2;
    int nheight = (50 - textheight(ch)) / 2;
    setfillcolor(RGB(255, 255, 255));
    settextcolor(RGB(0, 0, 0));
    fillroundrect(50, 50, 250, 100, 10, 10);
    outtextxy(50 + nwidth, 50 + nheight, "星期一");
    fillroundrect(50, 120, 250, 170, 10, 10);
    outtextxy(50 + nwidth, 120 + nheight, "星期二");
    fillroundrect(50, 190, 250, 240, 10, 10);
    outtextxy(50 + nwidth, 190 + nheight, "星期三");
    fillroundrect(50, 260, 250, 310, 10, 10);
    outtextxy(50 + nwidth, 260 + nheight, "星期四");
    fillroundrect(50, 330, 250, 380, 10, 10);
    outtextxy(50 + nwidth, 330 + nheight, "星期五");
    fillroundrect(50, 400, 250, 450, 10, 10);
    outtextxy(50 + nwidth, 400 + nheight, "星期六");
    fillroundrect(50, 470, 250, 520, 10, 10);
    outtextxy(50 + nwidth, 470 + nheight, "星期日");
    // 绘制返回按钮
    fillroundrect(50, 540, 250, 590, 10, 10);
    outtextxy(50 + nwidth, 540 + nheight, "返回");
    //绘制翻页按钮
    nwidth = (100 - textwidth(ch)) / 2;
    nheight = (50 - textheight(ch)) / 2;
    fillroundrect(400, 540, 500, 590, 10, 10);
    outtextxy(400 + nwidth, 540 + nheight, "上一页");
    fillroundrect(800, 540, 900, 590, 10, 10);
    outtextxy(800 + nwidth, 540 + nheight, "下一页");
    // Display page number
    settextcolor(BLACK);
    settextstyle(20, 10, "华文新魏");
    char pageStr[10];
    sprintf_s(pageStr, "%d", currentPage);
    outtextxy(600, 540, pageStr);
    outtextxy(620, 540, "/");
    outtextxy(640, 540, "3");
    //以一秒为间隔绘制系统时钟													//新增
    ShowTime();
    //TODD 清除区域
    if (weekday != -1) {
        clearrectangle(400, 50, 1080, 530);  // Clear data area, not buttons
        displaytoday(xian, weekday, 60, currentPage);  // Display the selected weekday flights
    }

    EndBatchDraw();
}

void DrawGraph_QueryTicket(Booking* book)
{
    setbkcolor(RGB(255, 255, 255));
    BeginBatchDraw();
    cleardevice();

    setlinestyle(PS_SOLID, 3);
    line(20, 20, 50, 20);
    line(20, 20, 30, 10);
    line(20, 20, 30, 30);
    //分割线
    setlinestyle(PS_SOLID, 1);
    line(300, 50, 300, 600);
    //绘制输入框
    settextstyle(0, 0, "黑体");
    settextcolor(RGB(0, 0, 0));
    outtextxy(50, 100, "航班号");
    setlinecolor(RGB(0, 0, 0));
    setfillcolor(RGB(255, 255, 255));
    fillrectangle(50, 130, 280, 160);
    //绘制"查询"
    setfillcolor(RGB(65, 165, 238));
    fillrectangle(50, 500, 280, 550);
    settextcolor(RGB(255, 255, 255));
    outtextxy(140, 515, "查询");
    //绘制用户输入内容
    settextcolor(BLACK);
    outtextxy(50, 130, flightnumber);
    //以一秒为间隔绘制系统时钟													//新增
    ShowTime();
    if (cursorVisible)
    {
        line(55 + textwidth(flightnumber), 130, 55 + textwidth(flightnumber), 160);
    }
    if (mainNode != NULL)
    {
        QueryTicket_Data(mainNode, 60);
    }
    EndBatchDraw();
}

//---------------------------------UIState------------------------------------//
void SwitchState(UserData* D, Node xian, Booking* Book, AltArray* Alt)
{
    switch (currentState)
    {
    case LoginMenu:
        LoginState(D, Book, Alt);
        break;
    case RegisterMenu:
        RegisterState(D);
        break;
    case MainPage:
        MainPageState(D, Book, Alt, xian);
        break;
    case WeekdayTicket:
        WeekdayTicketState(D, xian, Book, Alt);
        break;
    case QueryTicket:
        QueryTicketState(Book, xian, D, Alt);
        break;
    default:break;
    }
}

void LoginState(UserData* D, Booking* Book, AltArray* Alt)
{
    DrawGraph_Login();

    // 处理用户输入
    if (_kbhit()) {
        char key = _getch();
        KeyBoardOnClick_Login(key, D, Book, Alt);
    }

    if (GetTickCount() - lastCursorTime >= 500)
    {
        cursorVisible = !cursorVisible;
        lastCursorTime = GetTickCount();
    }

    if (MouseHit())
    {
        MOUSEMSG msg = GetMouseMsg();
        if (msg.uMsg == WM_LBUTTONDOWN)
        {
            MouseClick_Login(msg.x, msg.y, D, Book, Alt);
        }
    }

}
void RegisterState(UserData* D)
{
    DrawGraph_Register();

    // 处理用户输入
    if (_kbhit()) {
        char key = _getch();
        KeyBoardOnClick_Register(key, D);
    }

    if (GetTickCount() - lastCursorTime >= 500)
    {
        cursorVisible = !cursorVisible;
        lastCursorTime = GetTickCount();
    }

    if (MouseHit())
    {
        MOUSEMSG msg = GetMouseMsg();
        if (msg.uMsg == WM_LBUTTONDOWN)
        {
            MouseClick_Register(msg.x, msg.y, D);
        }
    }
}

void MainPageState(UserData* D, Booking* Book, AltArray* Alt, Node xian)
{
    DrawGraph_MainPage(xian);

    if (MouseHit())
    {
        MOUSEMSG msg = GetMouseMsg();
        if (msg.uMsg == WM_LBUTTONDOWN)
        {
            MouseClick_MainPage(msg.x, msg.y, D, Book, Alt);
        }
    }
}

void WeekdayTicketState(UserData* D, Node xian, Booking* Book, AltArray* Alt)
{
    DrawGraph_WeekdayTicket(xian, Book, Alt);

    if (MouseHit())
    {
        MOUSEMSG msg = GetMouseMsg();
        if (msg.uMsg == WM_LBUTTONDOWN)
        {
            MouseClick_WeekdayTicket(msg.x, msg.y, D, xian, Book, Alt);
        }
    }
}
void QueryTicketState(Booking* book, Node tree, UserData* D, AltArray* Alt)
{
    DrawGraph_QueryTicket(book);

    if (MouseHit())
    {
        MOUSEMSG msg = GetMouseMsg();
        if (msg.uMsg == WM_LBUTTONDOWN)
        {
            MouseClick_QueryTicket(msg.x, msg.y, book, tree, D, Alt);
        }
    }

    if (_kbhit()) {
        char key = _getch();
        KeyBoardOnClick_QueryTicket(key, book);
    }

    if (GetTickCount() - lastCursorTime >= 500)
    {
        cursorVisible = !cursorVisible;
        lastCursorTime = GetTickCount();
    }
}
//-----------------------------------OnClick---------------------------------//
//-----------------------------------Mouse-----------------------------------//

void Clo()
{
    int i, l1, l2, l3, l4;
    l1 = strlen(username);
    l2 = strlen(password);
    l3 = strlen(password_bak);
    l4 = strlen(flightnumber);
    for (i = 0;i < l1;i++)
    {
        username[i] = 0;
    }

    for (i = 0;i < l2;i++)
    {
        password[i] = 0;
    }
    for (i = 0;i < l3; i++)
    {
        password_bak[i] = 0;
    }
    for (i = 0;i < l4; i++)
    {
        flightnumber[i] = 0;
    }
}

void MouseClick_Login(int x, int y, UserData* D, Booking* Book, AltArray* Alt)
{
    if (x >= 350 && x <= 700 && y >= 250 && y <= 280)
    {
        cursorPos = 0;
    }
    else if (x >= 350 && x <= 700 && y >= 350 && y <= 380)
    {
        cursorPos = 1;
    }

    if (x >= 400 && x <= 650 && y >= 450 && y <= 500)   //Login
    {
        printf("%d", UserMatching(D));
        if (UserMatching(D) == 1)
        {
            MessageBox(GetHWnd(), "登录成功！", "提示", MB_OK);
            cursorPos = 0;
            ReadData(Book, username);
            ReadAlt(Alt);
            currentState = MainPage;
        }
        else
        {
            MessageBox(GetHWnd(), "用户名或密码错误！", "错误", MB_OK);
        }
    }
    if (x >= 400 && x <= 650 && y >= 550 && y <= 600)   //Exit
    {
        exit(0);
    }
    if (x >= 630 && x <= 705 && y >= 390 && y <= 410)
    {
        currentState = RegisterMenu;
        cursorPos = 0;
        Clo();
    }
}

void MouseClick_Register(int x, int y, UserData* D)
{
    if (x >= 350 && x <= 700 && y >= 200 && y <= 230)
    {
        cursorPos = 0;
    }
    else if (x >= 350 && x <= 700 && y >= 280 && y <= 310)
    {
        cursorPos = 1;
    }
    else if (x >= 350 && x <= 700 && y >= 360 && y <= 390)
    {
        cursorPos = 2;
    }

    if (x >= 400 && x <= 650 && y >= 450 && y <= 500)   //Register
    {
        if (UserMatching(D) == -1 || UserMatching(D) == 1)
        {
            MessageBox(GetHWnd(), "用户名已存在", "错误", MB_OK);
        }
        else if (strcmp(password, password_bak) == 0 && strlen(password) >= 5 && strlen(username) >= 5)
        {
            AddUser(D);
            MessageBox(GetHWnd(), "注册成功！", "提示", MB_OK);
        }
        else
        {
            MessageBox(GetHWnd(), "前后密码不一致！", "错误", MB_OK);
        }
    }
    if (x >= 400 && x <= 650 && y >= 550 && y <= 600)       // ReturnHome
    {
        currentState = LoginMenu;
        cursorPos = 0;
        SaveUser(D);
        Clo();
    }
}
void MouseClick_MainPage(int x, int y, UserData* D, Booking* Book, AltArray* Alt)
{
    if (x >= 30 && x <= 220 && y >= 150 && y <= 200)
    {
        printf("查询本周航班\n");
        currentState = WeekdayTicket;
    }
    if (x >= 30 && x <= 220 && y >= 240 && y <= 290)
    {
        printf("查看本人机票\n");
        currentState = QueryTicket;
        //printf("订票\n");
    }
    if (x >= 30 && x <= 220 && y >= 330 && y <= 380)
    {
        printf("退出登录\n");
        puts(username);
        SaveData(Book, username);
        SaveAlt(Alt);
        Clo();
        currentState = LoginMenu;
        //printf("退票\n");
    }
    /*if (x >= 30 && x <= 220 && y >= 420 && y <= 470)
    {
        printf("查看本人机票\n");
        currentState = QueryTicket;
    }
    if (x >= 30 && x <= 220 && y >= 510 && y <= 560)
    {
        printf("退出登录\n");
        puts(username);
        SaveData(Book, username);
        SaveAlt(Alt);
        Clo();
        currentState = LoginMenu;
    }*/
}
void MouseClick_WeekdayTicket(int x, int y, UserData* D, Node xian, Booking* Book, AltArray* Alt)
{
    Node curNode;
    curNode = xian;
    if (x >= 50 && x <= 250)
    {
        if (y >= 50 && y <= 100)
        {
            weekday = 1;
        }
        if (y >= 120 && y <= 170)
        {
            weekday = 2;
        }
        if (y >= 190 && y <= 240)
        {
            weekday = 3;
        }
        if (y >= 260 && y <= 310)
        {
            weekday = 4;
        }
        if (y >= 330 && y <= 380)
        {
            weekday = 5;
        }
        if (y >= 400 && y <= 450)
        {
            weekday = 6;
        }
        if (y >= 470 && y <= 520)
        {
            weekday = 0;
        }
    }
    if (x >= 50 && x <= 250 && y >= 540 && y <= 590)
    {
        printf("返回\n");
        //重置
        canPass = false;
        int flag = 0;
        int currentPage = 1;
        int itemsPerPage = 6;
        int weekday = -1;
        currentState = MainPage;
    }
    if (x >= 400 && x <= 500 && y >= 540 && y <= 590 && currentPage > 1)
    {
        currentPage--;
        printf("上一页\n");
    }
    if (x >= 800 && x <= 900 && y >= 540 && y <= 590 && currentPage < 3)
    {
        currentPage++;
        printf("下一页\n");
    }
    //购票交互
    if (x >= 1000 && x <= 1050 && canPass)
    {
        if (y >= 60 && y <= 110)
        {   //传参进第一个选项
            int hangban = buybuybuy(xian, weekday, 1, currentPage);
            printf("FindDataBook(Book, hangban)%d\n", FindDataBook(Book, hangban));
            if (FindDataBook(Book, hangban) != -1)
            {
                MessageBox(GetHWnd(), "你已经购买当前机票！", "警报", MB_OK);
            }
            else
            {
                curNode = finddatabynum(xian, hangban);
                if (curNode->data->piaoshu != 0)
                {
                    MessageBox(GetHWnd(), "购买成功！", "提示", MB_OK);
                    AddDataBook(Book, hangban);			//add buy ticket data
                    printf("curNode->data->piaoshu:%d\n", curNode->data->piaoshu);
                    curNode->data->piaoshu--;
                    SaveDatatotree(xian);
                }
                else
                {
                    MessageBox(GetHWnd(), "候补成功！", "提示", MB_OK);
                }
            }

        }
        if (y >= 140 && y <= 190)
        {
            int hangban = buybuybuy(xian, weekday, 2, currentPage);
            printf("FindDataBook(Book, hangban)%d\n", FindDataBook(Book, hangban));
            if (FindDataBook(Book, hangban) != -1)
            {
                MessageBox(GetHWnd(), "你已经购买当前机票！", "警报", MB_OK);
            }
            else
            {
                curNode = finddatabynum(xian, hangban);
                if (curNode->data->piaoshu != 0)
                {
                    MessageBox(GetHWnd(), "购买成功！", "提示", MB_OK);
                    AddDataBook(Book, hangban);			//add buy ticket data
                    printf("curNode->data->piaoshu:%d\n", curNode->data->piaoshu);
                    curNode->data->piaoshu--;
                    SaveDatatotree(xian);
                }
                else
                {
                    MessageBox(GetHWnd(), "候补成功！", "提示", MB_OK);
                }
            }
        }
        if (y >= 220 && y <= 270)
        {
            int hangban = buybuybuy(xian, weekday, 3, currentPage);
            printf("FindDataBook(Book, hangban)%d\n", FindDataBook(Book, hangban));
            if (FindDataBook(Book, hangban) != -1)
            {
                MessageBox(GetHWnd(), "你已经购买当前机票！", "警报", MB_OK);
            }
            else
            {
                curNode = finddatabynum(xian, hangban);
                if (curNode->data->piaoshu != 0)
                {
                    MessageBox(GetHWnd(), "购买成功！", "提示", MB_OK);
                    AddDataBook(Book, hangban);			//add buy ticket data
                    printf("curNode->data->piaoshu:%d\n", curNode->data->piaoshu);
                    curNode->data->piaoshu--;
                    SaveDatatotree(xian);
                }
                else
                {
                    MessageBox(GetHWnd(), "候补成功！", "提示", MB_OK);
                }
            }
        }
        if (y >= 300 && y <= 350)
        {
            int hangban = buybuybuy(xian, weekday, 4, currentPage);
            printf("FindDataBook(Book, hangban)%d\n", FindDataBook(Book, hangban));
            if (FindDataBook(Book, hangban) != -1)
            {
                MessageBox(GetHWnd(), "你已经购买当前机票！", "警报", MB_OK);
            }
            else
            {
                curNode = finddatabynum(xian, hangban);
                if (curNode->data->piaoshu != 0)
                {
                    MessageBox(GetHWnd(), "购买成功！", "提示", MB_OK);
                    AddDataBook(Book, hangban);			//add buy ticket data
                    printf("curNode->data->piaoshu:%d\n", curNode->data->piaoshu);
                    curNode->data->piaoshu--;
                    SaveDatatotree(xian);
                }
                else
                {
                    MessageBox(GetHWnd(), "候补成功！", "提示", MB_OK);
                }
            }
        }
        if (y >= 380 && y <= 430)
        {
            int hangban = buybuybuy(xian, weekday, 5, currentPage);
            printf("FindDataBook(Book, hangban)%d\n", FindDataBook(Book, hangban));
            if (FindDataBook(Book, hangban) != -1)
            {
                MessageBox(GetHWnd(), "你已经购买当前机票！", "警报", MB_OK);
            }
            else
            {
                curNode = finddatabynum(xian, hangban);
                if (curNode->data->piaoshu != 0)
                {
                    MessageBox(GetHWnd(), "购买成功！", "提示", MB_OK);
                    AddDataBook(Book, hangban);			//add buy ticket data
                    printf("curNode->data->piaoshu:%d\n", curNode->data->piaoshu);
                    curNode->data->piaoshu--;
                    SaveDatatotree(xian);
                }
                else
                {
                    MessageBox(GetHWnd(), "候补成功！", "提示", MB_OK);
                }
            }
        }
        if (y >= 460 && y <= 510)
        {
            int hangban = buybuybuy(xian, weekday, 6, currentPage);
            printf("FindDataBook(Book, hangban)%d\n", FindDataBook(Book, hangban));
            if (FindDataBook(Book, hangban) != -1)
            {
                MessageBox(GetHWnd(), "你已经购买当前机票！", "警报", MB_OK);
            }
            else
            {
                curNode = finddatabynum(xian, hangban);
                if (curNode->data->piaoshu != 0)
                {
                    MessageBox(GetHWnd(), "购买成功！", "提示", MB_OK);
                    AddDataBook(Book, hangban);			//add buy ticket data
                    printf("curNode->data->piaoshu:%d\n", curNode->data->piaoshu);
                    curNode->data->piaoshu--;
                    SaveDatatotree(xian);
                }
                else
                {
                    MessageBox(GetHWnd(), "候补成功！", "提示", MB_OK);
                }
            }
        }
    }
}
void MouseClick_QueryTicket(int x, int y, Booking* book, Node tree, UserData* D, AltArray* Alt)
{
    if (x >= 20 && x <= 50 && y >= 0 && y <= 30)
    {
        //返回
        currentState = MainPage;
        mainNode = NULL;
    }
    if (x >= 50 && x <= 280 && y >= 500 && y <= 550)
    {
        mainNode = NULL;
        //TODD:查询
        mainNode = finddatabynum(tree, Convert(flightnumber));
        ;
        if (mainNode == NULL || FindDataBook(book, Convert(flightnumber)) == -1)
        {
            mainNode = NULL;
            MessageBox(GetHWnd(), "未查询到当前航班的机票！", "警报", MB_OK);
        }
        else
        {
            clearrectangle(400, 50, 1080, 530);  // Clear data area, not buttons
            QueryTicket_Data(mainNode, 60);  // Display the selected weekday flights
        }
    }
    if (x >= 1000 && x <= 1050 && y >= 60 && y <= 110 && mainNode != NULL)
    {
        //返回
        printf("交互\n");
        if (mainNode != NULL)
        {
            DelDataBook(D, book, mainNode->data->num, Alt, tree);
            SaveDatatotree(tree);
        }
        mainNode = NULL;
    }

}

//-----------------------------------KeyBoard------------------------------------//
void KeyBoardOnClick_Login(char key, UserData* D, Booking* Book, AltArray* Alt)
{
    if (key == 13)
    {
        if (UserMatching(D) == 1)
        {
            MessageBox(GetHWnd(), "登录成功！", "提示", MB_OK);
            cursorPos = 0;
            ReadData(Book, username);
            ReadAlt(Alt);
            Clo();
            currentState = MainPage;

        }
        else
        {
            MessageBox(GetHWnd(), "用户名或密码错误！", "错误", MB_OK);
        }
    }

    else if (key == 8)
    {
        if (cursorPos == 0 && strlen(username) > 0)
        {
            username[strlen(username) - 1] = '\0';
        }
        else if (cursorPos == 1 && strlen(password) > 0)
        {
            password[strlen(password) - 1] = '\0';
        }
    }

    else if (key >= 32 && key <= 126) { // 可打印字符
        if (cursorPos == 0 && strlen(username) < MAX_USERNAME_LENGTH - 1) {
            username[strlen(username)] = key;
            username[strlen(username) + 1] = '\0';
        }
        else if (cursorPos == 1 && strlen(password) < MAX_PASSWORD_LENGTH - 1) {
            password[strlen(password)] = key;
            password[strlen(password) + 1] = '\0';
        }
    }
}
void KeyBoardOnClick_Register(char key, UserData* D)
{
    if (key == 13)
    {
        if (UserMatching(D) == -1)
        {
            MessageBox(GetHWnd(), "用户名已存在", "错误", MB_OK);
        }
        else if (strcmp(password, password_bak) == 0 && strlen(password) >= 5 && strlen(username) >= 5)
        {
            AddUser(D);
            MessageBox(GetHWnd(), "注册成功！", "提示", MB_OK);
        }
        else
        {
            MessageBox(GetHWnd(), "前后密码不一致！", "错误", MB_OK);
        }
    }

    else if (key == 8)
    {
        if (cursorPos == 0 && strlen(username) > 0)
        {
            username[strlen(username) - 1] = '\0';
        }
        else if (cursorPos == 1 && strlen(password) > 0)
        {
            password[strlen(password) - 1] = '\0';
        }
        else if (cursorPos == 2 && strlen(password_bak) > 0)
        {
            password_bak[strlen(password_bak) - 1] = '\0';
        }
    }

    else if (key >= 32 && key <= 126) { // 可打印字符
        if (cursorPos == 0 && strlen(username) < MAX_USERNAME_LENGTH - 1) {
            username[strlen(username)] = key;
            username[strlen(username) + 1] = '\0';
        }
        else if (cursorPos == 1 && strlen(password) < MAX_PASSWORD_LENGTH - 1) {
            password[strlen(password)] = key;
            password[strlen(password) + 1] = '\0';
        }
        else if (cursorPos == 2 && strlen(password_bak) < MAX_PASSWORD_BAK_LENGTH - 1) {
            password_bak[strlen(password_bak)] = key;
            password_bak[strlen(password_bak) + 1] = '\0';
        }
    }
}
void KeyBoardOnClick_QueryTicket(char key, Booking* book)
{
    if (key == 8)
    {
        if (cursorPos == 0 && strlen(flightnumber) > 0)
        {
            flightnumber[strlen(flightnumber) - 1] = '\0';
        }
    }

    else if (key >= 32 && key <= 126) { // 可打印字符
        if (cursorPos == 0 && strlen(flightnumber) < MAX_USERNAME_LENGTH - 1) {
            flightnumber[strlen(flightnumber)] = key;
            flightnumber[strlen(flightnumber) + 1] = '\0';
        }
    }
}
//-----------------------------------OnClick---------------------------------//


//----------------------------------UserData----------------------------------//
void ReadUser(UserData* D)
{
    FILE* file;
    fopen_s(&file, "User.txt", "r");//只读的方法
    if (fgetc(file) == -1)
    {
        fclose(file);
        return;
    }
    rewind(file);
    while (!feof(file))
    {
        D->list = (UserData*)malloc(sizeof(UserData));
        D = D->list;
        D->list = NULL;
        fgets(D->data.UserName, Max, file);
        fscanf_s(file, "%d\n", &D->data.hashName);
        fgets(D->data.UserPassword, Max, file);
        fscanf_s(file, "%d\n", &D->data.hashPassword);
        D->data.UserName[strlen(D->data.UserName) - 1] = 0;//覆盖末尾换行
        D->data.UserPassword[strlen(D->data.UserPassword) - 1] = 0;//覆盖末尾换行

        puts(D->data.UserName);
        puts(D->data.UserPassword);

    }
    if (fclose(file) != 0)
        printf("文件关闭失败");
}

int UserMatching(UserData* D)
{
    int hash1, hash2;
    UserData* B;
    int i = 0;
    hash1 = Encrypt(username);
    hash2 = Encrypt(password);
    B = D->list;
    while (B)
    {
        if (hash1 == B->data.hashName)
        {
            if (hash2 == B->data.hashPassword)
            {
                printf("匹配成功");
                return 1;
            }
            else
            {
                return -1;
            }
        }
        B = B->list;
    }
    return 0;
}
int Encrypt(char* ch)
{
    int i, l;
    char t;
    l = strlen(ch);
    hash1[0] = 0;
    t = '0';
    for (i = 0;i < l;i++)
    {
        hash1[i + 1] = (hash1[i] * Base1 % Mod1 + (ch[i] - t)) % Mod1;
    }
    return hash1[i];
}

void SaveUser(UserData* D)
{
    FILE* file;
    fopen_s(&file, "User.txt", "w");//写的方法打开
    D = D->list;
    while (D)
    {
        fputs(D->data.UserName, file);
        fputc('\n', file);
        fprintf(file, "%d\n", D->data.hashName);
        fputs(D->data.UserPassword, file);
        fputc('\n', file);
        fprintf(file, "%d", D->data.hashPassword);
        if ((D->list))
            fputc('\n', file);
        else
            break;
        D = D->list;
    }
}
void CinUser(UserData* B)
{
    B->data.hashName = Encrypt(username);
    B->data.hashPassword = Encrypt(password);
    strcpy_s(B->data.UserName, username);
    strcpy_s(B->data.UserPassword, password);
}

void AddUser(UserData* D)
{
    UserData* B;
    B = D;
    while (B->list)
    {
        B = B->list;
    }
    B->list = (UserData*)malloc(sizeof(UserData));
    B = B->list;
    CinUser(B);
    B->list = NULL;
}

//----------------------------------- PlaneData-----------------------------//
void displaytoday(Node tree, int weekday, int y, int page) {
    int zhishao = (page - 1) * 6;  // 根据当前页码计算起始显示的索引，每页6条数据
    int zuiduo = page * 6;          // 每页的结束索引
    int count = 0;                  // 用于记录已经显示的数据条数
    time_t now = time(NULL);
    struct tm* local_time = localtime(&now);
    while (tree && count < zuiduo) {  // 当树节点存在并且还没有显示完当前页的数据
        char str[100];
        char zhuangtai[100];
        char from[100];
        char to[100];
        int qf_min = tree->data->qfmin + tree->data->qfhour * 60;
        int current_hour = local_time->tm_hour;
        int current_min = local_time->tm_min + 60 * current_hour;
        int current_week = local_time->tm_wday;
        // 将航班信息复制到str中
        sprintf_s(str, "荆门->武汉 起飞日期：星期%d %d:%d ", tree->data->qfweek, tree->data->qfhour, tree->data->qfmin);

        if (tree->data->weekday == weekday) {
            canPass = true;
            // 跳过当前页之前的数据
            if (count < zhishao) {
                count++;
                tree = tree->ltag == 0 ? tree->left : tree->right;  // 向下遍历9
                continue;  // 跳过当前数据，继续处理下一条
            }

            // 将data内时间转换为tm型
            struct tm qf;  // 起飞时间
            struct tm jl;  // 降落时间
            qf.tm_hour = tree->data->qfhour;
            qf.tm_min = tree->data->qfmin;
            jl.tm_hour = tree->data->jlhour;
            jl.tm_min = tree->data->jlmin;

            // 状态字符串更新
            if (tree->data->weekday == current_week && current_min > qf_min - 120 && current_min <= qf_min) {
                sprintf_s(zhuangtai, "即将起飞");
            }
            else if (tree->data->weekday < current_week) {
                sprintf_s(zhuangtai, "未起飞");
            }
            else if (tree->data->weekday == current_week && current_min <= qf_min - 120) {
                sprintf_s(zhuangtai, "未起飞");
            }
            else if (tree->data->zhuangtai == 0) {
                sprintf_s(zhuangtai, "等待");
            }

            // 航班的起点和终点
            if (tree->data->where == 1) {  // 荆门飞北京
                sprintf_s(from, "荆门");
                sprintf_s(to, "北京");
            }
            else if (tree->data->where == 2) {  // 北京飞荆门
                sprintf_s(from, "北京");
                sprintf_s(to, "荆门");
            }
            else if (tree->data->where == 3) {  // 武汉飞荆门
                sprintf_s(from, "武汉");
                sprintf_s(to, "荆门");
            }
            else if (tree->data->where == 4) {  // 荆门飞武汉
                sprintf_s(from, "荆门");
                sprintf_s(to, "武汉");
            }
            // 显示该航班信息
            weeksee(400, y, from, to, qf, jl, zhuangtai, tree->data->num, tree->data->piaoshu);
            y += 80;
            count++;  // 当前页显示数据条数加1
        }
        // 如果已经显示了当前页的数据，跳出循环
        if (count >= zuiduo) {
            break;
        }
        // 继续遍历树节点
        tree = tree->ltag == 0 ? tree->left : tree->right;
    }
}

void weeksee(int x, int y, const char* from, const char* to, struct tm time1, struct tm time2, const char* text, int flightID, int ticketNum)
{
    x = 400;
    //形参列表（x坐标，y坐标，起点，终点，起飞时间结构体，降落时间结构体，飞机状态）
    settextcolor(BLACK);
    settextstyle(20, 10, "华文新魏");
    //初始化字符串
    char TimeStr1[64] = { 0 };
    char TimeStr2[64] = { 0 };
    char flightIDStr[64] = { 0 };
    char ticketNumStr[64] = { 0 };
    _snprintf_s(TimeStr1, 64, _TRUNCATE, "起飞时间：%02d:%02d",
        time1.tm_hour,         //小时
        time1.tm_min          //分钟
    );
    _snprintf_s(TimeStr2, 64, _TRUNCATE, "降落时间：%02d:%02d",
        time2.tm_hour,         //小时
        time2.tm_min          //分钟
    );
    _snprintf_s(flightIDStr, 64, _TRUNCATE, "航班号：%d",
        flightID
    );
    _snprintf_s(ticketNumStr, 64, _TRUNCATE, "票数：%d",
        ticketNum
    );
    //打印起点
    outtextxy(x, y, from);
    setlinestyle(PS_SOLID, 3);
    //画箭头
    line(x + textwidth(from) + 20, y + 10, x + 200 - 20, y + 10);
    line(x + 200 - 20, y + 10, x + 200 - 40, y);
    line(x + 200 - 20, y + 10, x + 200 - 40, y + 20);
    setlinestyle(PS_SOLID);
    //打印目的地
    outtextxy(x + 200, y, to);
    //打印航班号
    outtextxy(x + 300, y, flightIDStr);
    //打印票数
    outtextxy(x + 450, y, ticketNumStr);
    //打印起飞时间
    outtextxy(x, y + 30, TimeStr1);
    outtextxy(x + 190, y + 30, "-");
    //打印降落时间
    outtextxy(x + 200, y + 30, TimeStr2);
    //打印飞机状态
    outtextxy(x + 400, y + 30, "状态:");
    if (strcmp("即将起飞", text) == 0)
        settextcolor(RED);
    if (strcmp("已起飞", text) == 0)
        settextcolor(BLUE);
    if (strcmp("即将降落", text) == 0)
        settextcolor(GREEN);
    outtextxy(x + 460, y + 30, text);
    settextcolor(BLACK);
    //购买按钮
    if (ticketNum != 0)
    {
        setfillcolor(RGB(65, 165, 238));
        fillroundrect(x + 600, y, x + 650, y + 50, 10, 10);
        settextcolor(RGB(255, 255, 255));
        outtextxy(x + 600, y + 20, "购买");
    }
    else
    {
        setfillcolor(RGB(255, 0, 0));
        fillroundrect(x + 600, y, x + 650, y + 50, 10, 10);
        settextcolor(RGB(0, 0, 0));
        outtextxy(x + 600, y + 20, "候补");
    }
}

void weekseeinmenu(int x, int y, const char* from, const char* to, struct tm time1, struct tm time2, const char* text)
{
    x = 400;
    //形参列表（x坐标，y坐标，起点，终点，起飞时间结构体，降落时间结构体，飞机状态）
    settextcolor(BLACK);
    settextstyle(20, 10, "华文新魏");
    //初始化时间字符串
    char TimeStr1[64] = { 0 };
    char TimeStr2[64] = { 0 };
    _snprintf_s(TimeStr1, 64, _TRUNCATE, "起飞时间：%02d:%02d",
        time1.tm_hour,         //小时
        time1.tm_min          //分钟
    );
    _snprintf_s(TimeStr2, 64, _TRUNCATE, "降落时间：%02d:%02d",
        time2.tm_hour,         //小时
        time2.tm_min          //分钟
    );
    //打印起点
    outtextxy(x, y, from);
    setlinestyle(PS_SOLID, 3);
    //画箭头
    line(x + textwidth(from) + 20, y + 10, x + 200 - 20, y + 10);
    line(x + 200 - 20, y + 10, x + 200 - 40, y);
    line(x + 200 - 20, y + 10, x + 200 - 40, y + 20);
    setlinestyle(PS_SOLID);
    //打印目的地
    outtextxy(x + 200, y, to);
    //打印起飞时间
    outtextxy(x, y + 30, TimeStr1);
    outtextxy(x + 190, y + 30, "-");
    //打印降落时间
    outtextxy(x + 200, y + 30, TimeStr2);
    //打印飞机状态
    outtextxy(x + 400, y + 30, "状态:");
    if (strcmp("即将起飞", text) == 0)
        settextcolor(RED);
    if (strcmp("已起飞", text) == 0)
        settextcolor(BLUE);
    if (strcmp("即将降落", text) == 0)
        settextcolor(GREEN);
    outtextxy(x + 460, y + 30, text);
    settextcolor(BLACK);
}

int buybuybuy(Node tree, int weekday, int choice, int page) {
    int zhishao = (page - 1) * 6;  // 根据当前页码计算起始显示的索引，每页6条数据
    int zuiduo = page * 6;          // 每页的结束索引
    int count = 0;                  // 用于记录已经显示的数据条数
    time_t now = time(NULL);
    struct tm* local_time = localtime(&now);
    while (tree && count < zuiduo) {  // 当树节点存在并且还没有显示完当前页的数据
        char str[100];
        char zhuangtai[100];
        char from[100];
        char to[100];
        int qf_min = tree->data->qfmin + tree->data->qfhour * 60;
        int current_hour = local_time->tm_hour;
        int current_min = local_time->tm_min + 60 * current_hour;
        int current_week = local_time->tm_wday;
        // 将航班信息复制到str中
        sprintf_s(str, "荆门->武汉 起飞日期：星期%d %d:%d ", tree->data->qfweek, tree->data->qfhour, tree->data->qfmin);

        if (tree->data->weekday == weekday) {
            canPass = true;
            // 跳过当前页之前的数据
            if (count < zhishao) {
                count++;
                tree = tree->ltag == 0 ? tree->left : tree->right;  // 向下遍历9
                continue;  // 跳过当前数据，继续处理下一条
            }

            // 将data内时间转换为tm型
            struct tm qf;  // 起飞时间
            struct tm jl;  // 降落时间
            qf.tm_hour = tree->data->qfhour;
            qf.tm_min = tree->data->qfmin;
            jl.tm_hour = tree->data->jlhour;
            jl.tm_min = tree->data->jlmin;

            // 状态字符串更新
            if (tree->data->weekday == current_week && current_min > qf_min - 120 && current_min <= qf_min) {
                sprintf_s(zhuangtai, "即将起飞");
            }
            else if (tree->data->weekday < current_week) {
                sprintf_s(zhuangtai, "未起飞");
            }
            else if (tree->data->weekday == current_week && current_min <= qf_min - 120) {
                sprintf_s(zhuangtai, "未起飞");
            }
            else if (tree->data->zhuangtai == 0) {
                sprintf_s(zhuangtai, "等待");
            }

            // 航班的起点和终点
            if (tree->data->where == 1) {  // 荆门飞北京
                sprintf_s(from, "荆门");
                sprintf_s(to, "北京");
            }
            else if (tree->data->where == 2) {  // 北京飞荆门
                sprintf_s(from, "北京");
                sprintf_s(to, "荆门");
            }
            else if (tree->data->where == 3) {  // 武汉飞荆门
                sprintf_s(from, "武汉");
                sprintf_s(to, "荆门");
            }
            else if (tree->data->where == 4) {  // 荆门飞武汉
                sprintf_s(from, "荆门");
                sprintf_s(to, "武汉");
            }
            // 显示该航班信息
//            weeksee(400, y, from, to, qf, jl, zhuangtai);
            count++;  // 当前页显示数据条数加1
        }
        // 如果已经显示了当前页的数据，跳出循环
        if (count == choice + zhishao) {
            printf("您选择了%d航班\n", tree->data->num);
            return tree->data->num;
        }
        // 继续遍历树节点
        tree = tree->ltag == 0 ? tree->left : tree->right;
    }
}

void menuready(Node tree, int y) {
    time_t now = time(NULL);
    struct tm* local_time = localtime(&now);
    int weekday = local_time->tm_wday;
    while (tree) {  // 当树节点存在并且还没有显示完当前页的数据
        char str[100];
        char zhuangtai[100];
        char from[100];
        char to[100];
        int qf_min = tree->data->qfmin + tree->data->qfhour * 60;
        int current_hour = local_time->tm_hour;
        int current_min = local_time->tm_min + 60 * current_hour;
        int current_week = local_time->tm_wday;
        // 将航班信息复制到str中
        sprintf_s(str, "荆门->武汉 起飞日期：星期%d %d:%d ", tree->data->qfweek, tree->data->qfhour, tree->data->qfmin);

        if (tree->data->weekday == weekday) {
            canPass = true;
            // 跳过当前页之前的数据


            // 将data内时间转换为tm型
            struct tm qf;  // 起飞时间
            struct tm jl;  // 降落时间
            qf.tm_hour = tree->data->qfhour;
            qf.tm_min = tree->data->qfmin;
            jl.tm_hour = tree->data->jlhour;
            jl.tm_min = tree->data->jlmin;

            // 状态字符串更新
            if (tree->data->weekday == current_week && current_min > qf_min - 120 && current_min <= qf_min) {
                sprintf_s(zhuangtai, "即将起飞");
            }
            else if (tree->data->weekday < current_week) {
                sprintf_s(zhuangtai, "未起飞");
            }
            else if (tree->data->weekday == current_week && current_min <= qf_min - 120) {
                sprintf_s(zhuangtai, "未起飞");
            }
            else if (tree->data->zhuangtai == 0) {
                sprintf_s(zhuangtai, "等待");
            }

            // 航班的起点和终点
            if (tree->data->where == 1) {  // 荆门飞北京
                sprintf_s(from, "荆门");
                sprintf_s(to, "北京");
            }
            else if (tree->data->where == 2) {  // 北京飞荆门
                sprintf_s(from, "北京");
                sprintf_s(to, "荆门");
            }
            else if (tree->data->where == 3) {  // 武汉飞荆门
                sprintf_s(from, "武汉");
                sprintf_s(to, "荆门");
            }
            else if (tree->data->where == 4) {  // 荆门飞武汉
                sprintf_s(from, "荆门");
                sprintf_s(to, "武汉");
            }
            // 显示该航班信息
            if (strcmp(zhuangtai, "即将起飞") == 0) {
                weekseeinmenu(400, y, from, to, qf, jl, zhuangtai);
                y += 80;
            }
        }
        // 继续遍历树节点
        tree = tree->ltag == 0 ? tree->left : tree->right;
    }
}


//---------------------------------------Binary_Tree------------------------------------------//
Node createNode(Data data) {
    Node node = (Node)malloc(sizeof(struct TreeNode));
    node->left = node->right = NULL;
    node->data = data;
    node->ltag = 0;
    node->rtag = 0;
    return node;
}

Node insert(Node root, Data data) {
    if (root) {
        if (root->data->num > data->num)
            root->left = insert(root->left, data);
        else if (root->data->num < data->num)
            root->right = insert(root->right, data);
    }
    else {
        root = createNode(data);
    }
    return root;
}

void inorder(Node root) {
    if (root == NULL) return;
    inorder(root->left);
    printf("航班号：%d\n", root->data->num);
    printf("星期几：%d\n", root->data->weekday);
    printf("起飞日期：星期%d %d:%d\n", root->data->qfweek, root->data->qfhour, root->data->qfmin);
    printf("降落日期：星期%d %d:%d\n", root->data->jlweek, root->data->jlhour, root->data->jlmin);
    if (root->data->island == 1)
        printf("未起飞\n");
    else if (root->data->island == 2)
        printf("起飞中\n");
    else
        printf("已降落\n");
    inorder(root->right);
}

Node copyNode(Node root) {
    if (root == NULL) return NULL;

    // 为新数据分配内存
    Data newData = (Data)malloc(sizeof(struct hangbandata));
    memcpy(newData, root->data, sizeof(struct hangbandata));

    // 为新节点分配内存并初始化
    Node node = createNode(newData);

    // 递归复制左右子树
    node->left = copyNode(root->left);
    node->right = copyNode(root->right);

    // 复制线索标记
    node->ltag = root->ltag;
    node->rtag = root->rtag;

    return node;
}


Node copytree(Node root) {
    return copyNode(root);
}

Node importDataFromFile(const char* filename) {
    Node root = NULL;
    FILE* fp;
    fopen_s(&fp, "a.txt", "r");

    char line[256];
    while (fgets(line, sizeof(line), fp) != NULL) {
        Data data = (Data)malloc(sizeof(struct hangbandata));
        sscanf_s(line, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d",
            &data->num,
            &data->weekday,
            &data->qfweek,
            &data->qfhour,
            &data->qfmin,
            &data->jlweek,
            &data->jlhour,
            &data->jlmin,
            &data->island,
            &data->where,
            &data->piaoshu,
            &data->zhuangtai
        );

        root = insert(root, data);
    }

    fclose(fp);
    return root;
}

Node pre = NULL;
void preorderThreaded(Node root) {
    if (root == NULL) return;

    if (root->left == NULL) {
        root->left = pre;
        root->ltag = 1;
    }
    if (pre && pre->right == NULL) {
        pre->right = root;
        pre->rtag = 1;
    }

    pre = root;

    if (root->ltag == 0)
        preorderThreaded(root->left);
    if (root->rtag == 0)
        preorderThreaded(root->right);
}

Node finddatabynum(Node tree, int num) {
    while (tree) {
        if (tree->data->num == num) {
            return tree;
        }
        if (tree->ltag == 0)
            tree = tree->left;
        else
            tree = tree->right;
    }
    return NULL;
}
//--------------------------------Buy_Ticket---------------------------//
void InitQ(Alternate* Q)
{
    Q->base = (int*)malloc(sizeof(int) * MaxQSize);
    if (Q->base == NULL)
        return;
    Q->Len = 0;
    Q->front = Q->rear = 0;
}

void EnQueue(Alternate* Q, int e)
{
    if (Q->Len == MaxQSize)
    {
        printf("队列已满");
        return;
    }
    Q->base[Q->rear] = e;
    Q->rear = (Q->rear + 1) % MaxQSize;
    Q->Len += 1;
}

int DeQueue(Alternate* Q, int* e)
{
    if (Q->Len == 0)
    {
        return 0;
    }
    *e = Q->base[Q->front];
    Q->front = (Q->front + 1) % MaxQSize;
    Q->Len -= 1;
    return 1;
}

int GetHead(Alternate Q)
{
    if (Q.Len)
        return Q.base[Q.front];
    return -1;
}

void QuitQueue(Alternate* Q, int id)
{
    int a[100], l, i, t;
    for (l = 0; DeQueue(Q, &t);)
    {
        if (t == id)
        {
            continue;
        }
        a[l++] = t;
    }
    for (i = 0; i < l; i++)
    {
        EnQueue(Q, a[i]);
    }
}

void PrintQueue(Alternate Q)
{
    int i;
    while (Q.Len)
    {
        printf("%d ", GetHead(Q));
        DeQueue(&Q, &i);
    }
    printf("\n");
}

void ReadAlt(AltArray* Alt)
{
    FILE* file;
    char name[Max];
    int i = 1, j, t;
    int ch;
    strcpy(name, "Alternate.txt");
    file = fopen(name, "r");
    if (file == NULL)
    {
        printf("打开文件Alternate失败");
        return;
    }
    while (!feof(file))
    {
        InitQ(&Alt->Q[i]);

        ch = 0;
        for (j = 0;ch != '\n' && !feof(file);j++)
        {
            fscanf(file, "%d", &t);
            ch = fgetc(file);
            if (t == -1)
                break;
            EnQueue(&Alt->Q[i], t);
        }
        i++;
    }
    Alt->len = i;
    for (i = 1;i < Alt->len;i++)
        PrintQueue(Alt->Q[i]);
    for (;i < MaxHB;i++)
        InitQ(&Alt->Q[i]);
}

void SaveAlt(AltArray* Alt)
{
    FILE* file;
    char name[Max];
    int i, t;
    strcpy(name, "Alternate.txt");
    file = fopen(name, "w");
    if (file == NULL)
    {
        return;
    }
    for (i = 1;i < MaxHB;i++)
    {
        while (Alt->Q[i].Len != 0)
        {
            DeQueue(&Alt->Q[i], &t);
            fprintf(file, "%d ", t);
        }
        if (MaxHB - 1 != i)
        {
            fprintf(file, " %d", -1);
            fputc('\n', file);
        }
    }
}

void AddQueu(AltArray* Alt, int num, int id)
{
    EnQueue(&Alt->Q[num], id);//为队列num号航班添加人的id索引表示候补
}

void ReadData(Booking* Book, char* ch)
{
    FILE* file;
    int i = 1;
    char name[Max];
    strcpy(name, ch);//用户名
    strcat(name, "Booking.txt");
    file = fopen(name, "a+");
    Book->Len = 0;
    if (file == NULL)
    {
        printf("打开失败");
        return;
    }
    rewind(file);
    if (fgetc(file) == -1)
    {
        printf("无票");
        return;
    }
    fclose(file);

    file = fopen(name, "r");
    if (file == NULL)
    {
        printf("打开失败");
        return;
    }
    while (!feof(file))
    {
        fscanf(file, "%d ", &Book->T[i]);
        i++;
    }
    Book->Len = i;
    printf("票原来有%d\n", Book->Len);
    fclose(file);
}

void SaveData(Booking* Book, char* ch)
{
    FILE* file;
    int i;
    char name[Max];
    strcpy(name, ch);
    strcat(name, "Booking.txt");
    file = fopen(name, "w");
    if (file == NULL)
    {
        printf("打开失败");
        return;
    }
    for (i = 1;i < Book->Len;i++)
    {
        fprintf(file, "%d ", Book->T[i].num);
        printf("%d\n", Book->T[i].num);
    }

    fclose(file);
}

void SaveDatatotree(Node xian)
{
    FILE* file;
    int i;
    char filename[Max] = "a.txt";
    file = fopen(filename, "w");
    if (file == NULL)
    {
        printf("打开失败");
        return;
    }
    //    for (i = 1;i < Book->Len;i++)
    //    {
    //        fprintf(file, "%d ", Book->T[i].num);
    //        printf("%d\n", Book->T[i].num);
    //    }
        //遍历线索化二叉树进行保存
    while (xian) {
        // 写入节点数据
        fprintf(file, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n",
            xian->data->num,
            xian->data->weekday,
            xian->data->qfweek,
            xian->data->qfhour, xian->data->qfmin,
            xian->data->jlweek, xian->data->jlhour, xian->data->jlmin,
            xian->data->island, xian->data->where, xian->data->piaoshu, xian->data->zhuangtai
        );
        // 移动到下一个节点
        if (xian->ltag == 0)
            xian = xian->left;
        else
            xian = xian->right;
    }
    fclose(file);
}

void AddDataBook(Booking* Book, int num)//添加票
{
    printf("%d", Book->Len);
    if (Book->Len + 1 <= MaxQSize)
    {
        if (Book->Len == 0)
            Book->Len = 1;
        Book->T[Book->Len].num = num;
        Book->Len += 1;
    }
}

void DelDataBook(UserData* D, Booking* Book, int num, AltArray* Alt, Node xian)//删除票
{
    Node curNode;
    curNode = xian;
    int i, j, t;
    Booking Data;
    char name[Max];
    for (j = i = 0;i < Book->Len;i++)
    {
        if (Book->T[i].num != num)
            Book->T[j++] = Book->T[i];
    }
    Book->Len = j;
    printf("%d", j);
    while (xian) {
        if (xian->data->num == num) {
            xian->data->piaoshu++;
            break;
        }
        xian = xian->ltag == 0 ? xian->left : xian->right;
    }
    //if (i != j)
    //{
    //	if (Alt->Q[num].Len != 0)
    //	{
    //		DeQueue(&Alt->Q[num], &t);
    //		//待写让文件加票
    //		strcpy(name, FindUserId(D, t));
    //		ReadData(&Data, name);
    //		AddDataBook(&Data, t);
    //		SaveData(&Data, name);
    //	}
    //	return;
    //}
}

void PrinT(Booking* Alt)
{
    int i;
    for (i = 1;i < Alt->Len;i++)
    {
        printf("%d", Alt->T[i]);
    }
}

char* FindUserId(UserData* D, int id)
{
    UserData* B;
    int i;
    B = D->list;
    for (i = 0;B;i++)
    {
        if (i == id)
            break;
        B = B->list;
    }
    return B->data.UserName;
}
int Partition(int stu[], int low, int high)
{
    int pivotkey;
    int flag = 0;
    stu[0] = stu[low];
    pivotkey = stu[low];
    while (low < high)
    {
        while (low < high && stu[high] >= pivotkey) --high;
        stu[low] = stu[high];
        while (low < high && stu[low] <= pivotkey) ++low;
        stu[high] = stu[low];
        printf("%d -- %d  = %d\n", low, high, flag);
        flag = 1;
    }
    stu[low] = stu[0];
    return low;
}

void QSort(int stu[], int low, int high)
{
    int pivotloc;
    if (low < high)
    {
        pivotloc = Partition(stu, low, high);
        QSort(stu, low, pivotloc - 1);
        QSort(stu, pivotloc + 1, high);
    }
}

void fun2(int stu[], int n)
{
    QSort(stu, 1, n);
}

int FindDataBook(Booking* Book, int num)
{
    int left = 0;
    int right = Book->Len - 1;
    int mid;

    fun2((int*)Book->T, Book->Len - 1);
    while (left <= right)
    {
        mid = left + (right - left) / 2;
        if (Book->T[mid].num == num)
        {
            return mid;
        }
        else if (Book->T[mid].num < num)
        {
            left = mid + 1;
        }
        else
        {
            right = mid - 1;
        }
    }
    return -1;
}
int Convert(char* ch)
{
    int i, l, t = 0;
    l = strlen(ch);
    for (i = 0;i < l;i++)
    {
        t = t * 10 + (ch[i] - '0');
    }
    return t;
}
void QueryTicket_Data(Node tree, int y) {
    int count = 0;
    time_t now = time(NULL);
    struct tm* local_time = localtime(&now);
    while (tree && count < 4) {
        char str[100];
        char zhuangtai[100];
        char from[100];
        char to[100];
        int qf_min = tree->data->qfmin + tree->data->qfhour * 60;
        int current_hour = local_time->tm_hour;
        int current_min = local_time->tm_min + 60 * current_hour;
        int current_week = local_time->tm_wday;
        // 将航班信息复制到str中
        sprintf_s(str, "荆门->武汉 起飞日期：星期%d %d:%d ", tree->data->qfweek, tree->data->qfhour, tree->data->qfmin);

        // 将data内时间转换为tm型
        struct tm qf;  // 起飞时间
        struct tm jl;  // 降落时间
        qf.tm_hour = tree->data->qfhour;
        qf.tm_min = tree->data->qfmin;
        jl.tm_hour = tree->data->jlhour;
        jl.tm_min = tree->data->jlmin;

        // 状态字符串更新
        if (tree->data->weekday == current_week && current_min > qf_min - 120 && current_min <= qf_min) {
            sprintf_s(zhuangtai, "即将起飞");
        }
        else if (tree->data->weekday < current_week) {
            sprintf_s(zhuangtai, "未起飞");
        }
        else if (tree->data->weekday == current_week && current_min <= qf_min - 120) {
            sprintf_s(zhuangtai, "未起飞");
        }
        else if (tree->data->zhuangtai == 0) {
            sprintf_s(zhuangtai, "等待");
        }

        // 航班的起点和终点
        if (tree->data->where == 1) {  // 荆门飞北京
            sprintf_s(from, "荆门");
            sprintf_s(to, "北京");
        }
        else if (tree->data->where == 2) {  // 北京飞荆门
            sprintf_s(from, "北京");
            sprintf_s(to, "荆门");
        }
        else if (tree->data->where == 3) {  // 武汉飞荆门
            sprintf_s(from, "武汉");
            sprintf_s(to, "荆门");
        }
        else if (tree->data->where == 4) {  // 荆门飞武汉
            sprintf_s(from, "荆门");
            sprintf_s(to, "武汉");
        }
        QuitTicket(400, y, from, to, qf, jl, zhuangtai);
        count++;
    }
    // 继续遍历树节点
    tree = tree->ltag == 0 ? tree->left : tree->right;
}

void QuitTicket(int x, int y, const char* from, const char* to, struct tm time1, struct tm time2, const char* text)
{

    x = 400;
    //形参列表（x坐标，y坐标，起点，终点，起飞时间结构体，降落时间结构体，飞机状态）
    settextcolor(BLACK);
    settextstyle(20, 10, "华文新魏");
    //初始化时间字符串
    char TimeStr1[64] = { 0 };
    char TimeStr2[64] = { 0 };
    _snprintf_s(TimeStr1, 64, _TRUNCATE, "起飞时间：%02d:%02d",
        time1.tm_hour,         //小时
        time1.tm_min          //分钟
    );
    _snprintf_s(TimeStr2, 64, _TRUNCATE, "降落时间：%02d:%02d",
        time2.tm_hour,         //小时
        time2.tm_min          //分钟
    );
    //打印起点
    outtextxy(x, y, from);
    setlinestyle(PS_SOLID, 3);
    //画箭头
    line(x + textwidth(from) + 20, y + 10, x + 200 - 20, y + 10);
    line(x + 200 - 20, y + 10, x + 200 - 40, y);
    line(x + 200 - 20, y + 10, x + 200 - 40, y + 20);
    setlinestyle(PS_SOLID);
    //打印目的地
    outtextxy(x + 200, y, to);
    //打印起飞时间
    outtextxy(x, y + 30, TimeStr1);
    outtextxy(x + 190, y + 30, "-");
    //打印降落时间
    outtextxy(x + 200, y + 30, TimeStr2);
    //打印飞机状态
    outtextxy(x + 400, y + 30, "状态:");
    if (strcmp("即将起飞", text) == 0)
        settextcolor(RED);
    if (strcmp("已起飞", text) == 0)
        settextcolor(BLUE);
    if (strcmp("即将降落", text) == 0)
        settextcolor(GREEN);
    outtextxy(x + 460, y + 30, text);
    settextcolor(BLACK);
    //购买按钮
    setfillcolor(RGB(255, 0, 0));
    fillroundrect(x + 600, y, x + 650, y + 50, 10, 10);
    settextcolor(RGB(0, 0, 0));
    outtextxy(x + 600, y + 20, "退票");
}


/*
Administrators
12345
LiYunLong
12345
HuangLeiGang
12345
*/

