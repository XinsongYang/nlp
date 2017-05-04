#include<stdio.h>
#include<iostream>
#include<fstream> 
#include<vector>
#include<set>
#include<string>
#include <algorithm>
#include <time.h>

using namespace std;

locale china("chs");//����

set<wstring> dictionary;
const int maxLength = 5;//Ĭ�������

//��ȥstring���ض����ַ�
void sub(string &str, const char deStr, const string subStr = "")
{
	int begin = 0;
	begin = str.find(deStr, begin);
	while (begin != -1)
	{
		str.replace(begin, 1, subStr);
		begin = str.find(deStr, begin);
	}
}


void sub(string &str, const string& deStr, const string subStr = "")
{
	int begin = 0;
	begin = str.find(deStr, begin);
	while (begin != -1)
	{
		str.replace(begin, deStr.length(), subStr);
		begin = str.find(deStr, begin + subStr.length() - deStr.length());
	}
}

//��ѧϰ�ı�����Ԥ����
void preLearn(const string fileIn, const string fileOut)
{
	string str, tmpStr;
	ifstream textIn(fileIn, ios::in);
	if (textIn)
	{
		while (textIn >> tmpStr)
		{
			//��ȥ�ʵ�ע������Ա�ע���֣����ӿո�������
			int split = min(tmpStr.find('/'), tmpStr.find('{'));
			str.append(tmpStr.substr(0, split));
			str.append(" ");
		}
	}
	else
		cout << "Error:cant open preText file" << endl;
	textIn.close();

	//ȡǰ80%
	str = str.substr(0, str.length() * 0.8);

	//��ȥ������ַ�
	for (char i = '0'; i <= '9'; i++)
		sub(str, i);
	sub(str, '-');
	sub(str, '[');
	sub(str, ']');

	//����
	ofstream textOut(fileOut, ios::out);
	if (textOut)
		textOut << str;
	else
		cout << "Error:cant open preText file" << endl;
	textOut.close();
}

//��Ԥ������ı��д���ʵ�
void learn(const string fileIn)
{
	wifstream learnIn(fileIn, ios::in);
	learnIn.imbue(china);//use locale object  
	if (learnIn)
	{
		wstring tmpStr;
		while (learnIn >> tmpStr)
		{
			dictionary.insert(tmpStr);
		}
		cout << "������Ŀ��" << dictionary.size() << endl;
	}
	else
		cout << "ERROR: Cannot open file 'learnIn.txt'." << endl;
	learnIn.close();
}

//����ʵ�
void dictionaryOut(const string fileOut)
{
	wofstream dictionaryOut(fileOut, ios::out);
	dictionaryOut.imbue(china);
	if (dictionaryOut)
	{
		for (set<wstring>::iterator it = dictionary.begin(); it != dictionary.end(); it++)
			dictionaryOut << *it << endl;
	}
	else
		cout << "ERROR: Cannot open file 'dictionaryOut.txt'." << endl;
	dictionaryOut.close();
}

//�Բ����ı�����Ԥ����
void preSeg(const string fileIn, const string fileOut)
{
	string str, tmpStr;
	ifstream textIn(fileIn, ios::in);
	if (textIn)
	{
		while (textIn >> tmpStr)
			str.append(tmpStr);
	}
	else
		cout << "Error:cant open preText file" << endl;
	textIn.close();

	//ȡ��80%
	str = str.substr(str.length() * 0.8, str.length());

	//ȥ��������ַ�
	for (char i = '0'; i <= '9'; i++)
		sub(str, i);
	for (char i = 'a'; i <= 'z'; i++)
		sub(str, i);
	sub(str, '-');
	sub(str, '[');
	sub(str, ']');
	sub(str, '{');
	sub(str, '}');

	//�����
	
	ofstream answerOut("answer.txt", ios::out);
	if (answerOut)
	{
		string answer = str;
		sub(answer, "//", "/");
		sub(answer, "/", "/ ");
		answerOut << answer;
	}
	else
		cout << "Error:cant open preSeg file" << endl;
	answerOut.close();
	

	//��������ı�
	sub(str, '/');
	ofstream textOut(fileOut, ios::out);
	if (textOut)
		textOut << str;
	else
		cout << "Error:cant open preSeg file" << endl;
	textOut.close();
}

