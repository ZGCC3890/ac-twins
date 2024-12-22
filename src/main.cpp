#include <iostream>
#include <cstring>
#include <vector>
#include <chrono>
#include "utils.h"
#include "ac_scan.h"

#include "./lzf/lzf_c.c"
#include "./lzf/lzf_d.c"
//#include "./lzf/lzf.h"

using namespace std;

int main(int argc, char** argv) {
    if (argc < 3)
        return 0;

    // *definitions
    int sets = atoi(argv[2]);        // date set
    int loop = atoi(argv[1]);        // loop
    char patFile[] = "pattern.txt";     // pattern�ļ���

    char szDir[256] = {0};
    if (sets == 1) {
        strcpy(szDir, "Alexa.cn");
    } else {
        strcpy(szDir, "Alexa.com");
    }
    //����ģʽ
    vector<PatternInfo> patInfo;
    LoadPattern(patInfo, patFile);
    cout << "LoadPattern Completed" << endl;

    //�ļ����뻺����
    vector<char> inputBuffer = LoadText("hamlet.txt");       // <ԭʼ�ļ���>
    cout << "LoadText Completed" << endl;

    //lzfѹ��
    LzfCompress(inputBuffer, "output.lzf");     // <���뻺����><ѹ���ļ���>
    cout << "LzfCompress Completed" << endl;

    //ת��ѹ���ļ�
    pair<vector<lzf_f>, string> res;
    res = Ltf("output.lzf");                               // <ѹ���ļ���>
    freopen("uncompressedText.txt", "wb", stdout);
    cout << res.second;
    CancelFileStream();
    cout << "LzfTransform Completed" << endl;

    freopen("result.txt", "wb", stdout);
    for (const auto &i: res.first) {
        cout << "(" << i.literals << " " << i.length << " " << i.distance << ")";
        //�����ʽ(literals length distance token)
    }
    CancelFileStream();
    cout << "Result.txt Completed" << endl << endl;

    lzf_f* metaData = res.first.data();
    int metaDataLength = res.first.size();
    const char *uncompressed = res.second.c_str();
    int uncompressedSize = res.second.size();

    int* state = new int[uncompressedSize]();     // ״̬����
    int* result = new int[patInfo.size()]();      // �������

    chrono::duration<double, milli> elapsed{};
    double cntTime = 0;
    auto start = chrono::high_resolution_clock::now();

    for (int i = 0; i < loop; ++i) {
        //��ʼ��AC�Զ���
        InitACAutomaton();
        //ѹ��ƥ��
        ACCompressedMatch(metaData, metaDataLength, patInfo, uncompressed, state, result);
    }
    auto end = chrono::high_resolution_clock::now();
    elapsed = end - start;
    cntTime += elapsed.count();
    for (int i = 0; i < patInfo.size(); ++i) {
        cout << "Pattern " << patInfo[i].pat << " matched " << result[i] / loop<< " times." << std::endl;
    }
    cout << loop << "��ƽ������ʱ��: " << cntTime / loop << " ����" << endl;
    delete[] result;
}

vector<char> LoadText(const char* filename){
    freopen(filename, "rb", stdin);
    vector<char> inputBuffer((istreambuf_iterator<char>(cin)), istreambuf_iterator<char>());
    if(inputBuffer.empty()){
        cerr << "Error: " << filename << "Ϊ��" << endl;
    }
    return inputBuffer;
}

void LzfCompress(vector<char> inputBuffer_, const char* filename){
    vector<char> outputBuffer_(inputBuffer_.size() * 2);
    unsigned int compressedSize = lzf_compress(inputBuffer_.data(), inputBuffer_.size(), outputBuffer_.data(), outputBuffer_.size());
    if(compressedSize == 0){
        cerr << "Error: ѹ��ʧ��" << endl;
    }
    freopen(filename, "wb", stdout);
    if (stdout == nullptr) {
        cerr << "Error: �޷�����" << filename << endl;
    }
    fwrite(outputBuffer_.data(), 1, compressedSize, stdout);
    inputBuffer_.clear();
    outputBuffer_.clear();
    CancelFileStream();
}

void LoadPattern(vector<PatternInfo>& patList, char* strPatFile)
{
    FILE* pFile = fopen(strPatFile, "r");
    if (pFile == NULL)
    {
        printf("could not open pattern file\n");
        exit(0);
    }

    while (true)
    {
        PatternInfo pi = { 0 };
        if (fscanf(pFile, "%[^\n]\n", pi.pat) == -1)
            break;
        pi.len = strlen(pi.pat);
        patList.push_back(pi);
    }
}

void CancelFileStream(){
    // ȡ���ض���
    freopen("CON","r",stdin);
    freopen("CON","w",stdout);
}