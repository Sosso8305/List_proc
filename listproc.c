#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <tlhelp32.h>
#include <psapi.h> // gcc Flag : -lpsapi
#include "listproc.h"


#define maxProcess 65000
#define skipProcess 1 // 1--> Skip system process (ID System Process: 0 & 4) 0--> Don't skip system process 
#define writeFile 0 // 1--> Write file 0--> Don't write file

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
    printf("=============================================================\n\n");
    
}

void write_list(List *list,char *filename){
    FILE *f = fopen(filename,"w");
    fprintf(f,"Count Proc: %d\n",list->countNode);
    fprintf(f,"=============================================================\n");
    fprintf(f,"||ID\t||nbThread||ParentID\t||Name\t\n");
    fprintf(f,"=============================================================\n");
    Node *temp = list->head;
    while(temp != NULL){
        fprintf(f,"||%d\t||%d\t||%d\t||%s\t\n",temp->data.ID,temp->data.cntThread,temp->data.parentID,temp->data.name);
        temp = temp->next;
    }
    fprintf(f,"=============================================================\n\n");
    fclose(f);
}



///////////////////////////////////////////////
//      Function: Debug Privilège           //
/////////////////////////////////////////////

void SetDebugPrivilege(){
    TOKEN_PRIVILEGES tp;
    LUID luid;
    HANDLE hproc;
    HANDLE hproc2;

    hproc = OpenProcess(PROCESS_ALL_ACCESS, FALSE, GetCurrentProcessId());
    OpenProcessToken(hproc, TOKEN_ALL_ACCESS, &hproc2);

    LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &luid);
    tp.PrivilegeCount = 1;
    tp.Privileges[0].Luid = luid;
    tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
    AdjustTokenPrivileges(hproc2, FALSE, &tp, sizeof(TOKEN_PRIVILEGES), NULL, NULL);
    CloseHandle(hproc2);
    CloseHandle(hproc);


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
    DataProcess data = {0};
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
    DataProcess data = {0};
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

List diffList(List *list1, List *list2){
    int swip = 0;
    List list;
    Node *temp;
    Node *temp2;
    Node *temp3;
    Node *temp4;


    swip = 0;
    initList(&list);
    temp3 = list1->head;
    temp4 = list2->head;

    while(swip<2){
        temp = temp3;
        while(temp != NULL){
            temp2 = temp4;
            while(temp2 != NULL){

                if(temp->data.ID == temp2->data.ID){
                break; 

                }
                temp2 = temp2->next;
            }

            if(temp2 == NULL){
                // add condition to skip the system process
                if(!skipProcess || !(temp->data.ID == 0 || temp->data.ID == 4)){
                    pushNodeTail(&list, temp->data);
                }
                
            }

            temp = temp->next;
        } 

        temp3 = list2->head;
        temp4 = list1->head;
        swip++; 
    } 

    return list;
}



///////////////////////////////////////////////
//         Function: DataProcess            //
/////////////////////////////////////////////

DataProcess ProcessEntryToDataProcess(LPPROCESSENTRY32 lppe32){
    DataProcess data;
    data.ID = lppe32->th32ProcessID;
    strcpy(data.name, lppe32->szExeFile);
    data.parentID = lppe32->th32ParentProcessID;
    data.cntThread = lppe32->cntThreads;
    return data;
}

DataProcess genDataProcess(int ID, char *name, int parentID, int cntThread){
    DataProcess data;
    data.ID = ID;
    strcpy(data.name, name);
    data.parentID = parentID;
    data.cntThread = cntThread;
    return data;
}




///////////////////////////////////////////////
//             Function: Main               //
/////////////////////////////////////////////


int main(){
    //initialize
    HANDLE Hlist_proc;
    HANDLE Hproc;
    HMODULE Hmodule;
    DWORD dwAccesProc;
    boolean flag;
    LPPROCESSENTRY32 lppe;
    DataProcess data;
    List listP;
    List listP2;
    List listDiff;
    int pid;
    char NameProc[sizeName];

    //set debug privilege
    SetDebugPrivilege();


    //get list process with 1th method
    Hlist_proc = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS,0);
    if(Hlist_proc == INVALID_HANDLE_VALUE) return -1;

    
    initList(&listP);
    flag = TRUE;
    while(flag){
        lppe = (LPPROCESSENTRY32)malloc(sizeof(PROCESSENTRY32));
        lppe->dwSize = sizeof(PROCESSENTRY32);
        flag = Process32Next(Hlist_proc, lppe);
        
        if(flag){ // check if the process is not the last
        data = ProcessEntryToDataProcess(lppe);
        pushNodeTail(&listP, data);
        }
    }
    
    free(lppe);
    CloseHandle(Hlist_proc);
    display_list(&listP);


    //get list process with 2th method
    initList(&listP2);
    pid=0;
    while (pid <= maxProcess){
        Hproc = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
        if (Hproc != NULL && Hproc != INVALID_HANDLE_VALUE){
            if(EnumProcessModules(Hproc, &Hmodule, sizeof(Hmodule), &dwAccesProc)){
                ZeroMemory(NameProc, sizeName);
                GetModuleBaseName(Hproc, Hmodule, NameProc, sizeName);
                data = genDataProcess(pid, NameProc, -1, -1);
                pushNodeTail(&listP2, data);
                
            }
        } 

        pid=pid+4;
   }

    display_list(&listP2);
   
    

    //diff list
    listDiff = diffList(&listP, &listP2);
    display_list(&listDiff);

    //write to file
    if(writeFile){
        write_list(&listP, "listP.txt");
        write_list(&listP2, "listP2.txt");
        write_list(&listDiff, "listDiff.txt");
    }


    //free memory
    freeList(&listP);
    freeList(&listP2);
    freeList(&listDiff);

    system("pause");

    return 0;
}
