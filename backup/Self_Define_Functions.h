#pragma once
#pragma comment(lib, "NLPIR.lib")
#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <cmath>
#include <sstream>
#include <algorithm>
#include <ctime>
#include "NLPIR.h"
#include "header.h"
using namespace std;

Hashwords *hashtable;
vector<Outpage> pagelist;
Heap record;

void initNLPIR()
{
	if (!NLPIR_Init())
	{
		printf("NLPIR initialize failed!\n");
		return;
	}
}

void readwebpage()
{
	ofstream outpage;
	fstream file;
	int n = 128444;
	//scanf("%d", &n);
	initNLPIR();
	hashtable = new Hashwords(hashsize);

	for (int i = 1; i <= n; i++)
	{
		char index[10] = "";
		itoa(i, index, 10);
		strcat(index, last);
		string filename = beg + index;
		file.open(filename);
		if (file.bad())
		{
			cout << "open file " << filename << " failed" << endl;
			continue;
		}
		Webpage webpage;
		webpage.setcode(i);
		webpage.readURL(file);
		webpage.readmaintitle(file);
		webpage.readcontent(file, filename);
		//webpage.setfilepath();
		webpage.print(outpage);
		hashtable->insert(&webpage);

		file.close();
		outpage.close();

		file.clear();
		outpage.clear();
	}
}

void savefile()
{
	ofstream resultout("hashtable.txt");
	vector<word_object>::iterator it;
	vector<listpair>::iterator listit;

	for (int i = 0; i < hashsize; i++)
	{
		resultout << "*" << endl;
		resultout << i << endl;
		for (it = hashtable->hashlist[i].begin(); it != hashtable->hashlist[i].end(); it++)
		{
			resultout << "#" << endl;
			resultout << it->getword() << endl;
			for (listit = it->list.begin(); listit != it->list.end(); listit++)
				resultout << listit->first << " " << listit->second << endl;
			resultout << "##" << endl;
		}
		resultout << "**" << endl;
	}

	resultout.close();
}

void readwordobject(ifstream &infile, int index)
{
	string input;
	int pageID, frq;
	getline(infile, input);
	word_object tmp(index, input);
	while (getline(infile, input))
	{
		if (input == "##")
		{
			hashtable->hashlist[index].push_back(tmp);
			return;
		}
		stringstream ss;
		ss << input;
		ss >> pageID >> frq;
		listpair tmppair(pageID, frq);
		tmp.list.push_back(tmppair);
	}
}

void readword(ifstream &infile)
{
	string input;
	int index;
	getline(infile, input);
	stringstream ss;
	ss << input;
	ss >> index;
	while (getline(infile, input))
	{
		if (input == "#")
			readwordobject(infile, index);
		if (input == "**")
			return;
	}
}

void readfile()
{
	ifstream infile("hashtable.txt");

	hashtable = new Hashwords(hashsize);
	string input = "";
	while (getline(infile, input))
	{
		if (input == "*")
			readword(infile);
	}

	infile.close();
}

void readoutpage(int sizeofpage)
{
	ifstream in("webpage.txt");
	Outpage nu;
	nu.setpageID(0);
	pagelist.push_back(nu);
	for (int i = 1; i <= sizeofpage; i++)
	{
		Outpage outpage;
		outpage.read(in);
		pagelist.push_back(outpage);
	}
	in.close();
}

void Searcher::search()
{
	doc.clear();
	iv.clear();
	int index;
	double weight;
	vector<word_object>::iterator wordit;
	vector<listpair>::iterator listit;
	map<int, point>::iterator mapit;
	for (int i = 0; i < searchkeysize; i++)
	{
		string nowword = input_keywords[i];
		index = getindex(nowword);
		for (wordit = hashtable->hashlist[index].begin(); wordit != hashtable->hashlist[index].end(); wordit++)
			if (wordit->getword() == nowword)
				for (listit = wordit->list.begin(); listit != wordit->list.end(); listit++)
				{
					if (listit->second == 1)
						weight = 305.5;
					else weight = (log((1.0 / listit->second)*20.0))*10.0;
					mapit = doc.find(listit->first);
					if (mapit == doc.end())
					{
						point tmp;
						tmp.score[i] = weight;
						doc[listit->first] = tmp;
					}
					else mapit->second.score[i] += weight;
				}
	}
	bool inflag = false;
	if (searchkeysize > 1)
	{
		inflag = true;
		for (mapit = doc.begin(); mapit != doc.end(); mapit++)
		{
			double score = 0.0, numerator = 0.0, denominator1 = 0.0, denominator2 = 0.0, denominator = 0.0, tmpscore;
			for (int i = 0; i < searchkeysize; i++)
			{
				tmpscore = mapit->second.score[i];
				numerator += tmpscore * query[i];
				denominator1 += tmpscore*tmpscore;
				denominator2 += query[i] * query[i];
			}
			denominator = sqrt(denominator1*denominator2);
			score = numerator / denominator;
			result[mapit->first] = score;
		}
	}
	else
	{
		for (mapit = doc.begin(); mapit != doc.end(); mapit++)
			result[mapit->first] = mapit->second.score[0];
	}
	map<int, double>::iterator resit;
	if (!inflag)
	{
		for (resit = result.begin(); resit != result.end(); resit++)
			resit->second += ((double)pagelist[resit->first].getsubsize() * log(resit->second)) / 10.0;
	}
	else
	{
		for (resit = result.begin(); resit != result.end(); resit++)
		{
			if(resit->second > 0.95)
				resit->second += (double)pagelist[resit->first].getsubsize() * 1.5;
			else if((resit->second>0.8)&&(resit->second <=0.95))
				resit->second += (double)pagelist[resit->first].getsubsize() * 0.5;
			else resit->second += (double)pagelist[resit->first].getsubsize() * 0.1;
		}
	}
	for (auto it = result.begin(); it != result.end(); it++)
		iv.push_back(make_pair(it->first, it->second));
	sort(iv.begin(), iv.end(), [](const pair<int, double> &x, const pair<int, double> &y) -> int {
		return x.second > y.second;
	});
	auto it = iv.begin();
	while (it != iv.end())
	{
		double tmp = it->second;
		for (it++; it != iv.end();)
		{
			if (it->second == tmp)
				it = iv.erase(it);
			else break;
		}
	}
}

void Searcher::showresult()
{
	vector<pair<int, double>>::iterator it;
	string tmp = "", next = "";
	for (it = iv.begin(); it != iv.end(); it++)
	{
		next = pagelist[it->first].geturl();
		if (tmp == "" || tmp != next)
		{
			cout << pagelist[it->first].gettitle() << endl;
			cout << pagelist[it->first].getoutline() << endl;
			cout << pagelist[it->first].geturl() << endl;
		}
		tmp = next;
	}
}

Searcher* mainsearch(string input)
{
	//long beg, end;
	int *fixindex = new int;
	*fixindex = 0;
	//beg = clock();
	Searcher *searcher = new Searcher();
	searcher->reset();
	searcher->getinput(input);
	Searcher *tsearcher = record.search(searcher->querycode, searcher->input_keywords.begin(), 
		searcher->input_keywords.end(), fixindex);
	if (tsearcher != NULL)
	{
		tsearcher->frq++;
		record.siftup(*fixindex);
		//end = clock();
		//cout << end - beg << " ms" << endl;
		delete searcher;
		return tsearcher;
	}
	else
	{
		searcher->search();
		delete tsearcher;
	}
	record.insert(searcher);
	//end = clock();
	//cout << end - beg << " ms" << endl;
	delete fixindex;
	return searcher;
}