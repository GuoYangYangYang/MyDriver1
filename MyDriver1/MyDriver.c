#include <ntifs.h> 

/*++

��������DRIVER_OBJECT��
	ÿ�������������Ψһ������������֮��Ӧ���������������
�������������ص�ʱ�򣬱��ں��еĶ����������������ġ�
	����������DRIVER_OBJECT���ݽṹ��ʾ������Ϊ������һ��
ʵ�����ں˼��أ����Ҷ�һ������ֻ����һ��ʵ����ȷ�е�˵����
���ں��е�I/O������������صġ�����������Ҫ��DriverEntry��
��ʼ����
	DRIVER_OBJECT���ݽṹ���£�

typedef struct _DRIVER_OBJECT {
	CSHORT Type;
	CSHORT Size;
	PDEVICE_OBJECT DeviceObject;
	ULONG Flags;
	PVOID DriverStart;
	ULONG DriverSize;
	PVOID DriverSection;
	PDRIVER_EXTENSION DriverExtension;
	UNICODE_STRING DriverName;
	PUNICODE_STRING HardwareDatabases;
	PFAST_IO_DISPATCH FastIoDispatch;
	PDRIVER_INITIALIZE DriverInit;
	PDRIVER_STARTIO DriverStartIo;
	PDRIVER_UNLOAD DriverUnload;
	PDRIVER_DISPATCH MajorFunction[IRP_MJ_MAXIMUM_FUNCTION + 1];
} DRIVER_OBJECT;
typedef struct _DRIVER_OBJECT *PDRIVER_OBJECT;

���У�
1.  DeviceObject:
	ÿ�������������һ�������豸�������У�ÿ���豸
������һ��ָ��ָ����һ�������������һ���豸����ָ
��ա��˴���DeviceObjectָ����������ĵ�һ���豸����
ͨ��DeviceObject���Ϳ��Ա�������������������豸����
�豸�������ɳ���Ա�Լ������ģ����ǲ���ϵͳ��ɣ�������
��ж�ص�ʱ�򣬱���ÿ���豸���󣬲�����ɾ����
2.  DriverName:
	����˼�壬DriverName��¼����������������֡�������
UNICODE�ַ�����¼�����ַ���һ��Ϊ\Driver\[������������]��
3.  HardwareDatabase:
	�����¼�����豸��Ӳ�����ݿ����������ͬ����UNICODE
�ַ�����¼�����ַ���һ��Ϊ\REGISTRY\MACHINE\HARDWARE\
DESCRIPTION\SYSTEM��
4.  FastIoDispatch:
	�ļ��������õ�����ǲ������
5.  DriverStartIo:
	��¼StartIO���̵ĺ�����ַ�����ڴ��л�������
6.  DriverUnload:
	ָ������ж��ʱ���õĻص�������ַ��
7.  MajorFunction:
	MajorFunction���¼����һ������ָ�����飬Ҳ����
MajorFunction��һ�����飬�����е�ÿ����Ա��¼��һ��ָ�룬
ÿһ��ָ��ָ�����һ������������������Ǵ���IRP����ǲ������

--*/

