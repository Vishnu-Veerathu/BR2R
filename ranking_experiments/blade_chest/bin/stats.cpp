#include <bits/stdc++.h>
using namespace std;

int main(){
	int n = 20;
	int n1 = 4;
	vector < vector <int> > v(n,vector <int> (n1));
	for(int i = 0;i < n;i++){
		for(int j = 0;j < n1;j++){
			cin >> v[i][j];
		}
	}

	vector <double> mean(n1,0);

	for(int i = 0;i < n;i++){
		for(int j = 0;j < n1;j++){
			mean[j] += v[i][j];
		}
	}

	for(int i = 0;i < n1;i++){
		mean[i] /= n;
	}

	vector <double> std(n1,0);
	for(int i = 0;i < n;i++){
		for(int j = 0;j < n1;j++){
			std[j] += (v[i][j]-mean[j])*(v[i][j]-mean[j]);
		}
	}

	for(int i = 0;i < n1;i++){
		std[i] /= n;
		std[i] = sqrt(std[i]);
	}
	for(int i = 0;i < n1;i++){
		cout << mean[i] << "+-" << std[i] << " ";
	}
	cout << endl;
}