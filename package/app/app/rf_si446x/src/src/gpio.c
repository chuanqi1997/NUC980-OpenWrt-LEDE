#include <stdint.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/types.h>
#include <linux/spi/spidev.h>
#include <pthread.h>
#include <poll.h>
#include "gpio.h"




int gpio_write_value(const char *pathname, const char *buf, size_t count)
{
	int fd;
	if ((fd = open(pathname, O_WRONLY)) == -1)
		return -1;
	if (write(fd, buf, count) != count){
		close(fd);
		return -1; 
	}
	return close(fd);
}
 
int gpio_is_requested(unsigned int gpio)
{
	int rv;
	char pathname[255];
	snprintf(pathname, sizeof(pathname), GPIO_VALUE, gpio);
	if ((rv = access(pathname, R_OK)) == -1)    
	return -1;
	return (rv == 0);
}
 
int gpio_request(unsigned int gpio)
{
	char buffer[16];
	snprintf(buffer, sizeof(buffer), "%d\n", gpio);
	return gpio_write_value(GPIO_EXPORT, buffer, strlen(buffer));
}
 
int gpio_free(unsigned int gpio)
{
	char buffer[16];
	snprintf(buffer, sizeof(buffer), "%d\n", gpio);
	return gpio_write_value(GPIO_UNEXPORT, buffer, strlen(buffer));
}
 
int gpio_direction_input(unsigned int gpio)
{
	char pathname[255];
	snprintf(pathname, sizeof(pathname), GPIO_DIRECTION, gpio);
	return gpio_write_value(pathname, "in", 3);
}

 int gpio_direction_edge(unsigned int gpio, const char * edge)
{
	char pathname[255];
	snprintf(pathname, sizeof(pathname), GPIO_EDGE, gpio);
	return gpio_write_value(pathname, edge, strlen(edge) + 1);
}

int gpio_direction_output(unsigned int gpio, int value)
{
	char pathname[255];
	char *val;
	snprintf(pathname, sizeof(pathname), GPIO_DIRECTION, gpio);
	val = value ? "high" : "low";
	return gpio_write_value(pathname, val, strlen(val) + 1);
}
 
int gpio_get_value(unsigned int gpio)
{
	int fd;
	char pathname[255];
	char buffer;
	snprintf(pathname, sizeof(pathname), GPIO_VALUE, gpio);
	if((fd = open(pathname, O_RDONLY)) == -1)
	        return -1;
	if (read(fd, &buffer, sizeof(buffer)) != sizeof(buffer))
	{
		close(fd);
		return -1;
	}
	if (close(fd) == -1)
	return -1;
	return buffer - '0';
}
 
int gpio_set_value(unsigned int gpio, int value)
{
	char pathname[255];
	snprintf(pathname, sizeof(pathname), GPIO_VALUE, gpio);
	return gpio_write_value(pathname, value ? "1" : "0", 2);
}


