/* Copyright (c) 2011, RidgeRun
* All rights reserved.
* 
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
* 1. Redistributions of source code must retain the above copyright
*    notice, this list of conditions and the following disclaimer.
* 2. Redistributions in binary form must reproduce the above copyright
*    notice, this list of conditions and the following disclaimer in the
*    documentation and/or other materials provided with the distribution.
* 3. All advertising materials mentioning features or use of this software
*    must display the following acknowledgement:
*    This product includes software developed by the RidgeRun.
* 4. Neither the name of the RidgeRun nor the
*    names of its contributors may be used to endorse or promote products
*    derived from this software without specific prior written permission.
* 
* THIS SOFTWARE IS PROVIDED BY RIDGERUN ''AS IS'' AND ANY
* EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
* DISCLAIMED. IN NO EVENT SHALL RIDGERUN BE LIABLE FOR ANY
* DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
* LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
* ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
* SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <poll.h>
#include <time.h>
#include <stdint.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sched.h>
#include <sys/ioctl.h>

// #if defined (WITH_ADC) && ! defined (USE_SYSFS_GPIO)
#ifndef USE_SYSFS_GPIO
// #include <wiringOli.h>
#define USE_SYSFS_GPIO		// You may want to remove that!
#endif

#include "interrupt.h"
#include <sys/time.h>
/****************************************************************
 * Constants
 ****************************************************************/
 
#define SYSFS_GPIO_DIR 		"/sys/class/gpio"
#define MAX_BUF 			64

#ifdef USE_SYSFS_GPIO
#define POLL_TIMEOUT (3 * 1000) /* 3 seconds */
// Time for easy calculations
static uint64_t epochMilli, epochMicro;

static int pinPass = -1;
static char pinPassName[10] = {};
static void (*callback)(void) = NULL;
static int gpio_fd;
#endif

/****************************************************************
 * gpio_export
 ****************************************************************/
int gpio_export(unsigned int gpio)
{
#ifdef USE_SYSFS_GPIO
	int fd, len;
	char buf[MAX_BUF];
 
	fd = open(SYSFS_GPIO_DIR "/export", O_WRONLY);
	if (fd < 0) {
		perror("gpio/export");
		return fd;
	}
 
	len = snprintf(buf, sizeof(buf), "%d", gpio);
	write(fd, buf, len);
	close(fd);
	return len;
#else
	return 0;
#endif
}

/****************************************************************
 * gpio_unexport
 ****************************************************************/
int gpio_unexport(unsigned int gpio)
{
#ifdef USE_SYSFS_GPIO
	int fd, len;
	char buf[MAX_BUF];
 
	fd = open(SYSFS_GPIO_DIR "/unexport", O_WRONLY);
	if (fd < 0) {
		perror("gpio/export");
		return fd;
	}
 
	len = snprintf(buf, sizeof(buf), "%d", gpio);
	write(fd, buf, len);
	close(fd);
	return len;
#else
	return 0;
#endif
}

/****************************************************************
 * gpio_set_dir
 ****************************************************************/
int gpio_set_dir(unsigned int gpio, unsigned int out_flag, char *pinName)
{
#ifdef USE_SYSFS_GPIO
	int fd, len;
	char buf[MAX_BUF];

	len = snprintf(buf, sizeof(buf), SYSFS_GPIO_DIR "/gpio%d%s/direction", gpio, pinName);
 
	fd = open(buf, O_WRONLY);
	if (fd < 0) {
		perror("gpio/set-dir");
		printf("%s\n", buf);
		return fd;
	}
 
	if (out_flag)
		write(fd, "out", 4);
	else
		write(fd, "in", 3);
 
	close(fd);
	return len;
#else
	pinMode(gpio, out_flag);
	return 0;
#endif
}

/****************************************************************
 * gpio_set_act_low
 ****************************************************************/
