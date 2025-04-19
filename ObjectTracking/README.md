# ObjectTracking
This module works as a post-processing module for the segmenter output data of the SegmenterParallel module. The module tracks objects across multiple frames. 
Possible usecases:
1. Particle velocimetry: Measurement of phase-space distribution (position and velocity) of particles for e.g. sincing velocity measurements
2. Data reduction for video-like input: Tracks objects and thus only saves one image per tracked object, which can greatly decrease data load


## Algorithm:
