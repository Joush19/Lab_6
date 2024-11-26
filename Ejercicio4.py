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
    duty_cycle = 0
    while True:
        set_motor_speed(duty_cycle)
        duty_cycle += 1
        if duty_cycle > 100:
            duty_cycle = 0
        time.sleep(0.5)
finally:
    pwm.stop()
    GPIO.cleanup()
