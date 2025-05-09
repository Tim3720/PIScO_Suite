# SegmenterParallel:
[ ] Implement more error checking (check image sizes for deviation, etc.)
[ ] Test libtorch integration

# GenerateCrops
[x] Implement code to genreate crops from the input files of the segmenter


# SinciVinci
[ ] Implement SinciVinci with the modules 

# Tracking based Segmenting
- Post processing step on the segmenter data? I.e. first run the segmenter on the images to generate data, then use the generated data to track objects accross multiple images.
- This would drastically reduce the number of objects for e.g. videos and would allow do measure velocity distribuitions etc.

# New layout for segmenter:
- View segmenter only as first step with possible additional post-processing steps on the generated data
- One easy and clear way for this would be the division of all steps in different modules, i.e. do the following:
1. Prepare data: download images (if needed), apply metadata and naming convention, etc.
2. Generate background-corrected images and store the corrected images in an efficient (and maybe temporary) way
3. Find objects on the background-corrected images and generate data files with important data for each object
4. Optional: Use object tracking algorithm to gather time dependent tracking information for objects and/or reduce
   number of objects by storing only one data entry for each tracked object
5. Optional: Generate crops from the object data
- Use more restricions, i.e. less possible options. This makes developement of post-processing modules easier while also
  enforcing predictable results. One example would be saving all object data in one file per profile.
- Use globaly defined data


# Ideas: 
- Cross-Correlation tracking: https://staff.fnwi.uva.nl/r.vandenboomgaard/IPCV20162017/LectureNotes/CV/Motion/CorrelationTracking.html
- stereoscopic PIV
