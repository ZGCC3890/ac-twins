# ac-twins
使用ac自动机重新实现的twins算法，对redis数据库中ziplist数据类型的压缩算法LZF所产生的.lzf压缩文件进行查询优化
## 工作参数
<匹配次数> <数据集选择>
数据集选择：  
1. Alexa.cn
2. Alexa.com
## 文件说明
### ./src：
#### ac_scan.cpp/.h  
ac自动机+twins实现的模式匹配，函数调用见头文件
#### lzf_to_four.cpp
.lzf文件解压并预处理，生成解压文件和metadata
### ./lzf
redis源码中lzf源码  
lzf_c 压缩源码  
lzf_d 解压源码
### ./cmake-build-debug
input.txt 输入文本（可在源码中修改输入文本名）  
pattern.txt 模式串  
output.lzf 输入文本的lzf压缩文件  

以下为测试调试用中途输出文件：  
state.txt 状态表
uncompressedText.txt .lzf文件解压后文本  
result.txt metadata