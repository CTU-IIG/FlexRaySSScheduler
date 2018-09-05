#!/usr/bin/python3

import argparse
import os
import random


# python3 generateMVTestInstances.py --steps 50 --instances 20 SAE_1.txt
# python3 generateMVTestInstances.py --runtest Instances_MV/ Results/ SAE_1.txt

class InstanceGenerator:
    def __init__(self, original_file_name: str):
        self.original_file_name = original_file_name

    def generate_config_files(self, instance_count: int = 10, iterations_count: int = 10):
        self.generate_instances(instance_count, iterations_count)

    def generate_instances(self, instance_count: int = 10, iterations_count: int = 10):
        for filename in os.listdir('{}/'.format(self.original_file_name)):
            if filename.split('.')[-1] == 'txt':
                self.generate_instance(filename, instance_count, iterations_count)

    def generate_instance(self, instance_config_file: str, instance_count: int = 10, iterations_count: int = 10):
        if not os.path.isdir("{}/{}".format(self.original_file_name, instance_config_file.split(".", 1)[0])):
            os.makedirs("{}/{}".format(self.original_file_name, instance_config_file.split(".", 1)[0]))
        if not os.path.isdir("{}/{}/Schedules".format(self.original_file_name, instance_config_file.split(".", 1)[0])):
            os.makedirs("{}/{}/Schedules".format(self.original_file_name, instance_config_file.split(".", 1)[0]))
        output_file_prefix = "{}/{}/{}".format(self.original_file_name, instance_config_file.split(".", 1)[0], instance_config_file.split('.', 1)[0])
        for i in range(0, instance_count):
            base_config = "{}_{}_{}_.txt".format(output_file_prefix, str(i), "00")
            cmd = "../Binary/BenchmarkGenerator -s {} -c {}/{} -o {}".format(random.randint(0, 32000),
                                                                          self.original_file_name, instance_config_file, base_config)
            os.system(cmd)
            print(cmd)
            for j in range(1, iterations_count):
                output_file = "{}_{}_{}_.txt".format(output_file_prefix, str(i), str(j).zfill(2))
                cmd = "../Binary/BenchmarkGenerator -s {} -i {} -c {}/{} -o {}".format(random.randint(0, 32000),
                                                                                    base_config,
                                                                                    self.original_file_name,
                                                                                    instance_config_file,
                                                                                    output_file)
                os.system(cmd)
                print(cmd)
                base_config = output_file


class TestRunner:
    def __init__(self, instances_directory, output_directory):
        self.instances_directory = instances_directory
        self.output_directory = output_directory

    def run_tests_sets(self):
        for filename in os.listdir('{}/'.format(self.instances_directory)):
            if filename.split('.')[-1] == 'txt':
                self.run_tests(filename.split('.', 1)[0])


    def run_tests(self, instance_file_name: str):
        if not os.path.isdir(self.output_directory):
            os.makedirs(self.output_directory)
        results_file = "{}/results.csv".format(self.output_directory)
        for filename in sorted(os.listdir(self.instances_directory + "/" + instance_file_name + "/")):
            if ".txt" not in filename:
                continue
            set_name, instance_id, iteration, ext = filename.split('_')
            i_iteration = int(iteration)
            schedule_ne = "{}/{}/Schedules/{}".format(self.instances_directory, instance_file_name, filename.split('.', 1)[0] + "sch.txt")
            schedule_e = "{}/{}/Schedules/{}".format(self.instances_directory, instance_file_name, filename.split('.', 1)[0] + "sche.txt")
            with open(results_file, 'a') as f:
                f.write("{};".format(filename))
            input_file = self.instances_directory + "/" + instance_file_name + "/" + filename
            if i_iteration == 0:
                algorithms = ["-aca -t"]
                for alg in algorithms:
                    schedule = schedule_e if "-t" in alg else schedule_ne
                    cmd = "../../MVFRScheduler/Release/MVFRSchedulerFT {} -c {} -o {} {}".format(alg,
                                                                                                 results_file,
                                                                                                 schedule,
                                                                                                 input_file)
                    print(cmd)
                    os.system(cmd)
            else:
                algorithms = ["-ai -t"]
                for alg in algorithms:
                    schedule = schedule_e if "-t" in alg else schedule_ne
                    schedule_post = "sche" if "-t" in alg else "sch"
                    original_schedule = "{}/{}/Schedules/{}_{}_{}_{}.txt".format(self.instances_directory,
                                                                                 instance_file_name,   
                                                                                 set_name,
                                                                                 instance_id,
                                                                                 str(i_iteration - 1).zfill(2),
                                                                                 schedule_post)
                    if "i" in alg:
                        if "-t" in alg:
                            cmd = "../../MVFRScheduler/Release/MVFRSchedulerFT -ai {} -c {} -o {} {}" \
                                .format(original_schedule,
                                        results_file,
                                        schedule,
                                        input_file)
                        else:
                            cmd = "../../MVFRScheduler/Release/MVFRSchedulerFT -ai {} -c {} -t -o {} {}" \
                                .format(original_schedule,
                                        results_file,
                                        schedule,
                                        input_file)

                    else:
                        cmd = "../../MVFRScheduler/Release/MVFRSchedulerFT {} -c {} {}".format(alg, results_file,
                                                                                                input_file)
                    print(cmd)
                    os.system(cmd)
            with open(results_file, 'a') as f:
                f.write("\n")


if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("folder", help="Folder with input benchmark instance generator configuration files")
    parser.add_argument("--instances", type=int, default=30, help="Number of instances in each step")
    parser.add_argument("--iterations", type=int, default=10, help="Number of generated iterations")
    parser.add_argument("--runtest", nargs=1, help="Run tests instead of generating ones")

    args = parser.parse_args()
    if args.runtest is not None:
        runner = TestRunner(args.folder, args.runtest[0])
        runner.run_tests_sets()
    else:
        gen = InstanceGenerator(args.folder)
        gen.generate_config_files(args.instances, args.iterations)
