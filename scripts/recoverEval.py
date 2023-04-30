import numpy as np

def plotBars(x1, x2, x3, y1, y2, y3, title, xlabel, y1label, y2label, x1ticks, x2ticks, yticks, savePath):
    import matplotlib.pyplot as plt
    plt.rc('text', usetex=True)
    plt.rc('font', family='Helvetica', size=13, weight="bold")
    tableau_colors = [
        (31, 119, 180),
        (174, 199, 232),
        (255, 127, 14),
        (255, 187, 120),
        (44, 160, 44),
        (152, 223, 138),
        (214, 39, 40),
        (255, 152, 150),
        (148, 103, 189),
        (197, 176, 213)
    ]
    for i in range(len(tableau_colors)):
        r, g, b = tableau_colors[i]
        tableau_colors[i] = (r / 255., g / 255., b / 255.)
    
    fig, ax = plt.subplots(nrows=len(y1), ncols=1, figsize=(10, 12))
    
    for i, ax1 in enumerate(ax):
        ax1.bar(x1, y1[i], color=tableau_colors[0], width=0.6)
        ax1.set_title(title[i])
        ax1.set_xlabel(xlabel)
        ax1.set_ylabel(y1label)
        rects = ax1.patches
        for rect, label in zip(rects, y1[i]):
            height = rect.get_height()
            ax1.text(rect.get_x() + rect.get_width() / 2, height, f"{label:.2f}", ha='center', va='bottom', fontsize=13)
        
        ax2 = ax1.twinx()
        ax2.bar(x2, y2[i], color=tableau_colors[1], width=0.6)
        ax2.set_ylabel(y2label)
        rects = ax2.patches
        for rect, label in zip(rects, y2[i]):
            height = rect.get_height()
            ax2.text(rect.get_x() + rect.get_width() / 2, height, f"{label:.2f}", ha='center', va='bottom', fontsize=13)
        
        ax1.set_xticks(np.append(x1, x2, axis=0), x1ticks + x2ticks)
        
        if x3 is not None:
            ax3 = ax1.twinx()
            ax3.plot(x3, y3[i], color=tableau_colors[2])
            ax3.yaxis.set_visible(False)
            for x_val, y_val in zip(x3, y3[i]):
                ax3.text(x_val, y_val, f'{y_val*100:.2f}\%', ha='center', va='bottom', color = "r", fontsize=13)

    plt.savefig(savePath, dpi=244, bbox_inches='tight', pad_inches=0.1)
    plt.show()
    

colName = [
    "cmdNum",
    "naivePersistByte",
    "logKV_duration",
    "naiveKV_duration",
    "logKV_fail",
    "naiveKV_fail",
    "logKV_miss",
    "naiveKV_miss",
    "logKVSize",
    "naiveSize",
    "gtSize",
    "logKV_missRate",
    "naiveKV_missRate"
]
colDict = {}
for i, name in enumerate(colName):
    colDict[name] = i
save_path = ""

data = np.loadtxt(f"{save_path}/recover/recover.txt", delimiter=" ")
rawData = data.reshape(12, 5, 11)

# average the multiple runs
avgData = np.mean(rawData, axis=1)
logMiss_rate = avgData[:, colDict["logKV_miss"]] / avgData[:, colDict["gtSize"]]
naiveMiss_rate = avgData[:, colDict["naiveKV_miss"]] / avgData[:, colDict["gtSize"]]
avgData = np.append(avgData, logMiss_rate.reshape(-1, 1), axis=1)
avgData = np.append(avgData, naiveMiss_rate.reshape(-1, 1), axis=1)



cmdNums = [100000,400000,700000,1000000]

duration = []
throughput = []
failDuration = []
errorRate = []
for i in range(4):
    data_to_plot = avgData[i*3:i*3+3]
    duration.append([
        np.mean(data_to_plot[:, colDict["logKV_duration"]]),
        *data_to_plot[:, colDict["naiveKV_duration"]]
    ])
    duration[-1] = [x / 1000 for x in duration[-1]]
    throughput.append([
        np.mean(data_to_plot[:, colDict["logKV_duration"]]),
        *data_to_plot[:, colDict["naiveKV_duration"]]
    ])
    throughput[-1] = [cmdNums[i] / x  for x in throughput[-1]]

    failDuration.append([
        np.mean(data_to_plot[:, colDict["logKV_fail"]]),
        *data_to_plot[:, colDict["naiveKV_fail"]]
    ])
    errorRate.append([
        np.mean(data_to_plot[:, colDict["logKV_missRate"]]),
        *data_to_plot[:, colDict["naiveKV_missRate"]]
    ])
    xAxis1 = np.arange(len(duration))
    xAxis2 = np.arange(len(duration)) + len(duration)

titles = [
    f"{i} Command" for i in cmdNums
]

plotBars(
    xAxis1, xAxis2, xAxis2, 
    throughput[0:2], failDuration[0:2], errorRate[0:2], 
    titles[0:2], "", "Throughput (opt/ms)", "Duration (ms)", 
    ["logKV", "naiveKV$2^{15}$", "naiveKV$2^{20}$", "naiveKV$2^{25}$"], ["logKV", "naiveKV$2^{15}$", "naiveKV$2^{20}$", "naiveKV$2^{25}$"], 
    ["logKV", "naiveKV"], f"{save_path}/recovery1.png")
plotBars(
    xAxis1, xAxis2, xAxis2, 
    throughput[2:4], failDuration[2:4], errorRate[2:4], 
    titles[2:4], "", "Throughput (opt/ms)", "Duration (ms)", 
    ["logKV", "naiveKV$2^{15}$", "naiveKV$2^{20}$", "naiveKV$2^{25}$"], ["logKV", "naiveKV$2^{15}$", "naiveKV$2^{20}$", "naiveKV$2^{25}$"], 
    ["logKV", "naiveKV"], f"{save_path}/recovery2.png")