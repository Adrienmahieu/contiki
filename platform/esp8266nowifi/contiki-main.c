/*
 * Copyright (c) 2002, Adam Dunkels.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above
 *    copyright notice, this list of conditions and the following
 *    disclaimer in the documentation and/or other materials provided
 *    with the distribution.
 * 3. The name of the author may not be used to endorse or promote
 *    products derived from this software without specific prior
 *    written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
 * GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * This file is part of the Contiki OS
 *
 */

/**
 * \ingroup platform
 *
 * \defgroup native_platform Native platform
 *
 * Platform running in the host (Windows or Linux) environment.
 *
 * Used mainly for development and debugging.
 * @{
 */
#include "ets_sys.h"
#include "osapi.h"
#include "user_interface.h"
#include "gpio.h"
#include "os_type.h"
#include "espmissingincludes.h"

#include <stdio.h>
#include <string.h>
#include <unistd.h>

#ifdef __CYGWIN__
#include "net/wpcap-drv.h"
#endif /* __CYGWIN__ */

#include "contiki.h"
#include "net/netstack.h"

#include "ctk/ctk.h"
#include "ctk/ctk-curses.h"

#include "dev/serial-line.h"

#include "net/ip/uip.h"

#include "dev/button-sensor.h"
#include "dev/pir-sensor.h"
#include "dev/vib-sensor.h"

#if NETSTACK_CONF_WITH_IPV6
#include "net/ipv6/uip-ds6.h"
#endif /* NETSTACK_CONF_WITH_IPV6 */

#include "net/rime/rime.h"

#ifdef SELECT_CONF_MAX
#define SELECT_MAX SELECT_CONF_MAX
#else
#define SELECT_MAX 8
#endif

//int main(int argc, char **argv);
int start(int argc, char **argv);

//char tab[1000];

static const struct select_callback *select_callback[SELECT_MAX];
static int select_max = 0;

SENSORS(&pir_sensor, &vib_sensor, &button_sensor);

os_timer_t blink_timer;
//os_timer_t start_timer;
LOCAL uint8_t led_state = 0;

static uint8_t serial_id[] = {0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08};
#if !NETSTACK_CONF_WITH_IPV6
static uint16_t node_id = 0x0102;
#endif /* !NETSTACK_CONF_WITH_IPV6 */

void blink_cb(void *arg) {
    // Update the LED's status.
    led_state = !led_state;
    GPIO_OUTPUT_SET(4, led_state);
    // Write a binary message (the 0x00 would terminate any string to os_printf).
    //uint8_t message[] = {0x03, 0x02, 0x01, 0x00, 0x01, 0x02, 0x03, '\r', '\n'};
    //uart0_tx_buffer(message, 9);

    // Write the LED status as a string.
    os_printf("Contiki LED state - %d.\n", led_state);
}

LOCAL void ICACHE_FLASH_ATTR set_blink_timer(uint16_t interval) {
    // Start a timer for the flashing of the LED on GPIO 4, running continuously.
    os_timer_disarm(&blink_timer);
    os_timer_setfn(&blink_timer, (os_timer_func_t *)blink_cb, (void *)0);
    os_timer_arm(&blink_timer, interval, 1);
}



/******************************************************************************
 * FunctionName : user_rf_cal_sector_set
 * Description  : SDK just reversed 4 sectors, used for rf init data and paramters.
 *                We add this function to force users to set rf cal sector, since
 *                we don't know which sector is free in user's application.
 *                sector map for last several sectors : ABCCC
 *                A : rf cal
 *                B : rf init data
 *                C : sdk parameters
 * Parameters   : none
 * Returns      : rf cal sector
*******************************************************************************/
uint32 ICACHE_FLASH_ATTR
user_rf_cal_sector_set(void)
{
    enum flash_size_map size_map = system_get_flash_size_map();
    uint32 rf_cal_sec = 0;

    switch (size_map) {
        case FLASH_SIZE_4M_MAP_256_256:
            rf_cal_sec = 128 - 5;
            break;

        case FLASH_SIZE_8M_MAP_512_512:
            rf_cal_sec = 256 - 5;
            break;

        case FLASH_SIZE_16M_MAP_512_512:
        case FLASH_SIZE_16M_MAP_1024_1024:
            rf_cal_sec = 512 - 5;
            break;

        case FLASH_SIZE_32M_MAP_512_512:
        case FLASH_SIZE_32M_MAP_1024_1024:
            rf_cal_sec = 1024 - 5;
            break;

        default:
            rf_cal_sec = 0;
            break;
    }

    return rf_cal_sec;
}

