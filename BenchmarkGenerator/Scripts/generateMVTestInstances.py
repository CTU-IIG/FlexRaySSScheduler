import argparse
import os
import random


# python3 generateMVTestInstances.py --steps 50 --instances 20 SAE_1.txt
# python3 generateMVTestInstances.py --runtest Instances_MV/ Results/ SAE_1.txt

class InstanceGenerator:
    def __init__(self, original_file_name: str):
        self.original_file_name = original_file_name
        with open(original_file_name, 'r') as f:
            self.original_config_file = f.readlines()

    def generate_config_files(self, steps: int = 10, instance_count: int = 10):
        for alpha_step in range(0, steps + 1):
            for gamma_step in range(0, steps + 1):
                alpha = int(alpha_step * 100 / steps)
                gamma = int(gamma_step * 100 / steps)
                if alpha + gamma <= 100:
                    output_file = "{}_{}_{}.txt".format(self.original_file_name.split(".", 1)[0], alpha, gamma)
                    with open(output_file, 'w') as f:
                        f.writelines(self.original_config_file[0:12])
                        f.write(str(gamma) + "\n")
                        f.write(str(alpha) + "\n")
                    InstanceGenerator.generate_instance(output_file, instance_count)
                    os.remove(output_file)

    @staticmethod
    def generate_instance(instance_config_file: str, instance_count: int = 10):
        if not os.path.isdir("Instances_MV"):
            os.makedirs("Instances_MV")
        output_file_prefix = "Instances_MV/" + instance_config_file.split(".", 1)[0]
        for i in range(0, instance_count):
            output_file = "{}_{}.txt".format(output_file_prefix, i)
            os.system("../Binary/BenchmarkGenerator -s {} -m -c {} -o {}".format(random.randint(0, 32000),
                                                                                 instance_config_file, output_file))


class TestRunner:
    def __init__(self, instances_directory, output_directory):
        self.instances_directory = instances_directory
        self.output_directory = output_directory

    def run_tests(self, instance_file_name: str):
        if not os.path.isdir(self.output_directory):
            os.makedirs(self.output_directory)
        for filename in sorted(os.listdir(self.instances_directory + "/")):
            if instance_file_name.split(".", 1)[0] in filename:
                results_file = "{}/results.csv".format(self.output_directory)
                with open(results_file, 'a') as f:
                    f.write("{};".format(filename))
                input_file = self.instances_directory + "/" + filename
                os.system("../../MVFRScheduler/Release/MVFRScheduler -aca -cb {} {}"
                          .format(results_file, input_file))


if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("file", help="Input benchmark instance generator configuration file")
    parser.add_argument("--steps", type=int, default=10, help="Number of steps to generate")
    parser.add_argument("--instances", type=int, default=30, help="Number of instances in each step")
    parser.add_argument("--runtest", nargs=2, help="Run tests instead of generating ones")

    args = parser.parse_args()
    if args.runtest is not None:
        runner = TestRunner(args.runtest[0], args.runtest[1])
        runner.run_tests(args.file)
    else:
        gen = InstanceGenerator(args.file)
        gen.generate_config_files(args.steps, args.instances)
