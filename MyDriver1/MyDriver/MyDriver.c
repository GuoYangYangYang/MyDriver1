#include <ntifs.h> 

// 2018-9-3 11:53
// �ڱ���DDK�ṩ������ʱ������ָ��ĳ�����̺�ĳ��ȫ�ֱ�����
// �����ҳ�ڴ滹�ǷǷ�ҳ�ڴ棬��Ҫ�����¶��壺
#define PAGEDCODE code_seg("PAGE")
#define LOCKEDCODE code_seg()
#define INITCODE code_seg("INIT")

#define PAGEDDATA data_seg("PAGE")
#define LOCKEDDATA data_seg()
#define INITDATA data_seg("INIT")

// �û��Զ�������ṹ
typedef struct _MYDATASTRUCT {
	// List Entry��Ҫ��Ϊ_MYDATASTRUCT�ṹ���һ����
	LIST_ENTRY ListEntry;
	// �������Զ��������
	ULONG x;
	ULONG y;
	ULONG number;
} MYDATASTRUCT, *PMYDATASTRUCT;

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
PDRIVER_DISPATCH MyDriverDispatchCreate(); // ��ǲ����
PDRIVER_DISPATCH MyDriverDispatchClose(); // ��ǲ����
PDRIVER_DISPATCH MyDriverDispatchWrite(); // ��ǲ����
PDRIVER_DISPATCH MyDriverDispatchRead(); // ��ǲ����
VOID DriverUnload(IN PDRIVER_OBJECT objDriver);
VOID DisplayItsProcessName();
VOID SomeFunction1();
VOID SomeFunction2();
VOID Test1();
VOID Test2();
VOID Test3();
VOID Test4();
VOID Test5();
NTSTATUS Test6();
VOID Test7();
VOID Test8();
NTSTATUS MyDriverDispatchRoutin(
	IN PDEVICE_OBJECT pDevObj,
	IN PIRP pIrp
);
VOID Test9();
VOID Test10();
VOID Test11();
VOID Test12();
VOID Test13();
VOID Test14();
VOID Test15();
VOID Test16();
VOID Test17();

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

	DisplayItsProcessName();

	// ʵ�麯��1�������������ڴ�
	Test1();

	// ʵ�麯��2������������
	Test2();

	// ʵ�麯��3��������Lookaside
	Test3();

	// ʵ�麯��4������������ʱ����
	Test4();

	// ʵ�麯��5��������try-except
	Test5();

	// ʵ�麯��6��������try-finally
	Test6();

	// ʵ�麯��7���������ַ����ĳ�ʼ��
	Test7();

	// ʵ�麯��8���������ַ����ĳ�ʼ��
	Test8();

	// ʵ�麯��9���������ַ����ĸ���
	Test9();

	// ʵ�麯��10���������ַ����ıȽ�
	Test10();

	// ʵ�麯��11���������ַ����Ĵ�д
	Test11();

	// ʵ�麯��12���������ַ��������ֵ�ת��
	Test12();

	// ʵ�麯��13�������������ַ���֮���ת��
	Test13();

	// ʵ�麯��14���������ļ��Ĵ���
	Test14();

	// ʵ�麯��15���������ļ��Ĵ�
	Test15();

	// ʵ�麯��16���������ļ��Ĵ�2
	Test16();

	// ʵ�麯��17���������ļ��Ĳ�ѯ���޸��ļ�����
	Test17();

	// ������ǲ����
	pDriverObject->MajorFunction[IRP_MJ_CREATE] = MyDriverDispatchRoutin;
	// pDriverObject->MajorFunction[IRP_MJ_CLOSE];
	// pDriverObject->MajorFunction[IRP_MJ_WRITE];
	// pDriverObject->MajorFunction[IRP_MJ_READ];

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

/*++

Windows��������ͽ��̵Ĺ�ϵ

����������Կ�����һ�������DLL�ļ���Ӧ�ó�����ص�����
�ڴ��У�ֻ�������ص�ַ���ں�ģʽ��ַ���������û�ģʽ��ַ����
�ܷ��ʵ�ֻ��������̵������ڴ棬�������������̵������ַ����
Ҫָ�����ǣ�Windows����������Ĳ�ͬ���������ڲ�ͬ�Ľ����С�
DriverEntry���̺�AddDevice������������ϵͳ��System������
�С����������Windows�зǳ��Ľ��̣�Ҳ��Windows��һ�����е�
���̡�����Ҫ���ص�ʱ����������л���һ���߳̽��������ص���
��ģʽ��ַ�ռ��ڣ�������DriverEntry���̡�
��������һЩ���̣����磬IRP_MJ_READ��IRP_MJ_WRITE����
ǲ������������Ӧ�ó���ġ������ġ��С���ν�����ڽ��̵ġ������ġ�
�У�ָ����������ĳ�����̵Ļ����У����ܷ��ʵ������ַ�������
�̵������ַ��
�ڴ����д�ӡһ��log��Ϣ��������Ϣ��ӡ����ǰ���̵Ľ�������
�����ǰ�����Ƿ���I/O����Ľ��̣���˵���ڽ��̵ġ������ġ��С�
���溯��������ʾ��ǰ���̵Ľ�������

--*/

VOID DisplayItsProcessName()
{
	// �õ���ǰ���̣�PsGetCurrentProcess�����ǵõ���ǰ���еĽ���
	PEPROCESS pEProcess = PsGetCurrentProcess();
	// �õ���ǰ��������
	PTSTR ProcessName = (PTSTR)((ULONG)pEProcess + 0x174);
	KdPrint(("%s\n", ProcessName));
}

/*++

��ҳ��Ƿ�ҳ�ڴ�

Windows�涨��Щ�����ڴ�ҳ���ǿ��Խ������ļ��У������ڴ�
����Ϊ��ҳ�ڴ档����Щ�����ڴ�ҳ��Զ���ύ�����ļ��У���Щ��
�汻��Ϊ�Ƿ�ҳ�ڴ档
��������ж�������DISPATCH_LEVEL֮��ʱ������DISPATCH_LEVEL
�㣩������ֻ��ʹ�÷Ƿ�ҳ�ڴ棬���򽫵�������������


--*/

/*--

�ڱ���DDK�ṩ������ʱ������ָ��ĳ�����̺�ĳ��ȫ�ֱ�����
�����ҳ�ڴ滹�ǷǷ�ҳ�ڴ棬��Ҫ�����¶��壺

#define PAGEDCODE code_seg("PAGE")
#define LOCKEDCODE code_seg()
#define INITCODE code_seg("INIT")

#define PAGEDDATA data_seg("PAGE")
#define LOCKEDDATA data_seg()
#define INITDATA data_seg("INIT")

--*/

/*++

�����ĳ���������뵽��ҳ�ڴ��У�������Ҫ�ں�����ʵ����
�������´��룺

#pragma PAGEDCODE
VOID SomeFunction1()
{
PAGED_CODE();
// do somethings
}

���У�PAGED_CODE()��DDK�ṩ�ĺ꣬��ֻ��check�汾����Ч��
���������������Ƿ����е���DISPATCH_LEVEL���ж����󼶣����
���ڻ��������ж����󼶣�������һ�����ԡ�

--*/

#pragma PAGEDCODE
VOID SomeFunction1()
{
	PAGED_CODE();
	KdPrint(("IN PAGEDCODE\n"));
	// do somethings
}

/*++

�����ĳ���������뵽�Ƿ�ҳ�ڴ��У�������Ҫ�ں�����ʵ���м�
�����´��룺

#pragma LOCKEDCODE
VOID SomeFunction2()
{
// do somethings
}

--*/

#pragma LOCKEDCODE
VOID SomeFunction2()
{
	KdPrint(("IN PAGEDCODE\n"));
	// do somethings
}

VOID Test1()
{
	KdPrint(("Test1��������beginning\n"));
	SomeFunction1();
	SomeFunction2();
	KdPrint(("Test1��������finishing\n"));
}

/*++

����һ���������������ĳ��������Ҫ�ڳ�ʼ����ʱ�������ڴ棬
Ȼ��Ϳ��Դ��ڴ���ж�ص����������ָ��������DriverEntry���
�£�������NTʽ��������DriverEntry��ܳ���ռ�ݺܴ�Ŀռ䣬Ϊ
�˽�ʡ�ڴ棬��Ҫ��ʱ�ش��ڴ���ж�ص����������£�

#pragma INITCODE
extern "C" NTSTATUS DriverEntry(
IN PDRIVER_OBJECT pDriverObject,
IN PUNICODE_STRING RegistryPath)
{
// do somethings
}

--*/

/*++

�����ں��ڴ�

Windows��������ʹ�õ��ڴ���Դʮ����󣬷����ڴ�ʱҪ����
��Լ����Ӧ�ó���һ�����ֲ������Ǵ����ջ��Stack���ռ��еġ�
��ջ�ռ䲻����Ӧ�ó�����ô���������������ʺϵݹ���û���
�ֲ������Ǵ��ͽṹ�塣�����Ҫ���ͽṹ�壬����Ҫ�ڶѣ�Heap��
�����롣
���������ڴ�ĺ��������¼�����ԭ�����£�

PVOID ExAllocatePool (
IN POOL_TYPE PoolType,
IN SIZE_T NumberOfBytes
);

PVOID ExAllocatePoolWithTag (
IN POOL_TYPE PoolType,
IN SIZE_T NumberOfBytes,
IN ULONG Tag
);

PVOID ExAllocatePoolWithQuota (
IN POOL_TYPE PoolType,
IN SIZE_T NumberOfBytes
);

PVOID ExAllocatePoolWithQuotaTag (
IN POOL_TYPE PoolType,
IN SIZE_T NumberOfBytes,
IN ULONG Tag
);

����PoolType�Ǹ�ö�ٱ����������ֵΪNonPagedPool�����
��Ƿ�ҳ�ڴ档�����ֵΪPagedPool��������ڴ�Ϊ��ҳ�ڴ档
NumberOfBytes�Ƿ����ڴ�Ĵ�С�����Ϊ4�ı�����
����ֵ������ڴ��ַ��һ�����ں�ģʽ��ַ���������0����
�������ʧ�ܡ�

--*/

