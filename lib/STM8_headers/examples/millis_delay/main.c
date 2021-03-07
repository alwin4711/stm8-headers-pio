/**********************
  Test 1ms ISR, millis(), micros() etc.
  
  supported hardware:
    - Sduino Uno (https://github.com/roybaer/sduino_uno)
    - STM8L Discovery board (https://www.st.com/en/evaluation-tools/stm8l-discovery.html)
  
  Functionality:
    - send millis every 500ms
    - use millis() and micros()
**********************/

/*----------------------------------------------------------
    INCLUDE FILES
----------------------------------------------------------*/
#include <stdint.h>
#include <stdio.h>
#include "config.h"
#define _MAIN_          // required for global variables
  #include "timer4.h"
  #include "uart.h"
#undef _MAIN_


/**
  \fn int putchar(int byte)
   
  \brief output routine for printf()
  
  \param[in]  data   byte to send
  
  \return  sent byte

  implementation of putchar() for printf(), using selected output channel.
  Use send routine set via putchar_attach()
  Return type depends on used compiler (see respective stdio.h)
*/
#if defined(__CSMC__)
  char putchar(char data) {
#else // Standard C
  int putchar(int data) {
#endif

  // send byte
  UART_write(data);
  
  // return sent byte
  return(data);
  
} // putchar



/////////////////
//    main routine
/////////////////
void main (void) {

  uint32_t  nextPrint = 0;
  
  // disable interrupts
  DISABLE_INTERRUPTS();

  // switch to 16MHz (default is 2MHz)
  sfr_CLK.CKDIVR.byte = 0x00;
  
  // configure LED pin as output
  #if defined(STM8L_DISCOVERY)
    sfr_PORTE.DDR.DDR7 = 1;     // input(=0) or output(=1)
    sfr_PORTE.CR1.C17  = 1;     // input: 0=float, 1=pull-up; output: 0=open-drain, 1=push-pull
    sfr_PORTE.CR2.C27  = 1;     // input: 0=no exint, 1=exint; output: 0=2MHz slope, 1=10MHz slope
  #elif defined(SDUINO)
    sfr_PORTC.DDR.DDR5 = 1;     // input(=0) or output(=1)
    sfr_PORTC.CR1.C15  = 1;     // input: 0=float, 1=pull-up; output: 0=open-drain, 1=push-pull
    sfr_PORTC.CR2.C25  = 1;     // input: 0=no exint, 1=exint; output: 0=2MHz slope, 1=10MHz slope
  #endif
    
  // init UART for 19.2kBaud
  UART_begin(19200);
  
  // init 1ms interrupt
  TIM4_init();
  
  // enable interrupts
  ENABLE_INTERRUPTS();   
  
    
  // main loop
  while(1) {
  
    // for low-power wait for next timer interrupt
    WAIT_FOR_INTERRUPT();
    
    // print time every 500ms. Either non-blocking or blocking
    if (millis() >= nextPrint) {
    //if (1) {
      printf("time %ld us\n", micros());
      
      // toggle LED
      #if defined(STM8L_DISCOVERY)
        sfr_PORTE.ODR.ODR7 ^= 1;
      #elif defined(SDUINO)
        sfr_PORTC.ODR.ODR5 ^= 1;
      #endif
      
      nextPrint += 500;
    }
    //delay(500);
    
  } // main loop

} // main

/*-----------------------------------------------------------------------------
    END OF MODULE
-----------------------------------------------------------------------------*/
