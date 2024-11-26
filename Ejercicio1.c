#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "driverlib/adc.h"
#include "driverlib/timer.h"
#include "driverlib/interrupt.h"

volatile uint32_t adcValue = 0; // Stores ADC reading
volatile uint32_t frequency = 1000; // Initial frequency in Hz

void Timer0A_Handler(void)
{
    TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);

    // Example: Toggle LED based on timer interrupt
    GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_0, ~GPIOPinRead(GPIO_PORTN_BASE, GPIO_PIN_0));
}

void ADC0_Handler(void)
{
    ADCIntClear(ADC0_BASE, 3);

    // Get the ADC value
    ADCSequenceDataGet(ADC0_BASE, 3, &adcValue);

    // Map ADC value to frequency range (e.g., 500 Hz to 2000 Hz)
    frequency = 500 + (adcValue * 1500) / 4095;

    // Update Timer Load Value for new frequency
    TimerLoadSet(TIMER0_BASE, TIMER_A, SysCtlClockGet() / frequency);
}

int main(void)
{
    SysCtlClockSet(SYSCTL_SYSDIV_4 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ);

    // Enable peripherals
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPION);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);

    // Configure GPIO for LED
    GPIOPinTypeGPIOOutput(GPIO_PORTN_BASE, GPIO_PIN_0);

    // Configure ADC
    ADCSequenceConfigure(ADC0_BASE, 3, ADC_TRIGGER_PROCESSOR, 0);
    ADCSequenceStepConfigure(ADC0_BASE, 3, 0, ADC_CTL_CH3 | ADC_CTL_IE | ADC_CTL_END);
    ADCSequenceEnable(ADC0_BASE, 3);

    // Configure Timer 0
    TimerConfigure(TIMER0_BASE, TIMER_CFG_PERIODIC);
    TimerLoadSet(TIMER0_BASE, TIMER_A, SysCtlClockGet() / frequency);
    TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
    TimerEnable(TIMER0_BASE, TIMER_A);

    // Enable interrupts
    IntEnable(INT_TIMER0A);
    IntEnable(INT_ADC0SS3);
    ADCIntEnable(ADC0_BASE, 3);
    IntMasterEnable();

    while (1)
    {
        ADCProcessorTrigger(ADC0_BASE, 3); // Start ADC conversion
        SysCtlDelay(SysCtlClockGet() / 30); // Delay for stability
    }
}
