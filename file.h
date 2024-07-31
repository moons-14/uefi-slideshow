#ifndef _FILE_H_
#define _FILE_H_

#include "graphics.h"

#define MAX_FILE_NAME_LEN 4
#define MAX_FILE_NUM 10
#define MAX_FILE_BUF 1024

#define EFI_FILE_MODE_READ 0x0000000000000001
#define EFI_FILE_MODE_WRITE 0x0000000000000002
#define EFI_FILE_MODE_CREATE 0x8000000000000000

struct FILE
{
    unsigned short name[MAX_FILE_NAME_LEN];
};

typedef unsigned char BYTE;  // 1byte
typedef unsigned short WORD; // 2byte
typedef unsigned int DWORD;  // 4byte
typedef int LONG;
struct BITMAPFILEHEADER
{
    WORD bftye;       // ファイルタイプ BM
    DWORD bfsize;     // ファイル全体のサイズ
    WORD bfreserved1; // 予約領域1
    WORD bfreserved2; // 予約領域2
    DWORD bfoffbits;  // ファイル先頭から画像データまでのオフセット
};
struct BITMAPINFOHEADER
{
    DWORD bisize; // 情報ヘッダのサイズ
    DWORD biw;     // 画像の幅
    DWORD bih;     // 画像の高さ
    WORD biplanes; // プレーン数 1
    WORD bibitcount; // 1画素あたりのビット数
    DWORD bicompression; // 圧縮形式
    DWORD bisizeimage; // 画像データのサイズ
    DWORD bixpelspermeter; // 水平解像度
    DWORD biypelspermeter; // 垂直解像度
    DWORD biclrused; // カラーパレットの色数
    DWORD biclrimportant; // カラーパレットの中で重要な色数
};

struct BMPImage
{
    struct BITMAPFILEHEADER fileHeader;
    struct BITMAPINFOHEADER infoHeader;
    char *data; // 24bitカラー
};

struct RGB
{
    unsigned char blue;
    unsigned char green;
    unsigned char red;
};

struct BGR
{
    unsigned char red;
    unsigned char green;
    unsigned char blue;
};


#endif