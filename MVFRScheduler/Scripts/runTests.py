import os

# directories = ["Porsche", "PorscheSync", "SAE_1", "SAE_2", "SAE_3", "SAE_4", "SAE_5", "SAE_6", "SAE_7"]
directories = ["SAE_6", "SAE_7"]
for folder in directories:
    for filename in os.listdir("../ChannelScheduler/" + folder + "/"):
        if filename.endswith('.txt'):
            print(
                "../Release/MVFRScheduler -ah -r -cb ../ChannelScheduler/" + folder + "/out_rev2_alpha.csv ../ChannelScheduler/" + folder + "/" + filename + "\n")
            os.system(
                "../Release/MVFRScheduler -ah -r -cb ../ChannelScheduler/" + folder + "/out_rev2_alpha.csv ../ChannelScheduler/" + folder + "/" + filename)
