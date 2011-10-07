#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdarg.h>
#include <memory.h>
#include <string.h>
#include <time.h>
#include <assert.h>
#include <errno.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "main.h"

#ifdef MINGW32
#include <windows.h>
#endif



#ifndef MINGW32
	#define c_016llX_format "%016llX"
	#define c_016llX_args(num) num
#else
	#define c_016llX_format "%08X%08X"
	#define c_016llX_args(num) \
		(unsigned int)(((num) >> 32)), \
		(unsigned int)((num) & 0xFFFFFFFF)
#endif

// ###########

typedef enum {
	TPM_HEX,
	TPM_DEC,
} PrintTimeMode;

PrintTimeMode printTimeMode = TPM_HEX;

// ###########

void logErrorBegin() {
	const char mes_err[] = "Ошибка: ";
	fprintf(stderr, mes_err);
}
void logErrorPartVaList(const char *fmt, va_list argp) {
	vfprintf(stderr, fmt, argp);
}
void logErrorEnd() {
	putc('\n', stderr);
}

void logErrorPart(const char *fmt, ...) {
	va_list argp;
	va_start(argp, fmt);
	logErrorPartVaList(fmt, argp);
	va_end(argp);
}
void logError(const char *fmt, ...) {
	va_list argp;
	logErrorBegin();
	va_start(argp, fmt);
	logErrorPartVaList(fmt, argp);
	va_end(argp);
	logErrorEnd();
}

// ###########

#define NTFS_TIME_OFFSET ((int64_t)(369 * 365 + 89) * 24 * 3600 * 10000000)
inline time_t fileTime2UTC (int64_t fileTime) {
	if (fileTime == 0) return 0;
	return (fileTime - (NTFS_TIME_OFFSET)) / 10000000;
}

void printTimeString (const char *prefix_str, const char* tabs, time_t time, char symbol, uint64_t raw) {
	printf("%s:%s", prefix_str, tabs);

	struct tm *pTimeStruct = gmtime(&time);
	#define c_str_size 256
	char str[c_str_size];
	strftime(str, c_str_size-1, "%d.%m.%Y %H:%M:%S", pTimeStruct);
	printf("%s", str);
	#undef c_str_size

	printf(" %c = ", symbol);

	if (printTimeMode == TPM_HEX) {
		printf(c_016llX_format, c_016llX_args(raw));
	} else {
		if (raw == 0) printf("0");
		else {
			uint64_t hi_dec = 10000000000000000000ull;
			while (raw / hi_dec == 0) hi_dec /= 10;

			do {
				printf ("%d", (unsigned char)(raw/hi_dec));
				raw %= hi_dec;
				hi_dec /= 10;
			} while(hi_dec != 0);
		}
	}

	printf ("\n");

}

// ###########

#include <stdio.h>

#ifdef MINGW32
#include <windows.h>
#include <wchar.h>
#else
#include <iconv.h>
#endif

void fprintStringCP1251(FILE *fout, const unsigned char *str_bytes, int len) {
	#ifndef MINGW32
	char *pIn;
	size_t inLen;
	const int c_outBuf_size = 5;
	char outBuf[c_outBuf_size];
	outBuf[c_outBuf_size-1] = 0;
	char *pOut;
	size_t outLen;
	iconv_t cd = iconv_open("UTF-8", "WINDOWS-1251");
	if (cd == (iconv_t)(-1)) return;
	int i;
	for (i=0; i<len; ++i) {
		pIn = (char *)&str_bytes[i];
		inLen = 1;
		outBuf[0] = outBuf[1] = outBuf[2] = outBuf[3] = 0;
		pOut = &(outBuf[0]);
		outLen = c_outBuf_size;
		iconv(cd, &pIn, &inLen, &pOut, &outLen);
		fprintf(fout, "%s", outBuf);
	}
	iconv_close(cd);
	#else
	wchar_t sym_wchar;
	char sym_cp866;
	int i;
	for (i=0; i<len; ++i) {
		MultiByteToWideChar(1251, 0, &str_bytes[i], 1, &sym_wchar, 1);
		WideCharToMultiByte(866, 0, &sym_wchar, 1, &sym_cp866, 1, NULL, NULL);
		fputc(sym_cp866, fout);
	}
	#endif
}

