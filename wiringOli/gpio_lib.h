/*
 * 
 * This file is part of pyA20.
 * gpio_lib.h is python GPIO extension.
 * 
 * Copyright (c) 2014 Stefan Mavrodiev @ OLIMEX LTD, <support@olimex.com> 
 * 
 * pyA20 is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 */

#ifndef _GPIO_LIB_H_
#define _GPIO_LIB_H_

#define SW_PORTC_IO_BASE 0x01c20800

/*Macros imported from pyA20*/
/*----------Start-----------*/
// Initialize return status 
#define SETUP_OK            0
#define SETUP_DEVMEM_FAIL   1
#define SETUP_MALLOC_FAIL   2
#define SETUP_MMAP_FAIL     3

// PIN Defs
// Header 1
#define PIN_PG0     SUNXI_GPG(0)
#define PIN_PG1     SUNXI_GPG(1)
#define PIN_PG2     SUNXI_GPG(2)
#define PIN_PG3     SUNXI_GPG(3)
#define PIN_PG4     SUNXI_GPG(4)
#define PIN_PG5     SUNXI_GPG(5)
#define PIN_PG6     SUNXI_GPG(6)
#define PIN_PG7     SUNXI_GPG(7)
#define PIN_PG8     SUNXI_GPG(8)
#define PIN_PG9     SUNXI_GPG(9)
#define PIN_PG10    SUNXI_GPG(10)
#define PIN_PG11    SUNXI_GPG(11)

// Header 2
#define PIN_PE0     SUNXI_GPE(0)
#define PIN_PE1     SUNXI_GPE(1)
#define PIN_PI0     SUNXI_GPI(0)
#define PIN_PE2     SUNXI_GPE(2)
#define PIN_PI1     SUNXI_GPI(1)
#define PIN_PE3     SUNXI_GPE(3)
#define PIN_PI2     SUNXI_GPI(2)
#define PIN_PE4     SUNXI_GPE(4)
#define PIN_PI3     SUNXI_GPI(3)
#define PIN_PE5     SUNXI_GPE(5)
#define PIN_PI10    SUNXI_GPI(10)
#define PIN_PE6     SUNXI_GPE(6)
#define PIN_PI11    SUNXI_GPI(11)
#define PIN_PE7     SUNXI_GPE(7)
#define PIN_PC3     SUNXI_GPC(3)
#define PIN_PE8     SUNXI_GPE(8)
#define PIN_PC7     SUNXI_GPC(7)
#define PIN_PE9     SUNXI_GPE(9)
#define PIN_PC16    SUNXI_GPC(16)
#define PIN_PE10    SUNXI_GPE(10)
#define PIN_PC17    SUNXI_GPC(17)
#define PIN_PE11    SUNXI_GPE(11)
#define PIN_PC18    SUNXI_GPC(18)
#define PIN_PI14    SUNXI_GPI(14)
#define PIN_PC23    SUNXI_GPC(23)
#define PIN_PI15    SUNXI_GPI(15)
#define PIN_PC24    SUNXI_GPC(24)
#define PIN_PB23    SUNXI_GPB(23)
#define PIN_PB22    SUNXI_GPB(22)

