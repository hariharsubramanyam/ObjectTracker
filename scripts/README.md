The ObjectTracker can create a JSON file with the trajectories of the objects. These trajectories can be very noisy. 

The `trajectory_smoothing.py` script applies a Gaussian filter to smooth the trajectories.

It outputs a CSV file with: `frame, x, y, transformed x, transformed y, tracker ID, tracker index` which is sorted in ascending order by frame.
