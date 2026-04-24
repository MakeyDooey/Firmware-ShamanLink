# ShamanLink: LPC55S69 Core 0 Firmware

This repository contains the Core 0 firmware for **ShamanLink**, a high-performance programmer, protocol orchestrator, and multi-bus gateway built around the NXP LPC55S69 dual-core microcontroller. This component (Core 0) serves as the master controller and OS orchestrator.

Core 0 handles the heavy lifting: maintaining the FreeRTOS environment, parsing command payloads via a custom Virtual Terminal (VT100) interface, and securely shuffling data between the High-Speed USB bus and external hardware using strict Thread-Safe pipelines.

---

## 🏗 System Architecture & FreeRTOS Integration

The firmware replaces traditional bare-metal polling loops with a highly deterministic, preemptive FreeRTOS architecture. This eliminates ISR (Interrupt Service Routine) blocking and guarantees that high-speed USB traffic does not lock up the CPU.

### 1. Task-Level USB Processing
Instead of handling heavy USB transactions directly inside the hardware interrupt vectors, we leverage the NXP OS abstraction (`USB_DEVICE_CONFIG_USE_TASK = 1`).
- The native USB hardware merely sets an OS flag when a packet arrives.
- A high-priority background task (`USB_Device_Task` @ Priority 4) wakes up, manages the USB protocol stack, and extracts the payload gracefully.

### 2. The Communication Pipeline (StreamBuffers)
Passing byte arrays directly between tasks is extremely prone to memory corruption. This firmware implements thread-safe data pipelines using **FreeRTOS StreamBuffers**.
- **`xRxStreamBuffer` (512B):** When the Host PC sends a character, the USB CDC Callback (`USB_DeviceCdcVcomCallback`) pushes the raw byte into this buffer using `xStreamBufferSendFromISR`. The buffer securely bridges the gap between the interrupt context and the application.
- **`xTxStreamBuffer` (1024B):** When the application needs to respond (e.g., printing "Hello World" or echoing a character), it writes to this buffer.

### 3. TX Gating & Semaphore Flow Control
A common failure in embedded USB applications is attempting to push a new transmission to the physical pins while the previous transmission is still active, causing buffer overruns or drops.
- We utilize a dedicated `USB_Tx_Task` and a Binary Semaphore (`xTxSemaphore`).
- The `USB_Tx_Task` waits indefinitely for data to arrive in `xTxStreamBuffer`. When data is present, it attempts to `Take` the semaphore. 
- If the hardware PHY is busy transmitting, the semaphore is locked, forcing the `USB_Tx_Task` to sleep and queue data seamlessly.
- Once the hardware finishes transmitting, the PHY interrupt callback `Gives` the semaphore back, instantly waking the `USB_Tx_Task` to pump the next payload.

---

## 💻 The Virtual Terminal (CLI Engine)

ShamanLink features a rich, VT100-compatible Command Line Interface (CLI) managed by the dedicated `CLI_Task` and backed by the industry-standard `FreeRTOS_CLI` subsystem.

### Modular Command Structure
To keep the codebase maintainable, all CLI logic is modularized in `cli_commands.c` and `cli_commands.h`. The system parses space-delimited arguments securely using robust stack-safe logic (via `configCOMMAND_INT_MAX_OUTPUT_SIZE 256` in `FreeRTOS_CLI.h`).

### Interactive Feedback & Backspace Logic
The `CLI_Task` operates recursively, parsing raw bytes pulled from the `xRxStreamBuffer`. It provides true terminal emulation:
- **Character Echoing:** Every valid character typed is instantly piped back into the `xTxStreamBuffer` so the user sees their inputs.
- **Visual Backspacing:** The system intercepts `\b` (0x08) or `DEL` (0x7F) characters. Instead of just dropping the char from memory, the firmware actively injects the sequence `\b \b` (Backspace, Space, Backspace) into the TX pipe, visually erasing the character from the user's terminal window in real-time.
- **Prompt Carriage:** The system autonomously renders the `\r\n> ` prompt whenever a command concludes.

### Available Commands
- `help`: Automatically generated matrix of all registered commands and parameter formatting.
- `rgb <color>`: Manipulates the onboard Common-Anode RGB LED array. Accepts `red`, `blue`, `green`, or `off`.

---

## 🔌 Hardware Abstractions & Pin Muxing

### Active-Low Telemetry RGB LEDs
The firmware interacts dynamically with physical hardware to provide visual telemetry. 
- The target RGB LEDs are wired in a **Common Anode** configuration. This means logic HIGH (`1`) turns the LED OFF, and logic LOW (`0`) sinks the current to turn the LED ON.
- The `rgb` CLI command interacts with macros (`LED_RED_ON()`, `LED_BLUE_OFF()`, etc.) abstracted in `board/board.h`.

### Exact Pin Mappings
The GPIO structure has been explicitly mapped through `pin_mux.c` and correlated to the semantic macros in `board.h` to drive the correct trace pins dynamically without conflicts:
- **Red LED:** `PIO1_1` (Port 1, Pin 1)
- **Blue LED:** `PIO1_2` (Port 1, Pin 2)
- **Green LED:** `PIO1_3` (Port 1, Pin 3)
- Clock generation for the `IOCON` block and `GPIO1` is autonomously initialized inside the `BOARD_InitPins` boot sequence.

---

## 🚀 Future Roadmap: Dual-Core Timing (OpenAMP)

While this architecture establishes an incredibly robust master controller (Core 0), the LPC55S69's primary asset is its secondary core.

- **Core 1 (Worker)** will be spun up to handle physically-demanding protocol bit-banging (such as SWD/JTAG programming or precise RS-485 Direction Enable gating).
- **RPMsg-Lite (OpenAMP)** will be used to pass payloads across the internal silicon bridge from Core 0 to Core 1 securely.
- By isolating the heavy USB Stack & CLI Parsing onto Core 0, Core 1 can achieve perfect nanosecond-level deterministic timing without being interrupted by Host PC activity.

---

## 🛠 Build & Flash Instructions

1. **Prerequisites:** Open the repository using **NXP MCUXpresso IDE**.
2. **Clock Configuration:** Ensure the main clock is configured for 150MHz. The USB1 PHY requires the external 16MHz crystal to synthesize the required 480MHz bus speeds.
3. **Compile:** Execute a standard compilation. The `IOCON_PIO_FUNC0` patch has been applied to `pin_mux.h` to resolve auto-generation config errors from the MCUXpresso pin tools.
4. **Deploy:** Flash over the CMSIS-DAP debugger to the LPC55S69.
5. **Interface:** Connect your Host PC to the newly enumerated "ShamanLink" COM port using a terminal (PuTTY, TeraTerm) configured to **115200 8N1**. Press `<ENTER>` to invoke the prompt.
