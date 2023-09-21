#include "em_device.h"
#include "em_chip.h"
#include "em_cmu.h"
#include "em_gpio.h"
#include "em_iadc.h"
#include "gatt_db.h"
#include "sl_status.h"
#define CLK_SRC_ADC_FREQ        40000000  // CLK_SRC_ADC - 40 MHz max
#define CLK_ADC_FREQ            10000000  // CLK_ADC - 10 MHz max in normal mode

#define NUM_INPUTS 2

#define IADC_INPUT_0_PORT_PIN     iadcPosInputPortBPin2; // PH pin
#define IADC_INPUT_1_PORT_PIN     iadcPosInputPortBPin5; // EC pin

#define IADC_INPUT_0_BUS          BBUSALLOC
#define IADC_INPUT_0_BUSALLOC     GPIO_BBUSALLOC_BEVEN0_ADC0
#define IADC_INPUT_1_BUS          BBUSALLOC
#define IADC_INPUT_1_BUSALLOC     GPIO_BBUSALLOC_BODD0_ADC0


#define HFXO_FREQ               39000000

static volatile double scanResult[NUM_INPUTS];
void initCMU(void)
{
  // Initialization structure for HFXO configuration
  CMU_HFXOInit_TypeDef hfxoInit = CMU_HFXOINIT_DEFAULT;

  // Check if device has HFXO configuration information in DEVINFO page
  if (DEVINFO->MODULEINFO & DEVINFO_MODULEINFO_HFXOCALVAL) {
  // Use the DEVINFO page's CTUNE values
  hfxoInit.ctuneXoAna = (DEVINFO->MODXOCAL & DEVINFO_MODXOCAL_HFXOCTUNEXOANA_DEFAULT)
    >> _DEVINFO_MODXOCAL_HFXOCTUNEXOANA_SHIFT;
  hfxoInit.ctuneXiAna = (DEVINFO->MODXOCAL & DEVINFO_MODXOCAL_HFXOCTUNEXIANA_DEFAULT)
    >> _DEVINFO_MODXOCAL_HFXOCTUNEXIANA_SHIFT;
  }

  // Configure HFXO settings
  CMU_HFXOInit(&hfxoInit);

  // Set system HFXO frequency
  SystemHFXOClockSet(HFXO_FREQ);

  // Enable HFXO oscillator, and wait for it to be stable
  CMU_OscillatorEnable(cmuOsc_HFXO, true, true);

  // Select HFXO as the EM01GRPA clock
  CMU_ClockSelectSet(cmuClock_EM01GRPACLK, cmuSelect_HFXO);
}


