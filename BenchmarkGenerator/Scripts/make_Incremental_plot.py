from mpl_toolkits.mplot3d import Axes3D
import numpy as np
import matplotlib

matplotlib.use("Qt4Agg")
import matplotlib.pyplot as plt
from matplotlib import cm
import argparse
import csv
import os

# python3 make_MV_vs_Incremental_plot.py --steps 50 --iterations 10 --instances 30 SAE_1.txt Results/results_MV_vs_Incremental.csv
number_of_evaluations = 4


class Plotter:
    def __init__(self, iterations: int):
        self.dataset = [[0] * number_of_evaluations for _ in range(iterations)]

    def read_data(self, results_file: str, iterations: int, instances: int):
        if not os.path.exists(results_file):
            raise ValueError("The file with results {} does not exists!".format(results_file))
        with open(results_file, 'r') as f:
            csv_file = csv.reader(f, delimiter=';')
            for row in csv_file:
                label = row[0]
                label = label.split(".", 1)[0]
                iteration = int(label.split("_")[4])
                self.dataset[iteration][0] += int(row[1])
                self.dataset[iteration][1] += int(row[3])
                self.dataset[iteration][2] += int(row[5])
                self.dataset[iteration][3] += int(row[7])

            for i in range(iterations):
                for j in range(number_of_evaluations):
                    self.dataset[i][j] /= instances

    def plot(self, iterations: int):
        if len(self.dataset) <= 0:
            raise ValueError("Dataset should not be empty")
        plt.rc("text", usetex=True)
        plt.rcParams["xtick.major.pad"]='10'
        fig = plt.figure(1)
        fig.patch.set_facecolor('white')
        axe_x = np.asarray([float(x) for x in range(iterations)])
        datasetzip = list(zip(*self.dataset))
        styles = ["s-", "D-", "o-", "^-", "*-"]
        labels = ["\\Large Lower bound", " \\Large Non-incremental scheduling", "\\Large Incremental scheduling with extensibility optimisation", "\\Large Incremental scheduling without extensibility optimisation"]
        cmap = cm.winter
        for i in range(number_of_evaluations):
            axe_y = np.asarray(datasetzip[i])
            plt.plot(axe_x, axe_y, styles[i], markersize=7, color=cmap((i+1)/float(number_of_evaluations + 1)), label="\\rmfamily {}".format(labels[i]))
        plt.axis([0, 9.5, 100, 150])
        plt.xticks(range(0, 10), ["\\rmfamily \\LARGE 1", "\\rmfamily \\LARGE 2", "\\rmfamily \\LARGE 3", "\\rmfamily \\LARGE 4", "\\rmfamily \\LARGE 5", "\\rmfamily \\LARGE 6", "\\rmfamily \\LARGE 7", "\\rmfamily \\LARGE 8", "\\rmfamily \\LARGE 9", "\\rmfamily \\LARGE 10"])
        plt.yticks(range(100, 150, 10), ["\\rmfamily \\LARGE 100", "\\rmfamily \\LARGE 110", "\\rmfamily \\LARGE 120", "\\rmfamily \\LARGE 130", "\\rmfamily \\LARGE 140"])
        plt.legend(loc="upper left")
        plt.xlabel("{\\rmfamily \\LARGE Iteration}")
        plt.ylabel("{\\rmfamily \\LARGE Number of allocated slots}")
        # plt.xticks(range(self.folders_count), self.folder_names, rotation=45)
        plt.show()


if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("file", help="Original file with the results")
    parser.add_argument("--instances", help="Number of instances", type=int, default=10)
    parser.add_argument("--iterations", help="Number of iterations", type=int, default=10)
    args = parser.parse_args()
    plotter = Plotter(args.iterations)
    plotter.read_data(args.file, args.iterations, args.instances)
    plotter.plot(args.iterations)
