from mpl_toolkits.mplot3d import Axes3D
import numpy as np
import matplotlib

matplotlib.use("Qt4Agg")
import matplotlib.pyplot as plt
from matplotlib import cm
import argparse
import csv
import os


# python3 make_MV_plot.py --steps 50 SAE_1.txt Results/results_MV_used_for_paper.csv


class Plotter:
    def __init__(self, steps):
        self.dataset = [[0 for gamma in range(steps)] for alpha in range(steps)]

    def read_data(self, original_file: str, results_file: str, steps: int, instances: int):
        if not os.path.exists(results_file) or not os.path.exists(original_file):
            raise ValueError("The result file: {} or original file: {} does not exists"
                             .format(results_file, original_file))
        step = int(100 / (steps - 1))
        with open(results_file, 'r') as f:
            csv_file = csv.reader(f, delimiter=';')
            for row in csv_file:
                label = row[0]
                label = label.split(".", 1)[0]
                orig_file_name_len = len(original_file.split(".", 1)[0])
                label = label[orig_file_name_len + 1:]
                alpha, gamma, instance_id = label.split("_")
                alpha = int(alpha)
                gamma = int(gamma)
                self.dataset[int(alpha / step)][int(gamma / step)] += int(row[1])
            for x in range(steps):
                for y in range(steps):
                    self.dataset[x][y] /= instances

    def plot(self, steps: int):
        if len(self.dataset) <= 0:
            raise ValueError("Dataset should not be empty")
        step = int(100 / (steps - 1))
        plt.rc("text", usetex=True)
        plt.rcParams['text.latex.preamble'] = [r'\usepackage[euler]{textgreek}']
        fig = plt.figure()
        fig.patch.set_facecolor('white')
        #fig.suptitle('Tak tohle vede do pekel!!!')
        ax = fig.add_subplot(111, projection='3d')

        axes = np.asarray([float(x) for x in range(0, 101, step)])
        xv, yv = np.meshgrid(axes, axes)
        zv = np.copy(yv)
        for i in range(len(self.dataset)):
            for j in range(len(self.dataset[0])):
                if self.dataset[i][j] != 0:
                    zv[i, j] = self.dataset[i][j]
                else:
                    zv[i, j] = np.NAN
        # zv = xv
        # ax.plot_wireframe(xv, yv, zv, cstride=1, rstride=1)
        ax.plot_surface(xv, yv, zv, alpha=0.4, cmap=cm.jet, cstride=1, rstride=1, vmin=50, vmax=190)
        ax.set_xlabel("{\\rmfamily \\LARGE\\textgamma} {\\normalsize[\\%]}")
        ax.set_xticklabels(["\\rmfamily \\large 0", "\\rmfamily \\large 20", "\\rmfamily \\large 40", "\\rmfamily \\large 60", "\\rmfamily \\large 80", "\\rmfamily \\large 100"])
        ax.set_ylabel("{\\rmfamily \\LARGE\\textalpha} {\\normalsize[\\%]}")
        ax.set_yticklabels(["\\rmfamily \\large 0", "\\rmfamily \\large 20", "\\rmfamily \\large 40", "\\rmfamily \\large 60", "\\rmfamily \\large 80", "\\rmfamily \\large 100"])
        ax.set_zlabel("{\\rmfamily \\LARGE Number of allocated slots}")
        ax.set_zticklabels(["\\rmfamily \\large 40", "\\rmfamily \\large 50", "\\rmfamily \\large 60", "\\rmfamily \\large 70", "\\rmfamily \\large 80", "\\rmfamily \\large 90", "\\rmfamily \\large 100", "\\rmfamily \\large 110", "\\rmfamily \\large 120"])
        ax.grid(True)
        # x = [6,3,6,9,12,24]
        # y = [3,5,78,12,23,56]
        # put 0s on the y-axis, and put the y axis on the z-axis
        # ax.plot(xs=x, ys=[0]*len(x), zs=y, zdir='z', label='ys=0, zdir=z')
        plt.show()


if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("file", help="Original file with the canfiguration")
    parser.add_argument("results", help="File with the stored results")
    parser.add_argument("--steps", help="Number of steps", type=int, default=10)
    parser.add_argument("--instances", help="Number of instances", type=int, default=30)
    args = parser.parse_args()
    plotter = Plotter(args.steps + 1)
    plotter.read_data(args.file, args.results, args.steps + 1, args.instances)
    plotter.plot(args.steps + 1)
