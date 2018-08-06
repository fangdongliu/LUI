#include<string>
#include<iostream>
using namespace std;

bool NextString(string&a, string&b, int& start) {

	int end;
	if ((start=a.find_first_not_of(' ', start))!=a.npos) {
		if ((end = a.find_first_of(' ', start)) != a.npos) {
			b = a.substr(start, end-start);
		}
		else {
			b = a.substr(start);
		}
		start = end;
		return true;
	}
	b = "";
	return false;
}

int main() {

	string str = "aDAFSGD enkdfgv kjddf knr fnef  ensd fmsmnef  wwjke f efna df  HGFHG ASFGDHFGS";
	string b;
	int start = 0;
	while (NextString(str, b, start)) {
		cout << b <<" "<<b.length()
			<<endl;
	}

	getchar();
}