void fprintStringCP866(FILE *fout, const unsigned char *str_bytes, int len) {
	#ifndef MINGW32
	char *pIn;
	size_t inLen;
	const int c_outBuf_size = 5;
	char outBuf[c_outBuf_size];
	outBuf[c_outBuf_size-1] = 0;
	char *pOut;
	size_t outLen;
	iconv_t cd = iconv_open("UTF-8", "CP-866");
	if (cd == (iconv_t)(-1)) return;
	int i;
	for (i=0; i<len; ++i) {
		pIn = (char *)&str_bytes[i];
		inLen = 1;
		outBuf[0] = outBuf[1] = outBuf[2] = outBuf[3] = 0;
		pOut = &(outBuf[0]);
		outLen = c_outBuf_size;
		iconv(cd, &pIn, &inLen, &pOut, &outLen);
		fprintf(fout, "%s", outBuf);
	}
	iconv_close(cd);
	#else
	int i;
	for (i=0; i<len; ++i) fputc(str_bytes[i], fout);
	#endif
}

void fprintStringUnicode(FILE *fout, const unsigned char *str_words, int len) {
	#ifndef MINGW32
	char *pIn;
	size_t inLen;
	const int c_outBuf_size = 9;
	char outBuf[c_outBuf_size];
	outBuf[c_outBuf_size-1] = 0;
	char *pOut;
	size_t outLen;
	iconv_t cd = iconv_open("UTF-8", "UTF-16LE");
	if (cd == (iconv_t)(-1)) return;
	int i;
	for (i=0; i<len; ++i) {
		pIn = (char *)&str_words[i*2];
		inLen = 2;
		outBuf[0] = outBuf[1] = outBuf[2] = outBuf[3] = 0;
		outBuf[4] = outBuf[5] = outBuf[6] = outBuf[7] = 0;
		pOut = &(outBuf[0]);
		outLen = c_outBuf_size;
		iconv(cd, &pIn, &inLen, &pOut, &outLen);
		fprintf(fout, "%s", outBuf);
	}
	iconv_close(cd);
	#else
	char sym_cp866;
	int i;
	for (i=0; i<len; ++i) {
		WideCharToMultiByte(866, 0, (wchar_t *)(&str_words[i*2]), 1, &sym_cp866, 1, NULL, NULL);
		fputc(sym_cp866, fout);
	}
	#endif
}

// ###########

void printDataHex (unsigned char *data, unsigned int data_size, unsigned int firstOffset) {
	#define c_wordCount 16
	unsigned int i, j = 0, offset;
	offset = firstOffset & (~(c_wordCount-1));
	if (offset != firstOffset) {
		printf("%08X: ", offset);
		for (j=0; j<firstOffset-offset; ++j) printf(".. ");
	}
	for (i=0; i<data_size; ++i) {
		if (j == 0) printf("%08X: ", offset);
		printf("%02X ", data[i]);
		++j;
		if (j == c_wordCount) {
			j = 0;
			offset += c_wordCount;
			printf("|\n");
		}
	}
	if (j != 0) {
		for (; j<16; ++j) printf(".. ");
		printf("|\n");
	}
	#undef c_wordCount
}

// ###########

/* IEEE 802 MAC address octet length */
#define IEEE_MAC_OCTETS 6

/* UUID binary representation according to UUID standards */
typedef struct {
    uint32_t  time_low;                  // bits  0-31 of time field
    uint16_t  time_mid;                  // bits 32-47 of time field
    uint16_t  time_hi_and_version;       // bits 48-59 of time field plus 4 bit version
    uint16_t  clock_seq_and_variant;     // bits  0-13 of clock sequence field plus 2 bit variant ((load as BigEndian))
    uint8_t   node[IEEE_MAC_OCTETS];     // bits  0-47 of node MAC address
} __attribute__((__packed__)) uuid_obj_t;

