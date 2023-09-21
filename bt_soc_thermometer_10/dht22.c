/*
 * DHT22.c
 *
 *  Created on: Jul 6, 2023
 *      Author: nthu8
 */
#include "sl_sleeptimer.h"
#include "em_gpio.h"
#include "ustimer.h"
#include "em_cmu.h"
#include "DHT22.h"
#include "app_log.h"
#include <stdio.h>
#include "sl_bluetooth.h"
#include "gatt_db.h"
#include "app_assert.h"
enum temperature_measurement_flag {
  TEMPERATURE_MEASUREMENT_FLAG_UNITS     = 0x1,
  TEMPERATURE_MEASUREMENT_FLAG_TIMESTAMP = 0x2,
  TEMPERATURE_MEASUREMENT_FLAG_TYPE      = 0x4,
};

/*sl_status_t bl_temperature_measurement_indicate(uint8_t connection,
                                                      float value,
                                                      bool fahrenheit)
{
  sl_status_t sc;
  uint8_t buf[5] = { 0 };
  temperature_dhtmeasurement_val_to_buf(value, fahrenheit, buf);
  sc = sl_bt_gatt_server_send_indication(
    connection,
    gattdb_temp,
    sizeof(buf),
    buf);
  return sc;
}*/

void humidmeasurement_val_to_buf(int32_t value,
                                 uint8_t *buffer)
{
  uint32_t tmp_value = ((uint32_t)value & 0x00ffffffu) \
                       | ((uint32_t)(-3) << 24);
  buffer[0] =  0;
  buffer[1] = tmp_value & 0xff;
  buffer[2] = (tmp_value >> 8) & 0xff;
  buffer[3] = (tmp_value >> 16) & 0xff;
  buffer[4] = (tmp_value >> 24) & 0xff;
}
sl_status_t bl_humidity_measurement_indicate(uint8_t connection,
                                                      float value,
                                                      bool fahrenheit)
{
  sl_status_t sc;
  uint8_t buf[5] = { 0 };
  humidmeasurement_val_to_buf(value, buf);
  sc = sl_bt_gatt_server_send_indication(
    connection,
    gattdb_humidity_0,
    sizeof(buf),
    buf);
  return sc;
}
void temperature_dhtmeasurement_val_to_buf(int32_t value,
                                               bool fahrenheit,
                                               uint8_t *buffer)
{
  uint32_t tmp_value = ((uint32_t)value & 0x00ffffffu) \
                       | ((uint32_t)(-3) << 24);
  buffer[0] = fahrenheit ? TEMPERATURE_MEASUREMENT_FLAG_UNITS : 0;
  buffer[1] = tmp_value & 0xff;
  buffer[2] = (tmp_value >> 8) & 0xff;
  buffer[3] = (tmp_value >> 16) & 0xff;
  buffer[4] = (tmp_value >> 24) & 0xff;
}

void DHT22_init(DHT *d, GPIO_Port_TypeDef _port, uint8_t _pin){
   d->_lastreadtime = sl_sleeptimer_get_tick_count64();
   USTIMER_Init();
   d->_port = _port;
   d->_pin = _pin;
}
float DHT22_readTemperature(DHT *d, bool force){
    float temp = 0;
      if (read(d, force))
      {
        temp = ((uint32_t)(d->data[2] & 0x7F)) << 8 | d->data[3];
        temp *= 0.1;
        if (d->data[2] & 0x80)
        {
          temp *= -1;
        }
      }
      return temp;
}
float DHT22_readHumidity(DHT *d, bool force){
    float hm = 0;
      if (read(d, force))
      {
        hm = ((uint32_t)d->data[0]) << 8 | d->data[1];
        hm *= 0.1;
      }
      return hm;
}
uint32_t expectPulse(DHT *d, bool level)
{
  uint32_t count = 0;
  while (GPIO_PinInGet(d->_port, d->_pin) == level)
  {
    if(count++ >= 1000 * CMU_ClockFreqGet(cmuClock_SYSCLK))
      return 0xffffffffu;
  }
  return count;
}
bool DHT22_read(DHT *d, bool force){
    uint64_t currenttime = sl_sleeptimer_get_tick_count64();
    if(!force && ((currenttime - d->_lastreadtime) < 66000)){
        return d->_lastresult;
    }
    d->_lastreadtime = sl_sleeptimer_get_tick_count64();
    d->data[0] = d->data[1] = d->data[2] = d->data[3] = d->data[4] = 0;
    GPIO_PinModeSet(d->_port, d->_pin, gpioModeInputPull, 1);
    d->ustimer = USTIMER_Delay(1000);
    GPIO_PinModeSet(d->_port, d->_pin, gpioModePushPull, 0);
    d->ustimer = USTIMER_Delay(1100);
    uint32_t cycles[80];
    {
        GPIO_PinModeSet(d->_port, d->_pin, gpioModeInputPull, 1);
        d->ustimer = USTIMER_DelayIntSafe(55);
        if(expectPulse(d, 0) == 0xffffffffu){
          d->_lastresult = false;
          return d->_lastresult;
        }
        if(expectPulse(d, 1) == 0xffffffffu){
          d->_lastresult = false;
          return d->_lastresult;
        }
        for (int i = 0; i < 80; i += 2){
            cycles[i] = expectPulse(d, 0);
            cycles[i + 1] = expectPulse(d, 1);
        }
    }
    for (int i = 0; i < 40; ++i)
    {
        uint32_t lowCycles = cycles[2 * i];
        uint32_t highCycles = cycles[2 * i + 1];
        if ((lowCycles == 0xffffffffu) || (highCycles == 0xffffffffu))
        {
            d->_lastresult = false;
            return d->_lastresult;
        }
        d->data[i / 8] <<= 1;
        if (highCycles > lowCycles)
        {
          d->data[i / 8] |= 1;
        }
    }
    if (d->data[4] == ((d->data[0] + d->data[1] + d->data[2] + d->data[3]) & 0xFF)){
        d->_lastresult = true;
        return d->_lastresult;
    }
    else
    {
        d->_lastresult = false;
        return d->_lastresult;
    }
}

//void DHT_init(void){

 //#define DHT_PORT gpioPortA
 //#define DHT_PIN 7
 //DHT *dht;
 //init(&dht, DHT_PORT, DHT_PIN);

//}

//void DHT_process(void){
         // float temp = readTemperature(&dht,false);
        //  float hud = readHumidity(&dht, false);
      //    printf("Temp:%f",temp);
       //   printf("Hud:%f",hud);
   //       sl_sleeptimer_delay_millisecond(1000);
//}