/*++

�豸����DEVICE_OBJECT��
	ÿ����������ᴴ��һ�������豸������DEVICE_OBJECT��
�ݽṹ��ʾ��ÿ���豸���󶼻���һ��ָ��ָ����һ���豸�������
���γ�һ���豸�����豸���ĵ�һ���豸����DRIVER_OBJECT�ṹ��
��ָ���ġ��豸���󱣴��豸������״̬����Ϣ��
	DEVICE_OBJECT���ݽṹ���£�

typedef struct _DEVICE_OBJECT {
	struct _DRIVER_OBJECT *DriverObject;
	struct _DEVICE_OBJECT *NextDevice;
	struct _DEVICE_OBJECT *AttachedDevice;
	struct _IRP *CurrentIrp;
	ULONG Flags;
	struct _DEVOBJ_EXTENSION *DeviceObjectExtension;
	ULONG DeviceType;
	int StackSize;
} DEVICE_OBJECT;
typedef struct _DEVICE_OBJECT *PDEVICE_OBJECT;

���У�
1.  DriverObject:
	ָ�����������е���������ͬ����һ���������������
����ָ�����ͳһ��������
2.  NextDevice:
	ָ����һ���豸��������ָ����һ���豸������ͬ����һ
������������豸��Ҳ����ͬһ���������򴴽��������豸����
ÿ���豸�������NextDevice���γ������Ӷ�����ö��ÿ��
�豸����
3.  AttachedDevice:
	ָ����һ���豸��������ָ���ǡ�����и���һ�������
���ӵ����������ʱ��AttachedDeviceָ��ľ����Ǹ�����
һ���������
4.  CurrentIrp:
	��ʹ��StartIO���̵�ʱ�򣬴���ָ����ǵ�ǰIRP�ṹ��
5.  Flags:
	������һ��32λ���޷������͡�ÿһ��λ�о���ĺ��壺
	DO_BUFFERED_IO����д����ʹ�û��巽ʽ��ϵͳ���ƻ���
���������û�ģʽ���ݡ�
	DO_EXCLUSIVE��һ��ֻ����һ���̴߳��豸�����
	DO_DIRECT_IO����д����ʹ��ֱ�ӷ�ʽ���ڴ��������ţ�
�����û�ģʽ���ݡ�
	DO_DEVICE_INITIALIZING���豸�������ڳ�ʼ����
	DO_POWER_PAGABLE��������PASSIVE_LEVEL���ϴ���
IRP_MJ_PNP����
	DO_POWER_INRUSH���豸�ϵ��ڼ���Ҫ�������
6.  DeviceExtension:
	ָ������豸����չ����ÿ���豸����ָ��һ���豸��չ
�����豸��չ�����¼�����豸�Լ����ⶨ��Ľṹ�壬Ҳ����
�ɳ���Ա�Լ�����Ľṹ�塣���⣬�����������У�Ӧ�þ�������
ȫ�ֱ�����ʹ�ã���Ϊȫ�ֱ����漰������ͬ�������⡣�����
�����ǽ�ȫ�ֱ��������豸��չ�
7.  DeviceType:
	ָ���豸�����͡�FILE_DEVICE_FILE_SYSTEMָ��Ϊ��
�ļ�ϵͳ�豸���󡣵����������豸ʱ��Ӧѡ��FILE_DEVICE_UNKOWN
���͵��豸��
8.  StackSize:
	�ڶ����������£�����������֮����γ����ƶ�ջ�Ľṹ��
IRP�����δ���߲㴫�ݵ���ײ㡣StackSize�����ľ������
������

--*/

NTSTATUS DriverEntry(IN PDRIVER_OBJECT pDriverObject, IN PUNICODE_STRING pRegistryPath);
NTSTATUS CreateDevice(IN PDRIVER_OBJECT pDriverObject);
VOID DriverUnload(PDRIVER_OBJECT objDriver);

/*++

����NTʽ������˵����Ҫ�ĺ�����DriverEntry���̡�ж�����̼�����
IRP����ǲ���̡�

--*/

/*++

�������ع�����������ں�����DriverEntry����

	�ͱ�д��ͨӦ�ó���һ�������������и���ں�����Ҳ��������
��ִ�еĺ������������ͨ��������ΪDriverEntry��������һ����
	DriverEntry��Ҫ�Ƕ�����������г�ʼ��������������ϵͳ
���������õġ���Windows���и�����Ľ��̽���ϵͳ���̣���
���̹������������и���ΪSystem�Ľ��̾���ϵͳ���̡�ϵͳ������
ϵͳ������ʱ�򣬾��Ѿ��������ˡ�
	�������ص�ʱ��ϵͳ���������µ��̣߳�����ִ��������е�
���������������һ�����������������������һ��DRIVER_OBJECT
�Ľṹ�塣���⣬ϵͳ���̵���ִ��������е����ù�����򣬲�ѯ��
���������Ӧ��ע����е��
	ϵͳ�̵߳������������DriverEntry����ʱ��ͬʱ����������
�����ֱ���pDriverObject��pRegistryPath������һ����ָ��ղ�
���������������ָ�룬����һ����ָ���豸������ļ����ַ�����
ָ�롣��DriverEntry�У���Ҫ�����Ƕ�ϵͳ���̴��������������
�г�ʼ�������⣬�豸������ļ�����ʱ����Ҫ�����£���Ϊ�����
�������ǳ��ڴ��ڵģ��������غ������ʧ��������Ժ���ʹ�����
UNICODE�ַ����ͱ����Ȱ������Ƶ���ȫ�ĵط���
	����ַ���������һ����\REGISTRY\MACHINE\SYSTEM\ControlSet
\Services\[������]�������������У��ַ�����UNICODE�ַ�����
��ʾ��UNICODE�ǿ��ַ�����ÿ���ַ���16λ��ʾ��

UNICODE�����ݽṹUNICODE_STRING��ʾ��

typedef struct _LSA_UNICODE_STRING {
	USHORT Length;
	USHORT MaximumLength;
	PWSTR Buffer;
} LSA_UNICODE_STRING, *PLSA_UNICODE_STRING, UNICODE_STRING, *PUNICODE_STRING;

���У�
1.  Length: 
	��¼����ַ����ö����ֽڼ�¼������ַ�����N���ַ���
��ôLength������N��2����
2.  MaximumLength:
	��¼buffer�Ĵ�С��Ҳ��������ṹ����ܼ�¼���ֽ�����
MaximumLengthҪ���ڻ����Length��
3.  Buffer:
	��¼�ַ�����ָ�롣��ASCII�ַ�����ͬ��������ַ���
ÿ���ַ�����16λ��

--*/

