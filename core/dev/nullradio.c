#include "dev/nullradio.h"
#include "ets_sys.h"

/*---------------------------------------------------------------------------*/
static int ICACHE_FLASH_ATTR
init(void)
{
  return 0;
}
/*---------------------------------------------------------------------------*/
static int ICACHE_FLASH_ATTR
prepare(const void *payload, unsigned short payload_len)
{
  return 1;
}
/*---------------------------------------------------------------------------*/
static int ICACHE_FLASH_ATTR
transmit(unsigned short transmit_len)
{
  return RADIO_TX_OK;
}
/*---------------------------------------------------------------------------*/
static int ICACHE_FLASH_ATTR
send(const void *payload, unsigned short payload_len)
{
  prepare(payload, payload_len);
  return transmit(payload_len);
}
/*---------------------------------------------------------------------------*/
static int ICACHE_FLASH_ATTR
radio_read(void *buf, unsigned short buf_len)
{
  return 0;
}
/*---------------------------------------------------------------------------*/
static int ICACHE_FLASH_ATTR
channel_clear(void)
{
  return 1;
}
/*---------------------------------------------------------------------------*/
static int ICACHE_FLASH_ATTR
receiving_packet(void)
{
  return 0;
}
/*---------------------------------------------------------------------------*/
static int ICACHE_FLASH_ATTR
pending_packet(void)
{
  return 0;
}
/*---------------------------------------------------------------------------*/
static int ICACHE_FLASH_ATTR
on(void)
{
  return 0;
}
/*---------------------------------------------------------------------------*/
static int ICACHE_FLASH_ATTR
off(void)
{
  return 0;
}
/*---------------------------------------------------------------------------*/
static radio_result_t ICACHE_FLASH_ATTR
get_value(radio_param_t param, radio_value_t *value)
{
  return RADIO_RESULT_NOT_SUPPORTED;
}
/*---------------------------------------------------------------------------*/
static radio_result_t ICACHE_FLASH_ATTR
set_value(radio_param_t param, radio_value_t value)
{
  return RADIO_RESULT_NOT_SUPPORTED;
}
/*---------------------------------------------------------------------------*/
static radio_result_t ICACHE_FLASH_ATTR
get_object(radio_param_t param, void *dest, size_t size)
{
  return RADIO_RESULT_NOT_SUPPORTED;
}
/*---------------------------------------------------------------------------*/
static radio_result_t ICACHE_FLASH_ATTR
set_object(radio_param_t param, const void *src, size_t size)
{
  return RADIO_RESULT_NOT_SUPPORTED;
}
/*---------------------------------------------------------------------------*/
const struct radio_driver nullradio_driver =
  {
    init,
    prepare,
    transmit,
    send,
    radio_read,
    channel_clear,
    receiving_packet,
    pending_packet,
    on,
    off,
    get_value,
    set_value,
    get_object,
    set_object
  };
/*---------------------------------------------------------------------------*/
