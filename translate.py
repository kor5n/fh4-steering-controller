import pyvjoy
import time

# Connect to the first vJoy device
j = pyvjoy.VJoyDevice(1)  # ID 1

# vJoy axis range: 0 (full left) -> 32768 (center) -> 65535 (full right)
MIN = 0
CENTER = 32768
MAX = 65535

# Simple test loop: sweep left → center → right
while True:
    # Sweep left
    for val in range(CENTER, MIN, -512):
        j.set_axis(pyvjoy.HID_USAGE_X, val)
        time.sleep(0.01)

    # Sweep right
    for val in range(MIN, MAX, 512):
        j.set_axis(pyvjoy.HID_USAGE_X, val)
        time.sleep(0.01)

    # Back to center
    for val in range(MAX, CENTER, -512):
        j.set_axis(pyvjoy.HID_USAGE_X, val)
        time.sleep(0.01)
