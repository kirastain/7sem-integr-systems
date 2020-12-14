import threading
from msg import *


def ProcessMessages():
	while True:
		m = Message.SendMessage(M_BROKER, M_GETDATA)
		if m.Header.Type == M_DATA:
			print("Message from: " +  str(m.Header.From))
			print(m.Data)
		else:
			time.sleep(1)


def Client():
	Message.SendMessage(M_BROKER, M_INIT)
	t = threading.Thread(target=ProcessMessages)
	t.start()

while True:
	print("\n1.Connect to Server \n2.Send global message(only if connected) \n3.Send message to certain user(only if connected) \n0. Exit program ")
	actionId = int(input())
	if (actionId == 1):
		Client()
		print("Your ID is: " + str(Message.ClientID))
	elif (actionId == 2):
		if(Message.ClientID == 0):
			print("Please, connect to server")
			continue
		print("Write your message:")
		Message.SendMessage(M_ALL, M_DATA, input())
	elif (actionId == 3):
		if (Message.ClientID == 0):
			print("Please, connect to server")
			continue
		print("Write reсiever ID:")
		recieverId = int(input())
		print("Write message")
		Message.SendMessage(recieverId, M_DATA, input())
	elif (actionId == 0):
		if (Message.ClientID != 0):
			Message.SendMessage(M_BROKER, M_EXIT)
		break
	else:
		print("Action Unknown")
