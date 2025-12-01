import time
import serial
import vgamepad as vg

#Arduino setup
MAPPING = 180
arduino_port = "COM3"
baud_rate = 115200
ser = serial.Serial(arduino_port, baud_rate, timeout=1)
time.sleep(2)
print("Connected to arduino")

#Controller setup
pressed_btns = set()
gamepad = vg.VX360Gamepad()
MAX_AXIS = 32767
CENTER_AXIS = 0
MIN_AXIS = -32768

def press_btn(btn):
    if btn not in pressed_btns:
        if btn == "RT":
            gamepad.right_trigger(255)
        elif btn == "LT":
            gamepad.left_trigger(255)
        else:
            gamepad.press_button(button=btn)
        pressed_btns.add(btn)
        gamepad.update()

def release_btn(btn):
    if btn in pressed_btns:
        if btn == "RT":
            gamepad.right_trigger(0)
        elif btn == "LT":
            gamepad.left_trigger(0)
        else:
            gamepad.release_button(button=btn)
        pressed_btns.remove(btn)  
        gamepad.update()


def main():
    try:
        print("waiting for input")
        while True:
            if ser.in_waiting > 0:
                line = ser.readline().decode("latin-1").strip()

                if "Gas" in line:
                    if "on" in line:
                        press_btn("RT")
                    elif "off" in line:
                        release_btn("RT")
                
                if "Brake" in line:
                    if "turn" in line:
                        press_btn("LT")
                    elif "crap" in line:
                        release_btn("LT")

                if "Steering" in line:
                    try:
                        d_read = float(line.split(" ")[1])
                        if d_read > MAPPING:
                            d_read = MAPPING
                        elif d_read < MAPPING:
                            d_read = 0
                        else:
                            d_read = MAPPING - d_read

                        gamepad.left_joystick(x_value=int(d_read / MAPPING * (-(MIN_AXIS) + MAX_AXIS) - MAX_AXIS -1), y_value=CENTER_AXIS)
                    except:
                        gamepad.left_joystick(x_value=CENTER_AXIS, y_value=CENTER_AXIS)
                    gamepad.update()
                

    except KeyboardInterrupt:
        gamepad.reset()
        gamepad.update() 
        print("RESET STEERING, SCRIPT STOPPING...")

main()