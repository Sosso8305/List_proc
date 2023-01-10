#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <tlhelp32.h>
#include "listproc.h"

void display_lpprocessentry32(LPPROCESSENTRY32 lppe){
    printf("====================================\n");
    printf("id: %ld\n",lppe->th32ProcessID);
    printf("name: %s\n",lppe->szExeFile);
    printf("parent id: %ld\n",lppe->th32ParentProcessID);
    printf("priority: %ld\n",lppe->pcPriClassBase);
    printf("count Thread: %ld\n",lppe->cntThreads);
    printf("size: %ld\n",lppe->dwSize);
    printf("associated exe: %ld\n",lppe->th32ModuleID);
    printf("default heap: %ld\n",lppe->th32DefaultHeapID);
    printf("flags: %ld\n",lppe->dwFlags);
    printf("====================================\n");

}

///////////////////////////////////////////////
//          Function: linked list           //
/////////////////////////////////////////////

// void initList(List *list){
//     list->head = NULL;
//     list->tail = NULL;
//     list->countNode = 0;
// }

// void pushNodeTail(List *list, DataProcess data){
//     Node *newNode = malloc(sizeof(Node));
//     newNode->data = data;
//     newNode->next = NULL;
//     newNode->prev = NULL;

//     if(list->head == NULL){
//         list->head = newNode;
//         list->tail = newNode;
//     }else{
//         list->tail->next = newNode;
//         newNode->prev = list->tail;
//         list->tail = newNode;
//     }
//     list->countNode++;
// }

// void pushNodeHead(List *list, DataProcess data){
//     Node *newNode = malloc(sizeof(Node));
//     newNode->data = data;
//     newNode->next = NULL;
//     newNode->prev = NULL;

//     if(list->head == NULL){
//         list->head = newNode;
//         list->tail = newNode;
//     }else{
//         list->head->prev = newNode;
//         newNode->next = list->head;
//         list->head = newNode;
//     }
//     list->countNode++;
// }

// DataProcess popNodeTail(List *list){
//     DataProcess data;
//     Node *temp = list->tail;
//     if(list->head == NULL){
//         printf("List is empty\n");
//         return data;
//     }else if(list->head == list->tail){
//         list->head = NULL;
//         list->tail = NULL;
//     }else{
//         list->tail = list->tail->prev;
//         list->tail->next = NULL;
//     }
//     data = temp->data;
//     free(temp);
//     list->countNode--;
//     return data;
// }

// DataProcess popNodeHead(List *list){
//     DataProcess data;
//     Node *temp = list->head;
//     if(list->head == NULL){
//         printf("List is empty\n");
//         return data;
//     }else if(list->head == list->tail){
//         list->head = NULL;
//         list->tail = NULL;
//     }else{
//         list->head = list->head->next;
//         list->head->prev = NULL;
//     }
//     data = temp->data;
//     free(temp);
//     list->countNode--;
//     return data;
// }

// void freeList(List *list){
//     while(list->head != NULL){
//         popNodeHead(list);
//     }
// }




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


