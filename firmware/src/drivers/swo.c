#include "stm32f1xx.h"
#include "swo.h"

void swoInit()
{
	uint32_t SWOPrescaler = (72000000 / SWO_SPEED) - 1; /* SWOSpeed in Hz, note that cpuCoreFreqHz is expected to be match the CPU core clock */
	CoreDebug->DEMCR = CoreDebug_DEMCR_TRCENA_Msk; /* enable trace in core debug */
	*((volatile unsigned*) (ITM_BASE + 0x400F0)) = 0x00000002; /* "Selected PIN Protocol Register": Select which protocol to use for trace output (2: SWO NRZ, 1: SWO Manchester encoding) */
	*((volatile unsigned*) (ITM_BASE + 0x40010)) = SWOPrescaler; /* "Async Clock Prescaler Register". Scale the baud rate of the asynchronous output */
	*((volatile unsigned*) (ITM_BASE + 0x00FB0)) = 0xC5ACCE55; /* ITM Lock Access Register, C5ACCE55 enables more write access to Control Register 0xE00 :: 0xFFC */
	ITM->TCR = ITM_TCR_TraceBusID_Msk | ITM_TCR_SWOENA_Msk | ITM_TCR_SYNCENA_Msk | ITM_TCR_ITMENA_Msk; /* ITM Trace Control Register */
	ITM->TPR = ITM_TPR_PRIVMASK_Msk; /* ITM Trace Privilege Register */
	ITM->TER = 1 << SWO_PORT_NUMBER; /* ITM Trace Enable Register. Enabled tracing on stimulus ports. One bit per stimulus port. */
	*((volatile unsigned*) (ITM_BASE + 0x01000)) = 0x400003FE; /* DWT_CTRL */
	*((volatile unsigned*) (ITM_BASE + 0x40304)) = 0x00000100; /* Formatter and Flush Control Register */
}

void swoPrintChar(char c) {
  volatile int timeout;

  /* Check if Trace Control Register (ITM->TCR at 0xE0000E80) is set */
  //if ((ITM->TCR&ITM_TCR_ITMENA_Msk) == 0) { /* check Trace Control Register if ITM trace is enabled*/
  //  return; /* not enabled? */
  //}

  /* Check if the requested channel stimulus port (ITM->TER at 0xE0000E00) is enabled */
  //if ((ITM->TER & (1ul << SWO_PORT_NUMBER))==0) { /* check Trace Enable Register if requested port is enabled */
  //  return; /* requested port not enabled? */
  //}

  timeout = 5000; /* arbitrary timeout value */
  while (ITM->PORT[0].u32 == 0) {
    /* Wait until STIMx is ready, then send data */
    timeout--;
    if (timeout==0) {
      return; /* not able to send */
    }
  }

  ITM->PORT[0].u16 = 0x08 | (c<<8);
}
