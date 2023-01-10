#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <tlhelp32.h>

int main(){
    //initialize
    HANDLE list_proc;
    boolean result;
    LPPROCESSENTRY32 lppe;


    list_proc = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS,0);
    if(list_proc == INVALID_HANDLE_VALUE) return -1;

    Process32Next(list_proc, lppe);

    printf("id: %i",lppe->th32ProcessID);

    return 0;


}