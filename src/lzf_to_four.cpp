#include <iostream>
#include <vector>
#include <bitset>
#include "utils.h"

using namespace std;

vector<lzf_f> l;

enum class ReadState {
    START,                  //当前字符为标识位
    LITERALS,               // 当前读入字符为literals
    TOKEN,                  // 当前读入字符为token
    LENGTH,                 // 当前读入字符为length
    LENGTH_EXTENSION,       // length长度>7，需要再读一个字符求和
    DISTANCE,               // 当前读入字符为distance
    END                     // 读入结束
};

pair<vector<lzf_f>, string> Ltf(const char* filename){
    freopen(filename, "rb", stdin);
    freopen("result.txt", "wb", stdout);

    char ch;                        // 当前读入字符
    int cnt = 0;                    // 用于计数辅助token读入
    string uncompressed;            // 解压字符串

    lzf_f tmp;
    ReadState curStatus = ReadState::START;

    while(cin.get(ch)){
        // 将读取到的字符转型方便后续操作
        unsigned int uc = (unsigned char)ch;
        // 开始态，根据读到的数据确定是后续字段是压缩字段还是未压缩字段
        if(curStatus == ReadState::START) {
            // 如果是压缩字段（uc>=0b11100000）则跳转到length读取
            if (uc >= 32) {
                curStatus = ReadState::LENGTH;
            }
            // 如果是未压缩字段则直接跳转到literals读取
            else {
                curStatus = ReadState::LITERALS;
            }
        }
        // literals即uc+1
        if(curStatus == ReadState::LITERALS){
            tmp.literals = uc + 1;
            curStatus = ReadState::TOKEN;
        }
        // token即未压缩字段本身，读取存入string即可
        else if(curStatus == ReadState::TOKEN){
            uncompressed += uc;
            ++cnt;
            if(cnt == tmp.literals && tmp.literals == 32) curStatus = ReadState::END;
            else if(cnt == tmp.literals) {
                curStatus = ReadState::LENGTH;
                continue;
            }    // cnt == literals说明token读入完成
        }

        if(curStatus == ReadState::LENGTH){
            // 判断高三位是不是111，是的话读下一位求和
            if((uc >> 5) == 0b111){
                tmp.length = 7;
                tmp.distance = (uc & 0b00011111);
                curStatus = ReadState::LENGTH_EXTENSION;
            }
            // 高三位不是111则先读取低五位存入distance然后跳转状态读distance低5位
            else{
                tmp.length = (uc >> 5) + 0b10;
                tmp.distance = (uc & 0b00011111);
                curStatus = ReadState::DISTANCE;
            }
        }
        // 如果length高三位为111即扩展后续8位加和
        else if(curStatus == ReadState::LENGTH_EXTENSION){
            tmp.length += uc + 2;
            curStatus = ReadState::DISTANCE;
        }
        // 左移8位后把后五位加和
        else if(curStatus == ReadState::DISTANCE){
            tmp.distance <<= 8;
            tmp.distance += uc;
            curStatus = ReadState::END;
        }
        if(curStatus == ReadState::END){
            l.push_back(tmp);
            int copyStartPos = uncompressed.size() - tmp.distance - 1;
            for (unsigned int i = 0; i < tmp.length; ++i) {
                uncompressed += uncompressed[copyStartPos + i];
            }
            // 初始化
            tmp.distance = 0;
            tmp.length = 0;
            tmp.literals = 0;
            cnt = 0;
            curStatus = ReadState::START;
        }
    }

    // 取消重定向
    CancelFileStream();
    pair<vector<lzf_f>, string> res;
    res.first = l;
    res.second = uncompressed;
    return res;
}