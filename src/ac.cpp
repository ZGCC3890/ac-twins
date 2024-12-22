#include <iostream>
#include <vector>
#include <bitset>
#include <queue>
#include "ac_scan.h"

using namespace std;

std::vector<ACNode> acTrie;  // 存储AC自动机的所有节点

void InitACAutomaton() {
    acTrie.clear();
    acTrie.push_back(ACNode());  // 初始化根节点
}

int CreateNode(){
    acTrie.emplace_back();
    return acTrie.size() - 1;
}

void InsertPattern(const char* pattern, int len, int index) {
    int currentNode = 0;
    for (int i = 0; i < len; ++i) {
        int childIndex = static_cast<unsigned char>(pattern[i]);
        if (acTrie[currentNode].children[childIndex] == -1) {       // 判断当前有无该字符子节点
            int newNode = CreateNode();                             // 没有则创建子节点
            acTrie[currentNode].children[childIndex] = newNode;
        }
        currentNode = acTrie[currentNode].children[childIndex];     // 有则跳转到子节点
    }
    acTrie[currentNode].isEnd = true;
    acTrie[currentNode].output.push_back(index);
}

void BuildFailPointer(){
    queue<int> q;

    // 初始化子节点失败指针
    for (int i = 0; i < ALPHABET_SIZE; ++i) {
        if (acTrie[0].children[i] != -1) {
            acTrie[acTrie[0].children[i]].fail = 0;
            q.push(acTrie[0].children[i]);
        } else {
            acTrie[0].children[i] = 0;  // 根节点的空子节点指向根节点
        }
    }

    while (!q.empty()) {
        int currentNode = q.front();
        q.pop();

        for (int i = 0; i < ALPHABET_SIZE; ++i) {
            int child = acTrie[currentNode].children[i];
            if (child != -1) {
                int failNode = acTrie[currentNode].fail;

                // 找到一个存在的失败路径
                while (acTrie[failNode].children[i] == -1 && failNode != 0) {
                    failNode = acTrie[failNode].fail;
                }

                acTrie[child].fail = acTrie[failNode].children[i];  // 更新失败指针
                acTrie[child].output.insert(acTrie[child].output.end(), acTrie[acTrie[child].fail].output.begin(), acTrie[acTrie[child].fail].output.end());

                q.push(child);
            }
        }
    }
}

int ACNextState(int currentState, char ch) {
    int childIndex = static_cast<unsigned char>(ch);
    while (acTrie[currentState].children[childIndex] == -1 && currentState != 0) {
        currentState = acTrie[currentState].fail;  // 沿失败指针回退
    }
    return acTrie[currentState].children[childIndex];
}

void ACCompressedScan(lzf_f* metaData, int metaDataLength, int* matchedCounts, const char* uncompressedText, int* state) {
    int currentState = 0;  // 从根节点开始
    int textIndex = 0;     // 当前字符位置
    int g_match = 0;

    for (int j = 0; j < metaDataLength; ++j) {
        lzf_f block = metaData[j];
        // 处理未压缩字段
        for (int i = 0; i < block.literals; ++i) {
            char currentChar = uncompressedText[textIndex];
            // AC 自动机状态转移
            currentState = ACNextState(currentState, currentChar);
            state[textIndex++] = currentState;
            // 检查结果
            if (!acTrie[currentState].output.empty()) {
                for (int patternIndex: acTrie[currentState].output) {
                    matchedCounts[patternIndex]++;
                }
            }
//            if(acTrie[currentState].isEnd) g_match++;
        }

        // 处理压缩字段
        if (block.length > 0) {
            int startIndex = textIndex - block.distance - 1;  // 计算偏移位置
            // 复制标识位
            bool coping = false;
            for (int i = 0; i < block.length; ++i) {
                if (!coping) {
                    // 更新当前文本中的状态
                    char currentChar = uncompressedText[textIndex];
                    // 当前字符前一位字符状态
                    int referenceState = state[textIndex - 1];
                    // 偏移位置前一位字符状态
                    int referenceRState = state[startIndex - 1 + i];
                    if (referenceState == referenceRState) {
                        coping = true;
                    } else {
                        currentState = ACNextState(currentState, currentChar);
                    }
                }
                if (coping) {
                    currentState = state[startIndex + i];
                }
                state[textIndex] = currentState;

                if (!acTrie[currentState].output.empty()) {
                    for (int patternIndex: acTrie[currentState].output) {
                        matchedCounts[patternIndex]++;
                    }
                }
//                if(acTrie[currentState].isEnd) g_match++;
                textIndex++;
            }
        }
    }
//    freopen("state.txt", "wb", stdout);
//    for (int i = 0; i < uncompressedText.size(); ++i) {
//        cout << state[i] << " ";
//    }
//    CancelFileStream();
}


void ACCompressedMatch(lzf_f* metaData, int metaDataLength, vector<PatternInfo> patInfo, const char* uncompressed, int* state, int* matchedCounts){
    acTrie.clear();
    CreateNode();  // 创建根节点

    // 插入所有模式并构建自动机
    for (int i = 0; i < patInfo.size(); ++i) {
        InsertPattern(patInfo[i].pat, patInfo[i].len, i);
    }
    BuildFailPointer();

    // 扫描压缩数据并进行模式匹配
    ACCompressedScan(metaData, metaDataLength, matchedCounts, uncompressed, state);
}