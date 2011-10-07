#ifndef MAIN_H_
#define MAIN_H_

typedef unsigned char byte;
typedef unsigned short int word;
typedef unsigned int dword;
typedef unsigned long long int qword;
typedef char ASCIZ;
typedef char ASCII;

const char fmt_word[] = "%04X";
const char fmt_dword[] = "%08X";
const char fmt_qword[] = "";
const char fmt_array[] = "";

// ################

struct {
	dword HeaderSize;		// 0x4C
	byte LinkCLSID[16];		// 00021401-0000-0000-C000-000000000046
	dword LinkFlags;
	dword FileAttributes;
	qword CreationTime;
	qword AccessTime;
	qword WriteTime;
	dword FileSize;
	dword IconIndex;
	dword ShowCommand;
	word HotKey;
	word Reserved[5];
} __attribute__((__packed__)) ShellLinkHeader;

const struct {
	const char *self;
	const char *HeaderSize; const char *LinkCLSID;
	const char *LinkFlags; const char *FileAttributes;
	const char *CreationTime; const char *AccessTime; const char *WriteTime;
	const char *FileSize; const char *IconIndex; const char *ShowCommand;
	const char *HotKey; const char *Reserved;
} ShellLinkHeader_strname = {
	"Заголовок ссылки",
	"Размер структуры", "Идентификатор класса",
	"Флаги присутствия структур", "Атрибуты целевого файла",
	"Время создания целевого файла", "Время последнего доступа к файлу",
	"Время последней модификации файла", "Размер целевого файла",
	"Индекс иконки", "Состояние окна программы", "Флаги комбинации клавиш",
	"Не используется"
};

const struct {
	const char *HeaderSize; const char *LinkCLSID;
	const char *LinkFlags; const char *FileAttributes;
	const char *CreationTime; const char *AccessTime; const char *WriteTime;
	const char *FileSize; const char *IconIndex; const char *ShowCommand;
	const char *HotKey; const char *Reserved;
} ShellLinkHeader_strtype = {
	fmt_dword, fmt_array, fmt_dword, fmt_dword,
	fmt_qword, fmt_qword, fmt_qword, "%u",
	fmt_dword, fmt_dword, fmt_word, fmt_array
};

typedef enum {
	LF_HasLinkTargetIDList 	= 0x01,
	LF_HasLinkInfo			= 0x02,
	LF_HasName				= 0x04,
	LF_HasRelativePath		= 0x08,
	LF_HasWorkingDir		= 0x10,
	LF_HasArguments			= 0x20,
	LF_HasIconLocation		= 0x40,
	LF_IsUnicode			= 0x80
} LinkFlagsValues;

typedef enum {
	FA_HIDE				= 0x0001,
	FA_NORMAL			= 0x0002,
	FA_SHOWMINIMIZED	= 0x0004,
	FA_SHOWMAXIMIZED	= 0x0008,
	FA_SHOWNOACTIVATE	= 0x0010,
	FA_SHOW				= 0x0020,
	FA_MINIMIZE			= 0x0040,
	FA_SHOWMINNOACTIVE	= 0x0080,
	FA_SHOWNA			= 0x0100,
	FA_RESTORE 			= 0x0200,
	FA_SHOWDEFAULT		= 0x0400
} FileAttributesValues;

// ################

word ItemIdList_Length;
byte *ItemIdList;

const char ItemIdList_Length_strname[] = "Длина структуры 'IDList'";
const char *ItemIdList_Length_strtype = fmt_word;

// ################

struct {
	dword LinkInfoSize;
	dword LinkInfoHeaderSize;		// 0x1C
	dword LinkInfoFlags;
	dword VolumeIDOffset;
	dword LocalBasePathOffset;
	dword CommonNetworkRelativeLinkOffset;
	dword CommonPathSuffixOffset;
} LinkInfo;

struct {
	dword LocalBasePathOffsetUnicode;
	dword CommonPathSuffixOffsetUnicode;
} LinkInfo_optional;

const struct {
	const char *self;
	const char *LinkInfoSize; const char *LinkInfoHeaderSize; const char *LinkInfoFlags;
	const char *VolumeIDOffset; const char *LocalBasePathOffset;
	const char *CommonNetworkRelativeLinkOffset; const char *CommonPathSuffixOffset;
} LinkInfo_strname = {
	"Местонахождение файла",
	"Размер структуры", "Размер заголовка", "Флаги присутствия структур",
	"Смещение структуры 'Том'", "Смещение структуры 'Базовый локальный путь'",
	"Смещение структуры 'Относительный сетевой путь'",
	"Смещение структуры 'Конечная часть пути'"
};

const struct {
	const char *self;
	const char *LocalBasePathOffsetUnicode;
	const char *CommonPathSuffixOffsetUnicode;
} LinkInfo_optional_strname = {
	"Местонахождение файла",
	"Смещение структуры 'Базовый локальный путь в UNICODE'",
	"Смещение структуры 'Конечная часть пути в UNICODE'"
};

const struct {
	const char *LinkInfoSize; const char *LinkInfoHeaderSize; const char *LinkInfoFlags;
	const char *VolumeIDOffset; const char *LocalBasePathOffset;
	const char *CommonNetworkRelativeLinkOffset; const char *CommonPathSuffixOffset;
} LinkInfo_strtype = {
	fmt_dword, fmt_dword, fmt_dword, fmt_dword, fmt_dword, fmt_dword, fmt_dword
};

