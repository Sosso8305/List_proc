#include <ntifs.h>
#include <ntddk.h>

#define DeviceName L"\\Device\\driver"
#define LnkDeviceName L"\\DosDevices\\driver" 


void HideProcess(){
	PEPROCESS pEProc;
	PEPROCESS pEProcFirst;
	PEPROCESS pEProcNext;
	int offset = 0x16c;
	int offset_lEntry = 0x0b8;
	char * name;
	int i;
	LIST_ENTRY * list_entry=NULL;
	
	pEProcFirst = IoGetCurrentProcess();
	pEProc = IoGetCurrentProcess();
	pEProcNext=NULL;
	
	i =0;
	while(i<100 && pEProcFirst != pEProcNext){
		pEProc = pEProcNext;
	
		name = (PUCHAR) pEProc + offset;
		DbgPrint("name: %s\n",name);
	
	
		list_entry= (LIST_ENTRY*)((PUCHAR) pEProc + offset_lEntry);
	
		pEProcNext = (PEPROCESS)((PUCHAR) list_entry->Flink - offset_lEntry);
		i++;
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
