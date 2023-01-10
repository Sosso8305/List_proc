#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <tlhelp32.h>
#include "listproc.h"

void display_lpprocessentry32(LPPROCESSENTRY32 lppe){
    printf("====================================\n");
    printf("id: %i\n",lppe->th32ProcessID);
    printf("name: %s\n",lppe->szExeFile);
    printf("parent id: %i\n",lppe->th32ParentProcessID);
    printf("priority: %i\n",lppe->pcPriClassBase);
    printf("count Thread: %i\n",lppe->cntThreads);
    printf("size: %i\n",lppe->dwSize);
    printf("associated exe: %i\n",lppe->th32ModuleID);
    printf("default heap: %i\n",lppe->th32DefaultHeapID);
    printf("flags: %i\n",lppe->dwFlags);
    printf("====================================\n");

}

int main(){
    //initialize
    HANDLE list_proc;
    boolean flag;
    LPPROCESSENTRY32 lppe;


    list_proc = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS,0);
    if(list_proc == INVALID_HANDLE_VALUE) return -1;

    
    flag = TRUE;
    while(flag){
        lppe = (LPPROCESSENTRY32)malloc(sizeof(PROCESSENTRY32));
        lppe->dwSize = sizeof(PROCESSENTRY32);
        flag = Process32Next(list_proc, lppe);
        
        display_lpprocessentry32(lppe);
        //add to list
    }
    

    return 0;
}


