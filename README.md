# ECE 473 – RTOS Lab 5: uC/OS-II on TM4C129X

This repository contains the code and design implementation for **Experiment 5: RTOS** from the course **ECE 473: Introduction to Digital Logic** 

---

## Objective

This lab introduced real-time operating system (RTOS) concepts using **uC/OS-II** on the **TM4C129XNCZAD (Tiva C Series)** microcontroller. Students gained hands-on experience with:

- Multitasking and task prioritization
- Event flags for task synchronization
- Mutexes for shared resource protection (UART)
- Peripheral interaction (LEDs and push buttons)

---

## System Overview

Four concurrent tasks were implemented:

| Task Name       | Priority | Description |
|----------------|----------|-------------|
| `ButtonMonitor` | High     | Monitors push buttons and posts event flags |
| `Blinky`        | Medium   | Blinks an LED at a variable rate when triggered by `SW1` |
| `ButtonAlert`   | Low      | Displays a debug message to UART when `SW2` is pressed |
| `DebuggingVars` | Lowest   | Periodically prints RTOS debugging variables |

### Synchronization

- **Event Flags** (`OSFlagPost`, `OSFlagPend`, `OSFlagAccept`)  
- **Mutex** for UART access to prevent race conditions between tasks

### Key uC/OS-II APIs Used

- `OSTaskCreateExt()`, `OSFlagCreate()`, `OSFlagPost()`, `OSFlagPend()`
- `OSMutexCreate()`, `OSMutexPend()`, `OSMutexPost()`

---

## Hardware Used

- **Microcontroller**: TM4C129XNCZAD (Tiva C LaunchPad)
- **Buttons**: SW1 and SW2 (GPIO input)
- **LEDs**: Onboard LEDs (GPIO output)
- **UART**: Terminal output for debug messages
