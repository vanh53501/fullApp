/***************************************************************************//**
 * @file
 * @brief Core application logic.
 *******************************************************************************
 * # License
 * <b>Copyright 2020 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * SPDX-License-Identifier: Zlib
 *
 * The licensor of this software is Silicon Laboratories Inc.
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 *
 ******************************************************************************/
#include <stdbool.h>
#include "em_common.h"
#include "sl_status.h"
#include "sl_simple_button_instances.h"
#include "sl_simple_timer.h"
#include "app_log.h"
#include "app_assert.h"
#include "sl_bluetooth.h"
#ifdef SL_COMPONENT_CATALOG_PRESENT
#include "sl_component_catalog.h"
#endif // SL_COMPONENT_CATALOG_PRESENT
#ifdef SL_CATALOG_CLI_PRESENT
#include "sl_cli.h"
#endif // SL_CATALOG_CLI_PRESENT
#include "sl_sensor_rht.h"
#include "sl_health_thermometer.h"
#include "app.h"
#include "em_device.h"
#include "em_chip.h"
#include "em_gpio.h"
#include "em_cmu.h"
#include "stdint.h"
#include "stdio.h"
#include "gatt_db.h"
#include "em_gpio.h"
#include "stdint.h"
#include "stdio.h"
#include "iadc.h"
#include "dht22.h"

#define RELAY1_PIN 6
#define RELAY1_PORT      gpioPortA
#define RELAY2_PIN 7
#define RELAY2_PORT      gpioPortA

#define BUTTON1_PORT     gpioPortD  // Change this to the actual GPIO port
#define BUTTON1_PIN      2

#define BUTTON2_PORT     gpioPortB   // Change this to the actual GPIO port
#define BUTTON2_PIN      1

#define RLED_PIN 4
#define RLED_PORT      gpioPortC

#define WLED_PIN 3
#define WLED_PORT      gpioPortD

typedef enum {
    DEVICE_OFF,
    DEVICE_ON
} DeviceState;

DeviceState deviceState = DEVICE_OFF;
DeviceState airpumpState = DEVICE_OFF;
DeviceState rledState = DEVICE_OFF;
DeviceState wledState = DEVICE_OFF;
void toggleDevice() {
    if (deviceState == DEVICE_OFF) {
        // Turn the device on
        GPIO_PinOutSet(RELAY1_PORT, RELAY1_PIN);
       // GPIO_PinOutSet(WLED_PORT, WLED_PIN);
        deviceState = DEVICE_ON;

    } else {
        // Turn the device off
        GPIO_PinOutClear(RELAY1_PORT, RELAY1_PIN);
       // GPIO_PinOutClear(WLED_PORT, WLED_PIN);
        deviceState = DEVICE_OFF;

    }
}
void toggleairpump() {
    if (airpumpState == DEVICE_OFF) {
        // Turn the device on
      //  GPIO_PinOutSet(RLED_PORT, RLED_PIN);
        GPIO_PinOutSet(RELAY2_PORT, RELAY2_PIN);
        airpumpState = DEVICE_ON;

    } else {
        // Turn the device off
       // GPIO_PinOutClear(RLED_PORT, RLED_PIN);
        GPIO_PinOutClear(RELAY2_PORT, RELAY2_PIN);
        airpumpState = DEVICE_OFF;

    }
}
void togglewled() {
    if (deviceState == DEVICE_OFF) {
        // Turn the device on

        GPIO_PinOutSet(WLED_PORT, WLED_PIN);
       wledState = DEVICE_ON;

    } else {
        // Turn the device off
        GPIO_PinOutClear(WLED_PORT, WLED_PIN);
        wledState = DEVICE_OFF;

    }
}
void togglerled() {
    if (airpumpState == DEVICE_OFF) {
        // Turn the device on
        GPIO_PinOutSet(RLED_PORT, RLED_PIN);
        rledState = DEVICE_ON;

    } else {
        // Turn the device off
        GPIO_PinOutClear(RLED_PORT, RLED_PIN);
       rledState = DEVICE_OFF;
    }
}
// The advertising set handle allocated from Bluetooth stack.
static uint8_t advertising_set_handle = 0xff;

