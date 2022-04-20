import sys


import speech_recognition as sr
r = sr.Recognizer()
mic = sr.Microphone()

import time
import serial

ser = serial.Serial(
    port = '/dev/ttyS0',
    baudrate = 9600,
    parity = serial.PARITY_NONE,
    bytesize = serial.EIGHTBITS,
    timeout = 1000,
    stopbits = serial.STOPBITS_ONE
)

counter = 0;

print("System active, transcription of audio input will be displayed below:")

while True:
    with mic as source:
        audio = r.listen(source)
    
    try:
        sys.stdout= open('outfile.txt', 'w')
        text = r.recognize_google(audio, language = 'en-US', show_all = False)
        print(text)
        text += "\n"
        transmission_length = len(text)
        ser.write(transmission_length.to_bytes(2, 'little'))
        ser.write(str(text).encode())
        text = ""
        sys.stdout.close()

        
    except:
        sys.stdout= open('outfile.txt', 'w')
        text = "..."
        counter = counter+1;
        print(counter)
        print("\n")
        text = "\n"
        sys.stdout.close()
