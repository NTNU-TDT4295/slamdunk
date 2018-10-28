# SlamDunk Visualizer

## slam
Performing HectorSlam on input lidar data. This data is accepted on
port 6002.

## slam_vis
Visualizing the output from slam. This data is accepted on port 6000.

## lidar
Visualizes the current lidar output. Accepts lidar data on port 6002.

## pc
Visualizes a 3D point cloud. New points can be sent as (x, y, z), with
each a 32-bit integer in millimeter resolution to port 6000.

## sim
A virtual lidar. Sends lidar-data on port 6002.

## Development
To test the slam algoritm without a lidar, the following command is
useful for playing back a prerecorded scan.

`pv -L 25k < test_data | nc 127.0.0.1 6002`

pv -L 25k rate-limits the data so slam is capable of keeping up with
incoming data.
