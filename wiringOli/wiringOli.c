/*
 * wiringOli.cpp
 *
 *  Created on: 14 oct. 2013
 *      Author: denm
 */

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <poll.h>
#include <time.h>
#include <sys/time.h>
#include <ctype.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <pthread.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/ioctl.h>

#include "wiringOli.h"
#include "interrupt.h"

// Pin definition
// static int pinToGpio[128] =
// {
//   SUNXI_GPG(0), SUNXI_GPG(1), SUNXI_GPG(2), SUNXI_GPG(3), SUNXI_GPG(4), SUNXI_GPG(5),        // GPIO-1 (12 I/O pins)
//   SUNXI_GPG(6), SUNXI_GPG(7), SUNXI_GPG(8), SUNXI_GPG(9), SUNXI_GPG(10), SUNXI_GPG(11),
//   SUNXI_GPI(0), SUNXI_GPI(1), SUNXI_GPI(2), SUNXI_GPI(3), SUNXI_GPI(10), SUNXI_GPI(11),      // GPIO-2 (29 I/O pins)
//   SUNXI_GPI(14), SUNXI_GPI(15),
//   SUNXI_GPC(3), SUNXI_GPC(7), SUNXI_GPC(16), SUNXI_GPC(17), SUNXI_GPC(18), SUNXI_GPC(23),
//   SUNXI_GPC(24), SUNXI_GPE(0), SUNXI_GPE(1), SUNXI_GPE(2), SUNXI_GPE(3), SUNXI_GPE(4),
//   SUNXI_GPE(5), SUNXI_GPE(6), SUNXI_GPE(7), SUNXI_GPE(8), SUNXI_GPE(9), SUNXI_GPE(10),
//   SUNXI_GPE(11), SUNXI_GPI(14), SUNXI_GPI(15),
//   SUNXI_GPH(0), SUNXI_GPH(2), SUNXI_GPH(7), SUNXI_GPH(9), SUNXI_GPH(10), SUNXI_GPH(11),      // GPIO-3 (40 I/O pins)
//   SUNXI_GPH(12), SUNXI_GPH(13), SUNXI_GPH(14), SUNXI_GPH(15), SUNXI_GPH(16), SUNXI_GPH(17),
//   SUNXI_GPH(18), SUNXI_GPH(19), SUNXI_GPH(20), SUNXI_GPH(21), SUNXI_GPH(22), SUNXI_GPH(23),
//   SUNXI_GPH(24), SUNXI_GPH(25), SUNXI_GPH(26), SUNXI_GPH(27),
//   SUNXI_GPB(3), SUNXI_GPB(4), SUNXI_GPB(5), SUNXI_GPB(6), SUNXI_GPB(7), SUNXI_GPB(8),
//   SUNXI_GPB(10), SUNXI_GPB(11), SUNXI_GPB(12), SUNXI_GPB(13), SUNXI_GPB(14), SUNXI_GPB(15),
//   SUNXI_GPB(16), SUNXI_GPB(17), SUNXI_GPH(24), SUNXI_GPH(25), SUNXI_GPH(26), SUNXI_GPH(27),  
//   SUNXI_GPB(1), SUNXI_GPB(0),                                                                // GPIO-2 I2C0 - SDA, SCL
//   SUNXI_GPB(22), SUNXI_GPB(23),                                                              //        UART0 - Tx, Rx
//   SUNXI_GPI(12), SUNXI_GPI(13),                                                              // UEXT1  UART6 - Tx, Rx
//   SUNXI_GPB(21), SUNXI_GPB(20),                                                              //        I2C2 - SDA, SCL
//   SUNXI_GPC(21), SUNXI_GPC(22), SUNXI_GPC(20), SUNXI_GPC(19),                                //        SPI2 - MOSI, MISO, CLK, CS0
//   SUNXI_GPI(20), SUNXI_GPI(21),                                                              // UEXT2  UART7 - Tx, Rx
//   SUNXI_GPB(19), SUNXI_GPB(18),                                                              //        I2C1 - SDA, SCL
//   SUNXI_GPI(18), SUNXI_GPI(19), SUNXI_GPI(17), SUNXI_GPI(16),                                //        SPI1 - MOSI, MISO, CLK, CS0

