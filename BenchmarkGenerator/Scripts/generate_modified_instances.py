#from mpl_toolkits.mplot3d import Axes3D
#import numpy as np
#import matplotlib
#matplotlib.use("Qt4Agg")
#import matplotlib.pyplot as plt
#from matplotlib import cm
import argparse
#import csv
import os
from functools import reduce


class modified_instance_generator:
    def __init__(self, instance_path):
        self.instance_path = instance_path
        with open(self.instance_path, 'r') as f:
            self.original_file = f.readlines()

    def generate_instaces(self, min_payload, max_payload, step_payload, output_directory):
        min_payload_sat = max(min_payload, max(map(int, self.original_file[11].split())))
        min_payload_sat = (int((min_payload_sat-1)/16)+1)*16
        for payload in range(min_payload_sat, max_payload+1, step_payload):
            for period_shift in range(0, 7):
                if int(self.original_file[1])/2**period_shift >= 1:
                    self.generate_instace(payload, period_shift, output_directory)

    def generate_instace(self, payload, period_shift, output_directory):
        output_filename = "{}/instance_payload{}_priodsh{}.txt".format(output_directory, payload, period_shift)
        with open(output_filename, 'w') as f:
            f.write(self.original_file[0])  # Signals Count
            cycle_length = int(int(self.original_file[1])/2**period_shift)
            f.write("{}\n".format(cycle_length))  # period
            f.write("{}\n".format(payload))  # Length of slot
            f.write(self.original_file[3])  # Number of variants
            f.write(self.original_file[4])  # Number of ECUs
            f.write(self.original_file[5])  # Number of signals in particular ECU
            f.write(self.original_file[6])  # ECU assignment
            f.write(self.original_file[7])  # Signal name
            f.write("{}\n".format(reduce(lambda x, y: "{} {}".format(x, y), map(lambda x: str(min(64*cycle_length, int(x))), self.original_file[8].split()))))  # Period
            f.write(self.original_file[9])  # Release date
            f.write("{}\n".format(reduce(lambda x, y: "{} {}".format(x, y), map(lambda x: str(min(64*cycle_length-1, int(x))), self.original_file[10].split()))))  # Deadline
            for i in range(11, len(self.original_file)):
                f.write(self.original_file[i])
        print(output_filename)


class TestRunner:
    def __init__(self, instances_directory, output_directory):
        self.instances_directory = instances_directory
        self.output_directory = output_directory

    def run_tests(self):
        if not os.path.isdir(self.output_directory):
            os.makedirs(self.output_directory)
        for filename in sorted(os.listdir(self.instances_directory + "/")):
            if "instance_payload" in filename:
                results_file = "{}/results.csv".format(self.output_directory)
                input_file = self.instances_directory + "/" + filename
                with open(input_file, 'r') as f:
                    cycle_length = f.readlines()[1].strip()
                with open(results_file, 'a') as f:
                    tmp = (''.join(filter(lambda x: str.isdigit(x) or x == "_", filename))).split('_')
                    f.write("{};{};{};".format(filename, tmp[1], tmp[2]))
                cmd = "../../MVFRScheduler/Release/MVFRScheduler -aca -c {} {}".format(results_file, input_file)
                print(cmd)
                os.system(cmd)
                cmd = "python3 get_max_slots.py -c {} -b 10000 -s {} -o {}".format(cycle_length, tmp[1], results_file)
                print(cmd)
                os.system(cmd)


if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("file", help="Path to the original benchmark instance")
    parser.add_argument("--min_payload", help="Number of iterations", type=int, default=16)
    parser.add_argument("--max_payload", help="Number of iterations", type=int, default=256)
    parser.add_argument("--step_payload", help="Number of iterations", type=int, default=16)
    parser.add_argument("--output_dir", help="Output directory", type=str, default='output')
    parser.add_argument("--runtest", nargs=2, help="Run tests instead of generating ones")
    args = parser.parse_args()

    if args.runtest is not None:
        runner = TestRunner(args.runtest[0], args.runtest[1])
        runner.run_tests()
    else:
        generator = modified_instance_generator(args.file)
        generator.generate_instaces(args.min_payload, args.max_payload, args.step_payload, args.output_dir)
