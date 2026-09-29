# Bare-Metal Preemptive Task Scheduler for STM32F446RE

A lightweight, bare-metal preemptive round-robin task scheduler implemented from scratch on the **ARM Cortex-M4** core (**STM32F446RE / NUCLEO-F446RE**).

This project demonstrates how operating system kernels and Real-Time Operating Systems (RTOS) manage multitasking at the register and assembly level without relying on third-party libraries like FreeRTOS.

---

## Features

- **Preemptive Round-Robin Scheduling:** Powered by the Cortex-M SysTick timer generating periodic 1 ms ticks.
- **Hardware-Assisted Context Switching:** Uses the low-priority **PendSV** exception to perform atomic task context saves and restores without disturbing active interrupt handlers.
- **Dual-Stack Pointer Architecture:**
  - **MSP (Main Stack Pointer):** Dedicated to the OS scheduler and exception/interrupt handlers.
  - **PSP (Process Stack Pointer):** Allocated individually to each user task to guarantee stack isolation.
- **Task Control Block (TCB):** Tracks private task states (`READY`, `BLOCKED`), stack top addresses (PSP), and blocking time counters.

---

## Hardware & Toolchain

| Component | Specification |
|---|---|
| **Development Board** | STMicroelectronics NUCLEO-F446RE |
| **Microcontroller** | STM32F446RET6 (ARM Cortex-M4F) |
| **Clock Source** | 16 MHz High-Speed Internal (HSI) RC |
| **Memory** | 512 KB Flash (`0x08000000`), 128 KB SRAM (`0x20000000`) |
| **Debugger** | Onboard ST-LINK/V2-1 via SWD (Serial Wire Debug) + SWO (PB3) |
| **IDE & Toolchain** | STM32CubeIDE / GNU Arm Embedded Toolchain (`arm-none-eabi-gcc`) |

---


## Context Switching Mechanism

When the `SysTick_Handler` fires or a task invokes `task_delay()`:
1. The **SysTick** exception handler triggers a **PendSV** request via the ICSR (`Interrupt Control and State Register`).
2. Cortex-M hardware automatically pushes **Stack Frame 1** (`xPSR`, `PC`, `LR`, `R12`, `R3`, `R2`, `R1`, `R0`) onto the current task's PSP.
3. The `PendSV_Handler`:
   - Saves remaining registers **Stack Frame 2** (`R4`–`R11`) via `STMDB R0!, {R4-R11}`.
   - Saves current PSP into the task's TCB.
   - Calls `update_next_task()` to select the next ready task.
   - Retrieves the new task's PSP from its TCB.
   - Pops `R4`–`R11` via `LDMIA R0!, {R4-R11}`.
   - Updates `PSP` and exits through `BX LR` (`0xFFFFFFFD`), triggering hardware unstacking.

---

## Task Workload

| Task | Delay / Period | Functionality |
|---|---|---|
| **Task 1** | 1000 ms | Blinks On-board Green LED (`PA5`) + prints status |
| **Task 2** | 500 ms | Prints status message |
| **Task 3** | 250 ms | Prints status message |
| **Task 4** | 125 ms | Prints status message |
| **Idle Task** | Background | Runs when all tasks are blocked/waiting |

---

## Repository Structure

```text
├── Inc/
│   ├── main.h                 # Task configurations, stack macros, and prototypes
│   └── led.h                  # LED pin definitions and delay counts
├── Src/
│   ├── main.c                 # Kernel initialization, TCBs, SysTick & PendSV handlers
│   ├── led.c                  # GPIO configuration and task print logging
│   ├── syscalls.c             # Low-level system calls & ITM FIFO trace driver
│   └── sysmem.c               # Dynamic memory allocation (_sbrk)
├── Startup/
│   └── startup_stm32f446retx.s# STM32F446RE vector table & reset routine
├── STM32F446RETX_FLASH.ld     # GNU Linker script (512 KB ROM / 128 KB RAM)
├── .gitignore                 # Excludes build outputs (*.o, *.elf, Debug/)
└── README.md                  # Project documentation
```

---

## How to Build and Run

### Option 1: In STM32CubeIDE

1. Open STM32CubeIDE and import this folder via **File → Open Projects from File System...**
2. Build the project: **Project → Build Project** (`Ctrl + B`).
3. Flash and run: **Run → Debug** (`F11`).

---

## 🖥️ Viewing ITM Data Console Output

Messages are streamed directly over the Cortex-M single-pin SWO trace without adding UART pin overhead:

1. In STM32CubeIDE, open **Run → Debug Configurations...**
2. In the **Debugger** tab:
   - Check **Enable Serial Wire Viewer (SWV)**.
   - Set **Core Clock** to `16` MHz (matching HSI clock).
3. Start debugging (`F11`).
4. While execution is paused at `main()`:
   - Open **Window → Show View → Other... → SWV → SWV ITM Data Console**.
   - Click the **Configure Trace** icon and check **ITM Stimulus Port 0**.
   - Click the red circle **Start Trace** button.
5. Press **Resume** (`F8`).

---