#define UUID_NTFS_TIMEOFFSET 0x00146BF33E42C000ll

typedef struct {
	uint64_t time;
	uint32_t seq;
	uint8_t mac[IEEE_MAC_OCTETS];
} ObjIDParsed;

bool parseObjectID (const unsigned char uuidBytes[], ObjIDParsed *pRez) {
	assert(sizeof(uuid_obj_t) == 16);
	assert(pRez != NULL);

	uuid_obj_t uuid_obj;

	uuid_obj.time_low = uuidBytes[0] | uuidBytes[1] << 8 | uuidBytes[2] << 16 | uuidBytes[3] << 24;
	uuid_obj.time_mid = uuidBytes[4] | uuidBytes[5] << 8;
	uuid_obj.time_hi_and_version = uuidBytes[6] | uuidBytes[7] << 8;
	uuid_obj.clock_seq_and_variant = uuidBytes[8] << 8 | uuidBytes[9];
	memcpy(&(uuid_obj.node), &uuidBytes[10], IEEE_MAC_OCTETS);

	// decode DCE 1.1 version 1 UUID
	if ((uuid_obj.clock_seq_and_variant & 0xC000) != 0x8000) {printf("variant\n"); return true;}
	if ((uuid_obj.time_hi_and_version & 0xF000) != 0x1000) {printf("version\n"); return true;}

	// decode system time
	pRez->time = uuid_obj.time_low |
			((uint64_t)uuid_obj.time_mid) << 32 |
			((uint64_t)(uuid_obj.time_hi_and_version & (~0x1000))) << 48;
	pRez->time -= UUID_NTFS_TIMEOFFSET;

	// decode clock sequence
	pRez->seq = uuid_obj.clock_seq_and_variant & (~0xC000);

	// decode node MAC address
	memcpy(&(pRez->mac[0]), &uuidBytes[10], IEEE_MAC_OCTETS);

	return false;
}

bool printParseObjectID (const unsigned char uuidBytes[]) {
	ObjIDParsed parsed;
	if (parseObjectID(uuidBytes, &parsed)) return true;

	printf("\t");
	printTimeString ("Время", " ", fileTime2UTC(parsed.time), 'T', parsed.time);
	/*printf ("\tВремя: ");
	printFileTime(parsed.time);
	printf("  T = " c_016llX_format "\n", c_016llX_args(parsed.time));*/

	printf("\tПорядковый номер часов: %u\n", parsed.seq);

	printf ("\tMAC-адрес: ");
	int i;
	for (i=0; i<6; ++i) printf("%02X", parsed.mac[i]);
	printf("\n");

	return false;
}

// ###########

