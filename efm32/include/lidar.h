#ifndef LIDAR_H
#define LIDAR_H

#include "setup.h"
#include "serial.h"

void stop_lidar();
void reset_lidar(bool verbose);
void health_lidar(bool verbose);
void sample_rate_lidar(bool verbose);
void init_scan_lidar(bool verbose);
void init_lidar(bool verbose);

#endif
