import socket
import pyaudio
import threading

#Initialise PyAudio
audio = pyaudio.PyAudio()

#Read port from config file if present,
#Create config file and prompt user to fill out port if not
try:
    f = open("serverConfig.txt", "r")
    fileLines = f.readlines()
    port = int(fileLines[0].split('=')[1])
    f.close()
except:
    f = open("serverConfig.txt", "w")
    port = input("Please enter the Server Port: ")
    f.write("Port="+port)
    f.close()

#Set up socket connection
host = '0.0.0.0'
server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
server_socket.bind((host, port))
server_socket.listen()
print("Waiting for incoming connections...")

#Array to store all client sockets
client_sockets = []
#Number of audio samples per chunk
chunk_size = 1024
#Audio sampling rate
sample_rate = 44100  

#Function for handle clients thread. 
def handle_client(client_socket, username):
    while True:
        #Try to receive data from client
        try:
            #Receive audio data from client
            data = client_socket.recv(chunk_size)
            #Send recorded audio data to all clients except the sender
            for socket in client_sockets:
                if socket != client_socket:
                    socket.sendall(data)
        #If no data is being received, client has disconnected.
        #This will close the socket, and announce that this user has disconnected.
        except:
            print(username + " Disconnected")
            for socket in client_sockets:
                if socket != client_socket:
                    socket.sendall((username + " Disconnected").encode())
            client_sockets.remove(client_socket)
            client_socket.close()
            break
    
#Accept connection, make a thread to handle each new client in parallel
while True:
    client_socket, addr = server_socket.accept()
    username = client_socket.recv(chunk_size).decode()
    print("Connection from:", username, addr)
    for socket in client_sockets:
        if socket != client_socket:
            socket.sendall((username + " Connected").encode())
    client_sockets.append(client_socket)
    #Create a new thread to handle the client connection
    client_thread = threading.Thread(target=handle_client, args=(client_socket, username))
    client_thread.start()
