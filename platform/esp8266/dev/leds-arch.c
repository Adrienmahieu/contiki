/*
 * Copyright (c) 2005, Swedish Institute of Computer Science.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the Institute nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE INSTITUTE AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE INSTITUTE OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * This file is part of the Contiki operating system.
 *
 */

/**
 * \file
 *         A brief description of what this file is.
 * \author
 *         Adam Dunkels <adam@sics.se>
 */
#include "ets_sys.h"
#include "gpio.h"

#include "dev/leds.h"

#define GPIO_LED_GREEN 4
#define GPIO_LED_YELLOW 2
#define GPIO_LED_RED 0

static unsigned char leds;
/*---------------------------------------------------------------------------*/
void ICACHE_FLASH_ATTR
leds_arch_init(void)
{
  leds = 0;
  gpio_init();
  GPIO_OUTPUT_SET(GPIO_LED_GREEN, 0);
  GPIO_OUTPUT_SET(GPIO_LED_YELLOW, 0);
  GPIO_OUTPUT_SET(GPIO_LED_RED, 0);
}
/*---------------------------------------------------------------------------*/
unsigned char ICACHE_FLASH_ATTR
leds_arch_get(void)
{
  return leds;
}
/*---------------------------------------------------------------------------*/
void ICACHE_FLASH_ATTR
leds_arch_set(unsigned char l)
{
  leds = l;
  GPIO_OUTPUT_SET(GPIO_LED_GREEN, (l & LEDS_GREEN) ? 1 : 0);
  GPIO_OUTPUT_SET(GPIO_LED_YELLOW, (l & LEDS_YELLOW) ? 1 : 0);
  GPIO_OUTPUT_SET(GPIO_LED_RED, (l & LEDS_RED) ? 1 : 0);
}
/*---------------------------------------------------------------------------*/
