#pragma once
#pragma comment(lib, "NLPIR.lib")
#include <vector>
#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <cmath>
#include <sstream>
#include <map>
#include "NLPIR.h"
using namespace std;
typedef pair<int, int> listpair;

const int keywordsize = 5;
const char *last = ".txt";
const int hashsize = 512;
string beg = "./Spidertext/";
string outbeg = "./output/";

class Webpage
{
private:
	string URL;
	string origin_maintitle;
	int code;
	string outline;
	string filepath;
	int subsize;

public:
	vector<string> subtitle;
	vector<string> keywords;
	vector<string> maintitle;

	Webpage(string url = "")
	{
		URL = url;
		code = 0;
		outline = "";
		filepath = "";
	}

	void setfilepath()
	{
		char tmp[10];
		itoa(code, tmp, 10);
		strcat(tmp, last);
		string ltmp = tmp;
		filepath = outbeg + ltmp;
	}

	void print(ofstream &out)
	{
		//out.open(filepath);
		out.open("webpage.txt", ios::app);
		out << code << " " << URL << endl;
		out << origin_maintitle << endl;
		out << outline << endl;
		out << subsize << endl;
	}

	string getURL()
	{
		return URL;
	}

	vector<string> getmaintitle()
	{
		return maintitle;
	}

	void readURL(fstream &file)
	{
		getline(file, URL);
	}

	void readmaintitle(fstream &file)
	{
		string tmt = "";
		getline(file, tmt);
		if (tmt[0] != '#')
			return;
		else
		{
			tmt.erase(tmt.begin());
			origin_maintitle = tmt;
			string result = NLPIR_GetKeyWords(tmt.c_str(), 3);
			string tmp;
			tmp.clear();
			for (int i = 0; i < result.length(); i++)
			{
				if (result[i] == '#')
				{
					maintitle.push_back(tmp);
					tmp.clear();
				}
				else tmp += result[i];
			}
		}
	}

	void setcode(int newcode)
	{
		code = newcode;
	}

	int getcode()
	{
		return code;
	}

	void readcontent(fstream &file, string filename)
	{
		string tmp = "", sub = "", maincontent = "";
		int count = 0;
		while (getline(file, tmp))
		{
			if (tmp[0] == '*')
			{
				tmp.erase(tmp.begin());
				sub += tmp;
				count++;
			}
			else
			{
				maincontent = tmp;
				break;
			}
		}
		subsize = count;
		while (getline(file, tmp))
			maincontent += tmp;
		outline = maincontent.substr(0, 100);
		string result = NLPIR_GetKeyWords(sub.c_str(), 5, false);
		tmp.clear();
		for (int i = 0; i < result.length(); i++)
		{
			if (result[i] == '#')
			{
				subtitle.push_back(tmp);
				tmp.clear();
			}
			else tmp += result[i];
		}
		result = NLPIR_GetKeyWords(maincontent.c_str(), 10, false);
		tmp.clear();
		for (int i = 0; i < result.length(); i++)
		{
			if (result[i] == '#')
			{
				keywords.push_back(tmp);
				tmp.clear();
			}
			else tmp += result[i];
		}
	}
};

class word_object
{
private:
	int wordID;
	string word;
public:
	vector<pair<int, int>> list;  // first is page ID, second is frequence;

	word_object(int wID = 0, string wd = "")
	{
		wordID = wID;
		word = wd;
	}

	void setID(int newID)
	{
		wordID = newID;
	}

	int getID()
	{
		return wordID;
	}

	void setword(string newword)
	{
		word = newword;
	}

	string getword()
	{
		return word;
	}

	void insertlist(int pageID, int frq)
	{
		listpair tmp(pageID, frq);
		list.push_back(tmp);
	}

	vector<listpair> getlist()
	{
		return list;
	}

	void print()
	{
		cout << "WordID = " << wordID << endl;
		cout << "Word = " << word << endl;
		vector<listpair>::iterator it;
		for (it = list.begin(); it != list.end(); it++)
			cout << it->first << "\t" << it->second << endl;
	}

};