// // Padding:

//   -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,                            // ... 111
//   -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,                                                    // ... 127
// };

static int sysFds [278] =
{
  -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
  -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
  -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
  -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
  -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
  -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
  -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
  -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
  -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
  -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
  -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
  -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
  -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
  -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
  -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
  -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
  -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
  -1, -1, -1, -1, -1, -1
} ;

static volatile int    pinPass = -1 ;
static volatile char    pinPassName[10] = {};
static pthread_mutex_t pinMutex ;

// ISR Data

static void (*isrFunctions [278])(void) ;

static int pinToGpio[278] =
{
  SUNXI_GPA(0), SUNXI_GPA(1), SUNXI_GPA(2), SUNXI_GPA(3), SUNXI_GPA(4), SUNXI_GPA(5),
  SUNXI_GPA(6), SUNXI_GPA(7), SUNXI_GPA(8), SUNXI_GPA(9), SUNXI_GPA(10), SUNXI_GPA(11),
  SUNXI_GPA(12), SUNXI_GPA(13), SUNXI_GPA(14), SUNXI_GPA(15), SUNXI_GPA(16), SUNXI_GPA(17),
  -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
  SUNXI_GPB(0), SUNXI_GPB(1), SUNXI_GPB(2), SUNXI_GPB(3), SUNXI_GPB(4), SUNXI_GPB(5),
  SUNXI_GPB(6), SUNXI_GPB(7), SUNXI_GPB(8), SUNXI_GPB(9), SUNXI_GPB(10), SUNXI_GPB(11),
  SUNXI_GPB(12), SUNXI_GPB(13), SUNXI_GPB(14), SUNXI_GPB(15), SUNXI_GPB(16), SUNXI_GPB(17),
  SUNXI_GPB(18), SUNXI_GPB(19), SUNXI_GPB(20), SUNXI_GPB(21), SUNXI_GPB(22), SUNXI_GPB(23),
  -1, -1, -1, -1, -1, -1, -1, -1,
  SUNXI_GPC(0), SUNXI_GPC(1), SUNXI_GPC(2), SUNXI_GPC(3), SUNXI_GPC(4), SUNXI_GPC(5),
  SUNXI_GPC(6), SUNXI_GPC(7), SUNXI_GPC(8), SUNXI_GPC(9), SUNXI_GPC(10), SUNXI_GPC(11),
  SUNXI_GPC(12), SUNXI_GPC(13), SUNXI_GPC(14), SUNXI_GPC(15), SUNXI_GPC(16), SUNXI_GPC(17),
  SUNXI_GPC(18), SUNXI_GPC(19), SUNXI_GPC(20), SUNXI_GPC(21), SUNXI_GPC(22), SUNXI_GPC(23),
  SUNXI_GPC(24),
  -1, -1, -1, -1, -1, -1, -1,
  SUNXI_GPD(0), SUNXI_GPD(1), SUNXI_GPD(2), SUNXI_GPD(3), SUNXI_GPD(4), SUNXI_GPD(5),
  SUNXI_GPD(6), SUNXI_GPD(7), SUNXI_GPD(8), SUNXI_GPD(9), SUNXI_GPD(10), SUNXI_GPD(11),
  SUNXI_GPD(12), SUNXI_GPD(13), SUNXI_GPD(14), SUNXI_GPD(15), SUNXI_GPD(16), SUNXI_GPD(17),
  SUNXI_GPD(18), SUNXI_GPD(19), SUNXI_GPD(20), SUNXI_GPD(21), SUNXI_GPD(22), SUNXI_GPD(23),
  SUNXI_GPD(24), SUNXI_GPD(25), SUNXI_GPD(26), SUNXI_GPD(27),
  -1, -1, -1, -1,
  SUNXI_GPE(0), SUNXI_GPE(1), SUNXI_GPE(2), SUNXI_GPE(3), SUNXI_GPE(4), SUNXI_GPE(5),
  SUNXI_GPE(6), SUNXI_GPE(7), SUNXI_GPE(8), SUNXI_GPE(9), SUNXI_GPE(10), SUNXI_GPE(11),
  -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
  SUNXI_GPF(0), SUNXI_GPF(1), SUNXI_GPF(2), SUNXI_GPF(3), SUNXI_GPF(4), SUNXI_GPF(5),
  -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
  -1, -1, -1, -1, -1, -1,
  SUNXI_GPG(0), SUNXI_GPG(1), SUNXI_GPG(2), SUNXI_GPG(3), SUNXI_GPG(4), SUNXI_GPG(5),
  SUNXI_GPG(6), SUNXI_GPG(7), SUNXI_GPG(8), SUNXI_GPG(9), SUNXI_GPG(10), SUNXI_GPG(11),
  -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
  SUNXI_GPH(0), SUNXI_GPH(1), SUNXI_GPH(2), SUNXI_GPH(3), SUNXI_GPH(4), SUNXI_GPH(5),
  SUNXI_GPH(6), SUNXI_GPH(7), SUNXI_GPH(8), SUNXI_GPH(9), SUNXI_GPH(10), SUNXI_GPH(11),
  SUNXI_GPH(12), SUNXI_GPH(13), SUNXI_GPH(14), SUNXI_GPH(15), SUNXI_GPH(16), SUNXI_GPH(17),
  SUNXI_GPH(18), SUNXI_GPH(19), SUNXI_GPH(20), SUNXI_GPH(21), SUNXI_GPH(22), SUNXI_GPH(23),
  SUNXI_GPH(24), SUNXI_GPH(25), SUNXI_GPH(26), SUNXI_GPH(27),
  -1, -1, -1, -1,
  SUNXI_GPI(0), SUNXI_GPI(1), SUNXI_GPI(2), SUNXI_GPI(3), SUNXI_GPI(4), SUNXI_GPI(5),
  SUNXI_GPI(6), SUNXI_GPI(7), SUNXI_GPI(8), SUNXI_GPI(9), SUNXI_GPI(10), SUNXI_GPI(11),
  SUNXI_GPI(12), SUNXI_GPI(13), SUNXI_GPI(14), SUNXI_GPI(15), SUNXI_GPI(16), SUNXI_GPI(17),
  SUNXI_GPI(18), SUNXI_GPI(19), SUNXI_GPI(20), SUNXI_GPI(21)
};

