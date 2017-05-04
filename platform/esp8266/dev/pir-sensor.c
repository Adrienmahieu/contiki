#include "ets_sys.h"
#include "dev/pir-sensor.h"

const struct sensors_sensor pir_sensor;

static int pir_value;

/*---------------------------------------------------------------------------*/
void ICACHE_FLASH_ATTR
pir_sensor_changed(int strength)
{
  pir_value += strength;
  sensors_changed(&pir_sensor);
}
/*---------------------------------------------------------------------------*/
static int ICACHE_FLASH_ATTR
value(int type)
{
  return pir_value;
}
/*---------------------------------------------------------------------------*/
static int ICACHE_FLASH_ATTR
configure(int type, int c)
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
SENSORS_SENSOR(pir_sensor, PIR_SENSOR,
	       value, configure, status);
