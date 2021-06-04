import numpy as np
import matplotlib.pyplot as plt

# Plotting tool, not general purpose. Used specifically to generate plots which were required for publishing results.

plt.rcParams.update({'font.size': 17})
plt.rc('axes', labelsize=22) 
plt.rc('figure', titlesize=20)
plt.rc('legend', fontsize=19)
########################################################################

res = np.loadtxt('../cpp/Kvar_results_rank2.txt')

rc_result = np.load('../python/Kvar_rc_results_rank2.npy')

# For plotting K variation
separate_idx = 2 # Kf

separate_vals = [2,5]

for val in separate_vals:
    res1 = []
    rc_res = []
    for i,res_row in enumerate(res):
        if np.isclose(res_row[separate_idx],val):
            res1.append(res_row)
            rc_res.append(rc_result[i])
    res1 = np.array(res1)
    rc_res = np.array(rc_res)
    x = res1[:,1]
    y1 = res1[:,5]
    y2 = res1[:,6]
    y3 = res1[:,7]
    y4 = res1[:,8]
    plt.plot(x,y1,label = 'Optimal')
    plt.plot(x,y2,label = 'MC + PairwiseBlockRank',marker = 'v')
    plt.plot(x,y3,label = 'MC + Copeland',marker = 's')
    plt.plot(x,y4,label = 'MC + Borda',marker = '+')
    plt.plot(x,rc_res,label = 'Rank Centrality',marker = 'x')
    plt.title('Upsets vs K with Known Fraction = '+str(val)+'*(num blocks)*ln(n)/n')
    plt.ylabel('Upsets')
    plt.xlabel('K as multiple of (matrix rank)*(n)')
    plt.legend()
    figure = plt.gcf()
    figure.set_size_inches(9.2, 7.5)
    plt.savefig("Upsets_vs_K_Kf"+str(val)+"_rank2"+".png", dpi=100)
    plt.show()

########################################################################

res = np.loadtxt('../cpp/Kvar_results_rank3.txt')

rc_result = np.load('../python/Kvar_rc_results_rank3.npy')

# For plotting K variation
separate_idx = 2 # Kf

separate_vals = [2,5]

for val in separate_vals:
    res1 = []
    rc_res = []
    for i,res_row in enumerate(res):
        if np.isclose(res_row[separate_idx],val):
            res1.append(res_row)
            rc_res.append(rc_result[i])
    res1 = np.array(res1)
    rc_res = np.array(rc_res)
    x = res1[:,1]
    y1 = res1[:,5]
    y2 = res1[:,6]
    y3 = res1[:,7]
    y4 = res1[:,8]
    plt.plot(x,y1,label = 'Optimal')
    plt.plot(x,y2,label = 'MC + PairwiseBlockRank',marker = 'v')
    plt.plot(x,y3,label = 'MC + Copeland',marker = 's')
    plt.plot(x,y4,label = 'MC + Borda',marker = '+')
    plt.plot(x,rc_res,label = 'Rank Centrality',marker = 'x')
    plt.title('Upsets vs K with Known Fraction = '+str(val)+'*(num blocks)*ln(n)/n')
    plt.ylabel('Upsets')
    plt.xlabel('K as multiple of (matrix rank)*(n)')
    plt.legend()
    figure = plt.gcf()
    figure.set_size_inches(9, 7.1)
    plt.savefig("Upsets_vs_K_Kf"+str(val)+"_rank3"+".png", dpi=100)
    plt.show()

########################################################################

res = np.loadtxt('../cpp/Kfvar_results_rank2.txt')

rc_result = np.load('../python/Kfvar_rc_results_rank2.npy')

# For plotting K variation
separate_idx = 1 # Kf

separate_vals = [5,10]

for val in separate_vals:
    res1 = []
    rc_res = []
    for i,res_row in enumerate(res):
        if np.isclose(res_row[separate_idx],val):
            res1.append(res_row)
            rc_res.append(rc_result[i])
    res1 = np.array(res1)
    rc_res = np.array(rc_res)
    x = res1[:,2]
    y1 = res1[:,5]
    y2 = res1[:,6]
    y3 = res1[:,7]
    y4 = res1[:,8]
    plt.plot(x,y1,label = 'Optimal')
    plt.plot(x,y2,label = 'MC + PairwiseBlockRank',marker = 'v')
    plt.plot(x,y3,label = 'MC + Copeland',marker = 's')
    plt.plot(x,y4,label = 'MC + Borda',marker = '+')
    plt.plot(x,rc_res,label = 'Rank Centrality',marker = 'x')
    plt.title('Upsets vs Known Fraction with K = '+str(val)+'*(matrix rank)*ln(n)')
    plt.ylabel('Upsets')
    plt.xlabel('Known Fraction as multiple of (num_blocks)*ln(n)/n')
    plt.legend()
    figure = plt.gcf()
    figure.set_size_inches(9, 7.1)
    plt.savefig("Upsets_vs_Kf_K"+str(val)+"_rank2"+".png", dpi=100)
    plt.show()

###########################################################################

res = np.loadtxt('../cpp/Kfvar_results_rank3.txt')

rc_result = np.load('../python/Kfvar_rc_results_rank3.npy')

# For plotting K variation
separate_idx = 1 # Kf

separate_vals = [5,10]

for val in separate_vals:
    res1 = []
    rc_res = []
    for i,res_row in enumerate(res):
        if np.isclose(res_row[separate_idx],val):
            res1.append(res_row)
            rc_res.append(rc_result[i])
    res1 = np.array(res1)
    rc_res = np.array(rc_res)
    x = res1[:,2]
    y1 = res1[:,5]
    y2 = res1[:,6]
    y3 = res1[:,7]
    y4 = res1[:,8]
    plt.plot(x,y1,label = 'Optimal')
    plt.plot(x,y2,label = 'MC + PairwiseBlockRank',marker = 'v')
    plt.plot(x,y3,label = 'MC + Copeland',marker = 's')
    plt.plot(x,y4,label = 'MC + Borda',marker = '+')
    plt.plot(x,rc_res,label = 'Rank Centrality',marker = 'x')
    plt.title('Upsets vs Known Fraction with K = '+str(val)+'*(matrix rank)*ln(n)')
    plt.ylabel('Upsets')
    plt.xlabel('Known Fraction as multiple of (num_blocks)*ln(n)/n')
    plt.legend()
    figure = plt.gcf()
    figure.set_size_inches(9, 7.1)
    plt.savefig("Upsets_vs_Kf_K"+str(val)+"_rank3"+".png", dpi=100)
    plt.show()