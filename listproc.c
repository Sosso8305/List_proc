#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <tlhelp32.h>
#include "listproc.h"



///////////////////////////////////////////////
//            Function: display             //
/////////////////////////////////////////////

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

void display_dataprocess(DataProcess data){
    printf("====================================\n");
    printf("id: %d\n",data.ID);
    printf("name: %s\n",data.name);
    printf("parent id: %d\n",data.parentID);
    printf("count Thread: %d\n ",data.cntThread);
    printf("====================================\n");

}

void display_dataprocess_table(DataProcess data){
    printf("||%d\t||%d\t||%d\t||%s\t\n",data.ID,data.cntThread,data.parentID,data.name);
}

void display_list(List *list){
    printf("Count Proc: %d\n",list->countNode);
    printf("=============================================================\n");
    printf("||ID\t||nbThread||ParentID\t||Name\t\n");
    printf("=============================================================\n");
    Node *temp = list->head;
    while(temp != NULL){
        display_dataprocess_table(temp->data);
        temp = temp->next;
    }
}




///////////////////////////////////////////////
//          Function: linked list           //
/////////////////////////////////////////////

void initList(List *list){
    list->head = NULL;
    list->tail = NULL;
    list->countNode = 0;
}

void pushNodeTail(List *list, DataProcess data){
    Node *newNode = (Node *) malloc(sizeof(Node));
    newNode->data = data;
    newNode->next = NULL;
    newNode->prev = NULL;

    if(list->head == NULL){
        list->head = newNode;
        list->tail = newNode;
    }else{
        list->tail->next = newNode;
        newNode->prev =list->tail;
        list->tail = newNode;
    }
    list->countNode++;
}

void pushNodeHead(List *list, DataProcess data){
    Node *newNode = (Node *) malloc(sizeof(Node));
    newNode->data = data;
    newNode->next = NULL;
    newNode->prev = NULL;

    if(list->head == NULL){
        list->head = newNode;
        list->tail = newNode;
    }else{
        list->head->prev = newNode;
        newNode->next = list->head;
        list->head = newNode;
    }
    list->countNode++;
}

DataProcess popNodeTail(List *list){
    DataProcess data;
    Node *temp = list->tail;
    if(list->head == NULL){
        printf("List is empty\n");
        return data;
    }else if(list->head == list->tail){
        list->head = NULL;
        list->tail = NULL;
    }else{
        list->tail = list->tail->prev;
        list->tail->next = NULL;
    }
    data = temp->data;
    free(temp);
    list->countNode--;
    return data;
}

DataProcess popNodeHead(List *list){
    DataProcess data;
    Node *temp = list->head;
    if(list->head == NULL){
        printf("List is empty\n");
        return data;
    }else if(list->head == list->tail){
        list->head = NULL;
        list->tail = NULL;
    }else{
        list->head = list->head->next;
        list->head->prev = NULL;
    }
    data = temp->data;
    free(temp);
    list->countNode--;
    return data;
}

void freeList(List *list){
    while(list->head != NULL){
        popNodeHead(list);
    }
}

int getCountNode(List *list){
    return list->countNode;
}



///////////////////////////////////////////////
//       Function: ProcessEntery32          //
/////////////////////////////////////////////

DataProcess ProcessEntryToDataProcess(LPPROCESSENTRY32 lppe32){
    DataProcess data;
    data.ID = lppe32->th32ProcessID;
    strcpy(data.name, lppe32->szExeFile);
    data.parentID = lppe32->th32ParentProcessID;
    data.cntThread = lppe32->cntThreads;
    return data;
}




///////////////////////////////////////////////
//             Function: Main               //
/////////////////////////////////////////////


int main(){
    //initialize
    HANDLE Hlist_proc;
    boolean flag;
    LPPROCESSENTRY32 lppe;
    DataProcess data;
    List listP;

    //get list process
    Hlist_proc = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS,0);
    if(Hlist_proc == INVALID_HANDLE_VALUE) return -1;

    
    initList(&listP);
    flag = TRUE;
    while(flag){
        lppe = (LPPROCESSENTRY32)malloc(sizeof(PROCESSENTRY32));
        lppe->dwSize = sizeof(PROCESSENTRY32);
        flag = Process32Next(Hlist_proc, lppe);
        
        data = ProcessEntryToDataProcess(lppe);
        pushNodeTail(&listP, data);
    }

    free(lppe);
    
      
    display_list(&listP);
    

    return 0;
}
