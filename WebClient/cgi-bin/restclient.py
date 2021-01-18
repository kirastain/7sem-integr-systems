from dataclasses import dataclass
import socket, struct, threading, time
import json
from msg import *
from controller import *

HOST = 'localhost'
PORT = 12345

def ProcessReceive():
    while True:
        try:
            print(GetList("Your messages: " Message.ClientID)['msg'])
        except Exception:
            pass
        time.sleep(10)

def Client():
    Message.SendMessage(M_BROKER, M_INIT)

    t = threading.Thread(target=ProcessReceive)
    t.start()
    while True: 
        n = int(input("1. Отправить пользователю \n2. Расслыка \n"))
        if (n == 1):
            id = int(input("Введите id пользователя\n"))
            s = input("Введите текст сообщения\n")
            SendMsg(Message.ClientID, s, id)
        elif (n == 2):
            s = input("Введите текст сообщения\n")
            SendMsg(Message.ClientID, s, M_ALL)
Client()