import argparse
import os
import random


# python3 generateMVTestInstances.py --steps 50 --instances 20 SAE_1.txt
# python3 generateMVTestInstances.py --runtest Instances_MV/ Results/ SAE_1.txt

class InstanceGenerator:
    @staticmethod
    def generate_instance(folder_name: str, instance_count: int = 10):
        for config_file in os.listdir(folder_name):
            if os.path.isdir('{}/{}'.format(folder_name, config_file)):
                continue
            config_file_without_ext = config_file.split('.', 1)[0]
            output_directory = "{}/{}".format(folder_name, config_file_without_ext)
            if not os.path.exists(output_directory):
                os.mkdir(output_directory)
            for i in range(0, instance_count):
                output_file = "{}/{}_{}.txt".format(output_directory, config_file_without_ext, i)
                cmd = "../Binary/BenchmarkGenerator -s {} -c {}/{} -o {}".format(random.randint(0, 32000),
                                                                                 folder_name, config_file,
                                                                                 output_file)
                # print(cmd)
                os.system(cmd)


class TestRunner:
    @staticmethod
    def run_tests(folder_name: str):
        for folder_file in os.listdir(folder_name):
            folder_folder_name = '{}/{}'.format(folder_name, folder_file)
            if not os.path.isdir(folder_folder_name):
                continue
            for filename in sorted(os.listdir(folder_folder_name + "/")):
                if folder_file in filename:
                    results_file = "{}/results.csv".format(folder_folder_name)
                    with open(results_file, 'a') as f:
                        f.write("{};".format(filename))
                    input_file = folder_folder_name + "/" + filename
                    algorithms = ['aw', 'ali', 'ada', 'alm', 'aca', 'ao']
                    for algorithm in algorithms:
                        if algorithm is not 'ada':
                            cmd = "../../MVFRScheduler/Release/MVFRScheduler -c {} -{} {}" \
                                .format(results_file, algorithm, input_file)
                        else:
                            cmd = "../../MVFRScheduler/Release/MVFRScheduler -{} {} {}" \
                                .format(algorithm, results_file, input_file)
                        print(cmd)
                        os.system(cmd)
                    with open(results_file, 'a') as f:
                        f.write("\n")


if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("folder", help="Folder with config files")
    parser.add_argument("--instances", type=int, default=30, help="Number of instances in each step")
    parser.add_argument("--runtest", action='store_true', help="Run tests instead of generating ones")

    args = parser.parse_args()
    if args.runtest is True:
        TestRunner.run_tests(args.folder)
    else:
        InstanceGenerator.generate_instance(args.folder, args.instances)
