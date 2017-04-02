from machine import ADC
from utime import sleep
adc = ADC(0)

THRESHOLD = 5

state = -1
nextState = -1
count = 0

def classify(reading):
    return 0 if reading > 25 else 1

def heartbeat(state):
    print(state)

while True:
    classification = classify(adc.read())
    if classification == nextState:
        count += 1
        if count >= THRESHOLD and nextState != state:
            heartbeat(nextState)
            state = nextState
    else:
        nextState = classification
        count = 0
    sleep(1)

