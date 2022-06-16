# Untitled - By: 27093 - Sat Oct 9 2021

import sensor, image, time

sensor.reset()
sensor.set_pixformat(sensor.RGB565)
sensor.set_framesize(sensor.QVGA)
sensor.skip_frames(time = 2000)

clock = time.clock()

while(True):
    uart = UART(3, 9600, timeout_char = 10)
