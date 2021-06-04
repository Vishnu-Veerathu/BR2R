#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <cmath>
#include <chrono> 
#include <fstream>
#include <bits/stdc++.h>
using namespace std;
using namespace std::chrono;

// This file allows for evaluation of the ranking algorithm alone, without OptSpace(or any reconstruction at all).
// A matrix following the BR2 model is created and then a fraction of the entries are flipped i.e, x becomes 1-x.
// Comparisons can then be made for the BR2R algorithm and Copeland ranking algorithm.


// Usage: Simply compile and run. Change settings in main function.

class Random{
public:
	Random(int seed): gen(seed) {};
	double sample(double min=0.0, double max=1.0){
		uniform_real_distribution<> dis(min,max);
		uses += 1;
		return dis(gen);

	}
    int sample_int(int min, int max){
        uniform_int_distribution<> dis(min,max);
        uses += 1;
        return dis(gen);
    }
	mt19937 gen;
	int uses = 0;
};

int uv_multiplier = 1;

// Should remove this, useless variables here.
double pgap = 0.5;
double sgap = 0; //log((pgap)/(1-pgap));

vector < Random> random_samplers;


//--------------------------------------------------------------------------------------------

bool beats(vector < vector < double > > &P, int arm1, int arm2){
    return P[arm1][arm2] > P[arm2][arm1];
}

//--------------------------------------------------------------------------------------------

// beats function when arms is a permutation of the arms and P follows the permutation order as well. 
bool perm_beats(vector < vector < double > > &P, vector <int> &arms, int idx1, int idx2){
    int arm1 = arms[idx1];
    int arm2 = arms[idx2];
    return beats(P,arm1,arm2);
}
//-------------------------------------------------------------------------------------------

// Find upsets for ranking w.r.t. P
int upsets(vector<int> &ranking, vector<vector<double>> &P)
{
    int upsets = 0;
    int n = ranking.size();
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < i; j++)
        {
            if (P[ranking[i]][ranking[j]] > P[ranking[j]][ranking[i]])
            {
                upsets++;
            }
        }
    }
    return upsets;
}
//--------------------------------------------------------------------------------------------

// Min Upsets from u, v and P, upsets wrt P_true
int min_upsets(vector <double> u, vector <double> v, vector <vector <double> > P, vector <vector <double> > P_true){
	set <pair < double, int> > s;
	int n = u.size();
	for(int i = 0;i < u.size();i++){
		s.insert({atan2(v[i],u[i]),i});
	}

	vector <int> ranking;
	for(auto e: s){
		ranking.push_back(e.second);
	}

	int mini = n*n;
	vector<int> best_ranking;

	for(int i = 0;i < n;i++){
		int ranking_eval = upsets(ranking,P);
		if(ranking_eval < mini){
			mini = ranking_eval;
			best_ranking = ranking;
		}
		ranking.push_back(ranking[0]);
		ranking.erase(ranking.begin());	
	}
	return upsets(best_ranking,P_true);
}

//-------------------------------------------------------------------------------------------


void generate_matrix(int n,int b,vector<vector<double > > &P, int seed){

	// cout << "Seed used for matrix generation: " << seed << endl;
	// cout << "Uses so far: " << random_samplers[seed].uses << endl;

    // generate random u and v
    vector<double> u(n);
    vector<double> v(n);
    for (int i = 0; i < n; i++)
    {
        u[i] = uv_multiplier * (2 * random_samplers[seed].sample() - 1);
        v[i] = uv_multiplier * (2 * random_samplers[seed].sample() - 1);
    }

    // cout << "u = " << endl;
    // for(int i = 0;i < u.size();i++){
    // 	cout << u[i] << " ";
    // }
    // cout << endl;

    vector<double> score(n);
    for (int i = 0; i < n; i++)
    {
        score[i] = uv_multiplier * (random_samplers[seed].sample());
    }

    int block_size = n/b;

    sort(score.begin(), score.end(), greater<double>());

    for(int i = 0;i < n;i++){
        for(int j = 0;j < n;j++){
            if(i/block_size == j/block_size){
                P[i][j] = exp(u[i]*v[j]);
            } else {
                P[i][j] = exp(score[i]-sgap*(i/block_size));
            }
        }
    }

    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n; j++)
        {
            if (j > i)
            {
                P[i][j] = P[i][j] / (P[i][j] + P[j][i]);
            }
            else if (i == j)
            {
                P[i][j] = 0.5;
            }
            else
            {
                P[i][j] = 1 - P[j][i];
            }
        }
    }
}