static bool report_button_flag = false;
// Connection handle.
static uint8_t app_connection = 0;

// The advertising set handle allocated from Bluetooth stack.



// Periodic timer handle.
static sl_simple_timer_t app_periodic_timer;

// Periodic timer callback.
static void app_periodic_timer_cb(sl_simple_timer_t *timer, void *data);

/**************************************************************************//**
 * Application Init.
 *****************************************************************************/
DHT d;
float temp, hud;
bool force = false;
SL_WEAK void app_init(void)
{
  IADC_app_init();
  sl_status_t sc;
  // Init temperature sensor.
 /* sc = sl_sensor_rht_init();
  if (sc != SL_STATUS_OK) {
     app_log_warning("Relative Humidity and Temperature sensor initialization failed.");
     app_log_nl();
   }*/
  DHT22_init(&d,gpioPortA,5);
  sl_button_disable(SL_SIMPLE_BUTTON_INSTANCE(0));
   CMU_ClockEnable(cmuClock_GPIO, true);
           GPIO_PinModeSet(RELAY1_PORT, RELAY1_PIN, gpioModePushPull, 0);
           GPIO_PinModeSet(RELAY2_PORT, RELAY2_PIN, gpioModePushPull, 0);
         GPIO_PinModeSet(BUTTON1_PORT, BUTTON1_PIN, gpioModeInputPull, 1);
           GPIO_PinModeSet(BUTTON2_PORT, BUTTON2_PIN, gpioModeInputPull, 1);
           GPIO_PinModeSet(RLED_PORT, RLED_PIN, gpioModePushPull, 0);
           GPIO_PinModeSet(WLED_PORT, WLED_PIN, gpioModePushPull, 0);
}

#ifndef SL_CATALOG_KERNEL_PRESENT
/**************************************************************************//**
 * Application Process Action.
 *****************************************************************************/
SL_WEAK void app_process_action(void)
{
  /////////////////////////////////////////////////////////////////////////////
  // Put your additional application code here!                              //
  // This is called infinitely.                                              //
  // Do not call blocking functions from here!                               //
 if (GPIO_PinInGet(BUTTON1_PORT, BUTTON1_PIN) == 1)
              {
                toggleDevice();
                for (volatile int i = 0; i < 1000000; i++);
         //   printf("%d\n",ds);
              }
 if (GPIO_PinInGet(BUTTON2_PORT, BUTTON2_PIN) == 1)
              {
               // toggleDevice();
                toggleairpump();
                for (volatile int i = 0; i < 1000000; i++);
              }
/* if (DHT22_read(&d,force))

           {
            temp = DHT22_readTemperature(&d,force);
            hud = DHT22_readHumidity(&d, force);
         }
   app_log_info("Temp:%f\n",temp);
   app_log_info("Hud:%f\n",hud);*/
   IADC_app_process_action();
 /* IADC_app_process_action();
  sl_sleeptimer_delay_millisecond(1000);*/

  /////////////////////////////////////////////////////////////////////////////
}
#endif

/**************************************************************************//**
 * Bluetooth stack event handler.
 * This overrides the dummy weak implementation.
 *
 * @param[in] evt Event coming from the Bluetooth stack.
 *****************************************************************************/
