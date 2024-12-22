#ifndef _UTILS_H_
#define _UTILS_H_

#include <windows.h>
#include <vector>

#define PAT_LEN 256
#define PAT_NUM 226
#define P_SIZE	3200000

typedef struct
{
	char pat[PAT_LEN];
	short len;
}PatternInfo;

struct lzf_f
{
    unsigned short int length = 0;        //压缩字段长度
    unsigned short int distance = 0;      //压缩字段偏移量
    unsigned short int literals = 0;      //未压缩字段长度
};
//取消文件流重定向
void CancelFileStream();
//加载文本
std::vector<char> LoadText(const char* filename);
//加载pattern
void LoadPattern(std::vector<PatternInfo>& patList, char* strPatFile);
//LZF压缩文本文件
void LzfCompress(std::vector<char> inputBuffer_, const char* filename);
//LZF压缩文件预处理
std::pair<std::vector<lzf_f>, std::string> Ltf(const char* filename);

#endif	// _UTILS_H_
