# Shared Memory Producer-Consumer

A C++ implementation demonstrating inter-process communication using shared memory with synchronization primitives. This project showcases how to safely share data between processes using Boost.Interprocess library.

## Overview

This project implements a classic producer-consumer pattern using shared memory for inter-process communication. The producer creates shared memory, writes data to it, and notifies the consumer. The consumer waits for the shared memory to be available, reads the data, and cleans up the shared memory.

## Features

- **Shared Memory Communication**: Uses Boost.Interprocess shared memory objects
- **Thread Synchronization**: Implements mutex and condition variables for safe data access
- **Producer-Consumer Pattern**: Demonstrates classic IPC pattern with proper synchronization
- **Exception Handling**: Robust error handling for shared memory operations
- **Modern C++**: Uses C++20 standard with threading support

## Architecture

### SharedBuffer Structure
```cpp
struct SharedBuffer {
    bip::interprocess_mutex mutex;           // Synchronization mutex
    bip::interprocess_condition data_ready; // Condition variable for signaling
    bool has_data;                          // Data availability flag
    char message[256];                      // Shared message buffer
};
```

### Components

1. **Producer Thread**: 
   - Creates shared memory object
   - Writes data to shared buffer
   - Signals consumer when data is ready
   - Uses scoped lock for thread safety

2. **Consumer Thread**:
   - Waits for shared memory to be available
   - Reads data from shared buffer
   - Cleans up shared memory resources
   - Handles connection retries with exception handling

## Dependencies

- **Boost.Interprocess**: For shared memory and synchronization primitives
- **C++20 Compiler**: For modern C++ features
- **CMake 4.0+**: For build system

## Building the Project

### Prerequisites
Make sure you have Boost libraries installed on your system:

**Ubuntu/Debian:**
```bash
sudo apt-get install libboost-all-dev
```

**CentOS/RHEL:**
```bash
sudo yum install boost-devel
```

**macOS (with Homebrew):**
```bash
brew install boost
```

### Build Instructions

1. Clone the repository:
```bash
git clone <repository-url>
cd SharedMemoryConsumerProducer
```

2. Create build directory:
```bash
mkdir build && cd build
```

3. Configure and build:
```bash
cmake ..
make
```

4. Run the executable:
```bash
./SharedMemoryConsumerProducer
```

## Usage

The program runs two threads simultaneously:
- **Producer**: Creates shared memory, writes "Hello from producer!" message, and signals completion
- **Consumer**: Waits for shared memory, reads the message, and cleans up resources

### Expected Output
```
Producer: preparing data...
Consumer: waiting for shared memory...
Consumer: exception caught: No such file or directory
Consumer: waiting for shared memory...
Consumer: connected to shared memory!
Consumer: waiting for data...
Producer: Data ready, notifying consumer
Producer: finishing process.
Consumer: received data: Hello from producer!
Consumer: finishing process.
```

## Key Concepts Demonstrated

### 1. Shared Memory Management
- **Creation**: `bip::shared_memory_object` with `create_only` flag
- **Access**: `bip::mapped_region` for memory mapping
- **Cleanup**: `bip::shared_memory_object::remove()` for resource cleanup

### 2. Synchronization Primitives
- **Mutex**: `bip::interprocess_mutex` for exclusive access
- **Condition Variable**: `bip::interprocess_condition` for signaling
- **Scoped Lock**: `bip::scoped_lock` for RAII-style locking

### 3. Thread Safety
- Proper locking mechanisms prevent race conditions
- Condition variables enable efficient waiting
- Exception handling ensures robust operation

## Technical Details

### Memory Layout
The shared memory contains:
- **Mutex**: 4-8 bytes (platform dependent)
- **Condition Variable**: ~48 bytes
- **Boolean Flag**: 1 byte
- **Message Buffer**: 256 bytes
- **Total**: ~310 bytes

### Synchronization Flow
1. Producer creates shared memory and buffer
2. Consumer attempts to connect (may retry on failure)
3. Consumer waits for data using condition variable
4. Producer writes data and signals consumer
5. Consumer processes data and cleans up

## Error Handling

The implementation includes comprehensive error handling:
- **Connection Retries**: Consumer retries connection if shared memory isn't ready
- **Exception Catching**: Handles `bip::interprocess_exception` gracefully
- **Resource Cleanup**: Ensures shared memory is properly removed

## Learning Objectives

This project demonstrates:
- Inter-process communication using shared memory
- Thread synchronization with mutexes and condition variables
- Modern C++ threading and RAII principles
- Boost.Interprocess library usage
- Producer-consumer pattern implementation

## Future Enhancements

Potential improvements could include:
- Multiple producer/consumer support
- Larger data structures and complex message types
- Performance benchmarking
- Cross-platform compatibility testing
- Configuration file support for message content

## License

This project is for educational purposes. Please ensure you have appropriate licenses for Boost libraries in your deployment environment.
