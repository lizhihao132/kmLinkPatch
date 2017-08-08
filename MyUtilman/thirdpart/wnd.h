#ifndef WND_H
#define WND_H
#include <windows.h>

//查找窗口
HWND findWindow(HWND parentWnd,const char* name);

//屏幕是否已经锁定了
BOOL hasScreenLocked();

//向 wnd 发送键盘消息, 不将 wnd 设置为最前面的窗口
void sendKeysWithoutSetForeground(HWND wnd, BYTE * vks, int klen);

//向最前面的窗口发送键盘消息
void sendToForegroundWnd(BYTE* vks, int klen);


#endif