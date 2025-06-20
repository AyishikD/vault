## 📦 Vault — Redis-Compatible Versioned Key-Value Store

Vault is a lightweight, thread-safe, Redis-compatible key-value store implemented in C++ with support for **versioning**, **commits**, and **checkouts**.

---

### 🚀 Features

* ✅ RESP protocol support (connect using `redis-cli`)
* ✅ Thread-safe TCP server on port `6389`
* ✅ Versioning: `COMMIT`, `CHECKOUT`, `DIFF`, `RESET`
* ✅ Basic Redis commands: `PING`, `SET`, `GET`, `QUIT`
* ✅ Compatible with Redis CLI (`redis-cli`)

---

### Supported Commands
| Command              | Description                       |
| -------------------- | --------------------------------- |
| `PING`               | Health check                      |
| `SET key val`        | Set a key                         |
| `GET key`            | Get a key                         |
| `COMMIT`             | Commit current state as a version |
| `VERSIONS`           | List committed versions           |
| `CHECKOUT <n>`       | Load a specific version           |
| `DIFF a b`           | (No-op for now, returns DIFF OK)  |
| `DELETE_VERSION <n>` | Delete a committed version        |
| `RESET`              | Reset to empty state              |
| `QUIT`               | Disconnect                        |

---

### 💡 DSA Focus: Core Algorithms & Data Structures

This project was built as a **System Design + DSA (SDSA)**-focused implementation. Below are the key DSA techniques used and their role:

#### ✅ Hash Map (`std::unordered_map`)

* **Used in**: `KVStore` for storing key-value pairs efficiently.
* **Why**: Provides `O(1)` average time complexity for `SET`, `GET`, and `DELETE`.

#### ✅ Versioning with Snapshots

* **Used in**: `COMMIT`, `CHECKOUT`, and `VERSIONS` support.
* **Data Structure**: `std::vector<std::unordered_map<std::string, std::string>>` is used to store snapshot copies of the key-value store.
* **Why**: Enables fast rollback and version control.

#### ✅ Thread Safety (Concurrency Control)

* **Used in**: `KVStoreThreadSafe` wrapper with `std::mutex`.
* **Why**: Ensures consistency during concurrent client access, crucial in multi-threaded TCP server.

#### ✅ TCP Server (Networking + Thread Handling)

* **Used in**: Handling simultaneous client connections using `std::thread`.
* **Why**: Efficient multi-client processing without blocking.

#### ✅ Custom RESP Protocol Parsing

* **Used in**: Command decoding from Redis CLI using `parseRESP()`.
* **Why**: Mimics Redis wire protocol (`*`, `$`, `+`, `:`) to support Redis CLI compatibility.

#### ✅ Efficient Serialization

* RESP bulk and simple strings are efficiently constructed using `std::string` and `std::ostringstream` patterns.

This project showcases practical DSA usage in a **systems-level application**, demonstrating how **hashing**, **mutex locking**, and **snapshot-based versioning** can be applied to build production-ready infrastructure.

---

### 🛠️ Build Instructions (Windows)

#### 📦 Requirements

* CMake (v3.16+)
* MSVC / Visual Studio 2022+
* Git
* Redis CLI (for testing)

#### 🧱 Build Steps

```bash
# Clone and enter the project
git clone https://github.com/AyishikD/vault.git
cd vaultkv

# Create build directory
mkdir build && cd build

# Generate Visual Studio solution
cmake .. -A x64

# Build the project
cmake --build . --config Debug

# Run the server
.\Debug\vault.exe
```

---

### 💻 Usage

Once server is running on port `6389`, open another terminal with `redis-cli`:

```bash
redis-cli -p 6389
```

Then, try the following commands:

```redis
> PING
PONG

> SET key1 value1
OK

> COMMIT
(integer) 1

> VERSIONS
1) "1"

> CHECKOUT 1
(integer) 1

> GET key1
"value1"
```

---