vector < vector <double > > introduce_flips(int n,vector< vector <double> > P, double eps, int seed){
    int flips = 0;
    vector < vector < double > > Q(n , vector <double> (n));
    for(int i = 0;i < n;i++){
        for(int j = 0;j <= i;j++){
            if(i == j){
                Q[i][j] = P[i][j];
            }
            else if(random_samplers[seed].sample() < eps){
            	flips += 1;
                Q[i][j] = 1-P[i][j];
                Q[j][i] = 1-P[j][i];
            } else{
                Q[i][j] = P[i][j];
                Q[j][i] = P[j][i];
            }
        }
    }
    // cout << "FLIPS = " << flips << endl;
    return Q;
}


//-------------------------------------------------------------------------------------------

// Finds copeland ranking.
vector<int> calc_copeland_ranking(vector <int> &arms,vector < vector < double > > &P){
	set < pair <int,int> > s;
	int n = arms.size();
	for(int i = 0;i < n;i++){
		int beats = 0;
		for(int j = 0;j < n;j++){
			if(perm_beats(P,arms,i,j)){
				beats++;
			}
		}
		s.insert({-beats,arms[i]});
	}

	vector <int> ranking;
	for(auto itr: s){
		ranking.push_back(itr.second);
	}

    return ranking;
}

//-------------------------------------------------------------------------------------------

// Find number of copeland upsets for ranking generated by P w.r.t. P_true.
int calc_copeland_upsets(vector < vector < double > > &P,vector < vector <double > > &P_true){
    int n = P.size();
    vector <int> arms;
    for(int i = 0;i < n;i++){
        arms.push_back(i);
    }
    vector <int> best_ranking = calc_copeland_ranking(arms,P);
    // cout << "Copeland ranking: " << endl;
    // for(auto e: best_ranking){
    //     cout << e << " ";
    // }
    // cout << endl;
    return upsets(best_ranking,P_true);
}

//-------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------

// Finds ranking assuming Rank 2 model. Makes additional recursive calls/considerations since all inputs may not by of Rank 2.
vector<int> optimal_estimated_ranking(vector <int> arms, vector < vector < double > > P){
	// vector <double> s;
	int n = arms.size();

    if(n <= 1){
        vector <int> best_ranking = arms;
        return best_ranking;
    }

	// Find a cycle
	int a;
	int b;
	int c;
	bool cycle = false;

	for(a = 0;a < n;a++){
		for(b = 0;b < n;b++){
			for(c=0;c<n;c++){
				if(a == b or b== c or c == a){
					continue;
				}
				if(perm_beats(P,arms,a,b) && perm_beats(P,arms,b,c) && perm_beats(P,arms,c,a)){
					cycle = true;
					// cout << P[arms[a]][arms[b]] << " " << P[arms[b]][arms[a]] << endl;
					// cout << P[arms[b]][arms[c]] << " " << P[arms[c]][arms[b]] << endl;
					// cout << P[arms[c]][arms[a]] << " " << P[arms[a]][arms[c]] << endl;
					goto skip;
				}
			}
		}
	}
	skip:

	if (!cycle || n <= 3){
		// cout << "COPELANDING" << endl;
		return calc_copeland_ranking(arms, P);
	}
    
	vector <int> s_a,s_b,s_c,s_lead,s_behind;
	for(int i = 0;i < n;i++){
        int arm = arms[i];
        bool beats_a = perm_beats(P,arms,i,a);
        bool beats_b = perm_beats(P,arms,i,b);
        bool beats_c = perm_beats(P,arms,i,c);

		if(beats_a && beats_b && beats_c){
			s_lead.push_back(arm);
		}
		else if(!beats_a && !beats_b && !beats_c){
			s_behind.push_back(arm);
		}
		else if(beats_a && !beats_b){
			s_c.push_back(arm);
		}
		else if(beats_b && !beats_c){
			s_a.push_back(arm);
		}
		else if(beats_c && !beats_a){
			s_b.push_back(arm);
		}	
	}

	// set <int> s = {0,1,2,16};
	// for(auto e: s){
	// 	for(auto e1: s){
	// 		cout << P[arms[e]][arms[e1]] <<  " ";
	// 	}
	// 	cout << endl;
	// }
	// cout << endl;

    // cout << a << " " << b << " " << c << endl;
    // cout << s_lead.size() << " " << s_a.size() << " " << s_b.size() << " " << s_c.size() << " " << s_behind.size() << endl;
    // for(auto e: s_lead){
    //     cout << e << " ";
    // }
    // cout << endl;
    // for(auto e: s_a){
    //     cout << e << " ";
    // }
    // cout << endl;
    // for(auto e: s_b){
    //     cout << e << " ";
    // }
    // cout << endl;
    // for(auto e: s_c){
    //     cout << e << " ";
    // }
    // cout << endl;
    // for(auto e: s_behind){
    //     cout << e << " ";
    // }
    // cout << endl;

	vector<int> ss_a = optimal_estimated_ranking(s_a,P);
	vector<int> ss_b = optimal_estimated_ranking(s_b,P);
	vector<int> ss_c = optimal_estimated_ranking(s_c,P);
	vector<int> ss_lead = optimal_estimated_ranking(s_lead,P);
	vector<int> ss_behind = optimal_estimated_ranking(s_behind,P);

	vector <int> ranking;
	for(int i = 0;i < ss_a.size();i++){
		ranking.push_back(ss_a[i]);
	}
	for(int i = 0;i < ss_b.size();i++){
		ranking.push_back(ss_b[i]);
	}
	for(int i = 0;i < ss_c.size();i++){
		ranking.push_back(ss_c[i]);
	}

	int mini = n*n;
	vector<int> best_ranking;
	int rsize = ranking.size();
	for(int i = 0;i < rsize;i++){
		int ranking_eval = upsets(ranking,P);
		if(ranking_eval < mini){
			mini = ranking_eval;
			best_ranking = ranking;
		}
		ranking.push_back(ranking[0]);
		ranking.erase(ranking.begin());	
	}

	vector <int> full_best_ranking;
	for(int i = 0;i < ss_lead.size();i++){
		full_best_ranking.push_back(ss_lead[i]);
	}
	for(int i = 0;i < rsize;i++){
		full_best_ranking.push_back(best_ranking[i]);
	}
	for(int i = 0;i < ss_behind.size();i++){
		full_best_ranking.push_back(ss_behind[i]);
	}

    return full_best_ranking;
}

