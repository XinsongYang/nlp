#include <stdio.h>
#include <iostream>
#include<fstream> 
#include <string>
#include <regex>
#include <vector>
#include <map>
#include <set>
#include <algorithm>
#include <time.h>
using namespace std;

map<string, map<string, double> > a;	//���Ե����Ե�ת�Ƹ���
map<string, map<string, double> > b;	//ÿ���ʸ������Եĸ���

//���ı��ֳ�ǰ80%�ͺ�20%
void segText(string fileIn, string fileTrain, string fileTest)
{
	ifstream in(fileIn, ios::in);
	ofstream train(fileTrain, ios::out);
	ofstream test(fileTest, ios::out);

	if (in && train && test)
	{
		vector<string> lines;
		string tmpLine;
		while (getline(in, tmpLine))
			lines.push_back(tmpLine);
		int segPos = lines.size() * 0.8;
		for (int i = 0; i < segPos; i++)
			train << lines[i] << endl;
		for (int i = segPos; i < lines.size(); i++)
			test << lines[i] << endl;
	}
	else
		cout << "ERROR: Cannot open file." << endl;

	in.close();
	train.close();
	test.close();
};

//�ı�Ԥ����
void preTrain(string fileIn, string fileOut)
{
	ifstream in(fileIn, ios::in);
	ofstream out(fileOut, ios::out);

	char ch;
	bool flag = 0;

	while (in.get(ch))
	{
		if (ch == ' ' && flag == 0)			//תΪ���ո� 
		{
			out << ' ';
			in.get(ch);
			flag = 0;
		}
		else if (ch == '/')
		{
			in.get(ch);
			out << '/';
			while (ch != ' ')
			{
				out << ch;
				in.get(ch);
			}
			out << ' ';
			in.get(ch);
			flag = 0;
		}
		else if (ch >= '0' && ch <= '9')	//ȥ���������ִ� 
		{
			while (ch != ' ')
				in.get(ch);
			in.get(ch);
			flag = 0;
		}
		else if (ch == '[')					//�ϲ��ϳɴ� 
		{
			in.get(ch);
			while (ch != ']')
			{
				if (ch == '{')					//ȥ��ע�� 
				{
					while (ch != '}')
						in.get(ch);
					flag = 0;
				}
				else if (!(ch == ' ' || ch == '/' || (ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z')))
					out << ch;
				in.get(ch);
			}
			out << "/";
			in.get(ch);
			out << ch;
			while (ch != ' ')
				in.get(ch);
			flag = 1;
		}
		else if (ch == '{')					//ȥ��ע�� 
		{
			while (ch != '}')
				in.get(ch);
			flag = 0;
		}
		else								//�������ֱ����� 
		{
			out << ch;
			flag = 0;
		}
	}
};

//�����ı�Ԥ����
void preSpeech(string fileIn, string fileOut)
{
	preTrain(fileIn, "answer.txt");
	ifstream in("answer.txt");
	ofstream out(fileOut);
	char ch;
	while (in.get(ch))
	{
		if (ch == '/')
		{
			out << "/  ";
			while (ch != ' ')
			{
				if (!in.get(ch))
					continue;
			}
		}
		else
			out << ch;
	}
}


//���A
void getA(string fileIn)
{
	ifstream in(fileIn);
	char ch;						//���ַ���ȡ 
	string str1 = "", str2 = "";	//ǰ��ʴ��� 
	bool flag = 1;					//�����ж� 
	map<string, double> tmp;				//����map 
	a.insert(pair<string, map<string, double> >("begin", tmp)); //a������һ��Ԫ��:����begin 
	int i = 0;
	while (1)
	{
		if (in.get(ch))
		{
			//	cout << "p";
			str1 = str2;		//����һ���ʵĴ���ת����str1 
			str2 = "";
			while (ch != '/') 	//ȡ��,�ʶ���֮��ch='/',����Ķ���Ϊ���� 
			{
				if (in.get(ch))
				{
					//			cout << "x";
				}
				else
					break;
			}

			if (in.get(ch))
			{
				while (ch != ' ' && ch != '/n')	//ȡ�µĴ��Դ���str2 
				{
					str2 += ch;
					if (in.get(ch))
					{
						//				cout << "t";
					}
					else
						break;
				}

				if (!(str2 == "wj" || str2 == "ww" || str2 == "wt") && a.find(str2) == a.end())	//����ǰ�ʲ�Ϊ��ĩ���,����Ϊ�������,���Ƚ��ôʴ������a�� 
				{
					map<string, double> tmpMap;
					a[str2] = tmpMap;
					//			cout << "*";
				}

				if (str2 == "wj" || str2 == "ww" || str2 == "wt")	//�ô���Ϊ��ĩ���,��˵���Ǿ���,str1����,��Ϊ��ĩ������ѱ�����a������ 
				{
					if (a[str1].find(str2) != a[str1].end())
						a[str1][str2]++;

					else
						a[str1][str2] = 1;

					flag = 1;	//��ʱ��һ����Ϊ���� 

					//			cout << "+";
				}

				else if (flag == 1)	//����,�����жϸô����Ƿ�����a[begin]��map��,��������ֱ��+1,����������map����1; 
				{
					if (a["begin"].find(str2) != a["begin"].end())
						a["begin"][str2]++;

					else
						a["begin"][str2] = 1;

					flag = 0;	//��һ���ʲ�Ϊ���� 

					//			cout << "-";
				}

				else	//��ǰ��Ϊ����,��˵���Ѿ���������һ������,str1�Ѵ��� 
				{
					if (a[str1].find(str2) != a[str1].end())
						a[str1][str2]++;

					else
						a[str1][str2] = 1;

					flag = 0;	//��һ���ʲ�Ϊ���� 

					//			cout << "/";
				}
				//		cout << i++ << "\n";	
			}
			else
				break;
		}
		else
			break;
	}

	map<string, map<string, double> >::iterator itr1 = a.begin();
	map<string, double>::iterator itr2;
	while (itr1 != a.end())
	{
		int total = 0;
		itr2 = itr1->second.begin();
		while (itr2 != itr1->second.end())
		{
			total += itr2->second;
			itr2++;
		}
		itr2 = itr1->second.begin();
		while (itr2 != itr1->second.end())
		{
			itr2->second /= total;
			itr2++;
		}
		itr1++;
	}
	//���a���� 
	ofstream out("A.txt", ios::out);
	if (out)
	{
		map<string, map<string, double> >::iterator itr11 = a.begin();
		map<string, double>::iterator itr22 = itr11->second.begin();

		while (itr11 != a.end())
		{
			itr22 = itr11->second.begin();
			out << endl << "****************" << endl << itr11->first << ":" << endl;
			while (itr22 != itr11->second.end())
			{
				out << "         " << itr22->first << ":" << itr22->second << endl;
				itr22++;
			}
			itr11++;
		}
		out.close();
	}
};


//���B
void outB()
{
	ofstream out("B.txt", ios::out);
	if (out)
	{
		for (map<string, map<string, double> >::iterator it = b.begin(); it != b.end(); it++)
		{
			out << it->first << ":" << endl;
			int tmp = 0;
			for (map<string, double>::iterator j = it->second.begin(); j != it->second.end(); j++)
				out << j->first << ":" << j->second << endl;
			out << endl;
		}
	}
	out.close();
};


//���B
void getB(const string fileName)
{
	ifstream in(fileName, ios::in);
	map < string, double > total;
	if (in)
	{
		//���ʶ�
		string tmpStr;
		while (in >> tmpStr)
		{
			//�ֳ��ʺʹ���
			int splitPos = tmpStr.find('/');
			string word = tmpStr.substr(0, splitPos);
			string speech = tmpStr.substr(splitPos + 1, tmpStr.length() - splitPos - 1);
			//��¼����Ƶ��
			map<string, double> tmp;
			if (b.find(word) == b.end())
			{
				map<string, double> tmp;
				b.insert(pair<string, map<string, double> >(word, tmp));
				total.insert(pair<string, double>(word, 0));
			}
			total[word]++;
			if (b[word].find(speech) == b[word].end())
				b[word].insert(pair<string, double>(speech, 0));
			b[word][speech] ++;
		}
	}
	else
		cout << "ERROR: Cannot open file 'learnIn.txt'." << endl;
	in.close();
	
	//����������Եĸ���
	for (map<string, map<string, double> >::iterator it = b.begin(); it != b.end(); it++)
	{
		for (map<string, double>::iterator j = it->second.begin(); j != it->second.end(); j++)
			j->second = j->second / total[it->first];
	}
	
	outB();
};


//v�Ľڵ㣬���浱ǰ���Եĸ��ʺ���һ������
class Vnode
{
public:
	double probability;
	string last;
};


//����һ���������Ծ��ӣ�����v�����ش����Ա�ע�ľ���
string viterbi(string sentence)
{
	vector<string> word;
	int last = 0;
	int tmp = sentence.find("/");
	while (tmp != -1)
	{
		word.push_back(sentence.substr(last, tmp - last));
		last = tmp + 3;
		tmp = sentence.find("/", last);
	}
	//for (vector<string>::iterator it = word.begin(); it != word.end(); it++)
		//cout << *it << "*" << endl;

	vector<map<string, Vnode>> v(word.size() + 1);
	Vnode begin;
	begin.probability = 1;
	begin.last = "";
	v[0].insert(pair<string, Vnode>("begin", begin));

	for (int i = 1; i < word.size() + 1; i++)
	{
		//�����ǰ����b�д���
		if (b.find(word[i - 1]) != b.end())
		{
			for (map<string, double>::iterator it = b[word[i - 1]].begin(); it != b[word[i - 1]].end(); it++)
			{
				double max = 0;
				string lastT = "";
				for (map<string, Vnode>::iterator j = v[i - 1].begin(); j != v[i - 1].end(); j++)
				{
					double tmp = j->second.probability * a[j->first][it->first] * it->second;
					if (tmp > max)
					{
						max = tmp;
						lastT = j->first;
					}
				}
				Vnode tmpNode;
				tmpNode.probability = max;
				tmpNode.last = lastT;
				v[i].insert(pair<string, Vnode>(it->first, tmpNode));
			}
		}
		//��B�в�����
		else
		{
			double max = 0;
			string lastT = "";
			string t = "";
			for (map<string, Vnode>::iterator j = v[i - 1].begin(); j != v[i - 1].end(); j++)
				for (map<string, double>::iterator it = a[j->first].begin(); it != a[j->first].end(); it++)
				{
					double tmp = j->second.probability * it->second;
					if (tmp > max)
					{
						t = it->first;
						max = tmp;
						lastT = j->first;
					}
				}
				
			Vnode tmpNode;
			tmpNode.probability = max;
			tmpNode.last = lastT;
			v[i].insert(pair<string, Vnode>(t, tmpNode));
		}
		
	}
	//ѡȡ����������V�ڵ�
	map<string, Vnode>::iterator maxV = v[word.size()].begin();
	for (map<string, Vnode>::iterator it = v[word.size()].begin(); it != v[word.size()].end(); it++)
	{
		if (it->second.probability > maxV->second.probability)
			maxV = it;
	}
	//��¼��������
	vector<string> speech(word.size());
	string tmpSpeech = maxV->first;
	for (int i = word.size(); i > 0; i--)
	{
		speech[i - 1] = tmpSpeech;
		tmpSpeech = v[i][tmpSpeech].last;
	}
	//���
	string sSentence = "";
	for (int i = 0; i < word.size(); i++)
	{
		sSentence.append(word[i]);
		sSentence.append("/");
		sSentence.append(speech[i]);
		sSentence.append("  ");
	}

	return sSentence;
}


//��¼һ���ַ�����ĳ�Ӵ���λ��
void seg(set<int> &pos, string line, string sub)
{
	int begin = 0;
	begin = line.find(sub, begin);
	while (begin != -1)
	{
		pos.insert(begin + sub.length() + 3);
		begin = line.find(sub, begin + sub.length() + 3);
	}
}


//���ı����д��Ա�ע
void speech(const string fileIn, const string fileOut)
{
	ifstream in(fileIn, ios::in);
	ofstream out(fileOut, ios::out);

	if (in && out)
	{
		//���ж�
		string tmpLine;
		while (getline(in, tmpLine))
		{
			//�����ָ����
			string sentence;
			set<int> segPos;
			seg(segPos, tmpLine, "��");
			seg(segPos, tmpLine, "��");
			seg(segPos, tmpLine, "��");
			seg(segPos, tmpLine, "����");
			int last = 0;

			//�Ծ���Ϊ��λ��Ǵ��Բ�����
			for (set<int>::iterator it = segPos.begin(); it != segPos.end(); it++)
			{
				out << viterbi(tmpLine.substr(last, *it - last));
				last = *it;
			}
			if (tmpLine.length() - last != 0)
				out << viterbi(tmpLine.substr(last, tmpLine.length() - last));
			out << endl;
		}
	}
	else
		cout << "ERROR: Cannot open file." << endl;
	in.close();
	out.close();
};


//�������
void analyse(string rFile, string aFile)
{
	int total = 0, correct = 0;
	float rate = 0;
	ifstream result(rFile, ios::in);
	ifstream answer(aFile, ios::in);
	ofstream analyse("analyse.txt", ios::out);

	if (result && answer && analyse)
	{
		string rWord,aWord;
		while (result >> rWord && answer >> aWord)
		{
			total++;
			if (rWord == aWord)
				correct++;
		}
		rate = (float)correct / (float)total;
		std::cout << "���������" << total << endl << "��ȷ������" << correct << endl << "��ȷ�ʣ�" << rate << endl;
		analyse << "���������" << total << endl << "��ȷ������" << correct << endl << "��ȷ�ʣ�" << rate;
	}
	else
		std::cout << "ERROR: Cannot open file." << endl;

	result.close();
	answer.close();
	analyse.close();
}


int main()
{
	time_t time1, time2, time3, time4, time5;
	time1 = time(NULL);

	segText("1998-01-105-����.txt", "preTrain.txt", "preTest.txt");
	preTrain("preTrain.txt", "train.txt");
	preSpeech("preTest.txt", "test.txt");

	time2 = time(NULL);
	cout << "Ԥ������ʱ��" << difftime(time2, time1) << endl;

	getA("train.txt");

	time3 = time(NULL);
	cout << "A��ʱ��" << difftime(time3, time2) << endl;
	
	getB("train.txt");

	time4 = time(NULL);
	cout << "B��ʱ��" << difftime(time4, time3) << endl;

	speech("test.txt", "result.txt");

	time4 = time(NULL);
	cout << "���Ա�ע��ʱ��" << difftime(time4, time3) << endl;

	analyse("result.txt", "answer.txt");

	system("pause");
	return 0;
}