/*++

����ṹ

DDK�ṩ�˱�׼��˫������˫��������Խ������γ�һ������BLINK
ָ��ָ��ǰһ��Ԫ�أ�FLINKָ��ָ����һ��Ԫ�ء�
������DDK�ṩ��˫����������ݽṹ��

typedef struct _LIST_ENTRY {
struct _LIST_ENTRY *Flink;
struct _LIST_ENTRY *Blink;
} LIST_ENTRY, *PLIST_ENTRY;

--*/

/*++

�����ʼ��

ÿ��˫����������һ������ͷ��Ϊ����ĵ�һ��Ԫ�ء�����ʹ��
����ͷ��Ҫ���г�ʼ������Ҫ������ͷ��Flink��Blink����ָ�붼ָ��
�Լ�������ζ������ͷ����������ǿ�������ʼ������ͷ��InitializeListHead
��ʵ�֡�
����ж������Ƿ�Ϊ�գ������ж�����ͷ��Flink��Blink�Ƿ�ָ
���Լ���DDKΪ����Ա�ṩ��һ��������ּ�飨IsLIstEmpty��
����Ա��Ҫ�Լ�����������ÿ��Ԫ�ص��������ͣ�����LIST_ENTRY
�ṹ��Ϊ�Զ���ṹ��һ������LIST_ENTRY�������ǽ��Զ����
���ݽṹ����һ������

typedef struct _MYDATASTRUCT {
// List Entry��Ҫ��Ϊ_MYDATASTRUCT�ṹ���һ����
LIST_ENTRY ListEntry;
// �������Զ��������
ULONG x;
ULONG y;
} MYDATASTRUCT, *PMYDATASTRUCT;

--*/

/*++

���ײ���������

������Ĳ��������ַ�ʽ��һ�����������ͷ�����룬һ������
�����β�����롣
��ͷ����������ʹ�����InsertHeadList���÷����£�
InsertHeadList(&head, &mydata->ListEntry);
���У�head��LIST_ENTRY�ṹ������ͷ��mydata���û������
���ݽṹ������������ListEntry�ǰ������е�LIST_ENTRY���ݽṹ

--*/

/*++

��β����������

����һ�ֲ��뷽�����������β�����в��롣��β����������ʹ
��InsertTailList���÷����£�
InsertTailList(&head, &mydata->ListEntry);
���У�head��LIST_ENTRY�ṹ������ͷ��mydata���û������
���ݽṹ������������ListEntry�ǰ������е�LIST_ENTRY���ݽṹ

--*/

/*++

������ɾ��

�Ͳ�������һ����ɾ��Ҳ�����ֶ�Ӧ�ķ�����һ���Ǵ�����ͷ��
ɾ����һ���Ǵ�����β��ɾ�����ֱ��ӦRemoveHeadList��
RemoveTailList��������ʹ�÷������£�
PLIST_ENTRY pEntry = RemoveHeadList(&head);
��
PLIST_ENTRY pEntry = RemoveTailList(&head);
���У�head������ͷ��pEntry�Ǵ�����ɾ��������Ԫ���е�
ListEntry�����������������
1. ���Զ�һ�����ݽṹ�ĵ�һ���ֶ���LIST_ENTRYʱ���磺
typedef struct _MYDATASTRUCT {
LIST_ENTRY ListEntry;
ULONG x;
ULONG y;
} MYDATASTRUCT, *PMYDATASTRUCT;
��ʱ��RemoveHeadList���ص�ָ����Ա������û��Զ����
ָ�룬����
PLIST_ENTRY pEntry = RemoveHeadList(&head);
PMYDATASTRUCT pMyData = (PMYDATASTRUCT) pEntry;
2. ���Զ�������ݽṹ�ĵ�һ���ֶβ���LIST_ENTRYʱ������
typedef struct _MYDATASTRUCT {
ULONG x;
ULONG y;
LIST_ENTRY ListEntry;
} MYDATASTRUCT, *PMYDATASTRUCT;
��ʱ��RemoveHeadList���ص�ָ�벻���Ե����û��Զ����
ָ�룬��ʱ��Ҫͨ��pEntry�ĵ�ַ��������Զ������ݵ�ָ�롣һ��
ͨ��pEntry���Զ��������е�ƫ��������pEntry��ȥ���ƫ��������
��õ��û��Զ���ṹ��ָ��ĵ�ַ��DDK���ṩ�˺�
CONTEAINING_RECORD�����÷����£�
PLIST_ENTRY pEntry = RemoveHeadList(&head);
PIRP pIrp = CONTAINING_RECORD(pEntry,
MYDATASTRUCT,
ListEntry);
DDK�����Զ������ݽṹ���۵�һ���ֶ��Ƿ�ΪListEntry����
�ö�ʹ��CONTAINING_RECORD��õ��Զ������ݽṹ��ָ�롣

--*/



VOID Test2()
{
	KdPrint(("Test2��������beginning\n"));
	LIST_ENTRY linkListHead;
	// ��ʼ������
	InitializeListHead(&linkListHead);

	PMYDATASTRUCT pData;
	ULONG i = 0;
	// �������в���10��Ԫ��
	KdPrint(("Begin insert to link list"));
	for (i = 0; i < 10; i++)
	{
		// �����ҳ�ڴ�
		pData = (PMYDATASTRUCT)
			ExAllocatePool(PagedPool, sizeof(MYDATASTRUCT));
		pData->number = i;
		// ��ͷ����������
		InsertHeadList(&linkListHead, &pData->ListEntry);
	}

	// ��������ȡ��������ʾ
	KdPrint(("Begin remove from link list\n"));
	while (!IsListEmpty(&linkListHead))
	{
		// ��β��ɾ��һ��Ԫ��
		PLIST_ENTRY pEntry = RemoveTailList(&linkListHead);
		pData = CONTAINING_RECORD(pEntry,
			MYDATASTRUCT,
			ListEntry);
		// KdPrint(("%d\n", pData->number));
		ExFreePool(pData);
	}
	KdPrint(("Test2��������finishing\n"));
}

/*++

ʹ��Lookaside

�������������ҪƵ���ش��ڴ������롢���չ̶���С���ڴ棬
��Ӧ��ʹ��Lookaside����
Lookaside��һ���Զ����ڴ����������ͨ����Lookaside����
�����ڴ棬Ч��Ҫ����ֱ����Windows�����ڴ档һ��������ʹ�ã�
1. ����Աÿ������̶���С���ڴ档
2. ����ͻ��յĲ���ʮ��Ƶ����
ʹ��Lookaside��������Ҫ��ʼ��Lookaside���������º�����
VOID ExInitializeNPagedLookasideList (
IN PNPAGED_LOOKASIDE_LIST Lookaside,
IN PALLOCATE_FUNCTION Allocate OPTIONAL,
IN PEREE_FUNCTION Free OPTIONAL,
IN ULONG Flags,
IN SIZE_T Size,
IN ULONG Tag,
IN USHORT Depth
);
��
VOID ExInitializePagedLookasideList (
IN PPAGED_LOOKASIDE_LIST Lookaside,
IN PALLOCATE_FUNCTION Allocate OPTIONAL,
IN PFREE_FUNCTION Free OPTIONAL,
IN ULONG Flags,
IN SIZE_T Size,
IN ULONG Tag,
IN USHORT Depth
);
�����������ֱ��ǶԷǷ�ҳ�ͷ�ҳLookaside������г�ʼ����

�ڳ�ʼ����Lookaside����󣬿��Խ��������ڴ�Ĳ����ˣ�
PVOID ExAllocateFromNPagedLookasideList (
IN PNPAGED_LOOKASIDE_LIST Lookaside
);
��
PVOID ExAllocateFromPagedLookasideList (
IN PPAGED_LOOKASIDE_LIST Lookaside
);
�����������ֱ��ǶԷǷ�ҳ�ڴ�ͷ�ҳ�ڴ�����롣

��Lookaside��������ڴ�Ĳ���������������������
VOID ExFreeToNPagedLookasideList (
IN PNPAGED_LOOKASIDE_LIST Lookaside,
IN PVOID Entry
);
��
VOID ExFreeToPagedLookaside (
IN PPAGED_LOOKASIDE_LIST Lookaside,
IN PVOID Entry
);
�����������ֱ��ǶԷǷ�ҳ�ڴ�ͷ�ҳ�ڴ�Ļ��ա�

��ʹ����Lookaside�������Ҫɾ��Lookaside����������
����������
VOID ExDeleteNPagedLookasideList (
IN PNPAGED_LOOKASIDE_LIST Lookaside
);
��
VOID ExDeletePagedLookasideList (
IN PPAGED_LOOKASIDE_LIST Lookaside
);
�����������ֱ��ǶԷǷ�ҳ�ͷ�ҳLookaside�����ɾ����

--*/

VOID Test3()
{
	KdPrint(("Test3��������beginning\n"));

	// ��ʼ��Lookaside����
	PAGED_LOOKASIDE_LIST pageList;
	ExInitializePagedLookasideList(&pageList, NULL,
		NULL, 0, sizeof(MYDATASTRUCT), '1234', 0);
	INT ARRAY_NUMBER = 50;
	PMYDATASTRUCT MyObjectArray[50];

	// ģ��Ƶ�������ڴ�
	for (int i = 0; i < ARRAY_NUMBER; i++)
	{
		MyObjectArray[i] = (PMYDATASTRUCT)
			ExAllocateFromPagedLookasideList(&pageList);
	}

	// ģ��Ƶ�������ڴ�
	for (int i = 0; i < ARRAY_NUMBER; i++)
	{
		ExFreeToPagedLookasideList(&pageList,
			MyObjectArray[i]);
		MyObjectArray[i] = NULL;
	}

	// ɾ��Lookaside����
	ExDeletePagedLookasideList(&pageList);

	KdPrint(("Test3��������finishing\n"));
}

