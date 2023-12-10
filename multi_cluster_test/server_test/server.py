import socket

def handle_client(client_socket):
	data = client_socket.recv(1024)
	
	if not data:
		print("Client disconnected")
		return

	print(f"Received from client: {data.decode('utf-8')}")

	response = "Message received from client"
	client_socket.send(response.encode('utf-8'))

	client_socket.close()

def main():
	server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

	server_address = ('', 8443)
	server_socket.bind(server_address)

	server_socket.listen(5)

	print("Server listening on port 8443...")

	while True:
		client_socket, client_address = server_socket.accept()

		print(f"Accepted connection from {client_address}")

		handle_client(client_socket)

if __name__ == "__main__":
	main()

	

	