int gpio_set_act_low(unsigned int gpio, unsigned int low_flag, char *pinName)
{
#ifdef USE_SYSFS_GPIO
	int fd, len;
	char buf[MAX_BUF];

	len = snprintf(buf, sizeof(buf), SYSFS_GPIO_DIR "/gpio%d%s/active_low", gpio, pinName);
 
	fd = open(buf, O_WRONLY);
	if (fd < 0) {
		perror("gpio/set-act-low");
		printf("%s\n", buf);
		return fd;
	}
 
	if (low_flag)
		write(fd, "1", 2);
	else
		write(fd, "0", 2);
 
	close(fd);
	return len;
#else
	return 0;
#endif
}

/****************************************************************
 * gpio_set_value
 ****************************************************************/
int gpio_set_value(unsigned int gpio, unsigned int value, char *pinName)
{
#ifdef USE_SYSFS_GPIO
	int fd, len;
	char buf[MAX_BUF];
 
	len = snprintf(buf, sizeof(buf), SYSFS_GPIO_DIR "/gpio%d%s/value", gpio, pinName);
 
	fd = open(buf, O_WRONLY);
	if (fd < 0) {
		printf("Error: Couldn't set gpio%d value", gpio);
		perror("gpio/set-value");
		printf("%s\n", buf);
		return fd;
	}
 
	if (value)
		write(fd, "1", 2);
	else
		write(fd, "0", 2);
 
	close(fd);
	//usleep(100);
	return len;
#else
	digitalWrite(gpio, value);
	return 0;
#endif
}

/****************************************************************
 * gpio_get_value
 ****************************************************************/
int gpio_get_value(unsigned int gpio, volatile unsigned int *value, char *pinName)
{
#ifdef USE_SYSFS_GPIO
	int fd, len;
	char buf[MAX_BUF];
	char ch;

	len = snprintf(buf, sizeof(buf), SYSFS_GPIO_DIR "/gpio%d%s/value", gpio, pinName);
 
	fd = open(buf, O_RDONLY);
	if (fd < 0) {
		printf("Error: Couldn't get gpio%d value", gpio);
		perror("gpio/get-value");
		printf("%s\n", buf);
		return fd;
	}
 
	read(fd, &ch, 1);

	if (ch != '0') {
		*value = 1;
	} else {
		*value = 0;
	}
 
	close(fd);
	//usleep(100);
	return len;
#else
	*value = digitalRead(gpio);
	return 0;
#endif
}


/****************************************************************
 * gpio_set_edge
 ****************************************************************/

int gpio_set_edge(unsigned int gpio, char *edge, char *pinName)
{
#ifdef USE_SYSFS_GPIO
	int fd, len;
	char buf[MAX_BUF];

	len = snprintf(buf, sizeof(buf), SYSFS_GPIO_DIR "/gpio%d%s/edge", gpio, pinName);
 
	fd = open(buf, O_WRONLY);
	if (fd < 0) {
		perror("gpio/set-edge");
		printf("%s\n", buf);
		return fd;
	}
 
	write(fd, edge, strlen(edge) + 1); 
	close(fd);
	return len;
#else
	return 0;
#endif
}

/****************************************************************
 * gpio_fd_open
 ****************************************************************/

int gpio_fd_open(unsigned int gpio, char *pinName)
{
#ifdef USE_SYSFS_GPIO
	int fd;
	char buf[MAX_BUF];

	snprintf(buf, sizeof(buf), SYSFS_GPIO_DIR "/gpio%d%s/value", gpio, pinName);
 
	fd = open(buf, O_RDONLY | O_NONBLOCK );
	if (fd < 0) {
		perror("gpio/fd_open");
		printf("%s\n", buf);
	}
	return fd;
#else
	return 0;
#endif
}

/****************************************************************
 * gpio_fd_close
 ****************************************************************/

int gpio_fd_close(int fd)
{
#ifdef USE_SYSFS_GPIO
	return close(fd);
#else
	return 0;
#endif
}
