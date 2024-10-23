def create_server_config(file_path, start_ip="127.0.0.2", num_servers=100):
    # Function to increment the IP address
    def increment_ip(ip):
        parts = ip.split('.')
        parts[-1] = str(int(parts[-1]) + 1)
        return '.'.join(parts)

    # Initial content for each server config block
    server_template = """
server {{
    host: {ip}
    port: 8080
    name:
    timeout: 5

    location / {{
        root: www
        dir-listing: false
        index: html/index.html
        allowed-methods: GET, POST, DELETE
    }}

    location /images {{
        root: www
        dir-listing: true
    }}
}}
"""

    current_ip = start_ip

    # Open the file in append mode to keep adding server blocks
    with open(file_path, 'w') as cfg_file:
        for i in range(num_servers):
            # Format the server block with the current IP
            server_block = server_template.format(ip=current_ip)
            # Write the server block to the file
            cfg_file.write(server_block + "\n")
            # Increment the IP for the next server
            current_ip = increment_ip(current_ip)

    print(f"Generated {num_servers} server configs in {file_path}")


# Usage example
if __name__ == "__main__":
    create_server_config("server.cfg", start_ip="127.0.0.2", num_servers=100)
