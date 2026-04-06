# LPC55S69 USB-CDC FreeRTOS CLI Firmware

This project provides a highly robust, multi-threaded Command Line Interface (CLI) via a USB Virtual COM (CDC) port for the NXP LPC55S69 Microcontroller. It replaces the standard, unscalable bare-metal ISR polling loops with a highly deterministic FreeRTOS architecture, utilizing `StreamBuffers`, `Semaphores`, and the official `FreeRTOS_CLI` extension.

## 🚀 Features

*   **100% FreeRTOS Managed USB Stack:** Native MCU USB hardware interrupts are fully decoupled to prevent CPU locking (`USB_DEVICE_CONFIG_USE_TASK = 1`).
*   **Asynchronous Data Pipelines:** Employs `xRxStreamBuffer` and `xTxStreamBuffer` to pass characters safely between the USB PHY and the application logic without tearing memory.
*   **Semaphore Gated Transmissions:** Eliminates USB controller "Dead-State" hangs by leveraging binary semaphores (`xTxSemaphore`) to tightly lock outbound logic until the physical USB PHY IN-transfer completely finishes.
*   **Dynamic VT100 Emulation:** Supports real-time text visualization and native character deletion (sending VT100 `\b \b` backspaces back to the host console dynamically).
*   **Component-Driven CLI Extensibility:** Modularized CLI commands (`cli_commands.c/h`) built on top of the standard FreeRTOS_CLI configuration.
*   **Dynamic Peripheral Control:** Real-time toggling of onboard hardware components (e.g., Active-Low RGB LED manipulation).

## 🛠 Hardware Configuration (LPC55S69)

### System Clocks & Configuration
*    **Core Clock:** Device operates at standard hardware PLL frequencies initialized via `BOARD_InitBootClocks`.
*    **USB Controller:** Locked to **USB1** PHY (`kUSB_ControllerLpcIp3511Hs0`).

### Onboard RGB LED Mappings
The system explicitly maps the CLI logic directly to the target GPIO matrix. Ensure your configuration aligns with standard PIO Muxing:
*   🔴 **RED:** `PIO1_1` (Port 1, Pin 1)
*   🔵 **BLUE:** `PIO1_2` (Port 1, Pin 2)
*   🟢 **GREEN:** `PIO1_3` (Port 1, Pin 3)
*   *Note: Operated under an active-low (Common Anode) topology, meaning a logic `0` sinks voltage to turn the LED on.*

## 📂 Project Architecture

```text
source/
├── virtual_com.c/h         # Main entrypoint, RTOS Task creation, StreamBuffers, and CDC Callbacks
├── FreeRTOS_CLI.c/h        # Core string parsing, payload interpretation, and callback mapping
├── cli_commands.c/h        # Custom application-level definitions for specific commands (e.g., `rgb`)
├── usb_device_config.h     # High-level USB stack routing overrides (USE_TASK toggles)
├── hardware_init.c         # Clock provisioning and NVIC gating
└── pin_mux.c               # IOCON routing pushing hardware traits onto PIO1_{1,2,3}
```

### Task Thread Map
1.  **`USB_Device_Task` (Priority 4):** OS-level daemon routing bare hardware USB tokens up to the application level.
2.  **`CLI_Task` (Priority 3):** Pops streaming characters dropped by the USB, buffers them into arrays, resolves VT100 logic (backspaces), and ultimately fires arrays into the `FreeRTOS_CLIProcessCommand` matrix.
3.  **`USB_Tx_Task` (Priority 3):** Halts entirely on an RTOS Semaphore. Wakes strictly to dump string data into external Host PCs via the CDC hardware buffer only when the system is not actively busy.

## 💻 Getting Started

### 1. Build and Flash
*   Load the project within **NXP MCUXpresso IDE**.
*   Select the `Debug` configuration and hit **Build** (Requires standard CMSIS/Redlib libraries).
*   Flash over to the evaluation target using a CMSIS-DAP / J-Link debugger.

### 2. Connect Your Terminal
*   Upon connecting the Target USB port to your PC, investigate your OS Device Manager for a new **Serial USB Device (COMxx)**.
*   Attach your preferred terminal emulator (e.g., PuTTY, TeraTerm, screen).
    *   **Baud Rate:** `115200`
    *   **Data Bits:** `8`
    *   **Stop Bits:** `1`
    *   **Parity:** `None`

### 3. Usage
Hit `<ENTER>` to spawn the prompt cursor (`> `).
*   Type `help` to list all dynamically registered capabilities.
*   Type `rgb red`, `rgb blue`, `rgb green`, or `rgb off` to watch hardware state respond dynamically to software abstractions!

## ⚠️ Edge Cases & Known Limitations To Consider

While architecturally rigid, observe the following constraints native to standard RTOS/Embedded mechanics:

1.  **StreamBuffer RX Overflows:** Pushing massive strings instantaneously via automated scripts—exceeding buffer capacity limits native to `xRxStreamBuffer`—will violently drop characters. Size `RX_BUFFER_SIZE` appropriately for extreme burst usage.
2.  **TX Semaphore Deadlocks:** If the physical USB bus drops ungracefully post-transmit but pre-callback, `USB_Tx_Task` technically starves on its binary semaphore. Timeouts (`pdMS_TO_TICKS`) are recommended over `portMAX_DELAY` for production environments facing harsh electrical disconnections.
3.  **Scripting Interference:** The CLI is implicitly modeled for human interaction (visual injected return characters and `\b \b` backspace erasures). Strictly automated M2M (Machine to Machine) Python interfaces may falter attempting to regex parse these visual VT100 modifiers.
4.  **CLI Command Stack Profiling:** Complex local variable structures nestled inside new custom CLI commands can silently blow past the 1024-word stack constraint set on the `CLI_Task`.

---
*Developed & Structurally Validated using NXP FreeRTOS SDK Frameworks.*
