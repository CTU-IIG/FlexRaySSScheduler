from mpl_toolkits.mplot3d import Axes3D
import numpy as np
import matplotlib

matplotlib.use("Qt4Agg")
import matplotlib.pyplot as plt
from matplotlib import cm
import matplotlib.lines as mlines
import argparse
import csv
import os

# python3 make_MV_Approaches_plot.py --instances 100 Instances_APP/
number_of_evaluations = 6


class Plotter:
    def __init__(self, folder):
        self.folders_count = 0
        for file in os.listdir(folder):
            if os.path.isdir(folder + "/" + file):
                self.folders_count += 1
        self.dataset = [[0] * number_of_evaluations for _ in range(self.folders_count)]
        self.folder_names = [""] * self.folders_count
        folder_counter = 0
        for file in sorted(os.listdir(folder), key=str.lower):
            if os.path.isdir(folder + "/" + file):
                self.folder_names[folder_counter] = file
                folder_counter += 1

    def read_data(self, folder_name: str, instances: int):
        if not os.path.exists(folder_name):
            raise ValueError("The folder {} does not exists!".format(folder_name))
        for folder_id, folder in enumerate(self.folder_names):
            results_file = "{}/{}/results.csv".format(folder_name, folder)

            with open(results_file, 'r') as f:
                csv_file = csv.reader(f, delimiter=';')
                instance_file = ""
                for row in csv_file:
                    label = row[0]
                    instance_file = "{}/{}/{}".format(folder_name, folder, label)
                    label = label.split(".", 1)[0]
                    self.dataset[folder_id][0] += int(row[1])
                    self.dataset[folder_id][1] += int(row[3])
                    self.dataset[folder_id][2] += int(row[5])
                    self.dataset[folder_id][3] += int(row[7])
                    self.dataset[folder_id][4] += int(row[9])
                    self.dataset[folder_id][5] += int(row[11])
                if folder != "porsche":
                    ins = instances
                else:
                    ins = 1;
                row1 = self.dataset[folder_id][5] / ins
                for i in range(number_of_evaluations):
                    self.dataset[folder_id][i] = 100.0 * self.dataset[folder_id][i] / ins / row1
                slot_length = 0
                with open(instance_file, 'r') as inf:
                    slot_length = int(inf.readlines()[2])
                print(slot_length)
                self.dataset[folder_id][0] /= 64 * slot_length

    def plot(self):
        if len(self.dataset) <= 0:
            raise ValueError("Dataset should not be empty")
        plt.rc("text", usetex=True)
        fig = plt.figure(1)
        fig.patch.set_facecolor('white')

        axe_x = np.asarray([float(x) for x in range(self.folders_count)])
        datasetzip = list(zip(*self.dataset))
        styles = ["v", "s", "D", "o", "^", "*"]
        labels = ["Volume of messages", "LB for independent schedule", "Independent schedule", "LB for Multi-Variant schedule", "Multivariant schedule", "Scheduling all signals to one schedule"]
        cmap = cm.winter
        for i in reversed(range(number_of_evaluations)):
            axe_y = np.asarray(datasetzip[i])
            plt.plot(axe_x, axe_y, styles[i], color = cmap(1 - i/float(6)), zorder=i, markersize=14, label='\\rmfamily {}'.format(labels[i]))
            # line = mlines.Line2D([], [], marker=styles[i], label='Text')
        plt.legend(loc="upper left", bbox_to_anchor=(0, 0.4), ncol=2, fancybox=True, shadow=True)
        plt.axis([-0.5, 7.5, 0, 110])
        plt.xlabel("{\\rmfamily \\LARGE Iteration}")
        plt.ylabel("{\\rmfamily \\LARGE Bandwidth w.r.t common schedule [\%]}")
        x_labels = ["Synth", "SAE_1", "SAE_2", "SAE_3", "SAE_4", "SAE_5", "SAE_6", "SAE_7"]
        plt.yticks(range(0, 105, 20), ['\\rmfamily \\LARGE 0', '\\rmfamily \\LARGE 20', '\\rmfamily \\LARGE 40', '\\rmfamily \\LARGE 60', '\\rmfamily \\LARGE 80', '\\rmfamily \\LARGE 100'])
        plt.xticks(range(self.folders_count), map(lambda x: '\\LARGE {}}}'.format(x).replace('_', '{\\large ') if '_' in x else '\\LARGE {}'.format(x), x_labels), rotation=45)
        plt.show()


if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("folder", help="Original file with the instances and results")
    parser.add_argument("--instances", help="Number of instances", type=int, default=10)
    args = parser.parse_args()
    plotter = Plotter(args.folder)
    plotter.read_data(args.folder, args.instances)
    plotter.plot()
