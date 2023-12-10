import socket
from time import sleep

def send_message(message, host, port):
	try:
		client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

		client_socket.connect((host, port))

		client_socket.sendall(message.encode('utf-8'))

		response = client_socket.recv(1024)

		print(f"Received response from server: {response.decode('utf-8')}")

	except Exception as e:
		print(f"Error: {e}")

	finally:
		client_socket.close()

def main():
	server_host = '192.168.49.2'
	server_port = 8443

	message = "Hello, this is a test message"
	
	while True:
		print("Sending message")
		send_message(message, server_host, server_port)
		
		sleep(10)

if __name__ == "__main__":
	main()


