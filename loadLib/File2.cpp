#include <iostream>
#include <tchar.h>
#include <windows.h>
#include "./include/rtl-sdr.h"
#define xstr(s) str(s)
#define str(s) #s


//This project tries to load rtlsdr.dll and access it's functions

int _tmain(int argc, _TCHAR* argv[])
{
char file[] = xstr(RTLSDR_API);
std::cout<<file<<rtlsdr_get_device_name(0);
getchar();
}
