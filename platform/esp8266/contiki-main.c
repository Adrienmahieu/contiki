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

#include "dev/leds.h"
#include "dev/watchdog.h"
#include "sys/clock.h"
#include "sys/timer.h"
#include "sys/etimer.h"

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

int main(int argc, char **argv);

static struct timer t;

static const struct select_callback *select_callback[SELECT_MAX];
static int select_max = 0;


PROCESS(example_process, "Exemple Process");
//AUTOSTART_PROCESSES(&example_process);

PROCESS_THREAD(example_process, ev, data)
{
    //static struct etimer et;
    PROCESS_BEGIN();
    
    //etimer_set(&et, CLOCK_SECOND);
    
    while(1) {
        PROCESS_WAIT_EVENT();
        os_printf("EXAMPLE PROCESS\n");
    }
    
    PROCESS_END();
}

SENSORS(&pir_sensor, &vib_sensor, &button_sensor);

static uint8_t serial_id[] = {0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08};
#if !NETSTACK_CONF_WITH_IPV6
static uint16_t node_id = 0x0102;
#endif /* !NETSTACK_CONF_WITH_IPV6 */

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
    main(0, NULL);
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

 static void ICACHE_FLASH_ATTR print_processes(struct process * const processes[])
 {
 /* const struct process * const * p = processes; */
    os_printf("Starting");
    while(*processes != NULL) {
        os_printf(" ’%s’", (*processes)->name);
        processes++;
    }
    os_printf("\n");
 }
/*---------------------------------------------------------------------------*/
int contiki_argc = 0;
char **contiki_argv;

int ICACHE_FLASH_ATTR
main(int argc, char **argv)
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
  process_start(&example_process, NULL);
  
  ctimer_init();
  leds_init();
  
   
  os_printf("----------[ Running %s on ESP8266 ]----------\n\n", CONTIKI_VERSION_STRING);
  //Linking fails if use
  //print_processes(autostart_processes);
  
  //initialisation timer 1 seconde
  timer_set(&t, CLOCK_SECOND);


  //LED + delay + watchdog Test (led.c to assign correct gpio to LEDS)
  for(int i=0; i<1; i++) {
      os_printf("LED Test\n");
      os_printf("LED Test verte\n");
      leds_arch_set(LEDS_GREEN);
      clock_delay(1000);
      watchdog_periodic();
      os_printf("LED Test jaune\n");
      leds_arch_set(LEDS_YELLOW);
      clock_delay(1000);
      watchdog_periodic();
      os_printf("LED Test rouge\n");
      leds_arch_set(LEDS_RED);
      clock_delay(1000);
      watchdog_periodic();
      
      os_printf("LED Test verte + jaune\n");
      leds_arch_set(LEDS_GREEN|LEDS_YELLOW);
      clock_delay(1000);
      watchdog_periodic();
      os_printf("LED Test jaune + rouge\n");
      leds_arch_set(LEDS_YELLOW|LEDS_RED);
      clock_delay(1000);
      watchdog_periodic();
      os_printf("LED Test verte + jaune + rouge\n");
      leds_arch_set(LEDS_GREEN|LEDS_YELLOW|LEDS_RED);
      clock_delay(1000);
      watchdog_periodic();
      os_printf("LED Test off\n");
      leds_off(LEDS_ALL);
  }
  //rtimer_init();
  
  //Clock + watchdog test
  os_printf("\nClock Test\n");
  for(int i=0; i<5; i++) {
      os_printf("Ticks : %ld\n", clock_time());
      //os_printf("Ticks : %ld\n", clock_time());
      os_printf("Seconds : %ld\n", clock_seconds());
      clock_delay(500);
      watchdog_periodic();
  }
  
  
  static char msg[] = "MESSAGE";
  //process_post(&example_process, PROCESS_EVENT_CONTINUE, msg);
  
  //process_poll(&example_process);
  
  while(1) {
      int r;
      if(timer_expired(&t)) {
          os_printf("Timer Expired\n");
          process_post(&example_process, PROCESS_EVENT_CONTINUE, msg);
          timer_restart(&t);
      }
      do {
          watchdog_periodic();
          r = process_run();
          //os_printf("R: %d\n", r);
      } while(r > 0);
      
      watchdog_periodic();
  }
  
  
 /* while(1) {
      int r;
      do {
          watchdog_periodic();
          r = process_run();
          process_post(&example_process, PROCESS_EVENT_CONTINUE, msg);
          //os_printf("R: %d\n", r);
      } while(r > 0);
      
  }*/

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

