# TCP Port Scanner

A command-line TCP port scanner written in C for Linux. Given a target host and
a range of ports, it reports which ports are open by attempting a TCP connection
to each one.

Built with the standard POSIX socket API (`socket()`, `connect()`), and developed
and tested on Linux.

---

## What it does

- Takes a **target IP**, **start port**, and **end port** as command-line arguments.
- Validates input: argument count, IP format, and port range.
- Attempts a TCP connection to each port in the range (a "TCP connect scan").
- Reports the ports that are **open**.

A port is reported open when a full TCP connection succeeds — i.e. a service is
listening and accepting connections on that port.

## Usage

```
./scanner <target-ip> <start-port> <end-port>
```

Example:

```
$ ./scanner 127.0.0.1 20 100
Port 22: open
```

(Port 22 = SSH, which is running on the test machine.)

## Building

Requires `gcc` (install on RHEL-family systems with `sudo dnf install gcc`).

```
gcc scanner.c -o scanner
```

Then run it as shown in Usage above.

## How it works

For each port in the requested range, the scanner:

1. Creates a TCP socket (`socket(AF_INET, SOCK_STREAM, 0)`).
2. Fills in a `sockaddr_in` with the target IP and the current port
   (the port is converted to network byte order with `htons`).
3. Calls `connect()`. A return value of `0` means the connection succeeded, so
   the port is open; a non-zero return means it is closed or filtered.
4. Closes the socket before moving to the next port.

Input is validated before use: the program checks that the required arguments are
present, that the IP address parses, and that the port range is sane
(`1`–`65535`, with start ≤ end).

## Limitations / future work

- **Sequential and blocking.** Ports are scanned one at a time, and `connect()`
  blocks until it succeeds or the OS times out. On remote hosts, filtered ports
  can make each attempt slow, so large scans of remote targets are slow.
- **No connection timeout.** A future version could use non-blocking sockets with
  `select()` to cap how long each port attempt waits.
- **Not concurrent.** A future version could scan many ports at once using
  threads, dramatically speeding up large ranges.
- **TCP connect scan only.** A more advanced version could use raw sockets to
  perform a SYN ("half-open") scan.


## What I learned

A from-scratch project to practice C, the POSIX socket API, and network
programming on Linux. Building on prior work parsing captured packets, this tool
covers the *sending* side: constructing connections, interpreting results, and
handling untrusted command-line input defensively. The "future work" items above
map to operating-systems concepts (blocking vs. non-blocking I/O, `select`-based
event loops, and threaded concurrency) that I'm studying in parallel.

---
Niangado
