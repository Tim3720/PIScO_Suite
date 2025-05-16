import h5py
import numpy as np
import matplotlib.pyplot as plt

# Path to your HDF5 file
filename = "/home/tim/Documents/ArbeitTestData/TestResults/segmenter_output.h5"

with h5py.File(filename, "r") as f:
    # Get sorted image group names (assumes they start with 'image_')
    image_names = sorted(f.keys())
    for i in range(len(image_names )):
        first_image = image_names[i]
        
        print(f"First image group: {first_image}")
        
        # Get sorted object group names within the first image
        object_names = sorted(f[first_image].keys())
        print(object_names)

        plt.imshow(np.array(f[first_image]["background_corrected"][()]), cmap="gray")
        plt.show()
        for j in range(1, len(object_names)):
            first_object = object_names[j]
            
            print(f"First object group: {first_object}")
            
            # Access the object group
            object_group = f[first_image][first_object]
            
            # Print all datasets in this object
            # for dataset_name in object_group:
            #     data = object_group[dataset_name][()]  # Load dataset as NumPy array
                # print(f"{dataset_name}: {data}")

            crop = np.array(f[first_image][first_object]["crop"][()])

            plt.imshow(crop, cmap="gray")
            plt.show()