/*++

����ʱ����

һ����������̣��ڷ������������ͬʱ���Ὣ����ʱ����һ��
�������û�������ʱ�����ǳ������е�ʱ��ز����ٵģ����ɱ�����
�ṩ����Բ�ͬ�Ĳ���ϵͳ������ʱ������ʵ�ַ�����ͬ�����ӿ�
��������һ�¡�

1. �ڴ�临�ƣ����ص���

���������򿪷��У������õ��ڴ�ĸ��ơ�DDK�ṩ���º�����

VOID RtlCopyMemory (
IN VOID UNALIGNED *Destination,
IN CONST VOID UNALIGNED *Source,
IN SIZE_T Length
);

Destination: ��ʾҪ�����ڴ��Ŀ�ĵ�ַ��
Source: ��ʾҪ�����ڴ��Դ��ַ��
Length: ��ʾҪ�����ڴ�ĳ��ȣ���λ���ֽڡ�
�͸ú����������ƵĻ���RtlCopyBytes�������������Ĳ�����ȫ
һ��������Ҳ��ȫһ����ֻ���ڲ��õ�ƽ̨���в�ͬ��ʵ�֡�

2. �ڴ�临�ƣ����ص���

��RtlCopyMemory���Ը����ڴ棬�����ڲ�û�п����ڴ��ص���
����������εȳ����ڴ棬ABCD�������Ҫ��A��C�ε��ڴ渴�Ƶ�B
��D����ڴ��ϣ���ʱB��C�ε��ڴ�����ص��Ĳ��֡�
RtlCopyMemory�������ڲ�ʵ�ַ���������memory����ʵ�ֵġ�
����C99���壬memoryû�п����ص��Ĳ��֣���������ܱ�֤�ص���
���Ƿ񱻸��ơ�
Ϊ�˱�֤�ص�����Ҳ����ȷ���ƣ�C99�涨memmove���������
��������������������ڴ��Ƿ��ص��������жϣ���ȴ�������ٶȡ�
�������Ա��ȷ�����Ƶ��ڴ�û���ص�����ѡ��ʹ��memopy������
������ܱ�֤�ڴ��Ƿ��ص�����ʹ��memmove������ͬ����Ϊ�˱�֤��
��ֲ�ԣ�DDK�ú��memmove�����˷�װ�����Ʊ�ΪRtlMoveMemory��

VOID RtlMoveMemory (
IN VOID UNALIGNED *Destination,
IN CONST VOID UNALIGNED *Source,
IN SIZE_T Length
);

Destination: ��ʾҪ�����ڴ��Ŀ�ĵ�ַ��
Source: ��ʾҪ�����ڴ��Դ��ַ��
Length: ��ʾҪ�����ڴ�ĳ��ȣ���λ���ֽڡ�

3. ����ڴ�

�������򿪷��У��������õ���ĳ���ڴ������ù̶��ֽ���䡣
DDKΪ����Ա�ṩ�˺���RtlFillMemory��ʵ�ʵĺ���ʱmemset����

VOID RtlFillMemory (
IN VOID UNALIGNED *Destination,
IN SIZE_T Length,
IN UCHAR Fill
);

Destination: Ŀ�ĵ�ַ��
Length: ���ȡ�
Fill: ��Ҫ�����ֽڡ�

���������򿪷��У�������Ҫ��ĳ���ڴ����㣬DDK�ṩ�ĺ���
RtlZeroBytes��RtlZeroMemory��

VOID RtlZeroMemory (
IN VOID UNALIGNED *Destination,
IN SIZE_T Length
);

Destination: Ŀ�ĵ�ַ��
Length: ���ȡ�

4. �ڴ�Ƚ�

�������򿪷��У������õ��Ƚ������ڴ��Ƿ�һ�¡��ú�����
RtlCompareMemory���������ǣ�

ULONG RtlEqualMemory (
CONST VOID *Source1,
CONST VOID *Source2,
SIZE_T Length
);

Source1: �Ƚϵĵ�һ���ڴ��ַ��
Source2: �Ƚϵĵڶ����ڴ��ַ��
Length: �Ƚϵĳ��ȣ���λΪ�ֽڡ�
����ֵ: ��ȵ��ֽ�����

RtlEqualMemoryͨ���жϷ���ֵ��Length�Ƿ���ȣ����ж���
���ڴ��Ƿ���ȫһ�¡��������ڴ�һ�µ�����·��ط���ֵ���ڲ�һ
�µ�����·����㡣

--*/

#define BUFFER_SIZE 1024
#pragma INITCODE
VOID Test4()
{
	KdPrint(("Test4��������beginning\n"));

	PUCHAR pBuffer = (PUCHAR)ExAllocatePool(PagedPool,
		BUFFER_SIZE);

	// ��������ڴ�
	RtlZeroMemory(pBuffer, BUFFER_SIZE);
	PUCHAR pBuffer2 = (PUCHAR)ExAllocatePool(PagedPool,
		BUFFER_SIZE);

	// �ù̶��ֽ�����ڴ�
	RtlFillMemory(pBuffer2, BUFFER_SIZE, 0xAA);

	// �ڴ渴��
	RtlCopyMemory(pBuffer, pBuffer2, BUFFER_SIZE);

	// �ж��ڴ��Ƿ�һ��
	ULONG ulRet = RtlCompareMemory(pBuffer, pBuffer2, BUFFER_SIZE);
	if (ulRet == BUFFER_SIZE)
	{
		KdPrint(("The two blocks are same.\n"));
	}

	KdPrint(("Test4��������finishing\n"));
}

/*++

ʹ��C++���Է����ڴ�

΢�������û���ṩ�ں�ģʽ�µ�newģʽ����Ӧ�ö�new��
delete�������������ء�
����new��delete�������������ַ�����һ����ȫ�����أ�һ��
�����������ء�

--*/

/*++

��������

��C�����У�����������8λ��16λ��32λ�������ͣ�����DDK��
��������һ��64λ�ĳ�����������64λ������ֻ���޷�����ʽ����
LONGLONG���ͱ�ʾ��64λ�������ĳ���ǰ����һ�����֣��������
i64��β���磺

LONGLONG llvalue = 100i64;

����LONGLONG���⣬DDK���ṩ��һ��64λ�����ı�ʾ��������
LARGE_INTEGER���ݽṹ����������LONGLONG�ǻ��������ݣ���
LARGE_INTEGER�����ݽṹ��LARGE_INTEGER��������:

typedef union _LARGE_INTEGER {
struct {
ULONG LowPart;
LONG HighPart;
};
struct {
ULONG LowPart;
LONG HighPart;
} u;
LONGLONG QuadPart;
} LARGE_INTEGER;

���Ǹ������壬�������е�����Ԫ�ؿ�����Ϊ��LARGE_INTEGER
���������塣

1. LARGE_INTEGER������Ϊ��������������ɡ�һ���ǵ�32λ����
��HighPart��һ���Ǹ�32λ������HighPart����little endian��
����£���32λ������ǰ����32λ�����ں󡣸�ֵ100���£�

LARGE_INTEGER LargeValue;
LargeValue.LowPart = 100;
LargeValue.HighPart = 0;

2. LARGE_INTEGER������Ϊ��������������ɣ�һ���ǵ�32λ����
��HighPart��һ���Ǹ�32λ������HighPart����big endian��
����£���32λ������ǰ����32λ�����ں󡣸�ֵ100���£�

LARGE_INTEGER LargeValue;
LargeValue.u.LowPart = 100;
LargeValue.u.HighPart = 0;

3. LARGE_INTEGER�ȼ���LONGLONG���ݡ�����������£������ֵ
100�����£�

LARGE_INTEGER LargeValue;
LargeValue.QuadPart = 100i64;

--*/

/*++

����״ֵ̬

DDK�󲿷ֺ����ķ���ֵ������NTSTATUS���͡��ο�DDK.h�ļ���
NTSTATUS������LONG�ȼۡ�

--*/

/*++

����ڴ������

���������򿪷��У����ڴ�Ĳ���Ҫ����С�ģ����ĳ���ڴ���
ֻ���ġ�������������ͼȥд�������ͻᵼ��ϵͳ�ı�����ͬ������
ĳ���ڴ��ǲ��ɶ�������£�����������ͼȥ����ͬ���ᵼ��ϵͳ��
������
DDK�ṩ�����ֺ��������ڲ�֪��ĳ���ڴ��Ƿ�ɶ�д������£�
��̽����ڴ�ɶ�д�ԡ������������ֱ���ProbeForRead��
ProbeForWrite��

VOID ProbeForRead (
IN CONST VOID *Address,
IN SIZE_T Length,
IN ULONG Alignment
);

Address: ��Ҫ�������ڴ�ĵ�ַ��
Length: ��Ҫ�������ڴ�ĳ��ȣ���λ���ֽڡ�
Alignment: �����ö��ڴ����Զ����ֽڶ���ġ�

VOID ProbeForWrite (
IN CONST VOID *Address,
IN SIZE_T Length,
IN ULONG Alignment
);

Address: ��Ҫ�������ڴ�ĵ�ַ��
Length: ��Ҫ�������ڴ�ĳ��ȣ���λ���ֽڡ�
Alignment: �����ö��ڴ����Զ����ֽڶ���ġ�
�������������Ƿ��ظö��ڴ��Ƿ�ɶ�д�����ǵ����ɶ�д��ʱ��
������һ���쳣������쳣��Ҫ�õ�΢��������ṩ�ġ��ṹ���쳣��
����취��

--*/

