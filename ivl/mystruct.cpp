#include"mystruct.h"

BlifElaborate::BlifElaborate()
{
}

void BlifElaborate::BlifElaborateRead()
{
    ifstream file("test1.blif"); // �滻Ϊ����ļ�·��
    if (!file.is_open()) {
        cerr << "�޷����ļ�" << endl;
    }

    string line;
    while (getline(file, line)) {

        istringstream iss(line);
        string token;

        // ��ȡ��һ�����ʣ�ͨ����ָ��
        iss >> token;
        if (token == ".model") {
            // ��ȡģ�����ƣ������ո�
            std::string model_name;
            if (iss >> model_name) { // ȷ�����Զ�ȡģ������
                setFileName(model_name); // ʹ�����ú��������ļ���
            }
        }

        else if (token == ".inputs") {
            // ��ȡ����˿�
            while (iss >> token) {
                setInput(token); // ʹ�����ú����������˿�
                setWire(token);// ʹ�����ú���������ж˿�
            }
        }
        else if (token == ".outputs") {
            // ��ȡ����˿�
            while (iss >> token) {
                setOutput(token); // ʹ�����ú����������˿�
                setWire(token);// ʹ�����ú���������ж˿�
            }
        }
        else if (token == ".names") {
            BlifWire myBlifWire;//Ϊÿ��names������һ����װ�����ݽṹ

            // �������� .names ���е�����
            while (iss >> token) {
                setWire(token);// ʹ�����ú�����Ӷ˿���û�еĶ˿�
                myBlifWire.addHead(token);//������������ж˿�
            }
            string outputName = myBlifWire.getHeadoutput(); 
            myBlifWire.setOutput(outputName);//��������˿ڵķ���
            string nexline;
            while (getline(file, nexline) && (isdigit(nexline[0]) || nexline[0] == '-')) {
                const vector<string>& it = myBlifWire.getHead();//��ñ�ͷ
                string expr;
                bool first = true;
                for (size_t i = 0; i < it.size(); ++i) {
                    if (nexline[i] == '1') {
                        if (!first) expr += " & ";
                        expr += it[i];
                        first = false;
                    }
                    else if (nexline[i] == '0') {
                        if (!first) expr += " & ";
                        expr += "!" + it[i];
                        first = false;
                    }
                }
                myBlifWire.addLogic(expr);
            }
            myBlifWires.push_back(myBlifWire);


            if (!nexline.empty() && !isdigit(nexline[0])) {
                file.putback('\n');
                for (auto it = nexline.rbegin(); it != nexline.rend(); ++it) {
                    file.putback(*it);
                }
            }
        }
    }
    file.close();

}

void BlifElaborate::setFileName(const string& filename)
{
    fileName = filename;
}

string BlifElaborate::getFileName()
{
    return fileName;
}

void BlifElaborate::setInput(const string& inputName)
{
    input.push_back(inputName);
}

void BlifElaborate::setOutput(const string& outputName)
{
    output.push_back(outputName);
}

void BlifElaborate::setWire(const string& wireName)
{
    auto it = find(wire.begin(), wire.end(), wireName);
    if (it == wire.end()) {
        // ���outputName����outputs�У������
        wire.push_back(wireName);
    }
}

void BlifElaborate::Blif2Verilog()
{
    std::ofstream outFile(this->fileName + ".v");

    // д���ļ���
    string fileName = getFileName();
    outFile << "module " << fileName <<"(clk, rst";
    for (const auto& output : output) {
        outFile << "," << output;
    }
    for (const auto& input : input) {
        outFile << "," << input;
    }

    outFile << ");"<< endl;
    outFile << "input clk,rst;" << endl;
    for (const auto& output : output) {
        outFile << "output " << output << ";" << endl;
    }
    for (const auto& input : input) {
        outFile << "input  "<<input << ";" << endl;
    }
    outFile << endl;

    for (const auto& wire : wire) {
        outFile << "wire  " << wire << ";" << endl;
    }
    outFile << endl;

    for (auto blifWire : myBlifWires) {
        outFile << "assign " << blifWire.getOutput() << " = ";
        const auto& it = blifWire.getLogic();
        for (size_t i = 0; i < it.size(); ++i) {
            //�жϵ�ǰ���Ƿ���ڸ��Ͽ�
            bool hasAnd = false;
            for (char ch : it[i]) {
                if (ch == '&') {
                    hasAnd = true;
                }
            }
            //����
            if (hasAnd && it.size() > 1) {
                outFile << "(" << it[i] << ")";
            }
            else {
                outFile << " " << it[i] << " ";
            }
            if (i < it.size() - 1) {
                outFile << " | ";
            }
        }
        outFile << ";\n";
    }

    outFile << endl;

    outFile << "endmodule"<< endl;

    outFile.close();

    std::cout << "������д�뵽�ļ��С�" << std::endl;

}

BlifWire::BlifWire()
{
}

void BlifWire::setOutput(const string& outPutName)
{
    Output = outPutName;
}

void BlifWire::addLogic(const string& logicRow)
{
    logic.push_back(logicRow);
}

void BlifWire::addHead(const string& headName)
{
    head.push_back(headName);
}

const vector<string>& BlifWire::getHead()
{
    return head;
}

string BlifWire::getOutput()
{
    return Output;
}

string BlifWire::getHeadoutput()
{
    return head.back();
}


const vector<string>& BlifWire::getLogic()
{
    return logic;
}