/**************************************************************************//**
* @brief  IADC initialization
*****************************************************************************/
void initIADC(void)
{
  // Declare initialization structures
  IADC_Init_t init = IADC_INIT_DEFAULT;
  IADC_AllConfigs_t initAllConfigs = IADC_ALLCONFIGS_DEFAULT;
  IADC_InitScan_t initScan = IADC_INITSCAN_DEFAULT;

  // Scan table structure
  IADC_ScanTable_t scanTable = IADC_SCANTABLE_DEFAULT;

  CMU_ClockEnable(cmuClock_IADC0, true);

  // Use the EM01GRPACLK as the IADC clock
  CMU_ClockSelectSet(cmuClock_IADCCLK, cmuSelect_EM01GRPACLK);

  // Shutdown between conversions to reduce current
  init.warmup = iadcWarmupNormal;

  // Set the HFSCLK prescale value here
  init.srcClkPrescale = IADC_calcSrcClkPrescale(IADC0, CLK_SRC_ADC_FREQ, 0);


  init.timerCycles = CMU_ClockFreqGet(cmuClock_IADCCLK)/1000;

  initAllConfigs.configs[0].reference = iadcCfgReferenceInt1V2;
  initAllConfigs.configs[0].vRef = 1210;
  initAllConfigs.configs[0].osrHighSpeed = iadcCfgOsrHighSpeed2x;
  initAllConfigs.configs[0].analogGain = iadcCfgAnalogGain0P5x;


  initAllConfigs.configs[0].adcClkPrescale = IADC_calcAdcClkPrescale(IADC0,
                                  CLK_ADC_FREQ,
                                  0,
                                  iadcCfgModeNormal,
                                  init.srcClkPrescale);

  initScan.triggerSelect = iadcTriggerSelTimer;
  initScan.dataValidLevel = iadcFifoCfgDvl2;
  initScan.showId = true;

  /*
  * Configure entries in the scan table.  CH0 is single-ended from
  * input 0; CH1 is single-ended from input 1.
  */
  scanTable.entries[0].posInput = IADC_INPUT_0_PORT_PIN;
  scanTable.entries[0].negInput = iadcNegInputGnd;
  scanTable.entries[0].includeInScan = true;

  scanTable.entries[1].posInput = IADC_INPUT_1_PORT_PIN;
  scanTable.entries[1].negInput = iadcNegInputGnd;
  scanTable.entries[1].includeInScan = true;

  // Initialize IADC
  IADC_init(IADC0, &init, &initAllConfigs);

  // Initialize scan
  IADC_initScan(IADC0, &initScan, &scanTable);

  // Enable the IADC timer (must be after the IADC is initialized)
  IADC_command(IADC0, iadcCmdEnableTimer);

  // Allocate the analog bus for ADC0 inputs
  GPIO->IADC_INPUT_0_BUS |= IADC_INPUT_0_BUSALLOC;
  GPIO->IADC_INPUT_1_BUS |= IADC_INPUT_1_BUSALLOC;

  // Enable scan interrupts
  IADC_enableInt(IADC0, IADC_IEN_SCANFIFODVL);

  // Enable ADC interrupts
  NVIC_ClearPendingIRQ(IADC_IRQn);
  NVIC_EnableIRQ(IADC_IRQn);
}

/**************************************************************************//**
* @brief  IADC interrupt handler
*****************************************************************************/
void IADC_IRQHandler(void)
{
  IADC_Result_t sample;

  // While the FIFO count is non-zero...
  while (IADC_getScanFifoCnt(IADC0))
  {
  // Pull a scan result from the FIFO
  sample = IADC_pullScanFifoResult(IADC0);

  scanResult[sample.id] = sample.data * 2.42 / 0xFFF;
  }

  IADC_clearInt(IADC0, IADC_IF_SCANFIFODVL);
}
void IADC_app_init(void)
{

  initCMU();

  initIADC();

  IADC_command(IADC0, iadcCmdStartScan);

}

/***************************************************************************//**
* Ticking function
******************************************************************************/
float RandomFloat(float a, float b) {
  float random = ((float) rand()) / (float) 3276700;
  float diff = b - a;
  float r = random * diff;
  return a + r;
}
void IADC_app_process_action(void)
{
  float ph,ec;
    float a = 10,b = 5;
    float c = 100,d = 5;
    ph=a*scanResult[0]+b;
    ec=c*scanResult[1]+d;
   // ec=RandomFloat(0.056,0.088)/10000;
   // a=RandomFloat(0.056,0.088);
   //printf("%f\n", a);
    printf("PH: %f\n\r", ph);
    printf("EC: %f\n\r", ec);
   // printf("%f\n",scanResult[1]);
    sl_sleeptimer_delay_millisecond(100);
}
void sensormeasurement_val_to_buf(int32_t value,
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
sl_status_t bl_ph_measurement_indicate(uint8_t connection,
                                                      float value)
{
  sl_status_t sc;
  uint8_t buf[5] = { 0 };
  sensormeasurement_val_to_buf(value, buf);
  sc = sl_bt_gatt_server_send_indication(
    connection,
    gattdb_ph,
    sizeof(buf),
    buf);
  return sc;
}
sl_status_t bl_ec_measurement_indicate(uint8_t connection,
                                                      float value)
{
  sl_status_t sc;
  uint8_t buf[5] = { 0 };
  sensormeasurement_val_to_buf(value, buf);
  sc = sl_bt_gatt_server_send_indication(
    connection,
    gattdb_ec,
    sizeof(buf),
    buf);
  return sc;
}
