import sensor, image, time
import pyb
import math

ROBOT_A = True
YELLOW = 1
BLUE = 2
FRAME_WIDTH = 120
FRAME_HEIGHT = 120
draw = True

# Individual
if ROBOT_A:
    thresholds = [(48, 76, -18, 5, 14, 63),(65, 80, -24, 0, -60, -18)] # Yellow  is first
else:
    thresholds = [(62, 100, -21, 5, 35, 127),(50, 68, -10, 11, -128, -43)] # Yellow  is first

# Superteam
#if ROBOT_A:
    #thresholds = [((39, 57, 5, 40, 8, 57),), ((17, 26, -2, 41, -59, -14),)] # Yellow  is first
#else:
    #thresholds = [((38, 61, 4, 43, 7, 64),), ((16, 21, -1, 23, -35, -16),)] # Yellow  is first

sensor.reset()
LED1 = pyb.LED(2)
LED1.on()
sensor.set_pixformat(sensor.RGB565)
sensor.set_framesize(sensor.QQVGA)
sensor.skip_frames(time = 2000)
sensor.set_auto_gain(False)
sensor.set_auto_whitebal(False, (-4.19219, -6.02073, -4.30291))
sensor.set_brightness(2)
sensor.set_contrast(0)
sensor.set_saturation(1)
sensor.set_windowing((FRAME_WIDTH, FRAME_HEIGHT))
clock = time.clock()
uart = pyb.UART(3, 9600, timeout_char = 10)
LED1.off()

while(True):
    data = [60, 60, 60, 60]
    clock.tick()
    img = sensor.snapshot()
    blobs = img.find_blobs(thresholds, x_stride=2, y_stride = 2, area_threshold = 0, pixel_threshold = 200, merge = False, margin = 23)
    blobs = sorted(blobs, key=lambda blob: -blob.area())
    yellow = None
    blue = None
    for blob in blobs:
        if math.sqrt((blob.cx()-60)**2 + (blob.cy()-60)**2) < 58:
            if data[0] == 60 and data[1] == 60 and blob.code() == YELLOW:
                data[0] = 120 - blob.cx()
                data[1] = 120 - blob.cy()
            if data[2] == 60 and data[3] == 60 and blob.code() == BLUE:
                data[2] = 120 - blob.cx()
                data[3] = 120 - blob.cy()
    uart.writechar(255)
    uart.writechar(255)
    for byte in data:
        uart.writechar(byte)
        if draw:
            img.draw_circle(60, 60, 58)
            img.draw_line(int(round(FRAME_WIDTH)/2 - 10), int(round(FRAME_HEIGHT / 2)), int(round(FRAME_WIDTH / 2) + 10), int(round(FRAME_HEIGHT / 2)))
            img.draw_line(int(round(FRAME_WIDTH)/2), int(round(FRAME_HEIGHT / 2) + 10), int(round(FRAME_WIDTH / 2)), int(round(FRAME_HEIGHT / 2) -10))
            img.draw_line(int(round(FRAME_WIDTH)/2), int(round(FRAME_HEIGHT / 2) + 10), int(round(FRAME_WIDTH / 2)), int(round(FRAME_HEIGHT / 2) -10))
            img.draw_line(round(FRAME_WIDTH / 2), round(FRAME_HEIGHT / 2), 120 - data[2], 120 - data[3])
            img.draw_line(round(FRAME_WIDTH / 2), round(FRAME_HEIGHT / 2), 120 - data[0], 120 - data[1])
