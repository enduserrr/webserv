import socket
import time
import sys

SERVER_HOST = "127.0.0.1"
SERVER_PORT = 8080
REQUEST_ENDPOINT = "/www/uploads"
TOTAL_BODY_SIZE = 10 * 1024 * 1024 # 10 MB (size claimed to be sent)
SEND_CHUNK_SIZE = 8192 # per chunk
SEND_TOTAL_BEFORE_CLOSE = 1 * 1024 * 1024 # 1 MB (actually sent)


def create_request_headers(host, port, endpoint, content_length):
    headers = (
        f"POST {endpoint} HTTP/1.1\r\n"
        f"Host: {host}:{port}\r\n"
        f"User-Agent: Python Test Client\r\n"
        f"Content-Type: application/octet-stream\r\n"
        f"Content-Length: {content_length}\r\n"
        f"Connection: keep-alive\r\n"
        "\r\n" 
    )
    return headers.encode('utf-8')

def main():
    print(f"Connecting to {SERVER_HOST}:{SERVER_PORT}...")
    try:
        client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        # Optional: Set a timeout for the connect attempt itself
        # client_socket.settimeout(5)
        client_socket.connect((SERVER_HOST, SERVER_PORT))
        print("Connected.")
        # Optional: Make socket non-blocking if needed, but likely not for this test
        # client_socket.setblocking(False)

        # Headers
        headers = create_request_headers(SERVER_HOST, SERVER_PORT, REQUEST_ENDPOINT, TOTAL_BODY_SIZE)
        print("Sending headers...")
        client_socket.sendall(headers)
        print(f"Headers sent ({len(headers)} bytes).")

        # Send Body (partial)
        print(f"Starting to send body data (will send {SEND_TOTAL_BEFORE_CLOSE / (1024*1024):.2f} MB out of claimed {TOTAL_BODY_SIZE / (1024*1024):.2f} MB)...")
        bytes_sent = 0
        chunk = b'\0' * SEND_CHUNK_SIZE
        while bytes_sent < SEND_TOTAL_BEFORE_CLOSE:
            try:
                sent = client_socket.send(chunk)
                if sent == 0:
                    print("Socket connection broken unexpectedly during send.")
                    break
                bytes_sent += sent
                # Optional: Small delayfor slower sending
                # time.sleep(0.01)
            except socket.error as e:
                print(f"Socket error during send: {e}")
                break

        print(f"Actually sent {bytes_sent} bytes of body data.")

    except socket.timeout:
         print("Connection timed out.")
    except socket.error as e:
        print(f"Socket error during connection/header send: {e}")
    except Exception as e:
        print(f"An unexpected error occurred: {e}")
    finally:
        # Abruptly close socket
        if 'client_socket' in locals() and client_socket:
            print("Closing socket abruptly.")
            client_socket.close()
            print("Socket closed.")

if __name__ == "__main__":
    main()