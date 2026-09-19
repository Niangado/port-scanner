# TCP Port Scanner

A concurrent command-line TCP port scanner written in C for Linux. Given a target
host and a range of ports, it reports which ports are open by attempting a TCP
connection to each one — using a fixed pool of worker threads and per-connection
timeouts so it stays fast and scales to the full 1–65535 port range.

Built with the standard POSIX socket API and POSIX threads (`pthreads`), and
developed and tested on Linux.

---

## Features

- **Threaded scanning with a bounded thread pool.** A fixed number of worker
  threads pull ports from a shared work queue, so scanning a large range uses a
  constant number of threads instead of one-thread-per-port.
- **Per-connection timeout.** Each connection attempt is non-blocking and capped
  with `select()`, so filtered/unresponsive ports fail faster.
  
- **Scales to the full port range.** Handles `1`–`65535` with a small, fixed
  worker pool.
- **Input validation.** Checks argument count, IP format, and that the port range
  is sane (`1`–`65535`, start ≤ end).
- **Thread-safe output.** Results are printed inside a critical section so
  concurrent workers don't interleave their output.

## Usage

```
./scanner <target-ip> <start-port> <end-port>
```

Example:

```
$ ./scanner 127.0.0.1 1 65535
Port 22: open
Port 631: open
Port 9090: open
```


## How it works

**Work queue + thread pool.** The program builds a queue of all ports in the
requested range, then spawns a fixed number of worker threads. Each worker loops:
It takes the next port from the shared queue (guarded so no two workers take the
same one), scans it, and repeats until the queue is drained. This keeps the thread
count constant regardless of how many ports are scanned.

**Scanning a single port (with timeout).** For each port, a worker:

1. Creates a TCP socket and sets it to **non-blocking** with `fcntl`.
2. Calls `connect()`, which returns immediately (the connection is now in
   progress).
3. Uses `select()` with a timeout to wait for the socket to become writable
   (which signals the connection attempt has finished).
4. If `select` times out, the port is treated as filtered/closed. If the socket
   becomes ready, `getsockopt(SO_ERROR)` checks whether the connection actually
   *succeeded* — success means the port is open.
5. Closes the socket.

**Synchronization.** The shared queue index is protected so workers never grab the
same port, and the result output is protected so concurrent prints don't
interleave.

## What I learned

A from-scratch project to practice C, the POSIX socket API, and concurrent systems
programming on Linux. It began as a simple sequential connect-scanner and grew,
in stages, into a concurrent tool — each stage applying operating-systems concepts
I was studying in parallel: **threads** and **mutual exclusion** for the worker
pool, a **concurrent work queue** shared safely across threads, and **`select`**
(usually taught for event-based concurrency) used here for its timeout capability
*inside* a thread pool. Building it reinforced how these concurrency primitives
compose into a real tool.

---

Niangado
