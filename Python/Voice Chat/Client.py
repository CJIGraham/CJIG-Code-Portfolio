import socket
import pyaudio
import threading
import time

#Initialise PyAudio
audio = pyaudio.PyAudio()

#Read IP Address, port, and Username from config file if present,
#Create config file and prompt user to fill out information if not
try:
    f = open("config.txt", "r")
    fileLines = f.readlines()
    IP = fileLines[0].split('=')[1][0:-1]
    username = fileLines[1].split('=')[1][0:-1]
    port = int(fileLines[2].split('=')[1])
    f.close()
except:
    f = open("config.txt", "w")
    username = input("Please enter your username: ")
    IP = input("Please enter the Server IP: ")
    port = input("Please enter the Server Port: ")
    f.write("IP Address="+ IP +"\nUsername="+username+"\nPort="+port)
    f.close()


#Set up socket connection
try:
    client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    client_socket.connect((IP, port))
    print("Connected to server")
except:
    print("Connection to server failed")
    time.sleep(3)
    exit(1)


#Send username to server as first message - for connected/disconnected announcements
client_socket.send(username.encode())

#Initialise audio streams
chunk_size = 1024  # number of audio samples per chunk
sample_rate = 44100  # audio sampling rate
playing_stream = audio.open(format=pyaudio.paInt16, channels=1, rate=sample_rate, input=True, output=True, frames_per_buffer=chunk_size)
recording_stream = audio.open(format=pyaudio.paInt16, channels=1, rate=sample_rate, input=True, output=True, frames_per_buffer=chunk_size)

#Function for receive data thread.
#If the message from the server contains 'connected' or 'disconnected',
#then the server has sent an announcement of another client joining or
#leaving, so this will be printed in the console. Otherwise the audio data
#will be output.
def receiveData():
    while True:
        data = client_socket.recv(chunk_size)
        if "Connected".encode() in data or "Disconnected".encode() in data:
            print(data.decode())
        else:
            playing_stream.write(data)

#Function for send data thread.
def sendData():
        while True:
            try:
                data = recording_stream.read(chunk_size)
                client_socket.sendall(data)
            except:
                pass

#Threads have been used to allow parallel processing of sending and receiving data.
receiveThread = threading.Thread(target=receiveData).start()
sendThread = threading.Thread(target=sendData).start()
