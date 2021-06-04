import numpy as np
import scipy.linalg
import warnings
import sys
# Need to be informed of divide by 0 errors (which may mess up things when using Python) etc...
warnings.filterwarnings("error")
debug = False

# Rank Centrality Ranking Method


# Simple MC from data.
def MC_from_data(n,data,alpha):
	p = alpha*np.ones([n,n])

	p = p + data
	
	# make max column sum 1
	column_max = np.max(np.sum(p,axis=0))
	p = p/column_max

	p += np.diag(1-np.sum(p,axis=0))
	return p

# RC ranking method.
def rc_ranking(n,data,alpha):
	p = MC_from_data(n,data,alpha)
	# print("eig finding")
	w,v = scipy.linalg.eig(p,left=False,right=True)

	w = np.real(w)
	max_eig_val = np.max(np.real(w))
	eig_vec = np.argmax(w)
	if debug:
		print("eigenvalues = ",w)
		print("eigenvectors = ",v)
	assert max_eig_val < 1+0.001 and max_eig_val > 1-0.001

	y = v[:,eig_vec]
	if debug:
		print(y)
	scores = y/np.sum(y)

	ranking = np.argsort(-scores)

	return ranking

# Calculate upsets of the ranking w.r.t. true_data.
def upsets(ranking,true_data):
	n = len(ranking)
	upsets = 0
	for i in range(n):
		for j in range(i):
			arm1 = ranking[j]
			arm2 = ranking[i]
			if(true_data[arm1,arm2] < true_data[arm2,arm1]):
				upsets += 1
	return upsets

######## Synth experiments ########

# runs = 10
# print('Data being loaded - May take a while for large datasets.')
# data = np.loadtxt("../cpp/Kfvar_data_rank3.txt")
# print('Data Load Finished.')

# n = len(data[0])
# entries = len(data)

# run_num = 0
# sum_upsets1 = 0

# num_trials = int(entries/(3*n))

# rc_results = np.array([])

# for i in range(num_trials):
# 	# Data format for the purpose of testing had to be compatible with the output data of the c++ codes.
# 	# Each run of a test outputs 3 matrices. The first matrix is the true matrix for the purpose of evaluation.
# 	# The second matrix has the Bernoulli simulations of each known entry, 0 for all unknown entries.
# 	# The third matrix is the same as the second one, with -1 instead of 0 for the unknown entries.
# 	# Ignored here, but may come in handy to implement other algorithms.
# 	true_data = data[3*i*n:(3*i+1)*n]
# 	real_data1 = data[(3*i+1)*n:(3*i+2)*n]
# 	real_data2 = data[(3*i+2)*n:(3*i+3)*n]

# 	ranking = rc_ranking(n,real_data1,0.001)

# 	u = upsets(ranking,true_data)
# 	print(u)
# 	sum_upsets1 += u

# 	run_num += 1
# 	if run_num >= runs:
# 		rc_results = np.append(rc_results,sum_upsets1/runs)
# 		run_num = 0
# 		sum_upsets1 = 0

# # The results are in the same order as the ones which would've been produced by the C++ codes.
# # This dependency is not a problem when testing, but eventually should be removed for convenience. 
# np.save('Kfvar_rc_results_rank3.npy',rc_results)

######## Real experiments ########

np.random.seed(0)
runs = 20

import sys

arr = list(map(int, input().split()))
n = arr[0]
print(n)
arr = list(map(int, input().split()))
m = arr[0]
print(m)
data = np.zeros((m,4), dtype=int)

for i in range(m):
	data[i] = np.array(list(map(int, input().split())), dtype=int)

train_frac = 0.7

upset_sum = 0

res = np.zeros(runs)

for i in range(runs):
	P_train = np.zeros((n,n))
	P_test = np.zeros((n,n))
	for j in range(m):
		if np.random.random() < train_frac:
			# print(data[j][0])
			P_train[data[j][0],data[j][1]] += data[j][2]
			P_train[data[j][1]][data[j][0]] += data[j][3]
		else:
			P_test[data[j][0]][data[j][1]] += data[j][2]
			P_test[data[j][1]][data[j][0]] += data[j][3]			

	ranking = rc_ranking(n,P_train,0.001)
	u = upsets(ranking,P_test)
	# print(u)
	# upset_sum += u
	# print(i,upset_sum/(i+1))
	res[i] = u

print(np.mean(res),np.std(res))