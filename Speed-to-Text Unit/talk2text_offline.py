import speech_recognition as sr
r = sr.Recognizer()
mic = sr.Microphone()

import time
import serial

ser = serial.Serial(
    port = '/dev/ttyS0',
    baudrate = 19200,
    parity = serial.PARITY_NONE,
    bytesize = serial.EIGHTBITS,
    timeout = 1000,
    stopbits = serial.STOPBITS_ONE
)



text = "Warning: No internet, accuracy may be reduced"
print(text)
ser.write(str(text).encode())
text = "\n"
ser.write(str(text).encode())

while True:
    with mic as source:
        audio = r.listen(source)
    
    try:
        text = r.recognize_sphinx(audio, language = 'en-US', show_all = False)
        print(text)
        text += "\n"
        transmission_length = len(text)
        ser.write(transmission_length.to_bytes(2, 'little'))
        ser.write(str(text).encode())

        
    except:
        text = "..."
        print(text)