NTSTATUS DriverEntry(IN PDRIVER_OBJECT pDriverObject, IN PUNICODE_STRING pRegistryPath)
{
	// �������������δ���ò����ľ���
	UNREFERENCED_PARAMETER(pRegistryPath);
	
	// NTSTATUS status;

	// ��ӡһ���ַ���
	KdPrint(("Enter DriverEntry\n"));

	// ע�������������ú������
	pDriverObject->DriverUnload = DriverUnload;

	// ���������豸����
	// status = CreateDevice(pDriverObject);

	KdPrint(("DriverEntry end\n"));

	/*++
		DriverEntry�ķ���ֵ��NTSTATUS�����ݣ�NTSTATUS�Ǳ�
	����Ϊ32λ���޷��ų����͡����������򿪷��У�����ϰ����
	NTSTATUS����״̬������0~0X7FFFFFFF������Ϊ����ȷ��״̬��
	��0X80000000~0XFFFFFFFF������Ϊ�Ǵ����״̬�������и��ǳ�
	���õĺ�NT_SUCCESS�����������״̬�Ƿ���ȷ��
		������NTSTATUSֵ�У�
	#define STATUS_SUCCESS           ((NTSTATUS)0x00000000L)
	#define STATUS_BUFFER_OVERFLOW   ((NTSTATUS)0x80000005L)
	#define STATUS_UNSUCCESSFUL      ((NTSTATUS)0xC0000001L)
	#define STATUS_NOT_IMPLEMENTED   ((NTSTATUS)0xC0000002L)
	#define STATUS_ACCESS_VIOLATION  ((NTSTATUS)0xC0000005L)
	#define STATUS_INVALID_HANDLE    ((NTSTATUS)0xC0000008L)
	#define STATUS_INVALID_PARAMETER ((NTSTATUS)0xC000000DL)
		DriverEntry�ķ���ֵ�����ʾ�ɹ�������ζ�ż���������
	����������ζ�ż�������ʧ�ܣ����ö���������������������
	--*/
	return STATUS_SUCCESS;
}
/*++

	�����Ҫ˵������DriverEntry����������"In"��"In","OUT",
"INOUT"��DDK�ж�������Ϊ�մ������ǵĹ��������ڳ���ע�ͣ�
������һ��"In"����ʱ��Ӧ���϶��ò����Ǵ�����������Ŀ�ġ�
"OUT"��������������������ں��������������"INOUT"���ڼȿ�
�������ֿ�������Ĳ���������DriverEntry���̣�����
DriverObjectָ����IN��������ʹ���߲��ܸı����ָ�뱾��
����ȫ���Ըı���ָ��Ķ���
	��DriverEntry�����У�һ������ж����������IRP����ǲ������
���⻹��һ���ִ��븺�𴴽��豸��������ж�����̺�������ǲ����
���Ƕ�������������á��豸�����е�MajorFunction��һ������ָ
�����飬IRP_MJ_CREATE��IRP_MJ_CLOSE��IRP_MJ_WRITE��������
�ĵڼ���Ԫ�ء�

--*/

