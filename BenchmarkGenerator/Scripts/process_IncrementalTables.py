import argparse
import csv
import os

# python3 make_MV_Approaches_plot.py --instances 100 Instances_APP/

class TableMaker:
    def read_data(self, folder_name: str, instances: int):
        if not os.path.exists(folder_name):
            raise ValueError("The folder {} does not exists!".format(folder_name))
        result_file = "{}/results.csv".format(folder_name)
        self.dataset = {}
        times = []
        instances_count = 0
        with open(result_file, 'r') as f:
            csv_file = csv.reader(f, delimiter=';')
            for row in csv_file:
                label = row[0]
                label, instantion, iteration, _ = label.split("_")
                instantion = int(instantion)
                instances_count = max(instantion + 1, instances_count)
                iteration = int(iteration)
                if label not in self.dataset:
                    self.dataset[label] = []
                if len(self.dataset[label]) <= iteration:
                    new_list = [0]*(1 + iteration - len(self.dataset[label]))
                    self.dataset[label].extend(new_list)
                if len(times) <= iteration:
                    times.extend([0]*(1 + iteration - len(times)))
                self.dataset[label][iteration] += int(row[1])
                times[iteration] += float(row[2])
            for dset in sorted(self.dataset):
                print(dset, end="")
                for value in self.dataset[dset]:
                    print(" & {0:.1f}".format(value/float(instances_count)), end="")
                print("\\\\")
            print("Ex. time [ms]", end="")
            for value in times:
                print(" & {0:.1f}".format(value/instances_count/len(self.dataset)), end="")
            print("\\\\")


if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("folder", help="Original file with the instances and results")
    parser.add_argument("--instances", help="Number of instances", type=int, default=10)
    args = parser.parse_args()
    tabler = TableMaker()
    tabler.read_data(args.folder, args.instances)