bool readAndParseFile(const char* fileNameIn) {
	assert(fileNameIn != NULL);

	FILE *fileIn = fopen (fileNameIn, "rb");
	if (fileIn == NULL) {logError("Ошибка при открытии файла '%s'", fileNameIn); return true;}

	bool rez = true;

	// TODO: errors checks
	struct stat buf;
	fstat(fileno(fileIn), &buf);
	off_t fileSize = buf.st_size;
	//printf("[1]%u\n", (unsigned int)fileSize);

	#define errAndClose(format, args...) { \
		logError(format, ##args); \
		goto close_in; \
	}

	#define readData(ptr, len) { \
		if ( \
			(bytesRead = fread ((char*)ptr, 1, len, fileIn)) != \
			len \
		) errAndClose ("Ошибка при чтении файла '%s'", fileNameIn); \
	}

	#define readField(field) readData(&field, sizeof(field))

	#define mallocAndReadData(ptr, len) { \
		ptr = malloc(len); \
		readData(ptr, len); \
	}

	#define checkRange(len, min, max) (!(  (len) >= (min) && (len) <= (max)  ))

	#define printField(structure, field) { \
		printf(structure##_strname.field); \
		printf(": "); \
		printf(structure##_strtype.field, structure.field); \
		putchar('\n'); \
	}

	#define printVarString(str, len) { \
		printf(str##_strname); \
		printf(": "); \
		fprintStringCP1251(stdout, (byte *)&str[0], len-1); \
		putchar('\n'); \
	}

	const char *mes_wrongFieldConst[] =
		{"Структура '%s', поле '%s': значение '", "' не совпадает с ожидаемым '", "'"};
	const char *mes_wrongField[] = {"Структура '%s', поле '%s': неверное значение '", "'"};
	const char *mes_wrongVar[] = {"'%s': неверное значение '", "'"};

	#define checkFieldConst(structure, field, valueMust) { \
		if (structure.field != valueMust) { \
			logErrorBegin(); \
			logErrorPart(mes_wrongFieldConst[0], structure##_strname.self, structure##_strname.field); \
			logErrorPart(structure##_strtype.field, structure.field); \
			logErrorPart(mes_wrongFieldConst[1]); \
			logErrorPart(structure##_strtype.field, valueMust); \
			logErrorPart(mes_wrongFieldConst[2]); \
			logErrorEnd(); \
			goto close_in; \
		} \
	}

	#define errWrongField(structure, field) { \
		logErrorBegin(); \
		logErrorPart(mes_wrongField[0], structure##_strname.self, structure##_strname.field); \
		logErrorPart(structure##_strtype.field, structure.field); \
		logErrorPart(mes_wrongField[1]); \
		logErrorEnd(); \
		goto close_in; \
	}

	#define errWrongVar(var) { \
		logErrorBegin(); \
		logErrorPart(mes_wrongVar[0], var##_strname); \
		logErrorPart(var##_strtype, var); \
		logErrorPart(mes_wrongVar[1]); \
		logErrorEnd(); \
		goto close_in; \
	}

	// #############

	unsigned int bytesRead;
	byte *str_offset;
	dword str_len;

	readField (ShellLinkHeader);
	checkFieldConst (ShellLinkHeader, HeaderSize, 0x4C);

	// TODO: check LinkCLSID
	// TODO: parse FileAttributes
	printField (ShellLinkHeader, FileSize);

	/*#define printTime(time, tabs, sym) { \
		printf("%s:"tabs, ShellLinkHeader_strname.time); \
		printFileTime(ShellLinkHeader.time); \
		printf("  " sym " = " c_016llX_format "\n", c_016llX_args(ShellLinkHeader.time)); \
	}*/
	#define printTime(time, tabs, sym) \
		printTimeString (ShellLinkHeader_strname.time, tabs, fileTime2UTC(ShellLinkHeader.time), sym, ShellLinkHeader.time);

	printTime(CreationTime, "\t   ", 'C');
	printTime(AccessTime, "  ", 'A');
	printTime(WriteTime, " ", 'M');

	dword flags = ShellLinkHeader.LinkFlags;
	if (flags & LF_HasLinkTargetIDList) {
		readField(ItemIdList_Length);
		if (checkRange(ItemIdList_Length, 1, fileSize)) errWrongVar(ItemIdList_Length);
		mallocAndReadData(ItemIdList, ItemIdList_Length);
		// TODO: ItemIdList
	}
	if (flags & LF_HasLinkInfo) {
		readField(LinkInfo.LinkInfoSize);
		if (checkRange(LinkInfo.LinkInfoSize, 1, fileSize)) errWrongField(LinkInfo, LinkInfoSize);
		byte *buffer = malloc(LinkInfo.LinkInfoSize);
		readData(buffer+sizeof(LinkInfo.LinkInfoSize), LinkInfo.LinkInfoSize-sizeof(LinkInfo.LinkInfoSize));
		dword length = LinkInfo.LinkInfoSize;
		memcpy(&LinkInfo, buffer, sizeof(LinkInfo));
		LinkInfo.LinkInfoSize = length;
		bool optional = (LinkInfo.LinkInfoHeaderSize >= 0x24);
		if (optional) {
			if (checkRange(LinkInfo.LinkInfoHeaderSize, 0x24, fileSize))
				errWrongField(LinkInfo, LinkInfoHeaderSize);
			memcpy(&LinkInfo_optional, buffer, sizeof(LinkInfo_optional));
		} else checkFieldConst(LinkInfo, LinkInfoHeaderSize, 0x1C);

		// ##########

		typedef enum {
			LIS_HEADER,
			LIS_SIZE,
			LIS_VolumeIDOffset,
			LIS_LocalBasePathOffset,
			LIS_CommonNetworkRelativeLinkOffset,
			LIS_CommonPathSuffixOffset,
			LIS_LocalBasePathOffsetUnicode,
			LIS_CommonPathSuffixOffsetUnicode
		} LinkInfoStruct;

		#define c_offsets_count_max 8
		int offsets_count = optional ? 8 : 6;
		dword offsets[c_offsets_count_max];
		offsets[LIS_HEADER]						= 0;
		offsets[LIS_SIZE]						= LinkInfo.LinkInfoSize;
		offsets[LIS_VolumeIDOffset]				= LinkInfo.VolumeIDOffset;
		offsets[LIS_LocalBasePathOffset]		= LinkInfo.LocalBasePathOffset;
		offsets[LIS_CommonNetworkRelativeLinkOffset] = LinkInfo.CommonNetworkRelativeLinkOffset;
		offsets[LIS_CommonPathSuffixOffset]		= LinkInfo.CommonPathSuffixOffset;
		if (optional) {
			offsets[LIS_LocalBasePathOffsetUnicode]		= LinkInfo_optional.LocalBasePathOffsetUnicode;
			offsets[LIS_CommonPathSuffixOffsetUnicode]	= LinkInfo_optional.CommonPathSuffixOffsetUnicode;
		}

		int sorted[c_offsets_count_max];
		int j, i, i_min, tmp;
		dword min, cur;
		for (i=0; i<offsets_count; ++i) sorted[i] = i;

		for (j=0; j<offsets_count-1; ++j) {
			min = offsets[sorted[j]]; i_min = j;
			for (i=j+1; i<offsets_count; ++i) {
				cur = offsets[sorted[i]];
				if (cur <= min) {min = cur; i_min = i;}
			}
			tmp = sorted[j];
			sorted[j] = sorted[i_min];
			sorted[i_min] = tmp;
		}
		int lengths[c_offsets_count_max];
		dword firstNotZero = 0;
		for (i=0; i<offsets_count-1; ++i) {
			if (firstNotZero == 0 && offsets[sorted[i]] != 0) firstNotZero = offsets[sorted[i]];
			lengths[sorted[i]] = offsets[sorted[i+1]] - offsets[sorted[i]];
		}
		//lengths[sorted[offsets_count-1]] = 0xFFFFFFFF;

		if (firstNotZero != LinkInfo.LinkInfoHeaderSize) errWrongField(LinkInfo, LinkInfoHeaderSize);

		// ##########

		const char mes_wrongAscizEnd[] = "Неверный конец ASCIZ-строки '%s'";

		if (LinkInfo.LinkInfoFlags & LIF_LocalVolume) {
			// TODO: контроль выхода за границы
			str_offset = buffer+LinkInfo.VolumeIDOffset;
			memcpy(&VolumeID, str_offset, sizeof(VolumeID));
			if (VolumeID.VolumeIDSize != lengths[LIS_VolumeIDOffset]) errWrongField(VolumeID, VolumeIDSize);
			checkFieldConst(VolumeID,VolumeLabelOffset, 0x10)
			printField (VolumeID, DriveSerialNumber);

			str_len = VolumeID.VolumeIDSize-sizeof(VolumeID);
			VolumeID_Data = malloc(str_len);
			memcpy(VolumeID_Data, str_offset+sizeof(VolumeID), str_len);
			if (VolumeID_Data[str_len-1] != 0)
				errAndClose(mes_wrongAscizEnd, VolumeID_Data_strname);
			printVarString(VolumeID_Data, str_len);

		// ######

			#define loadAndPrintVarStr(name, parent, offsetname) { \
				str_offset = buffer+parent.offsetname; \
				name##_Length = lengths[LIS_##offsetname]; \
				if (str_offset[name##_Length-1] != 0) \
					errAndClose(mes_wrongAscizEnd, name); \
				if (name##_Length-1 != 0) { \
					name = malloc(name##_Length); \
					memcpy(name, str_offset, name##_Length); \
					printVarString(name, name##_Length) \
				} \
			}

			loadAndPrintVarStr(LocalBasePath, LinkInfo, LocalBasePathOffset);
			// TODO: через дефис
		}

		// ######

		if (LinkInfo.LinkInfoFlags & LIF_NetworkVolume) {
			// TODO: контроль выхода за границы
			str_offset = buffer+LinkInfo.CommonNetworkRelativeLinkOffset;
			memcpy(&CommonNetworkRelativeLink, str_offset, sizeof(CommonNetworkRelativeLink));
			if (CommonNetworkRelativeLink.CommonNetworkRelativeLinkSize != lengths[LIS_CommonNetworkRelativeLinkOffset])
				errWrongField(CommonNetworkRelativeLink, CommonNetworkRelativeLinkSize);
			checkFieldConst(CommonNetworkRelativeLink, CommonNetworkRelativeLinkFlags, 0x02);
			checkFieldConst(CommonNetworkRelativeLink, NetNameOffset, 0x14);
			checkFieldConst(CommonNetworkRelativeLink, DeviceNameOffset, 0x00);
//			checkFieldConst(CommonNetworkRelativeLink, NetworkProviderType, 0x20000);

			str_len = CommonNetworkRelativeLink.CommonNetworkRelativeLinkSize-sizeof(CommonNetworkRelativeLink);
			CommonNetworkRelativeLink_NetName = malloc(str_len);
			memcpy(CommonNetworkRelativeLink_NetName, str_offset+sizeof(CommonNetworkRelativeLink), str_len);
			if (CommonNetworkRelativeLink_NetName[str_len-1] != 0)
				errAndClose(mes_wrongAscizEnd, CommonNetworkRelativeLink_NetName_strname);
			printVarString(CommonNetworkRelativeLink_NetName, str_len)
		}

		// ######

			loadAndPrintVarStr(CommonPathSuffix, LinkInfo, CommonPathSuffixOffset);

		// ######

		if (optional) {
			loadAndPrintVarStr(LocalBasePathUnicode, LinkInfo_optional, LocalBasePathOffsetUnicode);
			loadAndPrintVarStr(CommonPathSuffixUnicode, LinkInfo_optional, CommonPathSuffixOffsetUnicode);
		}

		free(buffer);
	}

	if (!(ShellLinkHeader.LinkFlags & LF_IsUnicode)) errWrongField(ShellLinkHeader, LinkFlags)

	#define readStringData(LF_flag, StringData) { \
		if (flags & LF_flag) { \
			readField(StringData.CountCharacters); \
			if (checkRange(StringData.CountCharacters, 1, fileSize/2)) { \
				logErrorBegin(); \
				logErrorPart(mes_wrongField[0], StringData##_strname, SomeString_Length_strname); \
				logErrorPart(SomeString_Length_strtype, StringData.CountCharacters); \
				logErrorPart(mes_wrongField[1]); \
				logErrorEnd(); \
				goto close_in; \
			} \
			mallocAndReadData(StringData.String, StringData.CountCharacters * 2); \
			printf(StringData##_strname); \
			printf(": "); \
			fprintStringUnicode(stdout, StringData.String, StringData.CountCharacters); \
			putchar('\n'); \
		} \
	}

	readStringData (LF_HasName, NameString);
	readStringData (LF_HasRelativePath, RelativePath);
	readStringData (LF_HasWorkingDir, WorkingDir);
	readStringData (LF_HasArguments, CommandLineArguments);
	readStringData (LF_HasIconLocation, IconLocation);

	// ###### ExtraData
{
	printf("\n[Дополнительные данные]");
	dword size, data_size;
	dword signature;
	byte *data;
	int offset;
	do {
		readField(size);
		if (size == 0) break;
		readField(signature);
		offset = ftell(fileIn);
		data_size = size-sizeof(size)-sizeof(signature);

		switch (signature) {
		case 0xA0000003:
			printf("\nСигнатура: %08X (%s),\n\tразмер: %u, данные:\n", signature, DLTDataBlock_strname.self, size);
			readField(DLTDataBlock);

			printf("%s: %08X\n", DLTDataBlock_strname.DLTDataSize, DLTDataBlock.DLTDataSize);
			printf("%s: %08X\n", DLTDataBlock_strname.DLTDataVersion, DLTDataBlock.DLTDataVersion);
			printf("%s: ", DLTDataBlock_strname.MachineIDStr);
			fprintStringCP866(stdout, (byte *)(DLTDataBlock.MachineIDStr), 15);
			printf("\n");
			int i;
			#define DLT_GUID_print(GUID_name, tabs) { \
				printf("%s:"tabs, DLTDataBlock_strname.GUID_name); \
				for (i=0; i<16; ++i) printf("%02X", DLTDataBlock.GUID_name[i]); \
				printf("\n"); \
			}
			const char mes_unsuppGUID[] = "Неподдерживаемый формат GUID\n";
			DLT_GUID_print(VolumeID, "\t\t\t");
			DLT_GUID_print(FileID, "\t\t\t");
			if (printParseObjectID(DLTDataBlock.FileID)) logError(mes_unsuppGUID);
			DLT_GUID_print(BirthVolumeID, "\t\t");
			DLT_GUID_print(BirthFileID, "\t");
			if (printParseObjectID(DLTDataBlock.BirthFileID)) logError(mes_unsuppGUID);
		break;
		default:
			mallocAndReadData(data, data_size);
			printf("\nСигнатура: %08X, размер: %u, данные:\n", signature, size);
			printDataHex(data, data_size, offset);
			free(data);
		}
	} while (ftell(fileIn) != fileSize);		// while (true)

	if (size != 0 ) errAndClose("Ошибка: не найдено окончание дополнительных данных");
	if (ftell(fileIn) != fileSize) errAndClose("Ошибка: файл считан не до конца");
}

	rez = false;

close_in:
	if (fclose (fileIn)) {logError("Ошибка при закрытии файла '%s'", fileNameIn); rez = true;}

	return rez;
}

// ############

#ifndef MINGW32
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#else
#include <windows.h>
#endif
bool getAndPrintFileTime(const char *fileName) {
#ifndef MINGW32
	struct stat statStruct;
	if (stat(fileName, &statStruct)) {logError("stat"); return true;}

	/*#define c_str_size 1024
	char str[c_str_size];
	uint64_t time;
	#define printTimeSingle(name, sym, value) { \
		time = value.tv_nsec | (((int64_t)value.tv_sec) << 32); \
		printf("%s", name); \
		struct tm *pTimeStruct = gmtime(&value.tv_sec); \
		strftime(str, c_str_size-1, "%d.%m.%Y %H:%M:%S", pTimeStruct); \
		printf("%s", str); \
		printf("  " sym " = " c_016llX_format "\n", c_016llX_args(time)); \
	}*/

	uint64_t time;
	#define printTimeSingle(name, tabs, sym, value) {\
		time = value.tv_nsec | (((int64_t)value.tv_sec) << 32); \
		printTimeString (name, tabs, value.tv_sec, sym, time); \
	}

	printTimeSingle("Время последнего доступа к файлу", "  ", 'A', statStruct.st_atim);
	printTimeSingle("Время последней модификации файла", " ", 'M', statStruct.st_mtim);
	printTimeSingle("Время последнего изменения файла", "  ", 'C', statStruct.st_ctim);

	//#undef c_str_size

	return false;
#else
	bool rez;

	FILETIME CreationTime, LastAccessTime, LastWriteTime;
	HANDLE hFile = CreateFile(
			fileName,
			GENERIC_READ,				// open for reading
			0,							// do not share
			NULL,						// no security
			OPEN_EXISTING,				// existing file only
			FILE_ATTRIBUTE_NORMAL,		// normal file
			NULL						// no attr. template
	);
	if (hFile == INVALID_HANDLE_VALUE) {logError("CreateFile"); return true;}
	if (GetFileTime(hFile, &CreationTime, &LastAccessTime, &LastWriteTime) == 0)
		{logError("GetFileTime"); rez = true; goto close;}

	uint64_t time;
	/*#define printTimeSingle(name, sym, ftime) { \
		time = ftime.dwLowDateTime | (((int64_t)ftime.dwHighDateTime) << 32); \
		printf("%s", name); \
		printFileTime(time); \
		printf("  " sym " = " c_016llX_format "\n", c_016llX_args(time)); \
	}*/
	#define printTimeSingle(name, tabs, sym, ftime) { \
		time = ftime.dwLowDateTime | (((int64_t)ftime.dwHighDateTime) << 32); \
		printTimeString (name, tabs, fileTime2UTC(time), sym, time); \
	}
	printTimeSingle("Время создания файла", "              ", 'C', CreationTime);
	printTimeSingle("Время последнего доступа к файлу", "  ", 'A', LastAccessTime);
	printTimeSingle("Время последней модификации файла", " ", 'M', LastWriteTime);
	rez = false;

close:
	if (CloseHandle(hFile) == 0) rez = true;
	return rez;
#endif
}

// ############

#define isPStorageNotNULL(pStorage) (pStorage != NULL && *pStorage == NULL)

bool commandLine (int argc, char **argv, char **pFileNameIn) {
	assert (isPStorageNotNULL (pFileNameIn));

	printTimeMode = TPM_HEX;
	bool mode_flag = false;

	int opt;
	while ((opt = getopt (argc, argv, ":hxd")) != -1) {
		switch (opt) {
		case 'h':
			printf ("\
Параметры запуска: %s [-h] [-x|-d] lnk_file \n\
Ключи:\n\
    -h    Вывести эту справку\n\
    -x    Шестнадцатеричный формат времени\n\
    -d    Десятичный формат времени\n",
					argv[0]);
			return true;
		break;
		case 'x':
			if (mode_flag) {logError("Повторное задание фомата времени"); return true;}
			mode_flag = true;
			printTimeMode = TPM_HEX;
		break;
		case 'd':
			if (mode_flag) {logError("Повторное задание фомата времени"); return true;}
			mode_flag = true;
			printTimeMode = TPM_DEC;
		break;
		break;
		case ':':
			logError("Не найдены аргументы ключа '-%c' \n", optopt);
			return true;
		break;
		case '?':
			logError("Неизвестный ключ '-%c'\n", optopt);
			return true;
		break;
		default:
			logError("Неверный ключ '-%c'\n", optopt);
			return true;
		break;
		}
	}

	argc -= optind;
	if (argc > 1) {logError("Неверное число аргументов"); return true;}
	if (argc < 1) {logError("Не задано имя файла"); return true;}
	*pFileNameIn = argv[optind];

	return false;
}


int main (int argc, char **argv) {

	bool rez;
	char *fileNameIn = NULL;

	if (commandLine (argc, argv, &fileNameIn)) {rez = true; goto err;}

	printf("Имя файла ярлыка: ");
	#ifdef MINGW32
	fprintStringCP1251(stdout, fileNameIn, strlen(fileNameIn));
	#else
	printf("%s", fileNameIn);
	#endif
	printf("\n\n");
	
	printf("[Метаданные файла ярлыка]\n");
	if (getAndPrintFileTime(fileNameIn)) {rez = true; goto err;}
	printf("\n[Содержимое ярлыка]\n");
	if (readAndParseFile(fileNameIn)) {rez = true; goto err;}

	rez = false;

err:
	return (rez ? EXIT_FAILURE : EXIT_SUCCESS);
}
