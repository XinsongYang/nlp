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

map<string, map<string, double> > a;	//词性到词性的转移概率
map<string, map<string, double> > b;	//每个词各个词性的概率

//将文本分成前80%和后20%
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

//文本预处理
void preTrain(string fileIn, string fileOut)
{
	ifstream in(fileIn, ios::in);
	ofstream out(fileOut, ios::out);

	char ch;
	bool flag = 0;

	while (in.get(ch))
	{
		if (ch == ' ' && flag == 0)			//转为单空格 
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
		else if (ch >= '0' && ch <= '9')	//去掉段首数字串 
		{
			while (ch != ' ')
				in.get(ch);
			in.get(ch);
			flag = 0;
		}
		else if (ch == '[')					//合并合成词 
		{
			in.get(ch);
			while (ch != ']')
			{
				if (ch == '{')					//去掉注音 
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
		else if (ch == '{')					//去掉注音 
		{
			while (ch != '}')
				in.get(ch);
			flag = 0;
		}
		else								//其他情况直接输出 
		{
			out << ch;
			flag = 0;
		}
	}
};

//测试文本预处理
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


//获得A
void getA(string fileIn)
{
	ifstream in(fileIn);
	char ch;						//逐字符读取 
	string str1 = "", str2 = "";	//前后词词性 
	bool flag = 1;					//句首判断 
	map<string, double> tmp;				//句首map 
	a.insert(pair<string, map<string, double> >("begin", tmp)); //a中已有一个元素:句首begin 
	int i = 0;
	while (1)
	{
		if (in.get(ch))
		{
			//	cout << "p";
			str1 = str2;		//将上一个词的词性转存入str1 
			str2 = "";
			while (ch != '/') 	//取词,词读完之后ch='/',后面的东西为词性 
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
				while (ch != ' ' && ch != '/n')	//取新的词性存入str2 
				{
					str2 += ch;
					if (in.get(ch))
					{
						//				cout << "t";
					}
					else
						break;
				}

				if (!(str2 == "wj" || str2 == "ww" || str2 == "wt") && a.find(str2) == a.end())	//若当前词不为句末点号,不管为何种情况,都先将该词存入矩阵a中 
				{
					map<string, double> tmpMap;
					a[str2] = tmpMap;
					//			cout << "*";
				}

				if (str2 == "wj" || str2 == "ww" || str2 == "wt")	//该词性为句末点号,则说明非句首,str1存在,不为句末点号且已被存入a矩阵中 
				{
					if (a[str1].find(str2) != a[str1].end())
						a[str1][str2]++;

					else
						a[str1][str2] = 1;

					flag = 1;	//此时下一个词为句首 

					//			cout << "+";
				}

				else if (flag == 1)	//句首,则先判断该词性是否已在a[begin]的map中,若存在则直接+1,否则插入句首map并置1; 
				{
					if (a["begin"].find(str2) != a["begin"].end())
						a["begin"][str2]++;

					else
						a["begin"][str2] = 1;

					flag = 0;	//下一个词不为句首 

					//			cout << "-";
				}

				else	//当前不为句首,则说明已经存入至少一个词性,str1已存在 
				{
					if (a[str1].find(str2) != a[str1].end())
						a[str1][str2]++;

					else
						a[str1][str2] = 1;

					flag = 0;	//下一个词不为句首 

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
	//输出a矩阵 
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


//输出B
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


//获得B
void getB(const string fileName)
{
	ifstream in(fileName, ios::in);
	map < string, double > total;
	if (in)
	{
		//按词读
		string tmpStr;
		while (in >> tmpStr)
		{
			//分出词和词性
			int splitPos = tmpStr.find('/');
			string word = tmpStr.substr(0, splitPos);
			string speech = tmpStr.substr(splitPos + 1, tmpStr.length() - splitPos - 1);
			//记录词性频次
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
	
	//计算各个词性的概率
	for (map<string, map<string, double> >::iterator it = b.begin(); it != b.end(); it++)
	{
		for (map<string, double>::iterator j = it->second.begin(); j != it->second.end(); j++)
			j->second = j->second / total[it->first];
	}
	
	outB();
};


//v的节点，储存当前词性的概率和上一个词性
class Vnode
{
public:
	double probability;
	string last;
};


//输入一个不带词性句子，计算v，返回带词性标注的句子
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
		//如果当前词在b中存在
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
		//若B中不存在
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
	//选取最后概率最大的V节点
	map<string, Vnode>::iterator maxV = v[word.size()].begin();
	for (map<string, Vnode>::iterator it = v[word.size()].begin(); it != v[word.size()].end(); it++)
	{
		if (it->second.probability > maxV->second.probability)
			maxV = it;
	}
	//记录词性序列
	vector<string> speech(word.size());
	string tmpSpeech = maxV->first;
	for (int i = word.size(); i > 0; i--)
	{
		speech[i - 1] = tmpSpeech;
		tmpSpeech = v[i][tmpSpeech].last;
	}
	//输出
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


//记录一个字符串中某子串的位置
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


//对文本进行词性标注
void speech(const string fileIn, const string fileOut)
{
	ifstream in(fileIn, ios::in);
	ofstream out(fileOut, ios::out);

	if (in && out)
	{
		//按行读
		string tmpLine;
		while (getline(in, tmpLine))
		{
			//按标点分割句子
			string sentence;
			set<int> segPos;
			seg(segPos, tmpLine, "。");
			seg(segPos, tmpLine, "！");
			seg(segPos, tmpLine, "？");
			seg(segPos, tmpLine, "……");
			int last = 0;

			//以句子为单位标记词性并返回
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


//结果分析
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
		std::cout << "标记总数：" << total << endl << "正确个数：" << correct << endl << "正确率：" << rate << endl;
		analyse << "标记总数：" << total << endl << "正确个数：" << correct << endl << "正确率：" << rate;
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

	segText("1998-01-105-带音.txt", "preTrain.txt", "preTest.txt");
	preTrain("preTrain.txt", "train.txt");
	preSpeech("preTest.txt", "test.txt");

	time2 = time(NULL);
	cout << "预处理用时：" << difftime(time2, time1) << endl;

	getA("train.txt");

	time3 = time(NULL);
	cout << "A用时：" << difftime(time3, time2) << endl;
	
	getB("train.txt");

	time4 = time(NULL);
	cout << "B用时：" << difftime(time4, time3) << endl;

	speech("test.txt", "result.txt");

	time4 = time(NULL);
	cout << "词性标注用时：" << difftime(time4, time3) << endl;

	analyse("result.txt", "answer.txt");

	system("pause");
	return 0;
}