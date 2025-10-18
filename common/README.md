# Thread-Safe Structured Logging Library

A high-performance, thread-safe structured logging library for C++ that provides JSON-like output with minimal overhead.

## Features

- **Header-Only**: Single header file implementation - no compilation or linking required
- **Thread-Safe**: Uses mutex-based synchronization to ensure safe concurrent logging
- **Structured Output**: Generates key-value pairs in a consistent format
- **High Performance**: Optimized for minimal overhead with benchmark comparisons
- **Flexible Output**: Supports any `std::ostream` (stdout, files, etc.)
- **Multiple Log Levels**: DEBUG, INFO, WARN, ERR
- **Automatic Timestamps**: Includes millisecond-precision timestamps
- **Type-Aware Formatting**: Automatically quotes strings and handles different data types

## Quick Start

```cpp
#include "logger.hpp"

int main() {
    zap::logger log;
    
    // Basic structured logging
    log << "message" << "Hello World" 
        << "user_id" << 12345 
        << "status" << "success";
    
    return 0;
}
```

## Output Format

The logger produces structured output in the following format:

```
{timestamp=1703123456789 lvl=INFO message="Hello World" user_id=12345 status="success"}
```

### Format Details

- **Timestamp**: Milliseconds since epoch
- **Level**: Log level (DEBUG, INFO, WARN, ERR)
- **Key-Value Pairs**: Alternating keys and values
- **String Quoting**: Strings are automatically quoted with double quotes
- **Numeric Values**: Numbers are output without quotes

## API Reference

### Log Levels

```cpp
enum LEVEL {
    DEBUG,  // Debug information
    INFO,   // General information
    WARN,   // Warning messages
    ERR,    // Error messages
};
```

### Logger Class

#### Constructor
```cpp
logger();                    // Default: outputs to std::cout
logger(std::ostream* out);   // Custom output stream
```

#### Methods
```cpp
void setLvl(LEVEL lvl);           // Set log level
void setOstream(std::ostream* out); // Set output stream
```

#### Streaming Operator
```cpp
template<typename T>
logger operator<<(T arg);
```

The streaming operator accepts any type and automatically formats it appropriately.

## Thread Safety

The library ensures thread safety through:

1. **Per-Stream Mutexes**: Each output stream has its own mutex
2. **Lock Table**: A singleton `OStreamLockTable` manages mutexes for different streams
3. **Atomic Operations**: Log entries are built in thread-local storage before being written

### Thread Safety Guarantees

- ✅ Multiple threads can log simultaneously to different streams
- ✅ Multiple threads can log to the same stream safely
- ✅ No data races or corrupted output
- ✅ Consistent ordering within each log entry

## Performance

The library is designed for high performance with minimal overhead:

### Benchmark Results

Run the included benchmarks to compare performance:

```bash
cd benchmarks
make all
```

This will run performance comparisons against:
- Direct `std::cout` usage
- Operating system logging mechanisms

### Performance Characteristics

- **Low Overhead**: Minimal runtime cost compared to direct output
- **Efficient Memory Usage**: Uses string streams for buffering
- **Fast Locking**: Per-stream mutexes reduce contention
- **Optimized String Handling**: Efficient string formatting and quoting
- **Performance Trade-off**: Approximately 4x slower than direct `std::cout` writes due to structured formatting and thread safety overhead

## Advanced Usage

### Custom Output Streams

```cpp
#include <fstream>

// Log to file
std::ofstream logfile("application.log");
zap::logger file_logger(&logfile);

file_logger << "event" << "user_login" 
           << "user_id" << 12345 
           << "ip" << "192.168.1.1";
```

### Multiple Loggers

```cpp
// Different loggers for different purposes
zap::logger debug_log;
zap::logger error_log;

debug_log.setLvl(zap::LEVEL::DEBUG);
error_log.setLvl(zap::LEVEL::ERR);

// Log to different streams
std::ofstream debug_file("debug.log");
std::ofstream error_file("error.log");

debug_log.setOstream(&debug_file);
error_log.setOstream(&error_file);
```

### Complex Data Types

```cpp
struct User {
    int id;
    std::string name;
};

// Custom types need to be streamable
std::ostream& operator<<(std::ostream& os, const User& user) {
    return os << "User{id=" << user.id << ", name=\"" << user.name << "\"}";
}

User user{123, "John Doe"};
log << "user" << user << "action" << "login";
```

## Building and Integration

### Requirements

- C++20 or later
- Standard library with threading support

### Header-Only Usage

Since this is a header-only library, simply include the header file:

```cpp
#include "logger.hpp"
// No additional compilation or linking required
```

### Compilation

```bash
g++ -std=c++20 -O3 -o your_app your_app.cpp
```

### CMake Integration

```cmake
# Add logger.hpp to your target
target_sources(your_target PRIVATE logger.hpp)

# Set C++ standard
set_target_properties(your_target PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON
)
```

## Best Practices

### 1. Log Level Management
```cpp
// Set appropriate log levels for different environments
#ifdef DEBUG
    log.setLvl(zap::LEVEL::DEBUG);
#else
    log.setLvl(zap::LEVEL::INFO);
#endif
```

### 2. Structured Data
```cpp
// Good: Structured key-value pairs
log << "user_id" << user.id 
    << "action" << "login" 
    << "timestamp" << get_current_time();

// Avoid: Unstructured messages
log << "User " << user.id << " logged in at " << get_current_time();
```

### 3. Performance Considerations
```cpp
// Good: Build log entry in one statement
log << "key1" << value1 << "key2" << value2;

// Avoid: Multiple separate log calls for related data
log << "key1" << value1;
log << "key2" << value2;
```

### 4. Error Handling
```cpp
try {
    // Some operation
    risky_operation();
    log << "status" << "success" << "operation" << "risky_operation";
} catch (const std::exception& e) {
    log << "status" << "error" 
        << "operation" << "risky_operation" 
        << "error" << e.what();
}
```

## Troubleshooting

### Common Issues

1. **Compilation Errors**: Ensure C++20 support is enabled
2. **Performance Issues**: Check if multiple threads are contending for the same stream
3. **Output Formatting**: Verify that custom types have proper `operator<<` implementations

### Debug Mode

Enable debug logging to troubleshoot issues:

```cpp
log.setLvl(zap::LEVEL::DEBUG);
log << "debug_info" << "Logger initialized successfully";
```

## Version History

- **v1.0**: Initial implementation with thread-safe structured logging
- Basic log levels and output formatting
- Performance benchmarks and optimization