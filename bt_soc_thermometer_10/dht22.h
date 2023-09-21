/*
 * DHT22.h
 *
 *  Created on: Jul 6, 2023
 *      Author: nthu8
 */

#ifndef DHT22_H_
#define DHT22_H_
#include <stdint.h>
#include <em_gpio.h>
#include "ustimer.h"
typedef struct DHT_22
{
  GPIO_Port_TypeDef _port;
  uint8_t data[5];
  uint8_t _pin;
  uint64_t _lastreadtime;
  bool _lastresult;
  Ecode_t ustimer;
}DHT;
void DHT22_init(DHT *d, GPIO_Port_TypeDef _port, uint8_t _pin);
float DHT22_readTemperature(DHT *d, bool force);
float DHT22_readHumidity(DHT *d, bool force);
bool DHT22_read(DHT *d, bool force);
uint32_t expectPulse(DHT *d, bool level);
sl_status_t sl_bt_ht_temperature_measurement_indicate(uint8_t connection,
                                                      int32_t value,
                                                      bool fahrenheit);
//void DHT_init(void);
//void DHT_process();
#endif /* DHT22_H_ */
