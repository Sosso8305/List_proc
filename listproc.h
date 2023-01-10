#define sizeName 256

typedef struct {
    int ID;     // th32ProcessID
    char name[sizeName];      //szExeFile 
    int parentID;       //th32ParentProcessID
    int cntThread;    //cntThreads
}DataProcess;

typedef struct {
    DataProcess data;
    struct Node *next;
    struct Node *prev;
}Node;

typedef struct {
    Node *head;
    Node *tail;
    int countNode;
}List;





// Function prototypes
void display_lpprocessentry32( LPPROCESSENTRY32 lppe32 );