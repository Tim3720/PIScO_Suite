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


# Ideas: 
- Cross-Correlation tracking: https://staff.fnwi.uva.nl/r.vandenboomgaard/IPCV20162017/LectureNotes/CV/Motion/CorrelationTracking.html
- stereoscopic PIV