// Header 3
#define PIN_PH0     SUNXI_GPH(0)
#define PIN_PB3     SUNXI_GPB(3)
#define PIN_PH2     SUNXI_GPH(2)
#define PIN_PB4     SUNXI_GPB(4)
#define PIN_PH7     SUNXI_GPH(7)
#define PIN_PB5     SUNXI_GPB(5)
#define PIN_PH9     SUNXI_GPH(9)
#define PIN_PB6     SUNXI_GPB(6)
#define PIN_PH10    SUNXI_GPH(10)
#define PIN_PB7     SUNXI_GPB(7)
#define PIN_PH11    SUNXI_GPH(11)
#define PIN_PB8     SUNXI_GPB(8)
#define PIN_PH12    SUNXI_GPH(12)
#define PIN_PB10    SUNXI_GPB(10)
#define PIN_PH13    SUNXI_GPH(13)
#define PIN_PB11    SUNXI_GPB(11)
#define PIN_PH14    SUNXI_GPH(14)
#define PIN_PB12    SUNXI_GPB(12)
#define PIN_PH15    SUNXI_GPH(15)
#define PIN_PB13    SUNXI_GPB(13)
#define PIN_PH16    SUNXI_GPH(16)
#define PIN_PB14    SUNXI_GPB(14)
#define PIN_PH17    SUNXI_GPH(17)
#define PIN_PB15    SUNXI_GPB(15)
#define PIN_PH18    SUNXI_GPH(18)
#define PIN_PB16    SUNXI_GPB(16)
#define PIN_PH19    SUNXI_GPH(19)
#define PIN_PB17    SUNXI_GPB(17)
#define PIN_PH20    SUNXI_GPH(20)
#define PIN_PH24    SUNXI_GPH(24)
#define PIN_PH21    SUNXI_GPH(21)
#define PIN_PH25    SUNXI_GPH(25)
#define PIN_PH22    SUNXI_GPH(22)
#define PIN_PH26    SUNXI_GPH(26)
#define PIN_PH23    SUNXI_GPH(23)
#define PIN_PH27    SUNXI_GPH(27)

// Header 4
#define PIN_PD16    SUNXI_GPD(16)
#define PIN_PD17    SUNXI_GPD(17)
#define PIN_PD18    SUNXI_GPD(18)
#define PIN_PD19    SUNXI_GPD(19)
#define PIN_PD20    SUNXI_GPD(20)
#define PIN_PD21    SUNXI_GPD(21)
#define PIN_PD22    SUNXI_GPD(22)
#define PIN_PD23    SUNXI_GPD(23)
#define PIN_PD8     SUNXI_GPD(8)
#define PIN_PD9     SUNXI_GPD(9)
#define PIN_PD10    SUNXI_GPD(10)
#define PIN_PD11    SUNXI_GPD(11)
#define PIN_PD12    SUNXI_GPD(12)
#define PIN_PD13    SUNXI_GPD(13)
#define PIN_PD14    SUNXI_GPD(14)
#define PIN_PD15    SUNXI_GPD(15)
#define PIN_PD0     SUNXI_GPD(0)
#define PIN_PD1     SUNXI_GPD(1)
#define PIN_PD2     SUNXI_GPD(2)
#define PIN_PD3     SUNXI_GPD(3)
#define PIN_PD4     SUNXI_GPD(4)
#define PIN_PD5     SUNXI_GPD(5)
#define PIN_PD6     SUNXI_GPD(6)
#define PIN_PD7     SUNXI_GPD(7)
#define PIN_PD26    SUNXI_GPD(26)
#define PIN_PD27    SUNXI_GPD(27)
#define PIN_PD24    SUNXI_GPD(24)
#define PIN_PD25    SUNXI_GPD(25)
#define PIN_PB3     SUNXI_GPB(3)
#define PIN_PB4     SUNXI_GPB(4)
#define PIN_PH8     SUNXI_GPH(8)
#define PIN_PB2     SUNXI_GPB(2)

/*-----------End------------*/
/*Macros imported from pyA20*/

#define SUNXI_GPIO_A	0
#define SUNXI_GPIO_B	1
#define SUNXI_GPIO_C	2
#define SUNXI_GPIO_D	3
#define SUNXI_GPIO_E	4
#define SUNXI_GPIO_F	5
#define SUNXI_GPIO_G	6
#define SUNXI_GPIO_H	7
#define SUNXI_GPIO_I	8

struct sunxi_gpio {
    unsigned int cfg[4];
    unsigned int dat;
    unsigned int drv[2];
    unsigned int pull[2];
};

/* gpio interrupt control */
struct sunxi_gpio_int {
    unsigned int cfg[3];
    unsigned int ctl;
    unsigned int sta;
    unsigned int deb;
};

struct sunxi_gpio_reg {
    struct sunxi_gpio gpio_bank[9];
    unsigned char res[0xbc];
    struct sunxi_gpio_int gpio_int;
};

