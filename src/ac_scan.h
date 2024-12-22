#ifndef _AC_SCAN_H
#define _AC_SCAN_H

#include "utils.h"

#define ALPHABET_SIZE 256

struct ACNode {
    int children[ALPHABET_SIZE];  // 子节点
    int fail;                     // 失败指针
    bool isEnd;                   // 是否为模式串的结束
    std::vector<int> output;      // 匹配模式的索引

    ACNode() {
        memset(children, -1, sizeof(children));
        fail = -1;
        isEnd = false;
    }
};

//初始化ac自动机
void InitACAutomaton();
//根据pattern建表
void InsertPattern(const char* pattern, int len, int index);
//构建失败指针
void BuildFailPointer();
//AC匹配
int ACNextState(int currentState, char ch);
void ACCompressedScan(lzf_f* metaData, int metaDataLength, int* matchedCounts, const char* uncompressed, int* state);
//整合函数
void ACCompressedMatch(lzf_f* metaData, int metaDataLength, std::vector<PatternInfo> patInfo, const char* uncompressed, int* state, int* matchedCounts);

#endif //_AC_SCAN_H
