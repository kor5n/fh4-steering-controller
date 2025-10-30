import serial 
import time
from pynput.keyboard import Controller, Key

keyboard = Controller()

arduino_port = "COM3"
baud_rate = 9600

ser = serial.Serial(arduino_port, baud_rate, timeout=1)
time.sleep(2)

print("Connected to arduino")

pressed_keys = set()

def press_key(key):
    if key not in pressed_keys:
        pressed_keys.add(key)
        keyboard.press(key)

def release_key(key):
    if key in pressed_keys:
        pressed_keys.remove(key)
        keyboard.release(key)

def move(string):
    if "Gas" in string:
        if "on" in string:
            press_key("w")
        else:
            release_key("w")
    
    if "Steer" in string:
        if "forward" in string:
            release_key("a")
            release_key("d")
        elif "right" in string:
            release_key("a")
            press_key("d")
        elif "left" in string:
            release_key("d")
            press_key("a")

while True:
    if ser.in_waiting > 0:
        line = ser.readline().decode("utf-8").strip()
        #print(f"Received: {line}")
        move(line)
