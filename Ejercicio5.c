#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_types.h"
#include "inc/hw_memmap.h"
#include "inc/hw_gpio.h"
#include "inc/hw_uart.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/pwm.h"
#include "driverlib/timer.h"
#include "driverlib/uart.h"

#define LED_PIN GPIO_PIN_1   // Pin for external LED
#define UART0_BASE 0x4000C000 // Base address for UART0

volatile uint32_t dutyCycle = 0;
volatile uint32_t blinkInterval = 1000; // Default to 1 second

void Timer1A_Handler(void)
{
    TimerIntClear(TIMER1_BASE, TIMER_TIMA_TIMEOUT);
    
    // Toggle LED based on the blink interval
    GPIOPinWrite(GPIO_PORTF_BASE, LED_PIN, GPIOPinRead(GPIO_PORTF_BASE, LED_PIN) ^ LED_PIN);
}

void UART0_Handler(void)
{
    uint32_t data;
    
    if(UARTCharsAvail(UART0_BASE)) {
        data = UARTCharGetNonBlocking(UART0_BASE);
        
        // Assuming the duty cycle is sent as an ASCII character
        if(data >= '0' && data <= '9') {
            dutyCycle = data - '0'; // Convert ASCII to integer
            if (dutyCycle < 50) {
                blinkInterval = 1000; // 1 second blink
            } else {
                blinkInterval = 500; // 0.5 second blink
            }
        }
    }
}

int main(void)
{
    // System setup
    SysCtlClockSet(SYSCTL_SYSDIV_4 | SYSCTL_OSC_MAIN | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN);
    
    // Enable peripherals
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);  // For the LED
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);  // For UART0
    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER1); // For Timer1
    
    // Configure LED Pin (PF1)
    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, LED_PIN);
    
    // UART0 Configuration
    UARTConfigSetExpClk(UART0_BASE, SysCtlClockGet(), 9600, (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE));
    
    // Timer1 Configuration
    TimerConfigure(TIMER1_BASE, TIMER_CFG_A_PERIODIC);
    TimerLoadSet(TIMER1_BASE, TIMER_A, SysCtlClockGet() / (1000 / blinkInterval) - 1); // Load timer with interval based on duty cycle
    TimerIntEnable(TIMER1_BASE, TIMER_TIMA_TIMEOUT);
    IntEnable(INT_TIMER1A); // Enable the interrupt for Timer1A
    TimerEnable(TIMER1_BASE, TIMER_A); // Start Timer1
    
    // Enable global interrupts
    IntMasterEnable();
    
    while (1) {
        // Wait for the UART interrupt to receive the duty cycle
    }
}