void sl_bt_on_event(sl_bt_msg_t *evt)
{
  sl_status_t sc;
  bd_addr address;
  uint8_t address_type;

  // Handle stack events
  switch (SL_BT_MSG_ID(evt->header)) {
    // -------------------------------
    // This event indicates the device has started and the radio is ready.
    // Do not call any stack command before receiving this boot event!
    case sl_bt_evt_system_boot_id:
      // Print boot message.
      app_log_info("Bluetooth stack booted: v%d.%d.%d-b%d\n",
                   evt->data.evt_system_boot.major,
                   evt->data.evt_system_boot.minor,
                   evt->data.evt_system_boot.patch,
                   evt->data.evt_system_boot.build);

      // Extract unique ID from BT Address.
      sc = sl_bt_system_get_identity_address(&address, &address_type);
      app_assert_status(sc);

      app_log_info("Bluetooth %s address: %02X:%02X:%02X:%02X:%02X:%02X\n",
                   address_type ? "static random" : "public device",
                   address.addr[5],
                   address.addr[4],
                   address.addr[3],
                   address.addr[2],
                   address.addr[1],
                   address.addr[0]);

      // Create an advertising set.
      sc = sl_bt_advertiser_create_set(&advertising_set_handle);
      app_assert_status(sc);

      // Generate data for advertising
      sc = sl_bt_legacy_advertiser_generate_data(advertising_set_handle,
                                                 sl_bt_advertiser_general_discoverable);
      app_assert_status(sc);

      // Set advertising interval to 100ms.
      sc = sl_bt_advertiser_set_timing(
        advertising_set_handle, // advertising set handle
        160, // min. adv. interval (milliseconds * 1.6)
        160, // max. adv. interval (milliseconds * 1.6)
        0,   // adv. duration
        0);  // max. num. adv. events
      app_assert_status(sc);

      // Start advertising and enable connections.
      sc = sl_bt_legacy_advertiser_start(advertising_set_handle,
                                         sl_bt_advertiser_connectable_scannable);
      app_assert_status(sc);

      app_log_info("Started advertising\n");
      break;

    // -------------------------------
    // This event indicates that a new connection was opened.
    case sl_bt_evt_connection_opened_id:
      app_log_info("Connection opened\n");

#ifdef SL_CATALOG_BLUETOOTH_FEATURE_POWER_CONTROL_PRESENT
      // Set remote connection power reporting - needed for Power Control
      sc = sl_bt_connection_set_remote_power_reporting(
        evt->data.evt_connection_opened.connection,
        sl_bt_connection_power_reporting_enable);
      app_assert_status(sc);
#endif // SL_CATALOG_BLUETOOTH_FEATURE_POWER_CONTROL_PRESENT

      break;

    // -------------------------------
    // This event indicates that a connection was closed.
    case sl_bt_evt_connection_closed_id:
      app_log_info("Connection closed\n");

      // Generate data for advertising
      sc = sl_bt_legacy_advertiser_generate_data(advertising_set_handle,
                                                 sl_bt_advertiser_general_discoverable);
      app_assert_status(sc);

      // Restart advertising after client has disconnected.
      sc = sl_bt_legacy_advertiser_start(advertising_set_handle,
                                         sl_bt_advertiser_connectable_scannable);
      app_assert_status(sc);
      app_log_info("Started advertising\n");
      break;
    case sl_bt_evt_gatt_server_attribute_value_id:
          // The value of the gattdb_led_control characteristic was changed.
      if (gattdb_pump == evt->data.evt_gatt_server_attribute_value.attribute) {
                 uint8_t data_recv;
                 size_t data_recv_len;

                 // Read characteristic value.
                 sc = sl_bt_gatt_server_read_attribute_value(gattdb_pump,
                                                             0,
                                                             sizeof(data_recv),
                                                             &data_recv_len,
                                                             &data_recv);
            (void)data_recv_len;
            app_log_status_error(sc);

            if (sc != SL_STATUS_OK) {
              break;
            }

            // Toggle pump.

            if (data_recv == 0x00) {

               toggleDevice();
              for (volatile int i = 0; i < 1000000; i++);
             // printf("%d\n",ds);
            } else if (data_recv == 0x01) {

              toggleDevice();
              for (volatile int i = 0; i < 1000000; i++);
            //  printf("%d\n",ds);
            }  else {
              app_log_error("Invalid attribute value: 0x%02x\n", (int)data_recv);
            }
          }

       if (gattdb_air_pump == evt->data.evt_gatt_server_attribute_value.attribute) {
                     uint8_t data_recv;
                     size_t data_recv_len;

                     // Read characteristic value.
                     sc = sl_bt_gatt_server_read_attribute_value(gattdb_air_pump,
                                                                 0,
                                                                 sizeof(data_recv),
                                                                 &data_recv_len,
                                                                 &data_recv);
                (void)data_recv_len;
                app_log_status_error(sc);

                if (sc != SL_STATUS_OK) {
                  break;
                }

                // Toggle pump.

                if (data_recv == 0x00) {

                  toggleairpump();
                //  for (volatile int i = 0; i < 1000000; i++);
                 // printf("%d\n",ds);
                } else if (data_recv == 0x01) {

                  toggleairpump();
                 // for (volatile int i = 0; i < 1000000; i++);
                //  printf("%d\n",ds);
                }  else {
                  app_log_error("Invalid attribute value: 0x%02x\n", (int)data_recv);
                }
              }
       if (gattdb_led == evt->data.evt_gatt_server_attribute_value.attribute) {
                            uint8_t data_recv;
                            size_t data_recv_len;

                            // Read characteristic value.
                            sc = sl_bt_gatt_server_read_attribute_value(gattdb_led,
                                                                        0,
                                                                        sizeof(data_recv),
                                                                        &data_recv_len,
                                                                        &data_recv);
                       (void)data_recv_len;
                       app_log_status_error(sc);

                       if (sc != SL_STATUS_OK) {
                         break;
                       }

                       // Toggle pump.

                       if (data_recv == 0x00) {

                         togglerled();
                        for (volatile int i = 0; i < 1000000; i++);
                        // printf("%d\n",ds);
                       } else if (data_recv == 0x01) {

                         togglewled();
                         for (volatile int i = 0; i < 1000000; i++);
                       //  printf("%d\n",ds);
                       }  else {
                         app_log_error("Invalid attribute value: 0x%02x\n", (int)data_recv);
                       }
                     }
              break;
    ///////////////////////////////////////////////////////////////////////////
    // Add additional event handlers here as your application requires!      //
    ///////////////////////////////////////////////////////////////////////////

    // -------------------------------
    // Default event handler.
    default:
      break;
  }
}