#define GPIO_BANK(pin)	((pin) >> 5)
#define GPIO_NUM(pin)	((pin) & 0x1F)

#define GPIO_CFG_INDEX(pin)	(((pin) & 0x1F) >> 3)
#define GPIO_CFG_OFFSET(pin)	((((pin) & 0x1F) & 0x7) << 2)

#define GPIO_PUL_INDEX(pin)	(((pin) & 0x1F )>> 4) 
#define GPIO_PUL_OFFSET(pin)	(((pin) & 0x0F) << 1)

/* GPIO bank sizes */
#define SUNXI_GPIO_A_NR		(32)
#define SUNXI_GPIO_B_NR		(32)
#define SUNXI_GPIO_C_NR		(32)
#define SUNXI_GPIO_D_NR		(32)
#define SUNXI_GPIO_E_NR		(32)
#define SUNXI_GPIO_F_NR		(32)
#define SUNXI_GPIO_G_NR		(32)
#define SUNXI_GPIO_H_NR		(32)
#define SUNXI_GPIO_I_NR		(32)

#define SUNXI_GPIO_NEXT(__gpio) ((__gpio##_START)+(__gpio##_NR)+0)

enum sunxi_gpio_number {
    SUNXI_GPIO_A_START = 0,
    SUNXI_GPIO_B_START = SUNXI_GPIO_NEXT(SUNXI_GPIO_A), //32
    SUNXI_GPIO_C_START = SUNXI_GPIO_NEXT(SUNXI_GPIO_B), //64
    SUNXI_GPIO_D_START = SUNXI_GPIO_NEXT(SUNXI_GPIO_C), //96
    SUNXI_GPIO_E_START = SUNXI_GPIO_NEXT(SUNXI_GPIO_D), //128
    SUNXI_GPIO_F_START = SUNXI_GPIO_NEXT(SUNXI_GPIO_E), //160
    SUNXI_GPIO_G_START = SUNXI_GPIO_NEXT(SUNXI_GPIO_F), //192
    SUNXI_GPIO_H_START = SUNXI_GPIO_NEXT(SUNXI_GPIO_G), //224	
    SUNXI_GPIO_I_START = SUNXI_GPIO_NEXT(SUNXI_GPIO_H) //256
};

/* SUNXI GPIO number definitions */
#define SUNXI_GPA(_nr) (SUNXI_GPIO_A_START + (_nr))
#define SUNXI_GPB(_nr) (SUNXI_GPIO_B_START + (_nr))
#define SUNXI_GPC(_nr) (SUNXI_GPIO_C_START + (_nr))
#define SUNXI_GPD(_nr) (SUNXI_GPIO_D_START + (_nr))
#define SUNXI_GPE(_nr) (SUNXI_GPIO_E_START + (_nr))
#define SUNXI_GPF(_nr) (SUNXI_GPIO_F_START + (_nr))
#define SUNXI_GPG(_nr) (SUNXI_GPIO_G_START + (_nr))
#define SUNXI_GPH(_nr) (SUNXI_GPIO_H_START + (_nr))
#define SUNXI_GPI(_nr) (SUNXI_GPIO_I_START + (_nr))

/* GPIO pin function config */
#define SUNXI_GPIO_INPUT (0)
#define SUNXI_GPIO_OUTPUT (1)
#define SUNXI_GPIO_PER (2)

#define SUNXI_PULL_NONE (0)
#define SUNXI_PULL_UP (1)
#define SUNXI_PULL_DOWN (2)

extern int sunxi_gpio_input(unsigned int pin);
extern int sunxi_gpio_init(void);
extern int sunxi_gpio_set_cfgpin(unsigned int pin, unsigned int val);
extern int sunxi_gpio_get_cfgpin(unsigned int pin);
extern int sunxi_gpio_output(unsigned int pin, unsigned int val);
extern int sunxi_gpio_set_pull(unsigned int pin, unsigned int pull);


extern unsigned int SUNXI_PIO_BASE;
#endif // _GPIO_LIB_H_ 
