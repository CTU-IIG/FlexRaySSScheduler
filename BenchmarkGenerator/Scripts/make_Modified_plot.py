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

payloads = 128#15
periods = 4

class Plotter:
    def __init__(self):
        self.dataset = [[0 for _ in range(periods)] for _ in range(payloads)]

    def read_data(self, results_file: str):
        if not os.path.exists(results_file):
            raise ValueError("The result file: {} does not exists".format(results_file))
        step_payload = 16
        min_payload = 32
        instances = 1
        with open(results_file, 'r') as f:
            csv_file = csv.reader(f, delimiter=',')
            for row in csv_file:
                payload = int(row[1])
                period = int(row[2])
                percentage = int(row[6])
                self.dataset[int((payload-min_payload)/step_payload)][period] += percentage if percentage < 150 else np.nan
            for x in range(payloads):
                for y in range(periods):
                    self.dataset[x][y] /= instances

    def plot(self):
        if len(self.dataset) <= 0:
            raise ValueError("Dataset should not be empty")
        step_payload = 16
        min_payload = 32
        plt.rc("text", usetex=True)
        plt.rcParams['text.latex.preamble'] = [r'\usepackage[euler]{textgreek}']
        fig = plt.figure()
        fig.patch.set_facecolor('white')
        # fig.suptitle('Tak tohle vede do pekel!!!')
        ax = fig.add_subplot(111, projection='3d')
        # fig = plt.figure()
        # ax = fig.gca(projection='3d')

        axes_pay = np.asarray([float(x) for x in range(min_payload, min_payload+step_payload*payloads, step_payload)])
        axes_peri = np.asarray([float(x) for x in range(0, periods)])
        xv, yv = np.meshgrid(axes_peri, axes_pay)
        zv = np.copy(yv)
        print(zv)
        for i in range(len(self.dataset)):
            for j in range(len(self.dataset[0])):
                # print("{},{}".format(i,j))
                if self.dataset[i][j] != 0:
                    zv[i, j] = self.dataset[i][j]
                else:
                    zv[i, j] = np.NAN
        # zv = xv
        # ax.plot_wireframe(xv, yv, zv, cstride=1, rstride=1)
        # surf = ax.plot_surface(xv, yv, zv, alpha=0.9, cmap=cm.jet, cstride=1, rstride=1, vmin=66, vmax=100, shade=False )
        # surf.set_facecolor((1,1,1,1))
        v_min_value = np.nanmin(zv)-4
        ax.scatter(xv, yv, zv, alpha=0.9, cmap=cm.viridis_r, c=zv, vmin=v_min_value, vmax=100)
        # ax.plot_wireframe(xv, yv, zv, alpha=0.9, cmap=cm.jet, cstride=1, rstride=1)
        ax.set_xlabel("{\\rmfamily \\large M} {\\normalsize[ms]}")
        ax.set_xticklabels(["\\rmfamily \\large 8", "", "", "", "\\rmfamily \\large 4", "", "", "", "\\rmfamily \\large 2", "", "", "", "\\rmfamily \\large 1"])
        ax.set_ylabel("{\\rmfamily \\large W} {\\normalsize[bit]}")
        # ax.set_yticklabels(["\\rmfamily \\large 0", "\\rmfamily \\large 20", "\\rmfamily \\large 40", "\\rmfamily \\large 60", "\\rmfamily \\large 80", "\\rmfamily \\large 100"])
        ax.set_zlabel("{\\rmfamily \\large Portion of the slot threshold [\%]}")
        # ax.set_zticklabels(["\\rmfamily \\large 40", "\\rmfamily \\large 50", "\\rmfamily \\large 60", "\\rmfamily \\large 70", "\\rmfamily \\large 80", "\\rmfamily \\large 90", "\\rmfamily \\large 100", "\\rmfamily \\large 110", "\\rmfamily \\large 120"])
        ax.grid(True)
        # x = [6,3,6,9,12,24]
        # y = [3,5,78,12,23,56]
        # put 0s on the y-axis, and put the y axis on the z-axis
        # ax.plot(xs=x, ys=[0]*len(x), zs=y, zdir='z', label='ys=0, zdir=z')
        plt.show()


if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("results", help="File with the stored results")
    args = parser.parse_args()
    plotter = Plotter()
    plotter.read_data(args.results)
    plotter.plot()
