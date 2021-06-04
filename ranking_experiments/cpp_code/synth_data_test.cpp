#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <cmath>
#include <chrono> 
extern "C"
{
	#include "./OptSpace_C/svdlib.h"
	#include "./OptSpace_C/OptSpace.h"
}
#include <fstream>
#include "graph.h"
#include <bits/stdc++.h>
using namespace std;
using namespace std::chrono;


// Class for generation of random numbers.
class Random{
public:
	Random(int seed): gen(seed) {};
	double sample(double min=0.0, double max=1.0){
		uniform_real_distribution<> dis(min,max);
		return dis(gen);

	}
    int sample_int(int min, int max){
        uniform_int_distribution<> dis(min,max);
        return dis(gen);
    }
	mt19937 gen;

};

// The file to store the created data should be given here.

ofstream data_store;
string data_storage_file = "Kfvar_data_rank3.txt";


// Parameters for running of the experiment.
int n = 600; // Number of arms.
int num_blocks = 3; // Number of blocks, blocks are considered to be equal in size in this experiment.
int optspace_rank = 12; // Rank to run OptSpace with. Taken to be 4*num_blocks
vector <int> K = {5,10}; // The set of number of Bernoulli Samples to create the data seen by the ranking algorithm.
vector <double> known_fraction = {1,2,5,7.5,10}; // The set of fraction of entries that are available to the ranking algorithm.
//The last 2 parameters are given as sets, the data generation and ranking algorithm is run for every pair of values among them.

// Optspace Iterations.
vector <int> iterations = {100};

// Number of runs for each parameter set.
int runs = 10;
vector < Random> random_samplers;

//--------------------------------------------------------------------------------------------

// Function to output data to the desired file.
void output_data(vector<vector< double > > &P){
	data_store.open(data_storage_file, std::ofstream::app);
	int n = P.size();
	for(int i = 0;i < n;i++){
		for(int j=0;j < n;j++){
			data_store << P[i][j] << " ";
		}
		data_store << endl;
	}
	data_store.close();
}

//--------------------------------------------------------------------------------------------

// Can be used to verify OptSpace recovery.
double calc_frob_norm(vector < vector < double > > &P1, vector <vector <double > > &P2){
    int n = P1.size();
    double norm = 0;
    for(int i = 0;i < n;i++){
        for(int j = 0; j<n;j++){
            norm += (P1[i][j]-P2[i][j])*(P1[i][j]-P2[i][j]);
        }
    }
    return norm;
}

//--------------------------------------------------------------------------------------------

// Simple comparison of 2 arms w.r.t. a matrix P.
bool beats(vector < vector < double > > &P, int arm1, int arm2){
    return P[arm1][arm2] > P[arm2][arm1];
}

//--------------------------------------------------------------------------------------------

// Simple comparison of 2 arms w.r.t. a matrix P, however the input values are indices in a
// permutation and the corresponding entries must be compared.
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

// Min Upsets from u, v and P, upsets wrt P_true. Uses the BR2 algorithm.
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

// Finds Borda ranking on the set of arms given as input w.r.t. a matrix P.
vector<int> calc_borda_ranking(vector <int> &arms, vector<vector<double > > &P){
    set < pair <double,int> > s;
    int n = arms.size();
    for(int i = 0;i < n;i++){
        int score = 0;
        for(int j = 0;j < n;j++){
            score += P[arms[i]][arms[j]] - P[arms[j]][arms[i]];
        }
        s.insert({-score,arms[i]});
    }

    vector <int> ranking;
    for(auto itr: s){
        ranking.push_back(itr.second);
    }
    return ranking;
}

//-------------------------------------------------------------------------------------------

// Finds Borda upsets given a P, a guess of the preference matrix reconstructed/available to
// the ranking algorithm, and the true preference matrix.
int calc_borda_upsets(vector < vector < double > > &P, vector < vector < double> > &P_true){
    int n = P.size();
    vector <int> arms;
    for(int i = 0;i < n;i++){
        arms.push_back(i);
    }
    vector <int> best_ranking = calc_borda_ranking(arms,P);
    return upsets(best_ranking,P_true);
}

