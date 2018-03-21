#include <iostream>
#include <fstream>
#include <string>
using namespace std;

int main()
{
	ifstream in("hashtable.txt",ios::in);
	ofstream out("keywords.txt");
	string input="";
	while(getline(in,input))
	{
		if(input=="#")
		{
			getline(in,input);
			out<<input<<endl;
		}
	}
	in.close();
}