//-------------------------------------------------------------------------------------------

// Number of upsets of P when ranked according to optimal Rank 2 method w.r.t. P_true.
int optimal_estimated_upsets(vector < vector < double > > &P,vector < vector <double > > &P_true){
    int n = P.size();
    vector <int> arms;
    for(int i = 0;i < n;i++){
        arms.push_back(i);
    }
    vector <int> best_ranking = optimal_estimated_ranking(arms,P);
    // cout << "cycle ranking " << endl;
    // for(auto e: best_ranking){
    //     cout << e << " ";
    // }
    // cout << endl;
    return upsets(best_ranking,P_true);
}

//-------------------------------------------------------------------------------------------


void swap_index(vector<vector<double > > &m, int idx1, int idx2){
    int n = m.size();
    for(int i = 0;i < n;i++){
        double temp = m[i][idx1];
        m[i][idx1] = m[i][idx2];
        m[i][idx2] = temp;
    }
    for(int i = 0;i < n;i++){
        double temp = m[idx1][i];
        m[idx1][i] = m[idx2][i];
        m[idx2][i] = temp;
    }
}


// For random shuffling of preference matrix.
void random_shuffle(vector<vector<double > > &m, int seed){
    int n = m.size();
    for(int i = 0;i < n-1;i++){
        int j = random_samplers[seed].sample_int(i,n-1);
        swap_index(m,i,j);
    }
}


void print_vec(vector<vector<double>> v){
	for(auto e: v){
		for(auto e1: e){
			cout << e1 << " ";
		}
		cout << endl;
	}
}
//-------------------------------------------------------------------------------------------

int main()
{
    int runs = 10;
    for (int i = 0; i < runs; i++)
    {
        Random r(i);
        random_samplers.push_back(r);
    }

    int n = 600;
    vector <int> blocks = {1,2,3,4};
    vector <double> eps = {0,1e-5,1e-3,1e-4,1e-3,1e-2,2e-2,3e-2,4e-2,5e-2,1e-1};

    int num_b = blocks.size();
    int num_eps = eps.size();

    vector < vector < double > > result1(num_b,vector<double>(num_eps,0));
    vector < vector < double > > result2(num_b,vector<double>(num_eps,0));


    for (int i = 0; i < runs; i++)
    {
        for (int j = 0; j < blocks.size(); j++)
        {
            for (int k = 0; k < eps.size(); k++)
            {
                vector<vector<double>> P(n, vector<double>(n, 0));
                generate_matrix(n, blocks[j], P, i);
                // cout << "generated P = " << endl;
                // print_vec(P);
                random_shuffle(P,i);
                // print_vec(P);
                auto Q = introduce_flips(n,P,eps[k], i);

                int sigma2 = calc_copeland_upsets(Q,P);
                result2[j][k] += sigma2;
                cout << "Sigma2 = " << sigma2 << endl;
                int sigma1 = optimal_estimated_upsets(Q,P);
                result1[j][k] += sigma1;
                cout << "Sigma1 = " << sigma1 << endl << endl;
            	Random r(i);
            	random_samplers[i] = r;
            }
        }
    }

    cout << "Blocks: " << endl;
    for(auto e: blocks){
        cout << e << " ";
    }
    cout << endl;
    cout << "Epsilon values: " << endl;
    for(auto e: eps){
        cout << e << " ";
    }
    cout << endl;

    cout << "Result 1:" << endl;
    print_vec(result1);
    cout << endl;
    cout << "Result 2:" << endl;
    print_vec(result2);

}