//-------------------------------------------------------------------------------------------

// Finds Copeland ranking on the set of arms given as input w.r.t. a matrix P.
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
    return upsets(best_ranking,P_true);
}

//-------------------------------------------------------------------------------------------

// Finds ranking assuming Block Rank 2 model. Makes additional recursive calls/considerations
// since all inputs may not by of Rank 2.
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
					goto skip;
				}
			}
		}
	}
	skip:

	if (!cycle || n <= 3){
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

// Evaluated the above ranking algorithm w.r.t. P_true.
int optimal_estimated_upsets(vector < vector < double > > &P,vector < vector <double > > &P_true){
    int n = P.size();
    vector <int> arms;
    for(int i = 0;i < n;i++){
        arms.push_back(i);
    }
    vector <int> best_ranking = optimal_estimated_ranking(arms,P);
    return upsets(best_ranking,P_true);
}

//-------------------------------------------------------------------------------------------

vector < vector < double > > ptr_to_vec(double **ptr, int n){
	vector < vector < double > > ret;
	for(int i = 0;i < n ;i++){
		vector <double> temp;
		for(int j = 0;j < n;j++){
			temp.push_back(ptr[i][j]);
		}
		ret.push_back(temp);
	}
	return ret;
}

//-------------------------------------------------------------------------------------------

// Simple utility functions for ease of implementation.
double** vec_to_ptr(vector < vector < double > > v, int n){
	double** ptr = (double**)calloc(n,sizeof(double*));
	for(int i = 0;i < n;i++){
		ptr[i] = (double*)calloc(n,sizeof(double));
		for(int j = 0;j < n;j++){
			ptr[i][j] = v[i][j];
		}
	}
	return ptr;
}

//-------------------------------------------------------------------------------------------

void print_vec(vector<vector<double>> v){
	for(auto e: v){
		for(auto e1: e){
			cout << e1 << " ";
		}
		cout << endl;
	}
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

void swap_index(vector<double> m, int idx1, int idx2){
    double temp = m[idx1];
    m[idx1] = m[idx2];
    m[idx2] = temp;
}

//-------------------------------------------------------------------------------------------

// For random shuffling of preference matrix.
void random_shuffle(vector<double> &u, vector <double> &v, vector <double> &s, vector<vector<double > > &m, int seed){
    int n = m.size();
    for(int i = 0;i < n-1;i++){
        int j = random_samplers[seed].sample_int(i,n-1);
        swap_index(m,i,j);
        swap_index(u,i,j);
        swap_index(v,i,j);
        swap_index(s,i,j);
    }
}

//-------------------------------------------------------------------------------------------

double bernoulli_sim(double seed,double p, int k){
	double sum = 0;
	if(k > 10001){
		return p;
	}
	for(int i = 0;i < k;i++){
		if(random_samplers[seed].sample() < p){
			sum += 1;
		}
	}
	// For prevention of divide by zero errors
	if(sum == 0){
		return 0.0001;
	}
	if(sum == k){
		return 1-0.0001;
	}
	return sum/k;
}

//-------------------------------------------------------------------------------------------

class TestResults{
public:
	double min_upsets = 0;
	double copeland_upsets = 0;
    double block_upsets = 0;
    double borda_upsets = 0;
	double unknowns = 0;
	double total_errors = 0;
	double known_errors = 0;
	double n = 0;
	double K = 0;
	double kf = 0;
	double iters = 0;
	double duration = 0;
};

TestResults run_test(int seed, int num_iters, int K, double known_pairs, double uv_multiplier = 1)
{
    // generate random u and v and score.
    vector<double> u(n);
    vector<double> v(n);
    for (int i = 0; i < n; i++)
    {
        u[i] = uv_multiplier * (2 * random_samplers[seed].sample() - 1);
        v[i] = uv_multiplier * (2 * random_samplers[seed].sample() - 1);
    }
    vector<double> score(n);
    for (int i = 0; i < n; i++)
    {
        score[i] = uv_multiplier * (random_samplers[seed].sample());
    }

    // Sort scores to enforce ordering among blocks.
    sort(score.begin(), score.end(), greater<double>());

    // Assume equal sized blocks.
    int block_size = n / num_blocks;

    // Create full preference matrix
    vector<vector<double>> P_true(n, vector<double>(n));
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n; j++)
        {
            if (i / block_size == j / block_size)
            {
                P_true[i][j] = exp(u[i] * v[j]);
            }
            else
            {
                P_true[i][j] = exp(score[i]);
            }
        }
    }

    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n; j++)
        {
            if (j > i)
            {
                P_true[i][j] = P_true[i][j] / (P_true[i][j] + P_true[j][i]);
            }
            else if (i == j)
            {
                P_true[i][j] = 0.5;
            }
            else
            {
                P_true[i][j] = 1 - P_true[j][i];
            }
        }
    }
    // Full preference matrix created

    // Find lowest upsets.
    int lowest_upsets = 0;
    for (int i = 0; i < num_blocks; i++)
    {
        vector<double> sub_u;
        vector<double> sub_v;
        vector<vector<double>> sub_P_true;

        for (int j = i * block_size; j < (i + 1) * block_size; j++)
        {
            sub_u.push_back(u[j]);
            sub_v.push_back(v[j]);
            vector<double> sub_P_row;
            for (int z = i * block_size; z < (i + 1) * block_size; z++)
            {
                sub_P_row.push_back(P_true[j][z]);
            }
            sub_P_true.push_back(sub_P_row);
        }
        int block_upsets = min_upsets(sub_u, sub_v, sub_P_true, sub_P_true);
        lowest_upsets += block_upsets;
        int prev_block_upsets = block_upsets;

        block_upsets = optimal_estimated_upsets(sub_P_true, sub_P_true);

        assert(prev_block_upsets == block_upsets);
    }

    cout << "LOWEST UPSETS FROM P_TRUE: " << lowest_upsets << endl;

    // Random shuffle the matrix
    random_shuffle(u,v,score,P_true,seed);

    // known values
    int total_known = 0;
    vector<vector<int>> known(n, vector<int>(n));
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n; j++)
        {
            if (i == j)
            {
                known[i][j] = 1;
                total_known += 1;
            }
            else if (j > i)
            {
                if (random_samplers[seed].sample() > known_pairs)
                {
                    known[i][j] = 0;
                }
                else
                {
                    known[i][j] = 1;
                    total_known += 2;
                }
            }
            else
            {
                known[i][j] = known[j][i];
            }
        }
    }

    cout << "TOTAL KNOWN: " << total_known << endl;

    // Create algo seen matrix and output known entries to datafile.
    vector<vector<double>> P_sim(n, vector<double>(n));
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n; j++)
        {
            if (j > i)
            {
                P_sim[i][j] = bernoulli_sim(seed, P_true[i][j], K);
            }
            else if (i == j)
            {
                P_sim[i][j] = 0.5;
            }
            else
            {
                P_sim[i][j] = 1 - P_sim[j][i];
            }
        }
    }
    // print_vec(P_sim);

    vector<vector<double>> P_log(n, vector<double>(n));
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n; j++)
        {
            P_log[i][j] = 1e0 * log(P_sim[i][j] / P_sim[j][i]);
        }
    }

    output_data(P_true);
    vector < vector <double > > P_sim_known_only(n, vector <double> (n,0));
    vector < vector <double > > P_sim_known_only1(n, vector <double> (n,-1));
    for(int i = 0;i < n;i++){
    	for(int j = 0;j < n;j++){
    		if(known[i][j]){
    			P_sim_known_only[i][j] = P_sim[i][j];
                P_sim_known_only1[i][j] = P_sim[i][j];
    		}
    	}
    }

    output_data(P_sim_known_only);
    output_data(P_sim_known_only1);
 
    // Create sparse matrix representation
    smat s;
    s.rows = n;
    s.cols = n;
    s.vals = total_known;
    s.pointr = (long *)calloc(n + 1, sizeof(long));
    s.rowind = (long *)calloc(s.vals, sizeof(long));
    s.value = (double *)calloc(s.vals, sizeof(double));
    s.pointr[0] = 0;
    int count = 0;
    for (int j = 0; j < n; j++)
    {
        for (int i = 0; i < n; i++)
        {
            if (known[i][j])
            {
                s.rowind[count] = i;
                s.value[count] = P_log[i][j];
                ++count;
            }
        }
        s.pointr[j + 1] = count;
    }

    // Use Optspace
    SMat sptr = &s;
    auto result = OptSpace(sptr, optspace_rank, num_iters, 0.001, 0, "iterinfo");
    int n1 = n;
    int n2 = optspace_rank;
    int n3 = n;

    // For result calculation
    double **res1 = (double **)calloc(n1, sizeof(double *));
    for (int i = 0; i < n1; i++)
    {
        res1[i] = (double *)calloc(n2, sizeof(double));
    }
    double **res2 = (double **)calloc(n1, sizeof(double *));
    for (int i = 0; i < n3; i++)
    {
        res2[i] = (double *)calloc(n3, sizeof(double));
    }

    // For reconstructed Matrix
    matrixmul(result->X, result->S, n1, n2, n2, res1, 0, 1, 0);
    matrixmul(res1, result->Y, n1, n2, n3, res2, 1, 1, 0);

    // Construct the algorithm seen 'guesstimate' of the preference matrix.
    auto P = ptr_to_vec(res2, n);

    // Can choose to compute frob_norm is results are unexpected.
    // cout << "Frob norm of recovery is: " << calc_frob_norm(P,P_log) << endl;

    // dg, kdg stand for disagreements and known disgreements. disagreements refers to number of pairs of arms where P, P_true differ on who beats who.
    // kdg is computed similarly, however is restricted to only the values that the ranking algorithm knows 'for sure' i.e, the entries available post-sampling.
    int dg = 0;
    int kdg = 0;
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n; j++)
        {
            if ((P[i][j] > P[j][i]) ^ (P_true[i][j] > P_true[j][i]))
            {
                dg += 1;
                if (known[i][j])
                {
                    kdg += 1;
                }
            }
        }
    }

    // P has been created. From here on no matrix should be used other than P for ranking.
    // P_true should be used only for evaluation purposes.
    P_sim.clear();
    P_sim_known_only.clear();
    P_sim_known_only1.clear();
    P_log.clear();

    cout << "PAIRWISE DISAGREE: " << dg << " " << kdg << endl;

    // For Copeland Based Upsets
    int copeland_upsets = calc_copeland_upsets(P, P_true);
    cout << "COPELAND UPSETS: " << copeland_upsets << endl;

    // To consider as one big block
    int borda_upsets = calc_borda_upsets(P, P_true);
    cout << "Borda UPSETS: " << borda_upsets << endl;

    // Split into SCCs to run the block rank algorithm. Can also be used to check if the block structure was recovered by OptSpace.
    Graph g(n);
    for (int i = 0; i < n; i++)
    {
        for (int j = i + 1; j < n; j++)
        {
            if (P[i][j] > P[j][i])
            {
                g.addEdge(j, i);
            }
            else
            {
                g.addEdge(i, j);
            }
        }
    }
    g.CalcSCC();

    // Calculate upsets for each block.
    int multi_block_upsets = 0;
    cout << "BLOCKS FOUND: " << g.SCCs.size() << endl;
    for (int i = 0; i < g.SCCs.size(); i++)
    {
        vector<vector<double>> sub_P;
        vector<vector<double>> sub_P_true;
        for (int j = 0; j < g.SCCs[i].size(); j++)
        {
            vector<double> sub_P_row, sub_P_true_row;
            for (int k = 0; k < g.SCCs[i].size(); k++)
            {
                int arm1 = g.SCCs[i][j];
                int arm2 = g.SCCs[i][k];
                sub_P_row.push_back(P[arm1][arm2]);
                sub_P_true_row.push_back(P_true[arm1][arm2]);
            }
            // out << g.SCCs[i][j] << " ";
            sub_P.push_back(sub_P_row);
            sub_P_true.push_back(sub_P_true_row);
        }

        int ind_block_upsets = optimal_estimated_upsets(sub_P, sub_P_true);
        multi_block_upsets += ind_block_upsets;
    }

    // Calculate upsets across blocks.
    for (int idx1 = 0; idx1 < g.SCCs.size(); idx1++)
    {
        for (int idx2 = idx1 + 1; idx2 < g.SCCs.size(); idx2++)
        {
            for (int idx3 = 0; idx3 < g.SCCs[idx1].size(); idx3++)
            {
                for (int idx4 = 0; idx4 < g.SCCs[idx2].size(); idx4++)
                {
                    int arm1 = g.SCCs[idx1][idx3];
                    int arm2 = g.SCCs[idx2][idx4];
                    if ((P_true[arm1][arm2] > P_true[arm2][arm1]) ^ (P[arm1][arm2] > P[arm2][arm1]))
                    {
                        multi_block_upsets += 1;
                    }
                }
            }
        }
    }
    cout << "BLOCK RANKING UPSETS: " << multi_block_upsets << endl;

    // Return the results
    TestResults ret;
    ret.min_upsets = lowest_upsets;
    ret.copeland_upsets = copeland_upsets;
    ret.block_upsets = multi_block_upsets;
    ret.borda_upsets = borda_upsets;
    ret.unknowns = n * n - total_known;
    ret.total_errors = dg;
    ret.known_errors = kdg;
    return ret;
}

