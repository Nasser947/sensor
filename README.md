# Sensor ADC Project

This repository has code to read ADC sample files, convert 12-bit raw counts to voltages, compute statistics, and validate input data.

## Repository contents
- `adc.c`, `adc.h` — ADC parsing and conversion
- `io.c`, `io.h` — File I/O and binary parsing
- `stats.c`, `stats.h` — Statistical calculations
- `main.c` — CLI entrypoint
- `logbook.pdf` — Development logbook
- `README.md`
- `fault_report.txt`
- `results.txt`

## Build instructions

### CLion
1. Open the project in CLion by selecting the repository folder.
2. CLion will detect the `CMakeLists.txt` and configure the project automatically.
3. Build with the IDE's build button or `Build | Build Project`.

### `gcc` (command line)
From the repository root run:

```bash
mkdir -p build
cd build
cmake ..
cmake --build .
```

This generates an executable `sensor` in `build/`.

Alternatively, to compile directly with `gcc` (no CMake), run from the repository root:

```bash
gcc -std=c99 -O2 -Wall -I. -o sensor main.c adc.c io.c stats.c
```

This produces an executable named `sensor` in the current directory.

## GitHub repository
https://github.com/Nasser947/sensor

## Submission
- logbook.docx
