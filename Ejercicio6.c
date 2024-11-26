#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_types.h"
#include "inc/hw_memmap.h"
#include "inc/hw_gpio.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/pwm.h"
#include "driverlib/adc.h"
#include "driverlib/timer.h"

#define LED_PIN GPIO_PIN_1   // Pin for external LED (PF1)
#define MOTOR_PIN GPIO_PIN_6 // Pin for DC motor PWM (PA6)
#define POTENTIOMETER_PIN 3  // Pin for potentiometer (PE3, ADC0)

volatile uint32_t pwmDutyCycle = 0;

void Timer0A_Handler(void)
{
    TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
    
    ADCProcessorTrigger(ADC0_BASE, 3);
    
    while (ADCIntStatus(ADC0_BASE, 3, false) == false);
    ADCIntClear(ADC0_BASE, 3);
    
    uint32_t potentiometerValue;
    ADCSequenceDataGet(ADC0_BASE, 3, &potentiometerValue);
    
    pwmDutyCycle = (potentiometerValue * 100) / 4095; // Convert 12-bit ADC value to 0-100 range
    
    PWMOutputState(PWM0_BASE, PWM_OUT_0_BIT, true); // Enable PWM output
    PWMPulseWidthSet(PWM0_BASE, PWM_OUT_0, (pwmDutyCycle * 20000) / 100); // Adjust duty cycle
}

int main(void)
{
    SysCtlClockSet(SYSCTL_SYSDIV_4 | SYSCTL_OSC_MAIN | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN);
    
    // Enable peripherals
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF); // LED
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA); // DC motor PWM
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE); // Potentiometer
    SysCtlPeripheralEnable(SYSCTL_PERIPH_PWM0); // PWM
    
    // Enable ADC0 for potentiometer
    SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0);
    ADCSequenceConfigure(ADC0_BASE, 3, ADC_TRIGGER_PROCESSOR, 0);
    ADCSequenceStepConfigure(ADC0_BASE, 3, 0, ADC_CTL_CH3 | ADC_CTL_IE | ADC_CTL_IE0);
    ADCSequenceEnable(ADC0_BASE, 3);
    ADCSequenceInterruptClear(ADC0_BASE, 3);
    
    // Configure LED Pin (PF1)
    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, LED_PIN);
    
    // Configure DC Motor PWM Pin (PA6)
    GPIOPinTypePWM(GPIO_PORTA_BASE, MOTOR_PIN);
    
    // Configure Timer to trigger ADC every 500ms
    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);
    TimerConfigure(TIMER0_BASE, TIMER_CFG_A_PERIODIC);
    TimerLoadSet(TIMER0_BASE, TIMER_A, SysCtlClockGet() / 2); // 500ms interval
    TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
    IntEnable(INT_TIMER0A);
    TimerEnable(TIMER0_BASE, TIMER_A);
    
    // Configure PWM
    PWMGenConfigure(PWM0_BASE, PWM_GEN_0, PWM_GEN_MODE_UP_DOWN | PWM_GEN_MODE_NO_SYNC);
    PWMGenPeriodSet(PWM0_BASE, PWM_GEN_0, 20000);  // 20ms period for PWM (50Hz frequency)
    PWMPulseWidthSet(PWM0_BASE, PWM_OUT_0, 0);     // Start with 0% duty cycle
    PWMGenEnable(PWM0_BASE, PWM_GEN_0);
    
    IntMasterEnable();
    
    while (1) {
        // Wait for Timer interrupt to process potentiometer value and adjust PWM
    }
}