// Translation between pin number and gpio number from fex
// static int convPinToGpio[81] = {55, 56, 57, 58, 59, 60,  0,  0,  0,  0, 
//                                  0,  0, 61, 62,  0,  0,  0, 53, 49, 50, 
//                                 51, 52, 54,  0,  0,  0,  0, 37, 38, 39,
//                                 40, 41, 42, 43, 44, 45, 46, 47, 48,  0,
//                                  0,  1,  2,  3,  4,  5,  6,  7,  8,  9,
//                                 10, 11, 12, 13, 14, 15, 16, 31, 32,  0,
//                                  0,  0,  0, 17, 18, 19, 20, 21, 22, 23,
//                                 24, 25, 26, 27, 28, 29, 30, 33, 34, 35,
//                                 36};

// sysfs gpio translation (w.r.t. Igor's oA20-micro debian 4.0.4)
static int convPinToGpio[278] = {  0, 1 , 2 , 3 , 4 , 5 , 6 , 7 , 8 , 9 , 10 ,
                                  11 , 12 , 13 , 14 , 15 , 16 , 17 , 18 , 19 , 20 ,
                                  21 , 22 , 23 , 24 , 25 , 26 , 27 , 28 , 29 , 30 ,
                                  31 , 32 , 33 , 34 , 35 , 36 , 37 , 38 , 39 , 40 ,
                                  41 , 42 , 43 , 44 , 45 , 46 , 47 , 48 , 49 , 50 ,
                                  51 , 52 , 53 , 54 , 55 , 56 , 57 , 58 , 59 , 60 ,
                                  61 , 62 , 63 , 64 , 65 , 66 , 67 , 68 , 69 , 70 ,
                                  71 , 72 , 73 , 74 , 75 , 76 , 77 , 78 , 79 , 80 ,
                                  81 , 82 , 83 , 84 , 85 , 86 , 87 , 88 , 89 , 90 ,
                                  91 , 92 , 93 , 94 , 95 , 96 , 97 , 98 , 99 , 100 ,
                                  101 , 102 , 103 , 104 , 105 , 106 , 107 , 108 , 109 , 110 , 
                                  111 , 112 , 113 , 114 , 115 , 116 , 117 , 118 , 119 , 120 ,
                                  121 , 122 , 123 , 124 , 125 , 126 , 127 , 128 , 129 , 130 ,
                                  131 , 132 , 133 , 134 , 135 , 136 , 137 , 138 , 139 , 140 ,
                                  141 , 142 , 143 , 144 , 145 , 146 , 147 , 148 , 149 , 150 ,
                                  151 , 152 , 153 , 154 , 155 , 156 , 157 , 158 , 159 , 160 ,
                                  161 , 162 , 163 , 164 , 165 , 166 , 167 , 168 , 169 , 170 ,
                                  171 , 172 , 173 , 174 , 175 , 176 , 177 , 178 , 179 , 180 ,
                                  181 , 182 , 183 , 184 , 185 , 186 , 187 , 188 , 189 , 190 ,
                                  191 , 192 , 193 , 194 , 195 , 196 , 197 , 198 , 199 , 200 ,
                                  201 , 202 , 203 , 204 , 205 , 206 , 207 , 208 , 209 , 210 ,
                                  211 , 212 , 213 , 214 , 215 , 216 , 217 , 218 , 219 , 220 ,
                                  221 , 222 , 223 , 224 , 225 , 226 , 227 , 228 , 229 , 230 ,
                                  231 , 232 , 233 , 234 , 235 , 236 , 237 , 238 , 239 , 240 ,
                                  241 , 242 , 243 , 244 , 245 , 246 , 247 , 248 , 249 , 250 ,
                                  251 , 252 , 253 , 254 , 255 , 256 , 257 , 258 , 259 , 260 ,
                                  261 , 262 , 263 , 264 , 265 , 266 , 267 , 268 , 269 , 270 ,
                                  271 , 272 , 273 , 274 , 275 , 276 , 277 };