/*++

�ṹ���쳣����try-except�飩

�ṹ���쳣������΢��������ṩ�Ķ��ش�����ƣ����ִ���ʽ
����һ���������ڳ��ִ��������£����ڳ��������

1. �쳣: �쳣�ĸ����������жϵĸ����������ĳ�δ��󴥷�һ��
�쳣������ϵͳ��Ѱ�Ҵ�������쳣�Ĵ���������������ṩ����
�������������������������û���ṩ�����������ɲ���
ϵͳ��Ĭ�ϴ��������������ں�ģʽ�£�����ϵͳĬ�ϴ����
��ķ��������ܼ򵥣�ֱ����ϵͳ���������������ϼ����������
��Ϣ��֮��ϵͳ�ͽ�������״̬��

2. �ؾ�: ����ִ�е�ĳ���ط������쳣����ʱ��ϵͳ��Ѱ�ҳ����
�Ƿ���һ��try{}�У�������try�ṩ���쳣������롣�����ǰ
try��û���ṩ�쳣������������һ���try�飬Ѱ���쳣����
���롣ֱ�������try{}��Ҳû���ṩ�쳣������룬���ɴӲ���
ϵͳ����

���������һ��Ѱ���쳣����Ļ��ƣ�����Ϊ�ؾ�һ�㴦��
�쳣����ͨ��try-except��������ġ�

__try
{
}
__except(filter_value)
{
}

�ڱ�__try{}��Χ�Ŀ��У���������쳣�������filter_value
����ֵ���ж��Ƿ���Ҫ��__except{}�д���filter_value����ֵ
�������ֿ��ܡ�

1. EXCEPTION_EXECUTE_HANDLER������ֵΪ1�����뵽__except
���д����������겻�ٻص�__try{}���У�תΪ�������С�

2. EXCEPTION_CONTINUE_SEARCH������ֵΪ0����ʹ��__except
���е��쳣����ת������һ��ؾ�����Ѿ�������㣬�������
ϵͳ�����쳣��������

3. EXCEPTION_CONTINUE_EXECUTION������ֵΪ-1.�ظ���ǰ����
��ָ���������õ���

ProbeForRead��ProbeForWrite�������Ժ�try-except��
��ϣ��������ĳ���ڴ��Ƿ�ɶ�д��

--*/

VOID Test5()
{
	PVOID badPointer = NULL;

	KdPrint(("Test5��������beginning\n"));

	__try
	{
		KdPrint(("Enter __try block\n"));

		// �жϿ�ָ���Ƿ�ɶ�����Ȼ�ᵼ���쳣
		ProbeForWrite(badPointer, 100, 4);

		// ���������������쳣�������Ժ���䲻�ᱻִ��
		KdPrint(("Leave __try block\n"));
	}

	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		KdPrint(("Catch the exception\n"));
		KdPrint(("The program will keep going\n"));
	}

	// �����ᱻִ��
	KdPrint(("Test5��������finishing\n"));

}

/*++

�ṹ���쳣����try-finally�飩

�ṹ���쳣����������һ��ʹ�÷�������������try-finally
�飬ǿ�Ⱥ������˳�ǰִ��һ�δ��롣

--*/

NTSTATUS Test6()
{
	KdPrint(("Test6��������beginning\n"));

	__try
	{
		KdPrint(("Enter __try block\n"));
		// do somethings
		KdPrint(("Leave __try block\n"));
		return STATUS_SUCCESS;
	}

	__finally
	{
		KdPrint(("Enter __finally block\n"));
		// do somethings
		KdPrint(("Leave __fianlly block\n"));
		KdPrint(("Test6��������finishing\n"));
	}

}

/*++

��������__try{}���У���������ʲô���루��ʹ��return
�����ߴ����쳣�����ڳ����˳�ǰ��������__finally{}���е�
���롢������Ŀ���ǣ����˳�ǰ��Ҫ����һЩ��Դ���յĹ�������
��Դ���մ�������λ�þ��Ƿ���������С�
����֮�⣬ʹ��try-finally�黹����ĳ�̶ֳ��ϼ򻯴��롣

--*/

/*++

ʹ�ú���Ҫע��ĵط�

DDK�ṩ�˴����ĺ꣬��ʹ����Щ���ʱ����Ҫ���⡰��Ч����

1. ����if��while��for��������䣬��ʡ��{}������������ȫ��֤
�����֡���Ч������

2. �ڱ�д���к��ʱ���ں��ǰ�����{}���磺
#define PRINT(mag ) {\
KdPrint (("===================\n"));\
KdPrint (mag);\
KdPrint (("===================\n"));\
}

--*/

/*++

�ں��µ��ַ�������

��Ӧ�ó���һ��������������Ҫ�������ַ����򽻵������а���
ASCII�ַ��������ַ���������DDK�����ANSI_STRING���ݽṹ��
UNICODE_STRING���ݽṹ��

--*/

/*++

ASCII�ַ����Ϳ��ַ���

��Ӧ�ó����У�����ʹ�������ַ���һ����char�͵��ַ�����
�����¼ANSI�ַ���������ָ��һ��char�����ָ�룬ÿ��char��
�����Ĵ�СΪһ���ֽڣ��ַ�������0��־�ַ�������������һ����
wchar_t�����ָ�룬wchar_t�ַ���СΪ�����ֽڣ��ַ�����0��־
�ַ���������
ANSI�ַ��Ĺ������£�
char* str1 = "abc";
str1ָ��ָ���������61 626 300
UNICODE�ַ��Ĺ������£�
wchar_t *str2 = L"abc";
str2ָ��ָ���������6100 620 063 000 000���ڹ����ַ���
��ʱ��ʹ��һ���ؼ��֡�L"�����������Զ���������Ҫ�Ŀ��ַ���
���������򿪷��У�DDK��char��wchar_t����滻��CHAR��
WCHAR���
������������ַ�����DDK�ṩ��Ӧ���ַ����������������磬
strpy��sprintf��strcat��strlen�ȡ���΢��˾������ֱ��ʹ��
��Щ������ȡ����֮����ʹ��ͬ�����ܵĺꡣ

--*/

/*++

ANSI_STRING�ַ�����UNICODE_STRING�ַ���

DDK����������Աʹ��C���Ե��ַ�������Ҫ����Ϊ����׼C����
�������������׵��»���������ȴ����������Ա�����ַ�����
���Ƚ��м��飬�����׵���������󣬴Ӷ�������������ϵͳ�ı�����
DDK��������Աʹ��DDK�Զ�����ַ������������ݸ�ʽ�Ķ������£�

typedef struct _STRING {
USHORT Length;
USHORT MaximumLength;
PCHAR Buffer;
} STRING;
typedef STRING ANSI_STRING;
typedef PSTRING PANSI_STRING;

typedef STRING OEM_STRING;
typedef PSTRING POEM_STRING;

������ݽṹ��ASCII�ַ��������˷�����
Length: �ַ��ĳ��ȡ�
MaximumLength: �����ַ�������������󳤶ȡ�
Buffer: ��������ָ�롣

�ͱ�׼���ַ�����ͬ��STRING�ַ���������0��־�ַ��������ַ�
��������Length�ֶΡ��ڱ�׼C�е��ַ����У����������������N��
��ôֻ������N-1���ַ����ַ�����������ΪҪ��һ���ֽڴ洢NULL��
����STRING�ַ����У��������Ĵ�СMaximumLength�������ַ���
���ȿ�����MaximumLength��������MaximumLength-1��
��ANSI_STRING���Ӧ��DDK�����ַ�����װ��UNICODE_STRING
���ݽṹ��

typedef struct _UNICODE_STRING {
USHORT Length;
USHORT MaximumLength;
PWSTR Buffer;
} UNICODE_STRING;

Length: �ַ��ĳ��ȡ���λ���ֽڡ������N���ַ�����ôLength
����N��2����
MaximumLength: �����ַ�������������󳤶ȣ���λҲ���ֽڡ�
Buffer: ��������ָ�롣

��ANSI_STRING��ͬ��UNICODE_STRING�Ļ������Ǽ�¼���ַ�
�Ļ�������ÿ��Ԫ���ǿ��ַ�����ANSI_STRINGһ�����ַ����Ľ���
������NULLΪ��־�����������ֶ�Length��

ANSI_STRING�ַ�����UNICODE_STRING�ַ�����KdPrintͬ��
�ṩ�˴�ӡlog�ķ�����

ANSI_STRING ansiString;
// ʡȥ��ʼ��
KdPrint("%Z\n", &ansiString); // ע����%Z

������ӡһ�ο��ַ���ʱ����Ҫ�������²�����

UNICODE_STRING uniString;
// ʡȥ��ʼ��
KdPrint("%wZ\n", &uniString); // ע����%wZ

--*/

/*++

�ַ���ʼ��������

ANSI_STRING�ַ�����UNICODE_STRING�ַ���ʹ��ǰ��Ҫ����
��ʼ���������ְ취����������ݽṹ��

1. ����һ����ʹ��DDK�ṩ����Ӧ�ĺ���

��ʼ��ANSI_STRING�ַ���

VOID RtlInitAnsiString (
IN OUT PANSI_STRING DestinationString,
IN PCSZ SourceString
);

DestinationString: Ҫ��ʼ����ANSI_STRING�ַ�����
SourceString: �ַ��������ݡ�

��ʼ��UNICODE_STRING�ַ���

VOID RtlInitUnicodeString (
IN OUT PUNCODE_STRING DestinationString,
IN PCWSTR SourceString
);

DestinationString: Ҫ��ʼ����UNICODE_STRING�ַ�����
SourceString: �ַ��������ݡ�

��RtlInitAnsiStringΪ������ʹ�÷����ǣ�

ANSI_STRING AnsiString1;
CHAR* string1 = "hello";
RtlInitAnsiString(&AnsiString, string1);

--*/


/*++

��ǲ����

��ǲ������Windows���������е���Ҫ��������������Ҫ��
���Ǹ�����I/O�������д󲿷�I/O����������ǲ�����д���ġ�
�û�ģʽ�����ж����������I/O����ȫ���ɲ���ϵͳת��Ϊ
һ������IRP�����ݽṹ����ͬ��IRP���ݻᱻ����ǲ������ͬ����ǲ
������Dispatch Function���С�

--*/

/*++

IRP����ǲ����

IRP�Ĵ����������WindowsӦ�ó����еġ���Ϣ�������ƣ�����
������ܵ���ͬ���͵�IRP�󣬻���벻ͬ����ǲ����������ǲ������
IRP�õ�����

--*/

