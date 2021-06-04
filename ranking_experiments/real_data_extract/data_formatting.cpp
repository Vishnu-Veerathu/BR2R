#include <bits/stdc++.h>

using namespace std;

// This file is used to convert data from the format present at https://github.com/csinpi/blade_chest/tree/master/datasets
// Simple usage - compile and run as ./{exec_name} {input data file path} {whether input data file has a tag} {output data file path}

pair <int,int> semicolonsplit(string s){
    int i;
    for(i = 0;i < s.size();i++){
        if(s[i] == ':'){
            break;
        }
    }
    return {stoi(s.substr(0,i)),stoi(s.substr(i+1,s.size()-i-1))};
}


int main(int argc, char* argv[]){

    vector < string> argList(argv + 1, argv + argc);
    cout << "CLIs: ";
    for(int i = 0;i < argList.size();i++){
        cout << argList[i] << " ";
    }
    cout << endl;

	ifstream datafile;
    datafile.open(argList[0]);

    bool has_tag = false;
    if(argList[1] == "1"){
        has_tag = true;
    }

    string outputfile = argList[2];


    ofstream datamatrix;
    datamatrix.open(outputfile);

    string dummy;
    datafile >> dummy;
    int num_players;
    datafile >> num_players;
    
    getline(datafile,dummy);
    vector <string> player_names(num_players);
    for(int i = 0;i < num_players;i++){
        getline(datafile,dummy);
        cout << dummy << endl;
    }

    vector < vector < pair <int,int> > > match_data(num_players,(vector < pair <int,int> >(num_players,{0,0})));
    datafile >> dummy;
    int num_matches;
    datafile >> num_matches;
    cout << "Number of Games: " << num_matches << endl;
    
    datamatrix << num_players << endl;
    datamatrix << num_matches << endl;

    for(int i = 0;i < num_matches;i++){
        if(has_tag){
            datafile >> dummy;
        }
        datafile >> dummy;
        pair <int,int> players = semicolonsplit(dummy);
        datafile >> dummy;
        pair <int,int> result = semicolonsplit(dummy);

        datamatrix << players.first << " " << players.second << " " << result.first << " " << result.second << endl;
    }
}