// Time for easy calculations
static uint64_t epochMilli, epochMicro;

/*
 * pinWiringOli:
 *	Get board pin corresponding to wiringOli pin numbering
 *********************************************************************************
 */
int pinWiringOli(int pin)
{
  // To be sure to have a correct pin number
  // return pinToGpio[pin & 127];
  return pinToGpio[pin];
}

/*
 * pinGpio:
 *	Get GPIO pin from fex to corresponding board pin
 *********************************************************************************
 */
int pinGpio(int pin)
{
  // Check we have the good range
  if ((pin < 0) || (pin > 81))
  {
    return 0;
  }
  return convPinToGpio[pin];
}

/*
 * pinMode:
 *	Sets the mode of a pin to be INPUT, OUTPUT
 *********************************************************************************
 */
void pinMode(int pin, int mode)
{
  sunxi_gpio_set_cfgpin(pinWiringOli(pin), mode);
}

/*
 * pullUpDownCtrl:
 *	Control the internal pull-up/down resistors on a GPIO pin
 *      pud=0 -> pull disable 1 -> pull-up 2 -> pull-down
 *********************************************************************************
 */

void pullUpDnControlGpio(int pin, int pud)
{
  sunxi_gpio_set_pull(pinWiringOli(pin), pud);
}

/*
 * digitalRead:
 *	Read the value of a given Pin, returning HIGH or LOW
 *********************************************************************************
 */
int digitalRead(int pin)
{
  return(sunxi_gpio_input(pinWiringOli(pin)));
}

/*
 * digitalWrite:
 *	Set an output bit
 *********************************************************************************
 */
void digitalWrite(int pin, int value)
{
  sunxi_gpio_output(pinWiringOli(pin), value);
}

/*
 * waitForInterrupt:
 *	Wait for interrupt on a GPIO pin
 *********************************************************************************
 */