/*++

IRP

��Windows�ں��У���һ�����ݽṹ����IRP��I/O Request
Package�����������������������������������ص���Ҫ���ݽṹ��
�ϲ�Ӧ�ó�����ײ���������ͨ��ʱ��Ӧ�ó���ᷢ��I/O���󡣲���
ϵͳ��I/O����ת��Ϊ��Ӧ��IRP���ݣ���ͬ���͵�IRP��������ʹ���
����ͬ����ǲ�����ڡ�
IRP�����������������ԣ�һ����MajorFunction����һ����
MinorFunction���ֱ��¼IRP����Ҫ���ͺ������͡�����ϵͳ����
MajorFunction��IRP����ǲ������ͬ����ǲ�����У�����ǲ�����л�
���Լ����ж����IRP��������MinorFunction��
NTʽ���������WDM������������DriverEntry������ע��
��ǲ�����ġ�

--*/

/*++

PDRIVER_DISPATCH MyDriverDispatchCreate()
{

}

PDRIVER_DISPATCH MyDriverDispatchClose()
{

}

PDRIVER_DISPATCH MyDriverDispatchWrite()
{

}

PDRIVER_DISPATCH MyDriverDispatchRead()
{

}

--*/

/*++

��DriverEntry����������pDriverObject�У��и�����ָ��
����MajorFunctio������ָ�������Ǹ����飬ÿ��Ԫ�ض���¼��һ
�������ĵ�ַ��ͨ������������飬���Խ�IRP�����ͺ���ǲ������
���������˴�ֻ���������͵�IRP��������ǲ��������IRP�����Ͳ�
��ֻ�������֡���������û�����õ�IRP���ͣ�ϵͳĬ����ЩIRP����
��_IopInvalidDeviceRequest����������
�ڽ���DriverEntry֮ǰ������ϵͳ�Ὣ_IopInvaildDeviceRequest
�ĵ�ַ��������MajorFunction���顣

--*/

/*++

IRP����

IRP�ĸ�������WindowsӦ�ó����С���Ϣ���ĸ����Win32���
�У��������ɡ���Ϣ�������ġ���ͬ����Ϣ���ᱻ�ַ�����ͬ����Ϣ����
�����С����û�ж�Ӧ�Ĵ�������������뵽ϵͳĬ�ϵ���Ϣ����
�����С�
IRP�Ĵ����������ַ�ʽ���ļ�I/O����غ�������CreateFile��
ReadFile��WriteFile��CloseHandle�Ⱥ�����ʹ����ϵͳ������
IRP_MJ_CREATE��IRP_MJ_READ��IRP_MJ_WRITE��IRP_MJ_CLOSE
�Ȳ�ͬ��IRP����ЩIRP�ᱻ���͵������������ǲ�����С�
���⣬�ں��е��ļ�I/O����������ZwCreateFile��ZwReadFile��
ZwWriteFile��ZwClose������ͬ���ᴴ����Ӧ��IRP_MJ_CREATE��
IRP_MJ_READ��IRP_MJ_WRITE��IRP_MJ_CLOSE�Ȳ�ͬ��IRP������
IRP���͵���Ӧ��������Ӧ��ǲ�����С�
����ЩIRP����ϵͳ��ĳ����������ģ�����IRP_MJ_SHUTDOWN
����Windows�ļ��弴������ڼ����ر�ϵͳ��ʱ�򷢳��ġ�

--*/

/*++

����ǲ�����ļ򵥴���

�󲿷ֵ�IRP��Դ���ļ�I/O����Win32API����CreateFile��
ReadFile�ȡ�������ЩIRP�򵥵ķ�����������Ӧ��ǲ�����У���IRP
��״̬����Ϊ�ɹ���Ȼ�����IRP�����󣬲�����ǲ�������سɹ�����
��IRP������ʹ�ú���IoCompleteRequest������Ĵ�����ʾ��һ��
��򵥵Ĵ���IRP�������ǲ������

--*/

NTSTATUS MyDriverDispatchRoutin(
	IN PDEVICE_OBJECT pDevObj,
	IN PIRP pIrp
)
{
	// �������������δ���ò����ľ���
	UNREFERENCED_PARAMETER(pDevObj);
	KdPrint(("Enter MyDriverDispatchRoutin\n"));
	// ��һ��IRP�ļ򵥲���
	NTSTATUS status = STATUS_SUCCESS;
	// ����IRP���״̬
	pIrp->IoStatus.Status = status;
	// ����IRP�����˶����ֽ�
	pIrp->IoStatus.Information = 0; // btyes xfered
									// ����IRP
	IoCompleteRequest(pIrp, IO_NO_INCREMENT);
	KdPrint(("Leave MyDriverDispatchRoutin\n"));
	return status;
}

/*++

��ǲ����������IRP�����״̬ΪSTATUS_SUCCESS������������
I/O�����Win32 API����WriteFile�����᷵��TRUE���෴�������
IRP�����״̬����Ϊ���ɹ�����ʱ����I/O�����Win32 API����
WriteFile�����᷵��FALSE���������ʱ������ʹ��GetLastError
Win32 API�õ�������롣���õĴ��������IRP���õ�״̬��һ�¡�
��������IRP�����״̬����ǲ������Ҫ�������IRP���������
�����ֽڡ��ڱ����У��������ֽ����򵥵�����Ϊ��0�������ReadFile
������IRP������ֽ���������豸д�˶����ֽڡ������ǲ������
IRP�������������ͨ��IoCompleteRequest������ɵġ�
IoCompleteRequest���������£�

VOID IoCompleteRequest (
IN PIRP Irp,
IN CCHAR PriorityBoost
);

Irp: ������Ҫ��������IRP��
PriorityBoost: �����ָ̻߳�ʱ�����ȼ���

��ReadFileΪ����ReadFile���ڲ�����������������:
1. ReadFile����ntdll�е�NtReadFile������ReadFile����
��Win32 API����NtReadFile������Native API��
2. ntdll�е�NtReadFile���뵽�ں�ģʽ��������ϵͳ������
��NtReadFile������
3. ϵͳ������NtReadFile����IRP_MJ_WRITE���͵�IRP��
Ȼ���������IRP���͵�ĳ�������������ǲ�����С�NtReadFile
Ȼ��ȥ����һ���¼�����ʱ��ǰ�߳̽��롰˯�ߡ�״̬��Ҳ����˵��ǰ
�̱߳�����ס�����̴߳��ڡ�Pending��״̬��
4. ����ǲ������һ��ὫIRP�������������IRP��ͨ��
IoCompleteRequest�����ڲ������øղŵȴ����¼������¼������߳�
���ָ����С�
���磬�ڶ�һ���ܴ���ļ��������豸��ʱ��ReadFile��������
���أ����ǵȴ�һ��ʱ�䡣���ʱ����ǵ�ǰ�̡߳�˯�ߡ����Ƕ�ʱ�䡣
IRP���󱻽�������־�������ִ����ϣ���ʱ��˯�ߡ����̱߳����ѡ�
IoCompleteRequest�����еĵڶ�������PriorityBoost����
һ�����ȼ���ָ���Ǳ��������߳��Ժ������ȼ��ָ����С�һ�������
���ȼ�����ΪIO_NO_INCREMENT����ĳЩ�����������Ҫ��������
�߳��ԡ����ȡ�����ݻָ����С�����̡����������豸��������Ҫ
����ط�Ӧ��

--*/

/*++

ͨ���豸������豸

Ҫ���豸������ͨ���豸�����ֲ��ܵõ����豸�ľ����ÿ��
�豸�����豸���ƣ���MyDriver1����������豸��Ϊ��\Device\MyDriver1Device����
�����豸���޷����û�ģʽ�µ�Ӧ�ó����ѯ�����豸��ֻ�ܱ��ں�
ģʽ�µĳ����ѯ����
��Ӧ�ó����У��豸����ͨ������������з��ʡ���������ͨ��
IoCreateSymbolicLink����������������MyDriver1���������
�豸����Ӧ�ķ�������\??\MyDriver1�����ڱ�д����ʱ����������
��д���ͱ���ˡ�\\.\MyDriver1����д��C���Ե��ַ�������
��\\\\.\\MyDriver1����
����Ĵ�����ʾ���������CreateFile�����豸������Լ�
�������CloseHandle�ر��豸������ڴ򿪺͹ر��豸�����ʱ��
����ϵͳ�ڲ��ᴴ��IRP������IRP���͵���Ӧ����ǲ�����С�
���ְ취�ǽ�AnsiString1�е�Bufferָ�����string1ָ�롣
���ֳ�ʼ�����ŵ��ǲ����򵥣�������������ڴ档����������һ��
���⣬����޸�string1��ͬʱ�ᵼ��AnsiString1�ַ������仯��

--*/

VOID Test7()
{
	KdPrint(("Test7��������beginning\n"));

	ANSI_STRING AnsiString1;
	
	CHAR * string1 = "hello";
	// ��ʼ��ANSI_STRING�ַ���
	RtlInitAnsiString(&AnsiString1, string1);
	KdPrint(("AnsiString1:%Z\n", &AnsiString1)); // ��ӡhello

	// �ı�string1���˴�������������ʹ��
	//string1[0] = 'H';
	//string1[1] = 'E';
	//string1[2] = 'L';
	//string1[3] = 'L';
	//string1[4] = 'O';
	// �ı�string1��AnsiStringͬ���ᵼ�±仯���˴���������
	// KdPrint(("AnsiString1:%Z\n", &AnsiString1)); // ��ӡHELLO
	KdPrint(("Test7��������finishing\n"));
}

/*++

	����һ�ַ����ǳ���Ա�Լ������ڴ档����ʼ���ڴ棬��������
����ʱ����Ҫ�����ַ���ռ�õ��ڴ档

--*/