/**************************************************************************//**
 * Callback function of connection close event.
 *
 * @param[in] reason Unused parameter required by the health_thermometer component
 * @param[in] connection Unused parameter required by the health_thermometer component
 *****************************************************************************/
void sl_bt_connection_closed_cb(uint16_t reason, uint8_t connection)
{
  (void)reason;
  (void)connection;
  sl_status_t sc;

  // Stop timer.
  sc = sl_simple_timer_stop(&app_periodic_timer);
  app_assert_status(sc);
}
static sl_status_t update_report_pump_characteristic(void)
{
  sl_status_t sc;
  uint8_t data_send;

  switch (sl_button_get_state(SL_SIMPLE_BUTTON_INSTANCE(0))) {
    case SL_SIMPLE_BUTTON_PRESSED:
      data_send = (uint8_t)SL_SIMPLE_BUTTON_PRESSED;
      break;

    case SL_SIMPLE_BUTTON_RELEASED:
      data_send = (uint8_t)SL_SIMPLE_BUTTON_RELEASED;
      break;

    default:
      // Invalid button state
      return SL_STATUS_FAIL; // Invalid button state
  }

  // Write attribute in the local GATT database.
  sc = sl_bt_gatt_server_write_attribute_value(gattdb_pump,
                                               0,
                                               sizeof(data_send),
                                               &data_send);
  if (sc == SL_STATUS_OK) {
    app_log_info("Attribute written: 0x%02x", (int)data_send);
  }

  return sc;
}


/**************************************************************************//**
 * Health Thermometer - Temperature Measurement
 * Indication changed callback
 *
 * Called when indication of temperature measurement is enabled/disabled by
 * the client.
 *****************************************************************************/


void sl_bt_smartgarden_indication_changed_cb(uint8_t connection,
                                                            sl_bt_gatt_client_config_flag_t client_config)
{
  sl_status_t sc;
  app_connection = connection;
  // Indication or notification enabled.
  if (sl_bt_gatt_disable != client_config) {
    // Start timer used for periodic indications.
    sc = sl_simple_timer_start(&app_periodic_timer,
                               SL_BT_HT_MEASUREMENT_INTERVAL_SEC * 1000,
                               app_periodic_timer_cb,
                               NULL,
                               true);
    app_assert_status(sc);
    // Send first indication.
    app_periodic_timer_cb(&app_periodic_timer, NULL);
  }
  // Indications disabled.
  else {
    // Stop timer used for periodic indications.
    (void)sl_simple_timer_stop(&app_periodic_timer);
  }
}
/**************************************************************************//**
 * Simple Button
 * Button state changed callback
 * @param[in] handle Button event handle
 *****************************************************************************/