int waitForInterrupt(int pin, int mS)
{
  struct pollfd polls;
  int fd, rc;
  uint8_t c ;

  // Get pin A20 number
  // pin = pinGpio(pin);

  // gpio_export(pin);
  // gpio_set_dir(pin, 0);
  // // Set interrupt edge, can be "none", "rising", "falling", or "both"
  // gpio_set_edge(pin, "rising");
  // fd = gpio_fd_open(pin);
  if ((fd = sysFds [pin]) == -1)
    return -2 ;
  // printf("Waiting on fd: %d of pin: %d\n", fd, pin);
	polls.fd = fd;
	polls.events = POLLPRI;

	rc = poll(&polls, 1, mS);      

	(void)read(polls.fd, &c, 1);
  lseek (fd, 0, SEEK_SET) ;

	// gpio_fd_close(fd);
	return rc;
}


// int piHiPri (const int pri)
// {
//   struct sched_param sched ;

//   memset (&sched, 0, sizeof(sched)) ;

//   if (pri > sched_get_priority_max (SCHED_RR))
//     sched.sched_priority = sched_get_priority_max (SCHED_RR) ;
//   else
//     sched.sched_priority = pri ;

//   return sched_setscheduler (0, SCHED_RR, &sched) ;
// }

/*
 * interruptHandler:
 *  This is a thread and gets started to wait for the interrupt we're
 *  hoping to catch. It will call the user-function when the interrupt
 *  fires.
 *********************************************************************************
 */

static void *interruptHandler (void *arg)
{
  int myPin ;

  (void)oliHiPri (55) ;  // Only effective if we run as root

  myPin   = pinPass ;
  pinPass = -1 ;

  for (;;)
    if (waitForInterrupt (myPin, -1) > 0) {
      // printf("Calling isr\n");
      isrFunctions [myPin] () ;
    }

  return NULL ;
}

int wiringOliISR (int pin, char *pinName, int mode, void (*function)(void))
{
  pthread_t threadId ;
  pthread_attr_t attr;
  char *modeS ;
  char fName   [64] ;
//  char  pinS [8] ;
//  pid_t pid ;
  int   count, i ;
  char  c ;
  int   bcmGpioPin ;
  // char pinName[10] = {};

  bcmGpioPin = pinGpio(pin) ;

  if (mode != INT_EDGE_SETUP)
  {
    /**/ if (mode == INT_EDGE_FALLING)
      modeS = "falling" ;
    else if (mode == INT_EDGE_RISING)
      modeS = "rising" ;
    else
      modeS = "both" ;

    // There's really no need to fork now,
    // as we're no longer using gpio utility here!
    // if ((pid = fork ()) < 0) {
    //   printf("Failed to fork!\n");
    //   return -1;
    // }

    // if (pid == 0) // Child, exec
    // {
      gpio_export(bcmGpioPin);
      gpio_set_dir(bcmGpioPin, 0, pinName);
      gpio_set_edge(bcmGpioPin, modeS, pinName);
      // sysFds [bcmGpioPin] = gpio_fd_open(bcmGpioPin, pinName);
    // }
    // else    // Parent, wait
    //   wait (NULL) ;
  }

  if (sysFds [bcmGpioPin] == -1)
  {
    sprintf (fName, "/sys/class/gpio/gpio%d%s/value", bcmGpioPin, pinName) ;
    // printf("Opening %s for ISR!\n", fName);
    // if ((sysFds [bcmGpioPin] = open (fName, O_RDWR)) < 0) {
    if ((sysFds [bcmGpioPin] = gpio_fd_open (bcmGpioPin, pinName)) < 0) {
      printf("Failed to gpio_fd_open!\n");
      return -1;
    }
  }

// Clear any initial pending interrupt

  ioctl (sysFds [bcmGpioPin], FIONREAD, &count) ;
  for (i = 0 ; i < count ; ++i)
    read (sysFds [bcmGpioPin], &c, 1) ;

  isrFunctions [bcmGpioPin] = function ;
  pinPass = bcmGpioPin ;
  strcpy(pinPassName, pinName);

  pthread_attr_init(&attr);
  pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
  pthread_mutex_lock (&pinMutex) ;

  pthread_create (&threadId, &attr, interruptHandler, NULL) ;
  while (pinPass != -1)
    delay (1) ;

  pthread_mutex_unlock (&pinMutex) ;
  pthread_attr_destroy ( &attr );

  return 0 ;
}

