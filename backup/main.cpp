#include <iostream>
#include <time.h>
#include "NLPIR.h"
#include "header.h"
#include "Self_Define_Functions.h"
using namespace std;

Searcher searcher;
int sizeofpage;

void load()
{
	long beg, end;
	beg = clock();
	readwebpage();
	savefile();
	//readfile();
	end = clock();
	cout << end - beg << endl;
}

void searchcore()
{
	//scanf("%d", &sizeofpage);
	sizeofpage = 128444;
	initNLPIR();
	readfile();
	readoutpage(sizeofpage);
	//mainsearch(string);
}