#include <stdio.h>
#include <stdlib.h>
#include <rplidar.h>
#include "net.h"
#include <sys/socket.h>
#include <math.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/fcntl.h>
#include <unistd.h>

#include <signal.h>
bool should_quit = false;
void ctrlc(int)
{
    should_quit = true;
}

#ifndef _countof
#define _countof(_Array) (int)(sizeof(_Array) / sizeof(_Array[0]))
#endif

using namespace rp::standalone::rplidar;

static float getAngle(const rplidar_response_measurement_node_hq_t& node) {
    return node.angle_z_q14 * 90.f / 16384.f;
}

int main(int argc, char *argv[]) {
	RPlidarDriver * drv = RPlidarDriver::CreateDriver(DRIVER_TYPE_SERIALPORT);
    if (!drv) {
        fprintf(stderr, "insufficent memory, exit\n");
        exit(-2);
    }

    rplidar_response_device_info_t devinfo;
    u_result op_result;
	if (!IS_OK(drv->connect("/dev/ttyUSB0", 115200))) {
		fprintf(stderr, "Failed to connect!\n");
		exit(-3);
	}
	op_result = drv->getDeviceInfo(devinfo);

	if (!IS_OK(op_result)) {
		fprintf(stderr, "Failed to connect!\n");
		exit(-3);
	}

    printf("RPLIDAR S/N: ");
    for (int pos = 0; pos < 16 ;++pos) {
        printf("%02X", devinfo.serialnum[pos]);
	}

    printf("\n"
            "Firmware Ver: %d.%02d\n"
            "Hardware Rev: %d\n"
            , devinfo.firmware_version>>8
            , devinfo.firmware_version & 0xFF
            , (int)devinfo.hardware_version);

	int soc = -1;

    signal(SIGINT, ctrlc);
    drv->startMotor();
    drv->startScan(0,1);

	int ticks_between_connect = 30;
	int ticks_until_connect = 0;

	while(1) {
        rplidar_response_measurement_node_hq_t nodes[8192];
        size_t   count = _countof(nodes);

		uint8_t buffer[count * 5];
		int buffer_i = 0;

        op_result = drv->grabScanDataHq(nodes, count);

        if (IS_OK(op_result)) {
            drv->ascendScanData(nodes, count);
            for (size_t pos = 0; pos < count; ++pos) {
				// Discard bad samples
				if (nodes[pos].quality < 20 || nodes[pos].dist_mm_q2 == 0) {
					continue;
				}

				int quality = nodes[pos].quality;
				float theta = nodes[pos].angle_z_q14 * 90.f / (1 << 14);
				float dist = nodes[pos].dist_mm_q2 / 1000.f / (1 << 2);

				if (dist > 3000.0f) {
					continue;
				}

				uint16_t angle_q = (uint16_t)(theta * 64.0f) << 1;
				uint16_t dist_q = (uint16_t)(dist * 4.0f * 1000.0f);

				buffer[buffer_i*5 + 0] = 0xa5;
				buffer[buffer_i*5 + 1] = angle_q & 0xff;
				buffer[buffer_i*5 + 2] = angle_q >> 8;
				buffer[buffer_i*5 + 3] = dist_q & 0xff;
				buffer[buffer_i*5 + 4] = dist_q >> 8;

				buffer_i += 1;


				// // deg to rad
				// theta *= M_PI / 180.0f;

				// float x = cos(theta) * dist;
				// float y = 0.0f;
				// float z = sin(theta) * dist;

				// buffer[pos*3 + 0] = (int32_t)(x * 1000.0f);
				// buffer[pos*3 + 1] = (int32_t)(y * 1000.0f);
				// buffer[pos*3 + 2] = (int32_t)(z * 1000.0f);
            }

			if (soc > 0) {
				size_t bytes_written = 0;
				size_t bytes_to_write = buffer_i * 5;
				uint8_t *b = (uint8_t *)buffer;
				while (bytes_written < bytes_to_write) {
					ssize_t err;
					err = send(soc, b + bytes_written, bytes_to_write - bytes_written, MSG_NOSIGNAL);
					if (err < 0) {
						perror("send");
						soc = -1;
						ticks_until_connect = ticks_between_connect;
						break;
					}
					bytes_written += err;

					if (bytes_written > bytes_to_write) {
						printf("Too much written!\n");
					}
				}
			}
        }

        if (should_quit){
            break;
        }

		ticks_until_connect -= 1;

		if (soc < 0 && ticks_until_connect < 0) {
			printf("Attempting connection... ");
			fflush(stdout);
			soc = net_client_connect("0.0.0.0", "6002");
			if (soc >= 0) {
				printf("Success!\n");
			}

			ticks_until_connect = ticks_between_connect;
		}
	}

	if (soc > 0) {
		close(soc);
	}

	drv->stop();
	drv->stopMotor();

	RPlidarDriver::DisposeDriver(drv);
}
