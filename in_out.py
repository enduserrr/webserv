import socket
import time
import datetime

# Configuration
servers = [
    ("127.0.0.1", 8080),
    ("127.0.0.2", 8081)
]
duration_seconds = 60  # Run for 1 minute
interval_seconds = 0.1   # Attempt every second

def try_connect(host, port):
    """Attempt to connect to host:port, return (success, message)."""
    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    sock.settimeout(1)  # 1-second timeout for connection attempt
    start_time = time.time()
    
    try:
        sock.connect((host, port))
        elapsed = (time.time() - start_time) * 1000  # ms
        return True, f"Connected to {host}:{port} in {elapsed:.2f} ms"
    except socket.timeout:
        return False, f"Failed to connect to {host}:{port}: Timeout"
    except socket.error as e:
        return False, f"Failed to connect to {host}:{port}: {str(e)}"
    finally:
        try:
            sock.close()  # Always disconnect
        except:
            pass

def main():
    print(f"Starting connection test at {datetime.datetime.now()}")
    print(f"Will run for {duration_seconds} seconds, connecting every {interval_seconds} second(s)")
    
    start_time = time.time()
    while time.time() - start_time < duration_seconds:
        loop_start = time.time()
        
        # Try connecting to each server
        for host, port in servers:
            success, message = try_connect(host, port)
            timestamp = datetime.datetime.now().strftime("%Y-%m-%d %H:%M:%S")
            print(f"[{timestamp}] {message}")
        
        # Sleep to maintain 1-second interval
        elapsed = time.time() - loop_start
        if elapsed < interval_seconds:
            time.sleep(interval_seconds - elapsed)
    
    print(f"Test completed at {datetime.datetime.now()}")

if __name__ == "__main__":
    main()