VOID Test8()
{
	KdPrint(("Test8��������beginning\n"));
	UNICODE_STRING UnicodeString1 = { 0 };

	// ���û�������С
	UnicodeString1.MaximumLength = BUFFER_SIZE;

	// �����ڴ�
	UnicodeString1.Buffer = (PWSTR)ExAllocatePool(
		PagedPool, BUFFER_SIZE
	);
	WCHAR* wideString = L"hello";

	// �����ַ����ȣ���Ϊ�ǿ��ַ����������ַ����ȵ�2��
	UnicodeString1.Length = 2 * wcslen(wideString);

	// ��֤�������㹻�󣬷��������ֹ
	ASSERT(UnicodeString1.MaximumLength >=
		UnicodeString1.Length);

	// �ڴ渴��
	RtlCopyMemory(
		UnicodeString1.Buffer,
		wideString,
		UnicodeString1.Length);

	// �����ַ�����
	UnicodeString1.Length = 2 * wcslen(wideString);

	KdPrint(("UnicodeString:%wZ\n", &UnicodeString1));

	// �����ڴ�
	ExFreePool(UnicodeString1.Buffer);
	UnicodeString1.Buffer = NULL;
	UnicodeString1.Length = UnicodeString1.MaximumLength = 0;

	KdPrint(("Test8��������finishing\n"));
}

/*++

	�������һ�������ڴ棬DDKͬ�������˼򻯺������ֱ���
RtlFreeAnsiString��RtlFreeUnicodeString�������������ڲ�
������ExFreePoolȥ�����ڴ档

--*/


/*++

�ַ�������

	DDK�ṩ���ANSI_STRING�ַ�����UNICODE_STRING�ַ�����
�����ַ�������ֱ��ǣ�
	ANSI_STRING�ַ������ƺ���

VOID RtlCopyString (
	IN OUT PSTRING DestinationString,
	IN PSTRING SourceString OPTIONAL
);

	DestinationString: Ŀ���ַ���
	SourceString: Դ�ַ���

	UNICODE_STRING�ַ������ƺ���

VOID RtlCopyUnicodeString (
	IN OUT PUNICODE_STRING DestinationString,
	IN PUNICODE_STRING SourceString
);

	DestinationString: Ŀ���ַ���
	SourceString: Դ�ַ���

	����Ĵ�����ʾ�����ʹ��RtlCopyUnicodeString������

--*/

VOID Test9()
{
	KdPrint(("Test9��������beginning\n"));

	// ��ʼ��UnicodeString1
	UNICODE_STRING UnicodeString1;
	RtlInitUnicodeString(&UnicodeString1, L"Hello World");

	// ��ʼ��UnicodeString2
	UNICODE_STRING UnicodeString2 = { 0 };
	UnicodeString2.Buffer = (PWSTR)ExAllocatePool(
		PagedPool, BUFFER_SIZE
	);
	UnicodeString2.MaximumLength = BUFFER_SIZE;

	// ����ʼ��UnicodeString2���Ƶ�UnicodeString1
	RtlCopyUnicodeString(&UnicodeString2, &UnicodeString1);

	// �ֱ���ʾUnicodeString1��UnicodeString2
	KdPrint(("UnicodeString1:%wZ\n", &UnicodeString1));
	KdPrint(("UnicodeString2:%wZ\n", &UnicodeString2));

	// ����UnicodeString2
	// ע��UnicodeString1��������
	RtlFreeUnicodeString(&UnicodeString2);

	KdPrint(("Test9��������finishing\n"));
}

/*++

�ַ����Ƚ�

	DDK�ṩ�˶�ANSI_STRING�ַ�����UNICODE_STRING�ַ�����
����ַ����Ƚϵ�����ֱ���ANSI_STRING�ַ����ȽϺ�����
UNICODE_STRING�ַ����ȽϺ�����

LONG RtlCompareString (
	IN PSTRING String1,
	IN PSTRING String2,
	BOOLEAN CaseInSensitive
);

	String1: Ҫ�Ƚϵĵ�һ���ַ���
	String2: Ҫ�Ƚϵĵڶ����ַ���
	CaseInSensitive: �Ƿ�Դ�Сд����
	����ֵ: �ȽϽ��

LONG RtlCompareUnicodeString(
	IN PUNICODE_STRING String1,
	IN PUNICODE_STRING String2,
	IN BOOLEAN CaseInSensitive
);

	String1: Ҫ�Ƚϵĵ�һ���ַ���
	String2: Ҫ�Ƚϵĵڶ����ַ���
	CaseInSensitive: �Ƿ�Դ�Сд����
	����ֵ: �ȽϽ��

	�����������Ĳ�����ʽ��ͬ�������������ֵΪ0�����ʾ����
�ַ�����ȡ����С��0�����ʾ��һ���ַ���С�ڵڶ����ַ�����
�������0�����ʾ��һ���ַ������ڵڶ����ַ�����
	ͬʱ��DDK���ṩ��RtlEqualString��RtlEqualUnicodeString
��������ʹ�÷��������������������ơ�ֻ�Ƿ���Ϊ���������ȣ���
������ȡ�

--*/

VOID Test10()
{
	KdPrint(("Test10��������beginning\n"));

	// ��ʼ��UnicodeString1
	UNICODE_STRING UnicodeString1;
	RtlInitUnicodeString(&UnicodeString1, L"Hello World");

	// ��ʼ��UnicodeString2
	UNICODE_STRING UnicodeString2;
	RtlInitUnicodeString(&UnicodeString2, L"Hello");

	// �ж��ַ����Ƿ����
	if (RtlEqualUnicodeString(&UnicodeString1, &UnicodeString2, TRUE))
	{
		KdPrint(("UnicodeString1 and UnicodeString2 are equal\n"));
	}
	else
	{
		KdPrint(("UnicodeString1 and UnicodeString2 are NOT equal"));
	}

	KdPrint(("Test10��������finishing\n"));
}

/*++

�ַ���ת���ɴ�д

	DDK�ṩ�˶�ANSI_STRING�ַ�����UNICODE_STRING�ַ�����
����ַ�����Сдת���ĺ�����

1. ANSI_STRING�ַ���ת���ɴ�д

VOID RtlUpperString (
	IN OUT PSTRING DestinationString,
	IN PSTRING SourceString
);

	DestinationString: Ŀ���ַ�����
	SourceString: Դ�ַ�����

2. UNICODE_STRING�ַ���ת���ɴ�д

NTSTATUS RtlUpcaseUnicodeString (
	IN OUT PUNICODE_STRING DestinationString OPTIONAL,
	IN PCUNICODE_STRING SourceString,
	IN BOOLEAN AllocateDestinationString
);

	DestinationString: Ŀ���ַ�����
	SourceString: Դ�ַ�����
	AllocateDestinationString: �Ƿ�ΪĿ���ַ��������ڴ档
	����ֵ: ����ת���Ƿ�ɹ���

	RtlUpcaseUnicodeString������RtlUpperString������һ��
����AllocateDestinationString���������ָ���Ƿ�ΪĿ���ַ���
�����ڴ档Ŀ���ַ�����Դ�ַ���������ͬһ���ַ�����
	DDK��Ȼ�ṩ��ת���ɴ�д�ĺ�������ȴû���ṩת����Сд��
������������ʾ��

--*/

VOID Test11()
{
	KdPrint(("Test11��������beginning\n"));

	// ��ʼ��UnicodeString1
	UNICODE_STRING UnicodeString1;
	RtlInitUnicodeString(&UnicodeString1, L"Hello World");

	// ��ʼ��UnicodeString2
	UNICODE_STRING UnicodeString2;
	// RtlInitUnicodeString(&UnicodeString2, L" ");

	// �仯ǰ
	KdPrint(("UnicodeString1:%wZ\n", &UnicodeString1));

	// ת���ɴ�д
	RtlUpcaseUnicodeString(&UnicodeString2, &UnicodeString1, TRUE);

	// �仯��
	KdPrint(("UnicodeString2:%wZ\n", &UnicodeString2));

	KdPrint(("Test11��������finishing\n"));
}

/*++

�ַ��������������໥ת��

	DDK�ṩ��UNICODE_STRING�ַ����������໥ת�����ں˺�����

1. ��UNICODE_STRING�ַ���ת�������Ρ�

	���������RtlUnicodeStringToInteger���������ǣ�

NTSTATUS RtlUnicodeStringToInteger (
	IN PUNICODE_STRING String,
	IN ULONG Base OPTIONAL,
	OUT PULONG Value
);

	String: ��Ҫת�����ַ���
	Base: ת�������Ľ��ƣ���2��8��10��16��
	Value: ��Ҫת��������
	����ֵ: ָ���Ƿ�ת���ɹ�

2. ������ת����UNICODE_STRING�ַ���

	�������RtlIntegerToUnicodeString���������ǣ�

NTSTATUS RtlIntegerToUnicodeString (
	IN ULONG Value,
	IN ULONG Base OPTIONAL,
	IN OUT PUNICODE_STRING String
);

	Value: ��Ҫת��������
	Base: ת�������Ľ��ƣ���2��8��10��16��
	String: ��Ҫת�����ַ���
	����ֵ: ָ���Ƿ�ת���ɹ�

	������ʾ��

--*/

VOID Test12()
{
	KdPrint(("Test12��������beginning\n"));

	// ��ʼ��UnicodeString1
	UNICODE_STRING UnicodeString1;
	RtlInitUnicodeString(&UnicodeString1, L"-100");

	ULONG lNumber;
	NTSTATUS nStatus = RtlUnicodeStringToInteger(
		&UnicodeString1, 10, &lNumber);
	if (NT_SUCCESS(nStatus))
	{
		KdPrint(("Conver to integer successfully!\n"));
		KdPrint(("Result:%d\n", lNumber));
	}
	else
	{
		KdPrint(("Conver to integer unsuccessfully!\n"));
	}

	// ����ת�����ַ���
	// ��ʼ��UnicodeString2
	UNICODE_STRING UnicodeString2 = { 0 };
	UnicodeString2.Buffer = (PWSTR)ExAllocatePool(
		PagedPool, BUFFER_SIZE
	);
	UnicodeString2.MaximumLength = BUFFER_SIZE;
	nStatus = RtlIntegerToUnicodeString(200, 10, &UnicodeString2);

	if (NT_SUCCESS(nStatus))
	{
		KdPrint(("Conver to string successfully!\n"));
		KdPrint(("Result:%wZ\n", &UnicodeString2));
	}
	else
	{
		KdPrint(("Conver to string unsuccessfully\n"));
	}

	// ����UnicodeString2��ע��UnicodeString1��������
	RtlFreeUnicodeString(&UnicodeString2);

	KdPrint(("Test12��������finishing\n"));
}

