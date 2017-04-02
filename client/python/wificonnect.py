import network
from utime import sleep
import socket

def connect():
    wlan = network.WLAN(network.STA_IF)
    wlan.active(True)
    wlan.connect(b"bguest")
    print("Connecting to bguest")
    while not wlan.isconnected():
        print("#", end="")
        sleep(1)
    print()
    s = socket.socket()
    s.connect(("1.1.1.1", 80))
    postdata = "buttonClicked=4&redirect_url=&err_flag=0&username=___&password=____&terms=accept&submit=CONNECT"
    s.send("POST /login.html HTTP/1.1\r\nHost: 1.1.1.1\r\nConnection: close\r\nContent-type: application/x-www-form-urlencoded\r\nContent-length: ")
    s.send(str(len(postdata)))
    s.send("\r\n\r\n")
    s.send(postdata)
    recvbuf = s.recv(1024)
    while recvbuf != b'':
        print(recvbuf.decode('ascii'), end='')
        recvbuf = s.recv(1024)
    print()
    return wlan

