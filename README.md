# 🌐 Fast Ethernet Simulation

[![C](https://img.shields.io/badge/C-Programming-A8B9CC?logo=c&logoColor=white)](https://en.wikipedia.org/wiki/C_(programming_language))
[![TCP/IP](https://img.shields.io/badge/TCP/IP-Networking-0078D4?logo=cisco&logoColor=white)](https://en.wikipedia.org/wiki/Internet_protocol_suite)
[![Socket API](https://img.shields.io/badge/Socket-API-4B275F?logo=socket.io&logoColor=white)](https://en.wikipedia.org/wiki/Berkeley_sockets)

## 🌟 Overview

A client-server implementation of Fast Ethernet simulation where each station in the network is emulated by a separate process. The communication switch acts as a central server, while multiple station processes act as clients. The simulation implements socket API programming to facilitate communication between stations through the central server.

&nbsp;

## ✨ Key Features

- **🔄 Fast Ethernet Simulation** - Simulates network behavior and data transmission
- **🖥️ Client-Server Architecture** - Uses TCP protocol for reliable communication
- **🔌 Multiple Stations** - Supports up to 10 concurrent station processes
- **📊 Activity Logging** - Comprehensive logging of all network activities
- **⚡ Concurrent Connections** - Leverages `select()` for handling multiple clients

&nbsp;

## 🛠️ Technology Stack

- **Language**: C Programming
- **Networking**: Socket API, TCP/IP
- **Concurrency**: Select-based multiplexing
- **I/O**: File handling for configuration and logging

&nbsp;

## 🚀 Getting Started

### Prerequisites

- GCC compiler
- Linux/Unix environment (recommended)
- Basic understanding of networking concepts

### Compilation

```bash
# Compile the Communication Switch (Server)
gcc Communication_Switch_Process.c -o switch

# Compile the Station Process (Client)
gcc Station_Process.c -o station
```

### Execution

```bash
# Run the Communication Switch first
./switch

# Run Station Processes in separate terminals
./station <station_number>
```

Where `<station_number>` is an integer between 1 and 10.

&nbsp;

## 🏗️ Project Structure

```
fast-ethernet-simulation/
├── Communication_Switch_Process.c    # Server implementation
├── Station_Process.c                 # Client implementation
├── Station_1_Input.txt               # Station 1 input file
├── Station_2_Input.txt               # Station 2 input file
├── ...
├── Station_10_Input.txt              # Station 10 input file
├── CS_Output.txt                     # Server output log
└── Station_Process_Output_*.txt      # Client output logs
```

&nbsp;

## 📱 Implementation Details

### Communication Switch Process
- Socket creation and binding to a specific port
- Listening for incoming connections
- Using `select()` to handle multiple clients concurrently
- Processing client requests and coordinating communication
- Logging activities to `CS_Output.txt`

### Station Process
- Socket creation and connection to the server
- Reading from station-specific input files
- Sending requests to the server and processing responses
- Logging activities to `Station_Process_Output_<Station Number>.txt`

&nbsp;

## 🔄 Communication Flow

1. The Communication Switch initializes and waits for connections
2. Station Processes connect to the switch
3. Stations read input files and send transmission requests
4. The switch coordinates data frame transmissions between stations
5. All activities are logged to respective output files

&nbsp;

## 📊 Output Files

- **CS_Output.txt**: Logs all server activities including:
  - Connection establishments
  - Frame transmission requests
  - Frame deliveries
  - Error conditions

- **Station_Process_Output_\<Station Number>.txt**: Logs individual station activities:
  - Connection status
  - Frames sent and received
  - Processing details

&nbsp;

## 💻 System Requirements

- Capable of handling 10 concurrent station processes
- Minimal system resource usage
- Fast response time for frame transmission requests

&nbsp;