/*++

ANSI_STRING�ַ�����UNICODE_STRING�ַ����໥ת��

	DDK�ṩ��ANSI_STRING�ַ�����UNICODE_STRING�ַ����໥
ת������غ�����
	
1. ��UNICODE_STRING�ַ���ת����ANSI_STRING�ַ�����

	DDK��������ת���ṩ�ĺ�����RtlUnicodeStringToAnsiString,
�������ǣ�

NTSTATUS RtlUnicodeStringToAnsiString (
	IN OUT PANSI_STRING DestinationString,
	IN PUNICODE_STRING SourceString,
	IN BOOLEAN AllocateDestinationString
);

	DestinationString: ��Ҫ��ת�����ַ���
	SourceString: ��Ҫת����Դ�ַ���
	AllocateDestinationString: �Ƿ���Ҫ�Ա�ת�����ַ�����
���ڴ档
	����ֵ: ָ���Ƿ�ת���ɹ���

2. ��ANSI_STRING�ַ���ת����UNICODE_STRING�ַ���

	DDK��������ת���ṩ�ĺ�����RtlAnsiStringToUnicodeString,
�������ǣ�
	
NTSTATUS RtlAnsiStringToUnicodeString (
	IN OUT PUNICODE_STRING DestinationString,
	IN PUNICODE_STRING SourceString,
	IN BOOLEAN AllocateDestinationString
);

	DestinationString: ��Ҫ��ת�����ַ���
	SourceString: ��Ҫת����Դ�ַ���
	AllocateDestinationString: �Ƿ���Ҫ�Ա�ת�����ַ�����
���ڴ档
	����ֵ: ָ���Ƿ�ת���ɹ���

	������ʾ��
	
--*/

VOID Test13()
{
	KdPrint(("Test13��������beginning\n"));

	// ��UNICODE_STRING�ַ���ת����ANSI_STRING�ַ���
	// ��ʼ��UnicodeString1
	UNICODE_STRING UnicodeString1;
	RtlInitUnicodeString(&UnicodeString1, L"Hello World");

	ANSI_STRING AnsiString1;
	NTSTATUS nStatus = RtlUnicodeStringToAnsiString(
		&AnsiString1, &UnicodeString1, TRUE
	);

	if (NT_SUCCESS(nStatus))
	{
		KdPrint(("Conver successfully!\n"));
		KdPrint(("Result:%Z\n", &AnsiString1));
	}
	else
	{
		KdPrint(("Conver unsuccessfully!\n"));
	}

	// ����AnsiString1
	RtlFreeAnsiString(&AnsiString1);

	// ��ANSI_STRING�ַ���ת����UNICODE_STRING�ַ���
	// ��ʼ��AnsiString2
	ANSI_STRING AnsiString2;
	RtlInitString(&AnsiString2, "Hello world");
	
	UNICODE_STRING UnicodeString2;
	nStatus = RtlAnsiStringToUnicodeString(
		&UnicodeString2, &AnsiString2, TRUE
	);

	if (NT_SUCCESS(nStatus))
	{
		KdPrint(("Conver successfully!\n"));
		KdPrint(("Result:%wZ\n", &UnicodeString2));
	}
	else
	{
		KdPrint(("Conver unsuccessfully!\n"));
	}

	// ����UnicodeString2
	RtlFreeUnicodeString(&UnicodeString2);

	KdPrint(("Test13��������finishing\n"));
}

/*++

�ں�ģʽ�µ��ļ�����

	���������򿪷��У���������ļ����в�������Win32API��ͬ��
DDK�ṩ����һ�׶��ļ��Ĳ���������

--*/

/*++

�ļ��Ĵ���

	���ļ��Ĵ������ߴ򿪶���ͨ���ں˺���ZwCreateFileʵ�ֵġ�
��Window API���ƣ�����ں˺�������һ���ļ�������ļ������в�
�������������������в����ġ����ļ�������Ϻ���Ҫ�ر����
�����

NTSTATUS ZwCreateFile (
	OUT PHANDLE FileHandle,
	IN ACCESS_MASK DesiredAccess,
	IN POBJECT_ATTRIBUTES ObjectAttributes,
	OUT PIO_STATUS_BLOCK IoStatusBlock,
	IN PLARGE_INTEGER AllocationSize OPTIONAL,
	IN ULONG FileAttributes,
	IN ULONG ShareAccess,
	IN ULONG CreateDisposition,
	IN ULONG CreateOption,
	IN PVOID EaBuffer OPTIONAL,
	IN ULONG EaLength
);

	FileHandle: ���ش��ļ��ľ����
	DesiredAccess: �Դ��ļ�����������������д����������
һ��ָ��ΪGENERIC_READ��GENERIC_WRITE��
	ObjectAttributes: ��OBJECT_ATTRIBUTES�ṹ�ĵ�ַ����
�ṹ����Ҫ��i���ļ�����
	IoStatusBlock: ָ��һ��IO_STATUS_BLOCK�ṹ���ýṹ����
ZwCreateFile�����Ľ��״̬��
	AllocationSize: ��һ��ָ�룬ָ��һ��64λ����������ָ��
�ļ���ʼ����ʱ�Ĵ�С���ò�������ϵ����������д�ļ����������
����������ô�ļ����Ƚ���0��ʼ��������д���������
	FileAttributes: 0����FILE_ATTRIBUTE_NORMAL��ָ����
�����ļ������ԡ�
	ShareAccess: FILE_SHARE_READ��0��ָ���ļ��Ĺ���ʽ��
�����Ϊ�����ݶ����ļ���������������߳�ͬʱ��ȡ���ļ������
Ϊд���ݶ����ļ������ܲ�ϣ�������̷߳��ʸ��ļ���
	CreateDisposition: FILE_OPEN��FILE_OVERWRITE_IF��
������ָ���ļ����ڻ򲻴���ʱӦ��δ���
	CreateOption: FILE_SYNCHRONOUS_IO_NONALERT��ָ������
�򿪲����;��ʹ�õĸ��ӱ�־λ��
	EaBuffer: һ��ָ�룬ָ���ѡ����չ�������� 
	EaLength: ��չ�������ĳ��ȡ�

	���������Ҫ��дCreateDisposition�������������ļ���
CreateDisposition��������ΪFILE_OPEN������봴���ļ���
CreateDisposition�������ó�FILE_OVERWRITE_IF����ʱ������
�ļ��Ƿ���ڣ����ᴴ�����ļ���
	�ļ�����ָ����ͨ���趨����������ObjectAttributes�����
������һ��OBJECT_ATTRIBUTES�ṹ��DDK�ṩ��OBJECT_ATTRIBUTES
�ṹ��ʼ���ĺ�InitializeObjectAttributes��

VOID InitializeObjectAttributes (
	OUT POBJECT_ATTRIBUTES InitializeAttributes,
	IN PUNICODE_STRING ObjectName,
	IN ULONG Attributes,
	IN HANDLE RootDirectory,
	IN PSECURITY_DESCRIPTOR SecurityDescriptor
);

	InitializeAttributes: ���ص�OBJECT_ATTRIBUTES�ṹ��
	ObjectName: �������ƣ���UNICODE_STRING��������������
�����ļ�����
	Attributes: һ����ΪOBJ_CASE_INSENSITIVE���Դ�Сд���С�
	RootDirectory: һ����ΪNULL��
	SecurityDescriptor: һ����ΪNULL��

	���⣬�ļ��������Ƿ������ӻ������豸����

	������ʾ��

--*/

VOID Test14()
{
	KdPrint(("Test14��������beginning\n"));

	OBJECT_ATTRIBUTES objectAttributes;
	IO_STATUS_BLOCK iostatus;
	HANDLE hfile;
	UNICODE_STRING logFileUnicodeString;

	// ��ʼ��UNICODE_STRING�ַ���
	RtlInitUnicodeString(
		&logFileUnicodeString,
		L"\\??\\C:\\1.log"
	);
	// ��д��"\\Device\\HarddiskVolume1\\1.LOG"

	// ��ʼ��objectAttributes
	InitializeObjectAttributes(
		&objectAttributes,
		&logFileUnicodeString,
		OBJ_CASE_INSENSITIVE,
		NULL,
		NULL
	);

	// �����ļ�
	NTSTATUS ntStatus = ZwCreateFile(
		&hfile,
		GENERIC_WRITE,
		&objectAttributes,
		&iostatus,
		NULL,
		FILE_ATTRIBUTE_NORMAL,
		FILE_SHARE_READ,
		FILE_OPEN_IF,
		FILE_SYNCHRONOUS_IO_NONALERT,
		NULL,
		0
	);

	if (NT_SUCCESS(ntStatus))
	{
		KdPrint(("Create file successfully!\n"));
	}
	else
	{
		KdPrint(("Create file unsuccessfully!\n"));
	}

	// �ļ�����
	// do sometihings
	KdPrint(("do somethings\n"));

	// �ر��ļ����
	ZwClose(hfile);

	KdPrint(("Test14��������finishing\n"));
}



/*++

�ļ�����

--*/

VOID Test15()
{
	KdPrint(("Test15��������beginning\n"));

	OBJECT_ATTRIBUTES objectAttributes;
	IO_STATUS_BLOCK iostatus;
	HANDLE hfile;
	UNICODE_STRING logFileUnicodeString;

	// ��ʼ��UNICODE_STRING�ַ���
	RtlInitUnicodeString(
		&logFileUnicodeString,
		L"\\??\\C:\\1.log"
	);
	// ����д��"\\Device\\HarddiskVolume1\\1.LOG"

	// ��ʼ��objectAttributes
	InitializeObjectAttributes(
		&objectAttributes,
		&logFileUnicodeString,
		OBJ_CASE_INSENSITIVE, // �Դ�Сд����
		NULL,
		NULL
	);

	// �����ļ�
	NTSTATUS ntStatus = ZwCreateFile(
		&hfile,
		GENERIC_READ,
		&objectAttributes,
		&iostatus,
		NULL,
		FILE_ATTRIBUTE_NORMAL,
		FILE_SHARE_READ,
		FILE_OPEN, // ���ļ�����������ڣ��򷵻ش���
		FILE_SYNCHRONOUS_IO_NONALERT,
		NULL,
		0
	);

	if (NT_SUCCESS(ntStatus))
	{
		KdPrint(("Open file successfully!\n"));
	}
	else
	{
		KdPrint(("Open file unsuccessfully!\n"));
	}

	// �ļ�����
	KdPrint(("do somethings\n"));

	// �ر��ļ����
	ZwClose(hfile);
	
	KdPrint(("Test15��������finishing\n"));
}

