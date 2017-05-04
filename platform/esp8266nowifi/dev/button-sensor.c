#include "ets_sys.h"
#include "dev/button-sensor.h"

const struct sensors_sensor button_sensor;

/*---------------------------------------------------------------------------*/
void ICACHE_FLASH_ATTR
button_press(void)
{
  sensors_changed(&button_sensor);
}
/*---------------------------------------------------------------------------*/
static int ICACHE_FLASH_ATTR
value(int type)
{
  return 0;
}
/*---------------------------------------------------------------------------*/
static int ICACHE_FLASH_ATTR
configure(int type, int value)
{
  return 0;
}
/*---------------------------------------------------------------------------*/
static int ICACHE_FLASH_ATTR
status(int type)
{
  return 0;
}
/*---------------------------------------------------------------------------*/
SENSORS_SENSOR(button_sensor, BUTTON_SENSOR,
	       value, configure, status);
