#define _WIN32_WINNT 0x0500
#define MOD_NOREPEAT  0x4000
#define F1	0x700000
#define F2	0x710000
#define F3	0x720000
#define F4	0x730000
#define ALT_F5	0x740001
#define ALT_F6	0x750001
#define ALT_F7	0x760001
#include<iostream>
#include<cstdio>
#include<cstring>
#include<cstdlib>
#include<conio.h>
#include<windows.h>
#include<winuser.h>
using namespace std;
string password;//密码，在内存中明文保存 
HWND target[5];//记录窗口句柄 
char wx[101][5];//记录窗口标题 
bool status[5]; //1->未占用 0->已占用 
inline void outputhelp()
{
	system("cls");
	cout<<"Awaiting orders...\n";
	cout<<"F1 Push/Pop Container 1\n";
	cout<<"F2 Push/Pop Container 2\n";
	cout<<"F3 Push/Pop Container 3\n";
	cout<<"F4 Push/Pop Container 4\n";
	cout<<"ALT+F5 Output all containers\n";
	cout<<"ALT+F6 Pop all containers\n";
	cout<<"ALT+F7 Terminate all windows in container\n";
	cout<<"*Do not input randomly in it!";
}
inline void sleep()
{
	system("ping>nul 127.1 -n 2");
}
BOOL WINAPI HandlerRoutine( DWORD dwCtrlType ){
    if( CTRL_CLOSE_EVENT == dwCtrlType ){
    	freopen("hdpp.dat","w",stdout);
    	for(int i=1;i<=4;i++)cout<<(int)target[i]<<endl;
    	fclose(stdout);
    	//退出时将所有句柄以int类型输出
		//若利用此处代码，在这儿添加while(1);会卡评测 ，一个测试点卡5s
		//不添加fclose将会无法保存文件 
	}
}
inline void init()
{
	//freopen成功的时候返回文件指针，失败返回0 
	if(freopen("hdpp.dat","r",stdin)!=0)
	{
		for(int i=1;i<=4;i++)
		{
			int x;
			cin>>x;
			target[i]=(HWND)x;//取得上次退出时遗留的句柄 
			if(GetWindowText(target[i],wx[i],999)==0) 
			{//若找不到此窗口 
				target[i]=0;
				status[i]=1;
			}else
			{
				status[i]=0;
			}
			
		}
	}else{
		for(int i=1;i<=4;i++)
		{
			status[i]=1;		
		}
	}
	bool stat[10];
	stat[1]=RegisterHotKey(NULL,1, MOD_NOREPEAT,VK_F1);
	stat[2]=RegisterHotKey(NULL,1, MOD_NOREPEAT,VK_F2);
	stat[3]=RegisterHotKey(NULL,1, MOD_NOREPEAT,VK_F3);
	stat[4]=RegisterHotKey(NULL,1, MOD_NOREPEAT,VK_F4);
	stat[5]=RegisterHotKey(NULL,1, MOD_NOREPEAT | MOD_ALT,VK_F5);
	stat[6]=RegisterHotKey(NULL,1, MOD_NOREPEAT | MOD_ALT,VK_F6);
	stat[7]=RegisterHotKey(NULL,1, MOD_NOREPEAT | MOD_ALT,VK_F7);
	//MOD_NOREPEAT 可以防止用户一直按着某个热键，无限接收WM_HOTKEY 
    if (stat[1] && stat[2] && stat[3] && stat[4] && stat[5] && stat[6] && stat[7] )
        printf("Load Successfully,all hotkeys have been registered!\n"),sleep();
	//注册热键 RegisterHotKey返回值为1 为注册成功 
}
string inputpwd()
{
	//getch()是C里面的函数，从键盘直接读入字符，输入一个存一个，无需回车
	//有用过ssh终端的都知道，输入密码时不会回显的！ 
	char tmp[99];
	int cnt=0;
	while(true){
		char x = getch();
		if(32<x && x<126){tmp[cnt]=x;cnt++;}
		if(x == 13)  //回车结束
			break;
	}
	string a1=tmp;
	a1=a1.substr(0,a1.length());
	return a1;
}
inline bool requirepwd()
{
	//弹出自己的窗口  GetConsoleWindow()返回自己的句柄 
	ShowWindow(GetConsoleWindow(),SW_SHOW);
	SetForegroundWindow(GetConsoleWindow());
	system("cls");
	cout<<"Password required(inputs will be hiden):\n\t";
	string pwd=inputpwd();
	system("cls");
	if(pwd==password) 
		return 1;
	else 
	{
		system("color 0c");
		cout<<"Wrong Password!\n";
		sleep();
		system("color 07");
		outputhelp();
		return 0;
	}
}
int hos(int x)
{
	system("cls");
	if (status[x])
	{
		target[x]=GetForegroundWindow();//获得整个windows当前激活的窗口句柄 
		if(target[x]==GetConsoleWindow()) {outputhelp();return -1;}
		for(int i=1;i<=4;i++)
		{
			if(x==i) continue;
			if(target[x]==target[i]) return -1;
		}
		GetWindowText(target[x],wx[x],999);
		//GetWindowText(句柄，char类型数组，最大窗口标题长度)
		//返回窗口标题的长度，找不到窗口返回0，将直接把窗口标题传入char类型数组 
		cout<<"Container"<<x<<":\n";
    	printf("\tHide: 0x%X \n",target[x]);
    	ShowWindow(target[x],SW_HIDE);
    	
	}            
	else
	{
		if(!requirepwd()) return 0;
		ShowWindow(target[x],SW_SHOWNA);
		SetForegroundWindow(target[x]);//设置整个windows激活的窗口 
		cout<<"Container"<<x<<":\n";
		printf("\tShow: 0x%X - %s\n",target[x],wx[x]);
		target[x]=0;
	}
	status[x]=not(status[x]);
	sleep();
	system("cls");
	outputhelp();
}
int main()
{
	SetConsoleCtrlHandler( HandlerRoutine, TRUE );//添加退出时的处理 
	system("mode con cols=60 lines=11");
	system("title AutoHider++");
	cout<<"Set Password(inputs will be hiden):\n\t";
	password=inputpwd();
	system("cls");
	
	init();

    outputhelp();
	MSG msg = {0};
    while (GetMessage(&msg, NULL, 0, 0))
    {
        if (msg.message == WM_HOTKEY)
        {

			switch(msg.lParam)
			{
				case F1:
					hos(1);
					break;
				case F2:
					hos(2);
					break;
				case F3:
					hos(3);
					break;	
				case F4:
					hos(4);
					break;		
				case ALT_F5:
					if(!requirepwd())break;
					cout<<"===============QUERIES===============\n";
					for(int i=1;i<=4;i++)
					{
						
						if (target[i]!=0)
						{
							
							printf("Container%d:\n\t0x%X - %s\n",i,target[i],wx[i]);
						}else
						{
							printf("Container%d:\n\tNULL\n",i);
						}
						
					}
					cout<<"=====================================\n";
					sleep();
					outputhelp();
					break;
				case ALT_F6:
					if(!requirepwd()){outputhelp();break;}
					for(int i=1;i<=4;i++)
					{
						ShowWindow(target[i],SW_SHOWNA);
						SetForegroundWindow(target[i]);
						cout<<"Container"<<i<<":\n";
						printf("\tShow: 0x%X - %s\n",target[i],wx[i]);
						target[i]=0;
						status[i]=1;
					}
					printf("All containers poped.\n");
					sleep();
					outputhelp();
					break;
				case ALT_F7:
					if(!requirepwd()){outputhelp();break;}
					for(int i=1;i<=4;i++)
					{
						if(target[i]==GetConsoleWindow())continue;
						SendMessage(target[i],WM_CLOSE,0,0);//关闭指定窗口 
						target[i]=0;
						status[i]=1;
					}
					printf("All windows terminated.\n");
					sleep();
					outputhelp();
					break;
			}
        }
    } 
}