int main()
{
	// Clear data file.
	data_store.open(data_storage_file);
	data_store.close();

    for (int i = 0; i < runs; i++)
    {
        Random r(i);
        random_samplers.push_back(r);
    }

    vector <TestResults> all_results;

    // Simple loops and calls to run_test to create and evaluate generated datasets.
    for (auto num_iters : iterations)
    {
        for (auto k : K)
        {
            for (auto known : known_fraction)
            {
                double sum_min_upsets = 0;
                double sum_copeland_upsets = 0;
                double sum_block_upsets = 0;
                double sum_borda_upsets = 0;
                double sum_unknowns = 0;
                double sum_total_errors = 0;
                double sum_known_errors = 0;
                string filename = "n" + to_string(n) + "itr" + to_string(num_iters) + "k" + to_string(k) + "known" + to_string(known);
                
                TestResults means;
                means.n = n;
                means.K = k;
                means.kf = known;
                means.iters = num_iters;

                for (int run = 0; run < runs; run++)
                {
                    cout << "RUN: " << run << " Iterations: " << num_iters << " K: " << k << " known_fraction: " << known << endl;

                    int k_val = int(k*optspace_rank*log(n) + 0.5);
                    double known_frac = known*num_blocks*log(n)/n;

                    cout << "Calculated K is " << k_val << " and Kf is " << known_frac << endl;

                    auto start = high_resolution_clock::now();
                    TestResults R = run_test(run, num_iters, k_val, known_frac);
                    auto stop = high_resolution_clock::now();

                    auto duration = duration_cast<microseconds>(stop - start);
                    cout << duration.count() << endl;
                    cout << "+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+" << endl;
                    means.min_upsets += R.min_upsets;
                    means.copeland_upsets += R.copeland_upsets;
                    means.block_upsets += R.block_upsets;
                    means.borda_upsets += R.borda_upsets;
                    means.unknowns += R.unknowns;
                    means.total_errors += R.total_errors;
                    means.known_errors += R.known_errors;
                    means.duration += duration.count();
                }
				all_results.push_back(means);
            }
        }
    }

    // Output the aggregated results to a file.
    ofstream result_file;
    result_file.open("Kfvar_results_rank3.txt");
    for(auto result: all_results){
    	result_file << result.n << " ";
    	result_file << result.K << " ";
    	result_file << result.kf << " ";
    	result_file << result.iters << " ";
        result_file << result.min_upsets/runs << " ";
        result_file << result.block_upsets/runs << " ";
        result_file << result.copeland_upsets/runs << " ";
        result_file << result.borda_upsets/runs << " ";
        result_file << result.duration / runs << endl;
        // result_file << result.unknowns/runs << " ";
        // result_file << result.total_errors/runs << " ";
        // result_file << result.known_errors/runs << endl;
    }
}