const struct {
	const char *LocalBasePathOffsetUnicode;
	const char *CommonPathSuffixOffsetUnicode;
} LinkInfo_optional_strtype = {
		fmt_dword, fmt_dword
};

typedef enum {
	LIF_LocalVolume = 0x01,
	LIF_NetworkVolume = 0x02
} LinkInfoFlagsValues;

// ################

struct {
	dword VolumeIDSize;
	dword DriveType;
	dword DriveSerialNumber;
	dword VolumeLabelOffset;		// 0x10
} VolumeID;
ASCIZ *VolumeID_Data;

const struct {
	const char *self;
	const char *VolumeIDSize;
	const char *DriveType;
	const char *DriveSerialNumber;
	const char *VolumeLabelOffset;
} VolumeID_strname = {
	"Том",
	"Размер структуры",
	"Тип тома",
	"Серийный номер",
	"Смещение имени тома"
};

const struct {
	const char *VolumeIDSize; const char *DriveType;
	const char *DriveSerialNumber; const char *VolumeLabelOffset;
} VolumeID_strtype = {
	fmt_dword, fmt_dword, fmt_dword, fmt_dword
};

const char VolumeID_Data_strname[] = "Имя тома";

// ################

dword LocalBasePath_Length;
ASCIZ *LocalBasePath;
dword CommonPathSuffix_Length;
ASCIZ *CommonPathSuffix;
dword LocalBasePathUnicode_Length;
ASCIZ *LocalBasePathUnicode;
dword CommonPathSuffixUnicode_Length;
ASCIZ *CommonPathSuffixUnicode;

const char LocalBasePath_strname[] = "Базовый локальный путь";
const char CommonPathSuffix_strname[] = "Конечная часть пути";
const char LocalBasePathUnicode_strname[] = "Базовый локальный путь в UNICODE";
const char CommonPathSuffixUnicode_strname[] = "Конечная часть пути в UNICODE";

const char LocalBasePath_Length_strname[] = "Длина структуры 'Базовый локальный путь'";
const char CommonPathSuffix_Length_strname[] = "Длина структуры 'Конечная часть пути'";
const char LocalBasePathUnicode_Length_strname[] = "Длина структуры 'Базовый локальный путь в UNICODE'";
const char CommonPathSuffixUnicode_Length_strname[] = "Длина структуры 'Конечная часть пути в UNICODE'";

const char *LocalBasePath_Length_strtype = fmt_dword;
const char *CommonPathSuffix_Length_strtype = fmt_dword;
const char *LocalBasePathUnicode_Length_strtype = fmt_dword;
const char *CommonPathSuffixUnicode_Length_strtype = fmt_dword;

// ################

struct {
	dword CommonNetworkRelativeLinkSize;
	dword CommonNetworkRelativeLinkFlags;	// 0x02
	dword NetNameOffset;					// 0x14
	dword DeviceNameOffset;					// 0x00
	dword NetworkProviderType;				// 0x20000
} CommonNetworkRelativeLink;

const struct {
	const char *self;
	const char *CommonNetworkRelativeLinkSize;
	const char *CommonNetworkRelativeLinkFlags;
	const char *NetNameOffset;
	const char *DeviceNameOffset;
	const char *NetworkProviderType;
} CommonNetworkRelativeLink_strname = {
	"Сетевой путь",
	"Размер структуры",
	"Флаги",
	"Смещение структуры 'Сетевое имя'",
	"Смещение структуры 'Сетевое устройство'",
	"Тип поставщика сети"
};

const struct {
	const char *self;
	const char *CommonNetworkRelativeLinkSize;
	const char *CommonNetworkRelativeLinkFlags;
	const char *NetNameOffset;
	const char *DeviceNameOffset;
	const char *NetworkProviderType;
} CommonNetworkRelativeLink_strtype = {
	fmt_dword, fmt_dword, fmt_dword, fmt_dword, fmt_dword, fmt_dword
};

ASCIZ *CommonNetworkRelativeLink_NetName;

const char CommonNetworkRelativeLink_NetName_strname[] = "Сетевое имя";

// ################

typedef struct {
	word CountCharacters;
	byte *String;
} SomeString;

const char SomeString_Length_strname[] = "Длина структуры";
const char *SomeString_Length_strtype = fmt_word;

SomeString NameString;
SomeString RelativePath;
SomeString WorkingDir;
SomeString CommandLineArguments;
SomeString IconLocation;

const char NameString_strname[] = "Описание";
const char RelativePath_strname[] = "Относительный путь";
const char WorkingDir_strname[] = "Рабочая директория";
const char CommandLineArguments_strname[] = "Аргументы командной строки";
const char IconLocation_strname[] = "Путь к иконке";


// ################

struct {
	dword DLTDataSize;
	dword DLTDataVersion;
	byte MachineIDStr[16];
	byte VolumeID[16];
	byte FileID[16];
	byte BirthVolumeID[16];
	byte BirthFileID[16];
} DLTDataBlock;

const struct {
	const char *self;
	const char *DLTDataSize;
	const char *DLTDataVersion;
	const char *MachineIDStr;
	const char *VolumeID;
	const char *FileID;
	const char *BirthVolumeID;
	const char *BirthFileID;
} DLTDataBlock_strname = {
	"Блок данных службы отслеживания изменившихся связей",
	"Количество байтов данных",
	"Версия",
	"Идентификатор (имя) машины",
	"Идентификатор тома",
	"Идентификатор файла",
	"Изначальный идентификатор тома",
	"Изначальный идентификатор файла"
};

// ###############################

#endif /* MAIN_H_ */
