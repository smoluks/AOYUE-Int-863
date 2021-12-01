#include "error.h"
#include "stm32f1xx.h"

void HardFault_Handler()
{
    volatile struct
      {
        uint32_t r0;
        uint32_t r1;
        uint32_t r2;
        uint32_t r3;
        uint32_t r12;
        uint32_t lr;
        uint32_t pc;
        uint32_t psr;
      }*stack_ptr; //Указатель на текущее значение стека(SP)


      asm(
          "TST lr, #4 \n" //Тестируем 3ий бит указателя стека(побитовое И)
          "ITE EQ \n"   //Значение указателя стека имеет бит 3?
          "MRSEQ %[ptr], MSP  \n"  //Да, сохраняем основной указатель стека
          "MRSNE %[ptr], PSP  \n"  //Нет, сохраняем указатель стека процесса
          : [ptr] "=r" (stack_ptr)
          );

      setError(ERR_HARDFAULT);

      NVIC_SystemReset();
}
