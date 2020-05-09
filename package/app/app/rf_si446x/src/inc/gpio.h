#ifndef __GPIO_H__
#define __GPIO_H__

#define GPIO_ROOT "/sys/class/gpio"
#define GPIO_EXPORT GPIO_ROOT "/export"
#define GPIO_UNEXPORT GPIO_ROOT "/unexport"
#define GPIO_DIRECTION GPIO_ROOT "/gpio%d/direction"
#define GPIO_EDGE GPIO_ROOT "/gpio%d/edge"
#define GPIO_ACTIVELOW GPIO_ROOT "/gpio%d/active_low"
#define GPIO_VALUE GPIO_ROOT"/gpio%d/value"



int gpio_write_value(const char *pathname, const char *buf, size_t count);
int gpio_is_requested(unsigned int gpio);
int gpio_request(unsigned int gpio);
int gpio_free(unsigned int gpio);
int gpio_direction_input(unsigned int gpio);
int gpio_direction_edge(unsigned int gpio, const char * edge);
int gpio_direction_output(unsigned int gpio, int value);
int gpio_get_value(unsigned int gpio);
int gpio_set_value(unsigned int gpio, int value);



#endif 
