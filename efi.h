#ifndef _EFI_H
#define _EFI_H

typedef unsigned long long EFI_STATUS;
typedef unsigned long long UINTN;

#define EFI_SUCCESS
#define EFI_ERROR
#define EFI_UNSUPPORTED

struct EFI_INPUT_KEY
{
    unsigned short ScanCode;
    unsigned short UnicodeChar;
};

struct EFI_GUID
{
    unsigned int Data1;
    unsigned short Data2;
    unsigned short Data3;
    unsigned char Data4[8];
};

enum EFI_MEMORY_TYPE
{
    EfiReservedMemoryType,
    EfiLoaderCode,
    EfiLoaderData,
    EfiBootServicesCode,
    EfiBootServicesData,
    EfiRuntimeServicesCode,
    EfiRuntimeServicesData,
    EfiConventionalMemory,
    EfiUnusableMemory,
    EfiACPIReclaimMemory,
    EfiACPIMemoryNVS,
    EfiMemoryMappedIO,
    EfiMemoryMappedIOPortSpace,
    EfiPalCode,
    EfiMaxMemoryType
};

struct EFI_SYSTEM_TABLE
{
    char _buf[44];
    struct EFI_SIMPLE_TEXT_INPUT_PROTOCOL
    {
        EFI_STATUS _buf;
        EFI_STATUS (*ReadKeyStroke)(struct EFI_SIMPLE_TEXT_INPUT_PROTOCOL *This, struct EFI_INPUT_KEY *Key);
        void *WaitForKey;
    } *ConIn;
    EFI_STATUS _buf2;
    struct EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL
    {
        EFI_STATUS _buf;
        EFI_STATUS (*OutputString)(struct EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL *This, unsigned short *String);
        EFI_STATUS _buf2[4];
        EFI_STATUS (*ClearScreen)(struct EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL *This);
    } *ConOut;
    EFI_STATUS _buf3[3];
    struct EFI_BOOT_SERVICES
    {
        char _buf1[24];

        //
        // Task Priority Services
        //
        EFI_STATUS _buf2[2];

        //
        // Memory Services
        //
        EFI_STATUS _buf3[3];
        EFI_STATUS (*AllocatePool)(enum EFI_MEMORY_TYPE PoolType, UINTN Size, void **Buffer);
        EFI_STATUS (*FreePool)(void *Buffer);

        //
        // Event & Timer Services
        //
        EFI_STATUS _buf4[2];
        EFI_STATUS (*WaitForEvent)(UINTN NumberOfEvents, void **Event, UINTN *Index);
        EFI_STATUS _buf4_2[3];

        //
        // Protocol Handler Services
        //
        EFI_STATUS _buf5[9];

        //
        // Image Services
        //
        EFI_STATUS _buf6[5];

        //
        // Miscellaneous Services
        //
        EFI_STATUS _buf7[2];
        EFI_STATUS (*SetWatchdogTimer)(UINTN Timeout, UINTN WatchdogCode, UINTN DataSize, unsigned short *WatchdogData);

        //
        // DriverSupport Services
        //
        EFI_STATUS _buf8[2];

        //
        // Open and Close Protocol Services
        //
        EFI_STATUS _buf9[3];

        //
        // Library Services
        //
        EFI_STATUS _buf10[2];
        EFI_STATUS (*LocateProtocol)(struct EFI_GUID *Protocol, void *Registration, void **Interface);
        EFI_STATUS _buf10_2[2];

        //
        // 32-bit CRC Services
        //
        EFI_STATUS _buf11;

        //
        // Miscellaneous Services
        //
        EFI_STATUS _buf12[3];
    } *BootServices;
};

extern struct EFI_SYSTEM_TABLE *ST;

// 画面描画のためのプロトコル
struct EFI_GRAPHICS_OUTPUT_PROTOCOL
{
    EFI_STATUS _buf[3];
    struct EFI_GRAPHICS_OUTPUT_PROTOCOL_MODE
    {
        unsigned int MaxMode;
        unsigned int Mode;
        struct EFI_GRAPHICS_OUTPUT_MODE_INFORMATION
        {
            unsigned int Version;
            unsigned int HorizontalResolution;
            unsigned int VerticalResolution;
            enum EFI_GRAPHICS_PIXEL_FORMAT
            {
                PixelRedGreenBlueReserved8BitPerColor,
                PixelBlueGreenRedReserved8BitPerColor,
                PixelBitMask,
                PixelBltOnly,
                PixelFormatMax
            } PixelFormat;
        } *Info;
        EFI_STATUS SizeOfInfo;
        EFI_STATUS FrameBufferBase;
    } *Mode;
};

struct EFI_GRAPHICS_OUTPUT_BLT_PIXEL
{
    unsigned char Blue;
    unsigned char Green;
    unsigned char Red;
    unsigned char Reserved;
};

// ファイル関連のプロトコル
struct EFI_FILE_PROTOCOL
{
    EFI_STATUS _buf;
    EFI_STATUS (*Open)(struct EFI_FILE_PROTOCOL *This, struct EFI_FILE_PROTOCOL **NewHandle, unsigned short *FileName, UINTN OpenMode, UINTN Attributes);
    EFI_STATUS (*Close)(struct EFI_FILE_PROTOCOL *This);
    EFI_STATUS _buf2;
    EFI_STATUS (*Read)(struct EFI_FILE_PROTOCOL *This, UINTN *BufferSize, void *Buffer);
    EFI_STATUS (*Write)(struct EFI_FILE_PROTOCOL *This, UINTN *BufferSize, void *Buffer);
    EFI_STATUS _buf3[4];
    EFI_STATUS (*Flush)(struct EFI_FILE_PROTOCOL *This);
};
struct EFI_SIMPLE_FILE_SYSTEM_PROTOCOL
{
    EFI_STATUS Revision;
    EFI_STATUS (*OpenVolume)(struct EFI_SIMPLE_FILE_SYSTEM_PROTOCOL *This, struct EFI_FILE_PROTOCOL **Root);
};
struct EFI_FILE_INFO
{
    unsigned char _buf[80];
    unsigned short FileName[];
};

extern struct EFI_SYSTEM_TABLE *ST;
extern struct EFI_GRAPHICS_OUTPUT_PROTOCOL *GOP;
extern struct EFI_SIMPLE_FILE_SYSTEM_PROTOCOL *SFSP;

void efi_init(struct EFI_SYSTEM_TABLE *SystemTable);

#endif