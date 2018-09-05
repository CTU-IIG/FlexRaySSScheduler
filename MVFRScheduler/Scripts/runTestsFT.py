import sys, os

# directories = ["Porsche", "PorscheSync", "SAE_1", "SAE_2", "SAE_3", "SAE_4", "SAE_5", "SAE_6", "SAE_7"]
directories = ["instances"]
for folder in directories:
    for filename in sorted(os.listdir("../Scripts/" + folder + "/")):
        if filename.endswith('.txt'):
            print(
            "../Release/MVFRScheduler -ah -r -cb ../Scripts/" + folder + "/out.csv ../Scripts/" + folder + "/" + filename + "\n")
            csv = open("instances/out.csv", "a")
            csv.write(filename + "; ")
            csv.close()
            os.system(
                "../Release/MVFRScheduler -ah -r -cb ../Scripts/" + folder + "/out.csv ../Scripts/" + folder + "/" + filename)
