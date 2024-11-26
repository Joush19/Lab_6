#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "driverlib/uart.h"
#include "driverlib/pin_map.h"
#include "inc/hw_types.h"
#include "inc/hw_uart.h"

volatile uint32_t distance = 0;

void UART0_Handler(void)
{
    if (UARTCharsAvail(UART0_BASE))
    {
        char received = UARTCharGet(UART0_BASE);
        if (received >= '0' && received <= '9') // Valid number
        {
            distance = distance * 10 + (received - '0');
        }
        else if (received == '\n') // End of number
        {
            // Handle LED logic based on distance
            if (distance > 10)
            {
                GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_1 | GPIO_PIN_0, 0x00);
                GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_4, 0x00);
            }
            else if (distance > 8)
            {
                GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_1, GPIO_PIN_1);
                GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_0, 0x00);
                GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_4, 0x00);
            }
            else if (distance > 6)
            {
                GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_1 | GPIO_PIN_0, GPIO_PIN_1 | GPIO_PIN_0);
                GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_4, 0x00);
            }
            else if (distance > 4)
            {
                GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_1 | GPIO_PIN_0, GPIO_PIN_1 | GPIO_PIN_0);
                GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_4, GPIO_PIN_4);
            }
            else
            {
                GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_1 | GPIO_PIN_0, GPIO_PIN_1 | GPIO_PIN_0);
                GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_4, GPIO_PIN_4);
            }
            distance = 0; // Reset for the next value
        }
    }
}

int main(void)
{
    SysCtlClockSet(SYSCTL_SYSDIV_4 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ);

    // Enable peripherals
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPION);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);

    // Configure GPIO for LEDs
    GPIOPinTypeGPIOOutput(GPIO_PORTN_BASE, GPIO_PIN_1 | GPIO_PIN_0);
    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_4);

    // Configure UART
    GPIOPinConfigure(GPIO_PA0_U0RX);
    GPIOPinConfigure(GPIO_PA1_U0TX);
    GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);
    UARTConfigSetExpClk(UART0_BASE, SysCtlClockGet(), 9600,
                        UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE);
    UARTIntEnable(UART0_BASE, UART_INT_RX);
    IntEnable(INT_UART0);
    IntMasterEnable();

    while (1)
    {
        // Main loop does nothing; UART handles input and LEDs
    }
}
