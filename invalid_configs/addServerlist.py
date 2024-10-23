def create_server_config(file_path, start_ip="127.0.0.1", start_port=8080, num_servers=1000, servers_per_port=253):
    # Function to increment the IP address
    def increment_ip(ip):
        parts = ip.split('.')
        parts[-1] = str(int(parts[-1]) + 1)
        return '.'.join(parts)

    # Initial content for each server config block
    server_template = """{ip}:{port}"""

    current_port = start_port
    server_count = 0

    # Open the file in write mode to start adding server blocks
    with open(file_path, 'w') as cfg_file:
        while server_count < num_servers:
            # Reset the IP for each new port block
            current_ip = start_ip

            # For every 253 servers, increment the port
            for _ in range(servers_per_port):
                if server_count >= num_servers:
                    break

                # Format the server block with the current IP and port
                server_block = server_template.format(ip=current_ip, port=current_port)
                # Write the server block to the file
                cfg_file.write(server_block + "\n")
                # Increment the IP for the next server
                current_ip = increment_ip(current_ip)
                server_count += 1

            # After writing 253 server blocks, increment the port and reset IP
            current_port += 1

    print(f"Generated {num_servers} server configs in {file_path}")


# Usage example
if __name__ == "__main__":
    create_server_config("serverlist.txt", start_ip="127.0.0.1", start_port=8080, num_servers=1000)
