import RPi.GPIO as GPIO
import serial
import time

# GPIO setup for HC-SR04
TRIG = 23
ECHO = 24

GPIO.setmode(GPIO.BCM)
GPIO.setup(TRIG, GPIO.OUT)
GPIO.setup(ECHO, GPIO.IN)

# UART setup
ser = serial.Serial('/dev/serial0', 9600)

def measure_distance():
    GPIO.output(TRIG, True)
    time.sleep(0.00001)
    GPIO.output(TRIG, False)

    while GPIO.input(ECHO) == 0:
        start_time = time.time()

    while GPIO.input(ECHO) == 1:
        stop_time = time.time()

    elapsed = stop_time - start_time
    distance = (elapsed * 34300) / 2  # Distance in cm
    return distance

try:
    while True:
        distance = measure_distance()
        ser.write(f"{int(distance)}\n".encode('utf-8'))  # Send distance
        time.sleep(0.1)
finally:
    GPIO.cleanup()