/*
 * initialiseEpoch:
 *	Initialise our start-of-time variable to be the current unix
 *	time in milliseconds and microseconds.
 *********************************************************************************
 */

static void initialiseEpoch()
{
  struct timeval tv ;

  gettimeofday (&tv, NULL) ;
  epochMilli = (uint64_t)tv.tv_sec * (uint64_t)1000    + (uint64_t)(tv.tv_usec / 1000) ;
  epochMicro = (uint64_t)tv.tv_sec * (uint64_t)1000000 + (uint64_t)(tv.tv_usec) ;
}

/*
 * delay:
 *	Wait for some number of milliseconds
 *********************************************************************************
 */

void delay(unsigned int howLong)
{
  struct timespec sleeper, dummy ;

  sleeper.tv_sec  = (time_t)(howLong / 1000) ;
  sleeper.tv_nsec = (long)(howLong % 1000) * 1000000 ;

  nanosleep (&sleeper, &dummy) ;
}

void delayMicrosecondsHard(unsigned int howLong)
{
  struct timeval tNow, tLong, tEnd ;

  gettimeofday (&tNow, NULL) ;
  tLong.tv_sec  = howLong / 1000000 ;
  tLong.tv_usec = howLong % 1000000 ;
  timeradd (&tNow, &tLong, &tEnd) ;

  while (timercmp (&tNow, &tEnd, <))
    gettimeofday (&tNow, NULL) ;
}

void delayMicroseconds(unsigned int howLong)
{
  struct timespec sleeper ;
  unsigned int uSecs = howLong % 1000000 ;
  unsigned int wSecs = howLong / 1000000 ;

  /**/ if (howLong ==   0)
    return ;
  else if (howLong  < 100)
    delayMicrosecondsHard (howLong) ;
  else
  {
    sleeper.tv_sec  = wSecs ;
    sleeper.tv_nsec = (long)(uSecs * 1000L) ;
    nanosleep (&sleeper, NULL) ;
  }
}

/*
 * millis:
 *	Return a number of milliseconds as an unsigned int.
 *********************************************************************************
 */

unsigned int millis()
{
  struct timeval tv;
  uint64_t now;

  gettimeofday(&tv, NULL);
  now  = (uint64_t)tv.tv_sec * (uint64_t)1000 + (uint64_t)(tv.tv_usec / 1000);

  return (uint32_t)(now - epochMilli);
}


/*
 * micros:
 *	Return a number of microseconds as an unsigned int.
 *********************************************************************************
 */

unsigned int micros()
{
  struct timeval tv ;
  uint64_t now ;

  gettimeofday (&tv, NULL) ;
  now  = (uint64_t)tv.tv_sec * (uint64_t)1000000 + (uint64_t)tv.tv_usec ;

  return (uint32_t)(now - epochMicro) ;
}

/*
 * wiringOliSetup:
 *	Must be called once at the start of your program execution.
 *
 * Default setup: Initialises the system into wiringOli Pin mode and uses the
 *	memory mapped hardware directly.
 *********************************************************************************
 */

int wiringOliSetup()
{

  initialiseEpoch() ;
  int result;
  result = sunxi_gpio_init();
  if(result == SETUP_DEVMEM_FAIL) 
  {
    printf("No access to /dev/mem. Try running as root !");
    return SETUP_DEVMEM_FAIL;
  }
  else if(result == SETUP_MALLOC_FAIL) 
  {
    printf("No memory !");
    return SETUP_MALLOC_FAIL;
  }
  else if(result == SETUP_MMAP_FAIL) 
  {
    printf("Mmap failed on module import");
    return SETUP_MMAP_FAIL;
  }
  else 
  {
    return SETUP_OK;
  }
return SETUP_OK;
}
