import RPi.GPIO as GPIO
import time

GPIO.setmode(GPIO.BCM)

IN1 = 17
IN2 = 27
ENA = 18

GPIO.setup(IN1, GPIO.OUT)
GPIO.setup(IN2, GPIO.OUT)
GPIO.setup(ENA, GPIO.OUT)

pwm = GPIO.PWM(ENA, 1000)
pwm.start(0)

def set_motor_direction(direction):
    if direction == "forward":
        GPIO.output(IN1, GPIO.HIGH)
        GPIO.output(IN2, GPIO.LOW)
    elif direction == "reverse":
        GPIO.output(IN1, GPIO.LOW)
        GPIO.output(IN2, GPIO.HIGH)

def set_motor_speed(duty_cycle):
    pwm.ChangeDutyCycle(duty_cycle)

try:
    set_motor_direction("forward")
    duty_cycles = [75, 25, 45, 50]
    for duty_cycle in duty_cycles:
        print(f"Testing with {duty_cycle}% duty cycle")
        set_motor_speed(duty_cycle)
        time.sleep(5)
finally:
    pwm.stop()
    GPIO.cleanup()
