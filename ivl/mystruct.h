#ifndef MYSTRUCT_H
#define MYSTRUCT_H


#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>

class BlifWire;
class BlifElaborate;


using namespace std;


class BlifWire
{
public:
    BlifWire();
    void setOutput(const string& outPutName);
    string getOutput(); //��ȡ����˿���

    void addHead(const string& headName);
    const vector<string>& getHead();//��ȡ��ͷ

    void addLogic(const string& logicRow);
    const vector<string>& getLogic();//��ȡ�߼�ʽ

    string getHeadoutput(); //��ȡ��ͷ������˿���
private:
    string Output;
    vector<string> head;  // ��ͷ������ {"d", "e", "f", "j"}
    vector<string> logic;  // �߼����ʽ
};

class BlifElaborate
{
public:
    BlifElaborate();
    void setFileName(const string& filename);
    string getFileName();
    void setInput(const string& inputName);
    void setOutput(const string& outputName);
    void setWire(const string& wireName);
    const vector<string>& getInput() const;
    const vector<string>& getOutput() const;
    void BlifElaborateRead();  // ����
    void Blif2Verilog();   //���

private:
    string fileName; //��blif�ļ�����
    vector<BlifWire> myBlifWires;//�洢���е����ӹ�ϵ�����ݽṹ
    vector<string> output; //blif�е�����˿�
    vector<string> input; //blif�е�����˿�
    vector<string> wire; //blif�еĶ˿�
    ifstream file;
};

class mystruct
{
};

#endif // !MYSTRUCT_H