/**************************************************************************//**
 * Timer callback
 * Called periodically to time periodic temperature measurements and indications.
 *****************************************************************************/
static void app_periodic_timer_cb(sl_simple_timer_t *timer, void *data)
{
 /* (void)data;
  (void)timer;
  sl_status_t sc;
  //if (GPIO_PinInGet(BUTTON1_PORT, BUTTON1_PIN) == 1){
      if (DHT22_read(&d,force))

           {
            temp = DHT22_readTemperature(&d,force);
            hud = DHT22_readHumidity(&d, force);
         }
   app_log_info("Temp:%f\n",temp);
   app_log_info("Hud:%f\n",hud);
   int32_t temperature, humidity;
   temperature = (int32_t)temp*1000;
   humidity = (int32_t)hud*1000;
  //  sl_sleeptimer_delay_millisecond(1000);

  // Send temperature measurement indication to connected client.
   sc = sl_bt_ht_temperature_measurement_indicate(app_connection,
                                                    temperature,
                                                    false);*/
   (void)data;
     (void)timer;
     sl_status_t sc;
     int32_t temperature = 0;
     uint32_t humidity = 0;
     float tmp_c = 0.0;
     float humid = 0.0;
     // float tmp_f = 0.0;

     // Measure temperature; units are % and milli-Celsius.
     sc = sl_sensor_rht_get(&humidity, &temperature);
     if (SL_STATUS_NOT_INITIALIZED == sc) {
       app_log_info("Relative Humidity and Temperature sensor is not initialized.");
       app_log_nl();
     } else if (sc != SL_STATUS_OK) {
       app_log_warning("Invalid RHT reading: %lu %ld\n", humidity, temperature);
     }

     // button 0 pressed: overwrite temperature with -20C.

     tmp_c = (float)temperature / 1000;
     humid = (float)humidity / 1000;
     app_log_info("Temperature: %5.2f C\n", tmp_c);
     app_log_infor("Humidity: %f %\n", humid);
     // Send temperature measurement indication to connected client.
     sc = sl_bt_ht_temperature_measurement_indicate(app_connection,
                                                    temperature,
                                                    false);
     // Conversion to Fahrenheit: F = C * 1.8 + 32
     // tmp_f = (float)(temperature*18+320000)/10000;
     // app_log_info("Temperature: %5.2f F\n", tmp_f);
     // Send temperature measurement indication to connected client.
     // sc = sl_bt_ht_temperature_measurement_indicate(app_connection,
     //                                                (temperature*18+320000)/10,
     //                                                true);
     if (sc) {
       app_log_warning("Failed to send temperature measurement indication\n");
     }
   sc = bl_humidity_measurement_indicate(app_connection,humidity,false);

  // Conversion to Fahrenheit: F = C * 1.8 + 32
  // tmp_f = (float)(temperature*18+320000)/10000;
  // app_log_info("Temperature: %5.2f F\n", tmp_f);
  // Send temperature measurement indication to connected client.
  // sc = sl_bt_ht_temperature_measurement_indicate(app_connection,
  //                                                (temperature*18+320000)/10,
  //                                                true);

//\}
}

#ifdef SL_CATALOG_CLI_PRESENT
void hello(sl_cli_command_arg_t *arguments)
{
  (void) arguments;
  bd_addr address;
  uint8_t address_type;
  sl_status_t sc = sl_bt_system_get_identity_address(&address, &address_type);
  app_assert_status(sc);
  app_log_info("Bluetooth %s address: %02X:%02X:%02X:%02X:%02X:%02X\n",
               address_type ? "static random" : "public device",
               address.addr[5],
               address.addr[4],
               address.addr[3],
               address.addr[2],
               address.addr[1],
               address.addr[0]);
}
#endif // SL_CATALOG_CLI_PRESENT
