import socket
import os

# Configuration
host = "localhost"
port = 8080
path = "/uploads"
file_path = "photo.jpg"

# Read photo.jpg
with open(file_path, "rb") as f:
    file_data = f.read()

# Create multipart/form-data body
boundary = "------------------------abc123"
form_data = (
    f"--{boundary}\r\n"
    f'Content-Disposition: form-data; name="file"; filename="photo.jpg"\r\n'
    f"Content-Type: image/jpeg\r\n"
    f"\r\n"
    f"{file_data.decode('latin1')}"  # Treat binary as latin1 to preserve bytes
    f"\r\n--{boundary}--\r\n"
)

# Split into chunks (e.g., 512 bytes each)
chunk_size = 512
chunks = [form_data[i:i+chunk_size] for i in range(0, len(form_data), chunk_size)]

# Build chunked body
chunked_body = ""
for chunk in chunks:
    chunked_body += f"{len(chunk.encode()):x}\r\n{chunk}\r\n"
chunked_body += "0\r\n\r\n"

# Full request
request = (
    f"POST {path} HTTP/1.1\r\n"
    f"Host: {host}:{port}\r\n"
    f"Content-Type: multipart/form-data; boundary={boundary}\r\n"
    f"Transfer-Encoding: chunked\r\n"
    f"\r\n"
    f"{chunked_body}"
)

# Send request
sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
sock.connect((host, port))
sock.sendall(request.encode('latin1'))  # Encode to preserve binary
response = sock.recv(4096).decode('latin1')
print(response)
sock.close()