//�ִ�
void segment(const string fileIn, const string fileOut)
{
	wstring text;
	wifstream textIn(fileIn, ios::in);
	textIn.imbue(china);
	if (textIn)
		textIn >> text;
	else
		cout << "ERROR: Cannot open file test.txt" << endl;
	textIn.close();

	wofstream textOut("result.txt", ios::out);
	textOut.imbue(china);
	if (textOut)
	{
		const int textLength = text.length();
		int begin = 0;
		while (begin < textLength)
		{
			int maxTest = maxLength;//������
			//��ʣ�೤��С��Ĭ��������ʱ��ȡʣ�೤��Ϊ������
			if (begin + maxLength >= textLength)
				maxTest = textLength - begin;
			int i;
			//�������ȵݼ���˳��ƥ�䵱ǰ�ַ����ʹʵ�
			for (i = maxTest; i > 1; i--)
			{
				//��������ֵ����ҵ���ǰ�Ӵ�����ִʳɹ����ӡ�/����begin�ƶ�
				const wstring tmpStr = text.substr(begin, i);
				if (dictionary.find(tmpStr) != dictionary.end())
				{
					textOut << tmpStr << "/ ";
					begin += i;
					break;
				}
			}
			//���򣬶Ե����ֽ��л���
			if (i == 1)
			{
				textOut << text.substr(begin, 1) << "/ ";
				begin++;
			}
			
		}
	}
	else
		cout << "ERROR: Cannot open file result.txt" << endl;
	textOut.close();
}

class Split
{
public:
	int left;
	int right;
};

//���/�ڴ�֮���λ��
void calSplit(const string file, vector<Split> &vec)
{
	ifstream fileIn(file, ios::in);
	if (fileIn)
	{
		string tmpStr;
		int last = 0;
		while (fileIn >> tmpStr)
		{
			int splitPos = tmpStr.find('/');
			Split tmpSplit;
			tmpSplit.left = last;
			tmpSplit.right = splitPos + last;
			vec.push_back(tmpSplit);
			last += splitPos;
		}
	}
	else
		cout << "Error:cant open answerIn" << endl;
	fileIn.close();
}

//�������
void analyse(const string resultFile, const string answerFile)
{
	vector<Split> answer;
	calSplit(answerFile, answer);
	vector<Split> result;
	calSplit(resultFile, result);
	
	int correct = 0;
	vector<Split>::iterator answerBegin = answer.begin();
	
	for (vector<Split>::iterator it = result.begin(); it != result.end(); it++)
	{
		for (vector<Split>::iterator j = answerBegin; j != answer.end(); j++)
		{
			if (j->left == it->left && j->right == it->right)
			{
				correct++;
				answerBegin = j;
				break;
			}
			if (j->left > it->right)
				break;
		}
	}
	int segNum = result.size(), num = answer.size();
	float precision = (float)correct / (float)segNum;
	float recall = (float)correct / (float)num;
	float measure = 2 * precision * recall / (precision + recall);
	cout <<endl << "���������" << endl;
	cout << "corret = " << correct << endl;
	cout << "segNum = " << segNum << endl;
	cout << "num = " << num << endl;
	cout << "precision = " << precision << endl;
	cout << "recall = " << recall << endl;
	cout << "F = " << measure << endl << endl;

	ofstream analyseOut("analyse.txt", ios::out);
	if (analyseOut)
	{
		analyseOut << "���������" << endl;
		analyseOut << "corret = " << correct << endl;
		analyseOut << "segNum = " << segNum << endl;
		analyseOut << "num = " << num << endl;
		analyseOut << "precision = " << precision << endl;
		analyseOut << "recall = " << recall << endl;
		analyseOut << "F = " << measure << endl << endl;
	}
	else
		cout << "cant open analyseOut" << endl;
	analyseOut.close();
}


int main()
{	
	time_t time1,time2,time3,time4;
	time1 = time(NULL);

	//ѧϰ
	preLearn("1998-01-105-����.txt", "preLearn.txt");
	learn("preLearn.txt");
	dictionaryOut("dictionary.txt");
	time2 = time(NULL);
	cout << "ѧϰ��ʱ��" << difftime(time2, time1) << endl;

	//�ִ�
	preSeg("1998-01-105-����.txt", "preSeg.txt");
	segment("preSeg.txt", "result.txt");	
	time3 = time(NULL);
	cout << "�ִ���ʱ��" << difftime(time3, time2) << endl;

	//�������
	analyse("result.txt", "answer.txt");
	time4 = time(NULL);
	cout << "������ʱ��" << difftime(time4, time3) << endl;

	cout << "�ܼ���ʱ��" << difftime(time4, time1) << endl;

	system("pause");
	return 0;
}
