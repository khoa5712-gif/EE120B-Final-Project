# Dinosaur Game - AVR Embedded Systems
**Youtube Link: https://youtube.com/shorts/h7p_Fh6AwQ0**

A hardware-software implementation of a side-scrolling dinosaur game, developed for the **EE120B Embedded Systems** course at UC Riverside.

## Technical Implementation
* **Microcontroller:** ATmega328 (AVR)
* **Architecture:** Concurrent State Machines (SMs) managed logic for rendering, scoring, and user input independently to ensure consistent timing.
* **Peripherals:** Interfaced a **TFT Display** via SPI, push-button inputs, and a **passive buzzer** for real-time audio feedback.
* **Memory Management:** Highly optimized graphics and game state to fit within the limited **MCU SRAM**.
* **Persistence:** Utilized **EEPROM** to save and retrieve high scores across power cycles.

## Key Features
* **Interrupt-Driven Logic:** Tuned frame timing to maintain smooth, responsive gameplay.
* **Custom Graphics:** Managed memory constraints while rendering dynamic game assets on a small-scale display.