/*++

�����豸����
	��NTʽ�������У������豸��������IoCreateDevice�ں˺���
��ɵġ�

NTSTATUS IoCreateDevice (
	IN PDRIVER_OBJECT DriverObject,
	IN ULONG DeviceExtensionSize,
	IN PUNICODE_STRING DeviceName OPTIONAL,
	IN DEVICE_TYPE DeviceType,
	IN ULONG DeviceCharacteristics,
	IN BOOLEAN Exclusive,
	OUT PDEVICE_OBJECT *DeviceObject
	);

���У�
1.  DriverObject:
	���������ÿ�����������С�����Ψһ������������֮��Ӧ����
ÿ����������������ɸ��豸����DriverObjectָ��ľ�������
�����ָ�롣
2.  DeviceExtensionSize:
	���������ָ���豸��չ�Ĵ�С��I/O����������������С����
�ڴ��д����豸��չ�������������������
3.  DeviceName:
	��������������豸��������֡�
4.  DeviceCharacteristics:
	��������������豸��������֡�
5.  Exclusive:
	��������������豸�����Ƿ�Ϊ�ں�ģʽ��ʹ�ã�һ��ΪTRUE��
6.  DeviceObject:
	���������I/O���������𴴽�����豸���󣬲������豸����
�ĵ�ַ��
7.  ����ֵ:
	���ش˺����ĵ���״̬��

	�豸������UNICODE�ַ���ָ���������ַ���������"\Device\
[�豸��]"����ʽ����Windows�µ������豸�������������������ģ�
���磬���̷�����C�̡�D�̡�E�̡�F�̾��Ǳ�����Ϊ"\Device\
HarddiskVolume1"��"\Device\HarddiskVolume2"��"\Device\
HarddiskVolume3"��"\Device\HarddiskVolume4"��
	��ȻҲ���Բ�ָ���豸���֣������IoCreateDevice��û��ָ
���豸��������֣�I/O���������Զ�����һ��������Ϊ�豸���豸
�������磬"\Device\00000001"��"\Deivece\00000002"��
"\Device\00000003"��
	���ָ�����豸����ֻ�ܱ��ں�ģʽ�µ�����������ʶ�𡣵���
���û�ģʽ�µ�Ӧ�ó����޷�ʶ������豸�����û�ģʽ�µ�Ӧ����
ʶ���豸�����ַ�������һ����ͨ�����������ҵ��豸���ڶ�����ͨ
���豸�ӿ��ҵ��豸��
	�������ӿ������Ϊ�豸��������һ�������������豸���������
ֻ�ܱ��ں�ģʽ������ʶ�𣬶�����Ҳ���Ա��û�ģʽ�µ�Ӧ�ó���
ʶ��

--*/

/*++

�����������ӵĺ���IoCreateSymBolicLink���亯���������£�

NTSTATUS IoCreateSymbolicLink(
	IN PUNICODE_STRING SymbolicLinkName,
	IN PUNICODE_STRING DeviceName
);

���У�
1.  SymbolicLinkName:
	����������������ӵ��ַ�������UNICODE�ַ�����ʾ��
2.  DeviceName:
	����������豸���������ַ�������UNICODE�ַ�����ʾ��
3.  ����ֵ:
	���ش������������Ƿ�ɹ���

	���ں�ģʽ�£������������ԡ�\??\����ͷ�ģ������ǡ�\DosDevices��
��ͷ�ģ�����C�̾��ǡ�\??\C:�������ߡ�\DosDevices\C:����������
�û�ģʽ�£������ԡ�\\.\����ͷ�ģ���C�̾��ǡ�\\.\C:����

--*/

/*++

�豸��չ

--*/
typedef struct _DEVICE_EXTENSION {
	PDEVICE_OBJECT pDevice; // ͨ��pDeviceָ���豸����
	UNICODE_STRING ustrDeviceName; // �豸����
	UNICODE_STRING ustrSymLinkName; // ����������
} DEVICE_EXTENSION, *PDEVICE_EXTENSION;

/*++

���У�
1.  pDevice:
	�豸�����е�DeviceExtensionָ���豸��չ��pDevice����
ָ���豸����
2.  ustrDeviceName:
	�豸����
3.  ustrSymLinkName:
	�豸�ķ�����������

	���豸��չ�У���¼���ϼ�����Ϣ���Ա������ص�����������ǲ
����ʹ�á�ʹ�õ�ʱ��ֻ��������豸�л�ȡ�����������´��룺
	pDevExt = (PDEVICE_EXTENSION)pDevObj->DeviceExtension;

--*/