/*++

�ļ��Ĵ�

	����ʹ��ZwCreateFile�������Դ��ļ���DDK���ṩ��һ��
�ں˺���ZwOpenFile��ZwOpenFile�ں˺����Ĳ�����ZwCreateFile
�Ĳ����򻯣��������Ա���ļ����ú������������£�

NTSTATUS ZwOpenFile (
	OUT PHANDLE FileHandle,
	IN ACCESS_MASK DesiredAccess,
	IN POBJECT_ATTRIBUTES ObjectAttributes,
	OUT PIO_STATUS_NLOCK IoStatusBlock,
	IN ULONG ShareAccess,
	IN ULONG OpenOptions
);

	FileHandle: ���ش򿪵��ļ����
	DesiredAccess: �򿪵�Ȩ�ޣ�һ����ΪGENERIC_ALL
	ObjectAttributes: objectAttributes�ṹ
	IoStatusBlock: ָ��һ���ṹ���ָ�롣�ýṹ��ָ����
�ļ���״̬
	ShareAccess: �����Ȩ�ޡ�������FILE_SHARE_READ����
FILE_SHARE_WRITE��
	OpenOption: ��ѡ�һ����ΪFILE_SYNCHRONOUS_IO_NONALERT
	����ֵ: ָ���ļ��Ƿ񱻳ɴ�

	����������ʾ��

--*/

VOID Test16()
{
	KdPrint(("Test16��������beginning\n"));

	OBJECT_ATTRIBUTES objectAttributes;
	IO_STATUS_BLOCK iostatus;
	HANDLE hfile;
	UNICODE_STRING logFileUnicodeString;

	// ��ʼ��UNICODE_STRING�ַ���
	RtlInitUnicodeString(
		&logFileUnicodeString, L"\\??\\C:\\1.log"
	);
	// ����д��"\\Device\\HarddiskVolume1\\1.LOG"

	// ��ʼ��objectAttributes
	InitializeObjectAttributes(
		&objectAttributes,
		&logFileUnicodeString,
		OBJ_CASE_INSENSITIVE,
		NULL,
		NULL
	);

	// �����ļ�
	NTSTATUS ntStatus = ZwOpenFile(
		&hfile,
		GENERIC_ALL,
		&objectAttributes,
		&iostatus,
		FILE_SHARE_READ | FILE_SHARE_WRITE,
		FILE_SYNCHRONOUS_IO_NONALERT
	);

	if (NT_SUCCESS(ntStatus))
	{
		KdPrint(("Create file successfully!"));
	}
	else
	{
		KdPrint(("Create file unsuccessfully!"));
	}

	// �ļ�����
	KdPrint(("do somethings"));

	// �ر��ļ����
	ZwClose(hfile);

	KdPrint(("Test16��������finishing\n"));
}

/*++

��ȡ���޸��ļ�����

	��ȡ���޸��ļ����ԣ�������ȡ�ļ���С����ȡ���޸��ļ�ָ��
λ�á���ȡ���޸��ļ�������ȡ���޸������ԣ�ֻ�����ԡ��������ԣ�
����ȡ���޸��ļ��������޸����ڵȡ�DDK�ṩ���ں˺���ZwSetInformationFile
��ZwQueryInformation���������л�ȡ���޸��ļ����ԡ�

NTSTATUS ZwSetInformationFile (
	IN HANDLE FileHandle,
	OUT PIO_STATUS_BLOCK IoStatusBlock,
	IN PVOID FileInformation,
	IN ULONG Length,
	IN FILE_INFORMATION_CLASS FileInformationClass
);

	FileHandle: �ļ����
	IoStatusBlocks: �������õ�״̬
	FileInformation: ����FileInformationClass��ͬ����ͬ��
��Ϊ������Ϣ��
	Length: FileInformation���ݵĳ���
	FileInformationClass: �����޸����Ե�����

NTSTATUS ZwQueryInformationFile (
	IN HANDLE FileHandle,
	OUT PIO_STATUS)BLOCK IoStatusBlock,
	OUT PVOID FileInformation,
	IN ULONG Length,
	IN FILE_INFORMATION_CLASS FileInformationClass
);

	FileHandle: �ļ����
	IoStatusBlock: �������õ�״̬
	FileInformation: ����FileInformationClass��ͬ����ͬ��
��Ϊ������Ϣ��
	Length: FileInformation���ݵĳ���
	FileInformationClass: �����޸����Ե�����

	����������������ͬ������FileInformationClassָ���޸Ļ���
��ѯ�����

1. ��FileInformationClass��FileStandardInformationʱ��
����������������FILE_STANDARD_INFORMATION�ṹ�壬�����ļ�
�Ļ�����Ϣ��

typedef struct FILE_STANDARD_INFORMATION {
	LARGE_INTEGER AllocationSize; // Ϊ�ļ�����Ĵ�С
	LARGE_INTEGER EndOfFile; // �����ļ���β���ж����ֽ�
	ULONG NumberOfLinks; // �ж��ٸ������ļ�
	BOOLEAN DeletePending; // �Ƿ�׼��ɾ��
	BOOLEAN Directory; // �Ƿ�ΪĿ¼
} FILE_STANDARD_INFORMATION, *PFILE_STANDARD_INFORMATION;

2. ��FileInformationClass��FileBasicInformationʱ������
�������������FILE_BASIC_INFORMATION�ṹ�壬�����ļ��Ļ���
��Ϣ��

typedef struct FILE_BASIC_INFORMATION {
	LARGE_INTEGER CreationTime; // �ļ�����ʱ��
	LARGE_INTEGER LastAccessTime; // ������ʱ��
	LARGE_INTEGER LastWriteTime; // ���дʱ��
	LARGE_INTEGER ChangeTime; // �޸��޸�ʱ��
	ULONG FileAttributes; // �ļ�����
} FILE_BASIC_INPORMATION, *PFILE_BASIC_INFORMATION;

	���У�ʱ������Ǵ�һ��LARGE_INTEGER�������������������
1601�꾭������100ns��
FileAttributes�����ļ����ԡ�
FILE_ATTRIBUTE_NORMAL����һ���ļ���
FILE_ATTRIBUTE_DIRECTORY������Ŀ¼��
FILE_ATTRIBUTE_READONLY�������ļ�Ϊֻ����
FILE_ATTRIBUTE_HIDDEN���������ļ���
FILE_ATTRIBUTE_SYSTEM����ϵͳ�ļ���

3. ��FileInformationClass��FileNameInformationʱ������
�������������FILE_NAME_INFORMATION�ṹ�壬�����ļ�����Ϣ��

typedef struct _FILE_NAME_INFORMATION {
	ULONG FileNameLength; // �ļ�������
	WCHAR FileName[1]; // �ļ���
} FILE_NAME_INFORMATION, *PFILE_NAME_INFORMATION;

4. ��FileInformationClass��FilePositionInformationʱ����
��������������FILE_POSITION_INFORMATION�ṹ�壬�����ļ���
��Ϣ��

typedef struct FILE_POSITION_INFORMATION {
	LARGE_INTEGER CurrentByteOffset; // ����ǰ�ļ�ָ��λ��
} FILE_POSITION_INFORMATION, *PFILE_POSITION_INFORMATION;

	����������ʾ��

--*/

VOID Test17()
{
	KdPrint(("Test17��������beginning\n"));

	OBJECT_ATTRIBUTES objectAttributes;
	IO_STATUS_BLOCK iostatus;
	HANDLE hfile;
	UNICODE_STRING logFileUnicodeString;

	// ��ʼ��UNICODE_STRING�ַ���
	RtlInitUnicodeString(
		&logFileUnicodeString,
		L"\\??\\C:\\1.log"
	);
	// ����д��"\\Device\\HarddiskVolume1\\1.LOG"

	// ��ʼ��objectAttributes
	InitializeObjectAttributes(
		&objectAttributes,
		&logFileUnicodeString,
		OBJ_CASE_INSENSITIVE, // �Դ�Сд����
		NULL,
		NULL
	);

	// �����ļ�
	NTSTATUS ntStatus = ZwCreateFile(
		&hfile,
		GENERIC_READ,
		&objectAttributes,
		&iostatus,
		NULL,
		FILE_ATTRIBUTE_NORMAL,
		0,
		FILE_OPEN, // ���ļ�����������򷵻ش���
		FILE_SYNCHRONOUS_IO_NONALERT,
		NULL,
		0
	);

	if (NT_SUCCESS(ntStatus))
	{
		KdPrint(("open file successfully\n"));
	}

	FILE_STANDARD_INFORMATION fsi;

	// ��ȡ�ļ�����
	ntStatus = ZwQueryInformationFile(
		hfile,
		&iostatus,
		&fsi,
		sizeof(FILE_STANDARD_INFORMATION),
		FileStandardInformation
	);

	if (NT_SUCCESS(ntStatus))
	{
		KdPrint(("file length:%u\n", fsi.EndOfFile.QuadPart));
	}

	// �޸ĵ�ǰ�ļ�ָ��
	FILE_POSITION_INFORMATION fpi;
	fpi.CurrentByteOffset.QuadPart = 100i64;

	ntStatus = ZwSetInformationFile(
		hfile,
		&iostatus,
		&fpi,
		sizeof(FILE_POSITION_INFORMATION),
		FilePositionInformation
	);

	if (NT_SUCCESS(ntStatus))
	{
		KdPrint(("update the file pointer successfully!\n"));
	}

	// �ر��ļ����
	ZwClose(hfile);

	KdPrint(("Test17��������finishing\n"));
}

/*




*/