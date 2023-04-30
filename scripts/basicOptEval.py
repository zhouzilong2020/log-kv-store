import matplotlib.pyplot as plt
import numpy as np
import os

iteration = 5
cmd_num = [100000, 200000, 400000, 600000]
hot_key_fractions = [0.01, 0.05, 0.1, 0.2]
hot_rates = [0.01, 0.7, 0.8, 0.9]
hit_rates = [0.1, 0.2, 0.6, 0.8, 0.99]
# hot_key_fractions = [0]
# hot_rates = [0]
# hit_rates = [0.99]
read_fractions = [0.1, 0.2, 0.4, 0.6, 0.8, 0.99]
all_factors = [
    hot_key_fractions,
    hot_rates,
    hit_rates,
    read_fractions
]
all_names = {
    0: "hot fraction",
    1: "hot rate",
    2: "hit rate",
    3: "read fraction",
}

TYPE_NUM = 2
ITERATION = iteration
DIM = 4             # 0: throughput, 1: duration, 2: memory usage, 3: minimum memory usage
throughputScale = 1000
memoryScale = 1


def plot_curves(
    x, y, labels, title,
    save_name, x_label, y_label,
    legend_label
):
    plt.rcParams['font.family'] = 'serif'
    fig, ax = plt.subplots(figsize=(15,6))
    
    for i in range(len(labels)):
        ax.plot(x, y[i], label=labels[i], marker='o')

    ax.legend(loc='best', fontsize=13)
    # ax.set_title(title, fontsize=17)
    ax.set_xlabel(x_label, fontsize=16)
    ax.set_ylabel(y_label, fontsize=16)
    ax.set_ylim(bottom=0)
    # plt.xticks(ticks=np.arange(len(labels)), labels=labels)
    plt.tick_params(axis='x', which='both', labelsize=15)
    plt.tick_params(axis='y', which='both', labelsize=14)
    
    plt.savefig(f"{save_name}")
    plt.show()

data_path = ""

exp_path = [
    # f"{data_path}/eval/correct/10w", 
    # f"{data_path}/eval/correct/20w", 
    f"{data_path}/eval/correct/40w", 
    # f"{data_path}/eval/correct/60w"，
]

def loadData(path, cmdNum):
    """
    Load data from files. The filenames must match the cartesian product of all factors.
    """
    data = np.zeros(
        (
            len(hot_key_fractions),
            len(hot_rates),
            len(hit_rates),
            len(read_fractions),
            TYPE_NUM,
            ITERATION,
            DIM
        )
    )

    dirList = os.listdir(path)
    dirList.sort()
   
    for i, filename in enumerate(dirList):
        if ".npy" not in filename:
            continue
        args = filename.split('-')
        args[-1] = args[-1].split('.')[0]
        args = [float(i) for i in args]

        idx = (
            hot_key_fractions.index(args[1]),   # put the data to pre-defined array positions
            hot_rates.index(args[2]),
            hit_rates.index(args[3]), 
            read_fractions.index(args[4])
        )

        try:
            buf = np.load(f"{path}/{filename}")
            buf = np.append(buf, np.full((TYPE_NUM, ITERATION, 1), args[-1]), axis=-1)
            data[idx] = buf
        except:
            print(f"File {path}/{filename} not found")
        print(f"File {path}/{filename} loaded")

    return data

def analysis(
        varaible, fixed, 
        varName, fixedNames, 
        rawdata, scale, metric_dim_idx, 
        save_path, metricName, 
        metricUnit, cmdNum
):
    """
    Plot the curves of the varaible with fixed factors.

    @param 
    varaible:   the varaible factor to be plotted on the x-axis
    fixed:      the fixed factors to be plotted in the legends
    varName:    the name of the varaible factor
    fixedNames: the names of the fixed factors
    rawdata:    the raw data array, typically contains the result data from one command number setting (e.g. 400,000)
    scale:      the scale of the metric currently being plotted
    metric_dim_idx: the index of the metric's dimension in the raw data array's corresponding index
    save_path:  the path to save the plot
    metricName: the name of the metric (a string in the y-axis label)
    metricUnit: the unit of the metric (a string in the y-axis label)
    cmdNum:     the command number setting (used in the title and the filename)

    """
    log_data = []
    naive_data = []
    log_labels = []
    naive_labels = []

    for i1, val1 in enumerate(fixed[0]):
        for i2, val2 in enumerate(fixed[1]):
            for i3, val3 in enumerate(fixed[2]):   
                metric = rawdata[i1, i2, i3, :, :, :, metric_dim_idx] / scale
                metric = np.sum(metric, axis = 2) / metric.shape[2]
                metric = np.log(metric)

                log_data.append(metric[:, 0].flatten())
                naive_data.append(metric[:, 1].flatten())
                log_labels.append(f"LogKV {fixedNames[0]}: {val1:.2f} {fixedNames[0]}: {val2:.2f} {fixedNames[2]}: {val3:.2f}")
                naive_labels.append(f"Naive  {fixedNames[0]}: {val1:.2f} {fixedNames[0]}: {val2:.2f} {fixedNames[2]}: {val3:.2f}")
                
    all_data = log_data + naive_data
    all_labels = log_labels + naive_labels


    plot_curves(
        varaible, all_data, all_labels, 
        f"{varName} ({cmdNum} Commands)",
        f"{save_path}/../{metricName}_{cmdNum}_All.png",
        "read fraction", f"{metricName} / {metricUnit}", f"{metricUnit}"
    )


allData = []
for idx, path in enumerate(exp_path):
    allData.append(loadData(path, cmd_num[idx]))

# rawdata = np.swapaxes(data, 0, 1)
for exp_idx, rawdata in enumerate(allData):

    analysis(
        all_factors[3],     # all possible values of read fraction

        # take the first and last element of the hot key fraction, hot rate, and hit rate vectors to reduce the curves
        # [ [vec[0], vec[-1]] for vec in all_factors[:3] ],   
        all_factors[:3],    # all possible values of the hot key fraction, hot rate, and hit rate

        all_names[3],       # read fraction
        [all_names[0], all_names[1], all_names[2]], # hot key fraction, hot rate, and hit rate
        rawdata, 
        throughputScale, 0, # throughput is the first metric in the raw data array
        exp_path[exp_idx],  # save to the raw data path
        "Throughput", "Kops/s", # y-axis label
        cmd_num[exp_idx]    # command number setting
    )