void initialisation() {
    gpio_init();

    // GPIO 4 is an output, start with it low (off).
    //PIN_FUNC_SELECT(PERIPHS_IO_MUX_GPIO4_U, FUNC_GPIO4);
    //gpio_output_set(0, BIT4, BIT4, 0);
    //gpio_output_set((1 << pin), 0, 0, 0);
    GPIO_OUTPUT_SET(4, led_state);
    //tab[0] = 100;
    //tab[200] = 100;

    // Start the LED timer at 2s per change.
    set_blink_timer(1000);
    start(0, NULL);
}


void ICACHE_FLASH_ATTR user_init()
{
    system_init_done_cb(initialisation);
}

/*---------------------------------------------------------------------------*/
/*static void ICACHE_FLASH_ATTR
set_rime_addr(void)
{
  linkaddr_t addr;
  int i;

  memset(&addr, 0, sizeof(linkaddr_t));
#if NETSTACK_CONF_WITH_IPV6
  memcpy(addr.u8, serial_id, sizeof(addr.u8));
#else
  if(node_id == 0) {
    for(i = 0; i < sizeof(linkaddr_t); ++i) {
      addr.u8[i] = serial_id[7 - i];
    }
  } else {
    addr.u8[0] = node_id & 0xff;
    addr.u8[1] = node_id >> 8;
  }
#endif
  linkaddr_set_node_addr(&addr);
  os_printf("Rime started with address ");
  for(i = 0; i < sizeof(addr.u8) - 1; i++) {
    os_printf("%d.", addr.u8[i]);
  }
  os_printf("%d\n", addr.u8[i]);
}*/


/*---------------------------------------------------------------------------*/
int contiki_argc = 0;
char **contiki_argv;

int ICACHE_FLASH_ATTR
start(int argc, char **argv)
{
#if NETSTACK_CONF_WITH_IPV6
#if UIP_CONF_IPV6_RPL
  os_printf(CONTIKI_VERSION_STRING " started with IPV6, RPL\n");
#else
  os_printf(CONTIKI_VERSION_STRING " started with IPV6\n");
#endif
#else
  os_printf(CONTIKI_VERSION_STRING " started\n");
#endif

  /* crappy way of remembering and accessing argc/v */
  contiki_argc = argc;
  contiki_argv = argv;

  process_init();
  process_start(&etimer_process, NULL);
  ctimer_init();
  //rtimer_init();

#if WITH_GUI
  process_start(&ctk_process, NULL);
#endif

  //set_rime_addr();

  //netstack_init();
  //os_printf("MAC %s RDC %s NETWORK %s\n", NETSTACK_MAC.name, NETSTACK_RDC.name, NETSTACK_NETWORK.name);
/*
#if NETSTACK_CONF_WITH_IPV6
  queuebuf_init();

  memcpy(&uip_lladdr.addr, serial_id, sizeof(uip_lladdr.addr));

  process_start(&tcpip_process, NULL);
  os_printf("Tentative link-local IPv6 address ");
  {
    uip_ds6_addr_t *lladdr;
    int i;
    lladdr = uip_ds6_get_link_local(-1);
    for(i = 0; i < 7; ++i) {
      os_printf("%02x%02x:", lladdr->ipaddr.u8[i * 2],
             lladdr->ipaddr.u8[i * 2 + 1]);
    }
    lladdr->state = ADDR_AUTOCONF;

    os_printf("%02x%02x\n", lladdr->ipaddr.u8[14], lladdr->ipaddr.u8[15]);
  }
#endif*/
  return 0;
}
/*---------------------------------------------------------------------------*/
void ICACHE_FLASH_ATTR
log_message(char *m1, char *m2)
{
  //fprintf(stderr, "%s%s\n", m1, m2);
}
/*---------------------------------------------------------------------------*/
void ICACHE_FLASH_ATTR
uip_log(char *m)
{
  //fprintf(stderr, "%s\n", m);
}
/*---------------------------------------------------------------------------*/
/** @} */
