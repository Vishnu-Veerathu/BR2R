#include <bits/stdc++.h>

using namespace std;


int main(){
	vector <int> counts(2,0);
	string s;
	int idx = 0;
	while(cin >> s){
		if(s == "Yes"){
			counts[idx%2]++;
		}
		idx++;
	}
	cout << "Total = " << idx/2 << endl;
	cout << counts[0] << " " << counts[1] << endl;
}