/*++

�豸�Ĵ�������

--*/
NTSTATUS CreateDevice(
	IN PDRIVER_OBJECT pDriverObject)
{
	NTSTATUS status;
	PDEVICE_OBJECT pDevObj;
	PDEVICE_EXTENSION pDevExt;

	// �����豸����
	UNICODE_STRING devName;
	RtlInitUnicodeString(&devName, L"\\Device\\MyDevice");

	// �����豸
	status = IoCreateDevice(pDriverObject,
		sizeof(DEVICE_EXTENSION),
		&devName,
		FILE_DEVICE_UNKNOWN,
		0, TRUE,
		&pDevObj);

	// �ж��豸�����Ƿ񴴽��ɹ�
	if (!NT_SUCCESS(status))
		return status;

	// ���豸����Ϊ�������豸
	pDevObj->Flags |= DO_BUFFERED_IO;

	// �õ��豸��չ
	pDevExt = (PDEVICE_EXTENSION)pDevObj->DeviceExtension;

	// �����豸��չ���豸����
	pDevExt->ustrDeviceName = devName;

	// ������������
	UNICODE_STRING symLinkName;
	RtlInitUnicodeString(&symLinkName, L"\\??\\MyDriver");
	pDevExt->ustrSymLinkName = symLinkName;

	// ������������
	status = IoCreateSymbolicLink(&symLinkName, &devName);

	// �ж��Ƿ�ɹ�������������
	if (!NT_SUCCESS(status))
	{
		// ɾ����������
		IoDeleteDevice(pDevObj);
		return status;
	}

	return STATUS_SUCCESS;
}

/*++

	�����ڴ����豸�����ʱ��ָ�����豸����ΪFILE_DEVICE_UNKNOWN��
˵�����豸�ǳ����豸֮����豸��һ�������豸��ʹ�������Ϊ�豸
���͡�
	�ڴ����豸����󣬶�Flags��DO_BUFFERED_IOλ�������ã�
�����ǽ��豸���óɡ��������豸�������ڽ��豸���óɡ������豸��
���ߡ�ֱ���豸��������趨�豸���豸��չ���豸��չ�ǳ���Ա
�Զ���ġ�

--*/

/*++

DriverUnload����
	
	�����������л�����DriverUnload���̣���������������ж��
��ʱ����á���NTʽ�����У�DriverUnloadһ�㸺��ɾ����DriverEntry
�д������豸���󣬲��ҽ��豸�����������ķ�������ɾ�������⣬
DriverUnload�������һЩ��Դ���л��ա�

--*/

VOID DriverUnload(IN PDRIVER_OBJECT pDriverObject)
{
	// �������������δ���ò����ľ���
	UNREFERENCED_PARAMETER(pDriverObject);

	PDEVICE_OBJECT pNextObj;

	// ʲôҲ������ֻ��ӡһ���ַ���
	KdPrint(("Enter DriverUnload\n"));

	// �õ���һ���豸����
	pNextObj = pDriverObject->DeviceObject;

	// ö�������豸����
	while (pNextObj != NULL)
	{
		// �õ��豸��չ
		PDEVICE_EXTENSION pDevExt = (PDEVICE_EXTENSION)
			pNextObj->DeviceExtension;

		// ɾ����������
		UNICODE_STRING pLinkName = pDevExt->ustrSymLinkName;
		IoDeleteSymbolicLink(&pLinkName);
		pNextObj = pNextObj->NextDevice;

		// ɾ���豸
		IoDeleteDevice(pDevExt->pDevice);
	}

	// ʲôҲ������ֻ��ӡһ���ַ���
	KdPrint(("Finish DriverUnload\n"));
}

/*++

	��DriverUnload�У��������������󡣸����������󣬾Ϳ��Ա���
�����ɸ��������󴴽����豸����ͨ�����������DeviceObject��
���Ϳ����ҵ������豸���������ϴ����У�����������ʵֻ������
һ���豸������ˣ���DriverUnload�ĵ����У�Ҳֻ��ɾ��һ���豸
����
	ɾ���豸����ĺ�����IoDeleteDevice������ԭ���ǣ�
	
VOID IoDeleteDevice(IN PDEVICE_OBJECT DeviceObject);

	���������Ҫ��ɾ�����豸����ָ�롣��DriverUnload�У�����
Ҫɾ���豸����ͬʱ��Ҫ���豸��������ķ������ӽ���ɾ����ɾ��
�������ӵĺ�����IoDeleteSymbolicLink������ԭ���ǣ�

NTSTATUS IoDeleteSymbolicLink(
	IN PUNICODE_STRING SymbolibLinkName
);

���У�
1.  SymbolicLinkName:
	��ʾ�Ѿ���ע���˵ķ������ӡ�
2.  ����ֵ��
	��ʾɾ�����������Ƿ�ɹ���

--*/