class Hashwords
{
private:
	int maxsize;

public:
	vector<word_object> *hashlist;

	Hashwords(int ms)
	{
		maxsize = ms;
		hashlist = new vector<word_object>[maxsize];
	}

	~Hashwords()
	{
		delete[] hashlist;
	}

	int getmaxsize()
	{
		return maxsize;
	}

	void insert(Webpage *webpage)
	{
		int pID = webpage->getcode();
		for (int i = 0; i < webpage->maintitle.size(); i++)
		{
			string nowmaintitle = webpage->maintitle[i];
			insertword(nowmaintitle, pID, 1);
		}
		vector<string>::iterator subit;
		for (subit = webpage->subtitle.begin();
			subit != webpage->subtitle.end(); subit++)
			insertword(*subit, pID, 2);
		int keyrank;
		vector<string>::iterator keyit;
		for (keyit = webpage->keywords.begin(), keyrank = 3;
			keyit != webpage->keywords.end(); keyit++, keyrank++)
			insertword(*keyit, pID, keyrank);
	}

	void insertword(string nowword, int pageID, int frq)
	{
		int ASCcount = 0, index = 0;
		bool flag = false;
		if (nowword.length() <= 1)
			ASCcount = nowword[0];
		else if (nowword.length() <= 2)
			ASCcount = nowword[0] + nowword[1];
		else if (nowword.length() <= 3)
			ASCcount = nowword[0] + nowword[1] + nowword[2];
		else ASCcount = nowword[0] + nowword[1] + nowword[2] + nowword[3];
		if (ASCcount < 0)
			ASCcount = abs(ASCcount) + 256;
		index = ASCcount%maxsize;
		vector<word_object>::iterator searchit;
		for (searchit = hashlist[index].begin();
			searchit != hashlist[index].end(); searchit++)
		{
			if (searchit->getword() == nowword)
			{
				flag = true;
				break;
			}
		}
		if (!flag)
		{
			word_object tmp(index, nowword);
			tmp.insertlist(pageID, frq);
			hashlist[index].push_back(tmp);
		}
		else searchit->insertlist(pageID, frq);
	}
};

class Outpage
{
private:
	int pageID;
	int subsize;
	string url;
	string title;
	string outline;
public:
	Outpage() = default;

	Outpage(int p, string u, string o, string t)
	{
		pageID = p;
		url = u;
		outline = o;
		title = t;
		subsize = 0;
	}

	int getsubsize()
	{
		return subsize;
	}

	void settitle(string newtitle)
	{
		title = newtitle;
	}

	string gettitle()
	{
		return title;
	}

	void setpageID(int newpageID)
	{
		pageID = newpageID;
	}

	int getpageID()
	{
		return pageID;
	}

	void seturl(string newurl)
	{
		url = newurl;
	}

	string geturl()
	{
		return url;
	}

	void setoutline(string newoutline)
	{
		outline = newoutline;
	}

	string getoutline()
	{
		return outline;
	}

	void read(ifstream &in)
	{
		stringstream ss, ss1;
		string input;
		getline(in, input);
		ss << input;
		ss >> pageID >> url;
		getline(in, title);
		getline(in, outline);
		input.clear();
		getline(in, input);
		ss1 << input;
		ss1 >> subsize;
	}
};

struct point
{
	double score[5];	// 5 is the keyword size;
	point()
	{
		memset(score, 0.0, sizeof(score));
	}
};

class Searcher
{
public:
	map<int, double> result;
	vector<string> input_keywords;
	vector<double> query;
	map<int, point> doc;
	vector<pair<int, double>> iv;
	const int maxsize = 512;
	int searchkeysize;
	int frq;
	int querycode;
	Searcher() = default;

	void getquerycode()
	{
		int sum = 0;
		for (int i = 0; i < input_keywords.size(); i++)
			for (int j = 0; j < input_keywords[i].length(); j++)
				sum += input_keywords[i][j];
		querycode = abs(sum);
	}

