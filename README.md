# 🧠 proc_usage

A lightweight Linux process monitoring tool written in C.  
It displays **CPU**, **Memory**, and **Disk usage** of a specific process using the `/proc` filesystem.

---

## 📌 Features

- Monitor a specific process by PID
- Show:
  - 🔧 CPU usage (%)
  - 💾 Memory usage (%)
  - 💽 Disk usage (mounted volumes)
- Real-time updates every few seconds
- Clean CLI interface with helpful options
- Implemented using POSIX threads and `/proc` parsing (no external dependencies)

---

## 📥 Build Instructions

### 🔧 Requirements

- GCC (e.g., `gcc`, `clang`)
- `make`
- Autotools (`autoconf`, `automake`, etc.)
- Linux OS (uses `/proc`)

### ▶️ Build

The `./build.sh` script configures the build environment using Autotools. After that, run `make` to compile the project.

```bash
git clone https://github.com/reri72/proc_usage.git
cd proc_usage
./build.sh
make
```

- A static library, libusage.a, will be generated in the src/ directory.
- A CLI executable, test, will be created in the test/ directory.

---

## 🚀 Run the Test Executable

```bash
./test/test -p <PID> [options]
```

| Option | Description |
|---|---|
| -p <pid> | **(Required)** Target process ID to monitor |
| -c | Show CPU usage (%) |
| -m | Show memory usage (%) |
| -d | Show disk usage (mounted directories) |
| -h | Display help message |


