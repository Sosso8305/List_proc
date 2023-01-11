#define sizeName 256

typedef struct {
    int ID;     // th32ProcessID
    char name[sizeName];      //szExeFile 
    int parentID;       //th32ParentProcessID
    int cntThread;    //cntThreads
}DataProcess;


typedef struct Node {
    DataProcess data;
    struct Node *next;
    struct Node *prev;
}Node;

typedef struct {
    Node *head;
    Node *tail;
    int countNode;
}List;


// Function prototypes linked list
void initList(List *list);
void pushNodeTail(List *list, DataProcess data);
void pushNodeHead(List *list, DataProcess data);
DataProcess popNodeTail(List *list);
DataProcess popNodeHead(List *list);
void freeList(List *list);
int getCountNode(List *list);

// Function prototypes
void display_lpprocessentry32( LPPROCESSENTRY32 lppe32 );
void display_dataprocess(DataProcess data);
void display_dataprocess_table(DataProcess data);
void display_list(List *list);
DataProcess ProcessEntryToDataProcess(LPPROCESSENTRY32 lppe32);
DataProcess genDataProcess(int ID,char *name,int parentID,int cntThread);
void SetDebugPrivilege();
void diffList(List *list1, List *list2);
//Node *findNode(List *list, int ID);