	void reset()
	{
		result.clear();
		input_keywords.clear();
		query.clear();
		doc.clear();
		iv.clear();
	}

	void getinput(string input)
	{
		string tmp = NLPIR_GetKeyWords(input.c_str(), 5, false);	// 5 is the key word size;
		string word;
		word.clear();
		int index = 1;
		for (int i = 0; i < tmp.size(); i++)
		{
			if (tmp[i] == '#')
			{
				input_keywords.push_back(word);
				word.clear();
				double weight = log((1.0 / index)*20.0)*10.0;
				query.push_back(weight);
				index++;
			}
			else word += tmp[i];
		}
		searchkeysize = input_keywords.size();
		getquerycode();
	}

	int getindex(string nowword)
	{
		int ASCcount;
		if (nowword.length() <= 1)
			ASCcount = nowword[0];
		else if (nowword.length() <= 2)
			ASCcount = nowword[0] + nowword[1];
		else if (nowword.length() <= 3)
			ASCcount = nowword[0] + nowword[1] + nowword[2];
		else ASCcount = nowword[0] + nowword[1] + nowword[2] + nowword[3];
		if (ASCcount < 0)
			ASCcount = abs(ASCcount) + 256;
		return ASCcount%maxsize;
	}

	bool isright(vector<string>::iterator beg, vector<string>::iterator end)
	{
		vector<string>::iterator tbeg;
		for (tbeg = input_keywords.begin(); tbeg != input_keywords.end() && beg != end; tbeg++, beg++)
			if (*tbeg != *beg)
				return false;
		return true;
	}

	void search();

	void showresult();
};

class Heap
{
public:
	Searcher *heap[101];
	const int maxheapsize = 100;
	int nowsize;

	void siftdown(int pos)
	{
		while (!isLeaf(pos))
		{
			int cindex = leftchild(pos);
			int right = rightchild(pos);
			if (right<nowsize&&heap[right]->frq>heap[cindex]->frq)
				cindex = right;
			if (heap[pos]->frq > heap[cindex]->frq)
				return;
			swap(heap[pos], heap[cindex]);
			pos = cindex;
		}
	}

	Heap()
	{
		nowsize = 0;
	}

	int size() const
	{
		return nowsize;
	}

	bool isLeaf(int pos) const
	{
		return (pos >= nowsize / 2) && (pos < nowsize);
	}

	int leftchild(int pos) const
	{
		return	2 * pos + 1;
	}

	int rightchild(int pos) const
	{
		return 2 * pos + 2;
	}

	int parent(int pos) const
	{
		return (pos - 1) / 2;
	}

	void buildheap()
	{
		for (int i = nowsize / 2 - 1; i >= 0; i--)
			siftdown(i);
	}

	void insert(Searcher *it)
	{
		if (nowsize == maxheapsize)
		{
			Searcher *tmp = heap[maxheapsize - 1];
			heap[maxheapsize - 1] = it;
			delete tmp;
			return;
		}
		int curr = nowsize++;
		heap[curr] = it;
		while (curr != 0 && heap[curr]->frq > heap[parent(curr)]->frq)
		{
			swap(heap[curr], heap[parent(curr)]);
			curr = parent(curr);
		}
	}

	void siftup(int pos)
	{
		while (pos != 0 && heap[pos]->frq > heap[parent(pos)]->frq)
		{
			swap(heap[pos], heap[parent(pos)]);
			pos = parent(pos);
		}
	}

	Searcher* search(int qc, vector<string>::iterator beg, vector<string>::iterator end, int *fixindex)
	{
		Searcher *result = NULL;
		for (int i = 0; i < nowsize; i++)
		{
			if (qc == heap[i]->querycode)
				if (heap[i]->isright(beg, end))
				{
					*fixindex = i;
					return heap[i];
				}
		}
		return NULL;
	}
};