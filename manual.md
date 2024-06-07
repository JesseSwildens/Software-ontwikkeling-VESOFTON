# User Manual
Project group: C++ Enjoyers

Names:
- Mirko van de Hoef (1805514)
- Tom Selier (1808444)
- Jesse Swildens (1799049)

EV6A

# High Level Design
![image](doxygen\img\HighLevelDesign.png)

## Application layer
The application layer's function is fluid. Any "module" can be placed here, it's important that the output of this layer is human readable strings, as defined by the project requirements.

An example output could be a string `"clearscherm, zwart"`.

In this project a UART application layer is used to send and receive commands.

Notable function are:


## Business Logic Layer
The goal of the Business Logic Layer (BL) is to convert the human readable string to something the machine can read. Because the output of the application layer is always a human readable string, the BL does not care what kind of application layer is attached to it. For all it cares, it communicates with a user in Morse code. 

The BL calls functions directly from the API layer, which will be described next.

Notable functions are:


## API-layer
todo

# Scripts
The implemented scripts are as follows:
- text, x, y, kleur, tekst, fontnaam\*, fontgrootte, fontstijl
- lijn, x1, y1, x2, y2, kleur, dikte
- rechthoek, x, y, breedte, hoogte, kleur, gevuld
- clearscherm, kleur
- bitmap, nr, x, y
- wacht, msec
- herhaal, aantal, hoevaak
- cirkel, x, y, radius
- hyperdrive

*/ Arial, Consolas, FreeMono, FreeSans, FreeSerif

## Hyperdrive
Hyperdrive is a custom command made to upgrade the bandwidth between the MCU and the terminal.
To be more specific the command upgrades the default 115200 Baud to 2000000 Baud. This is required to achieve the required bitrate for the run length encoded pixel data to allow of video streaming over the UART connection.

# Tips 'n Tricks
During the development of this project some tricks were designed in order to get better performance while not interfering with the display. As we tried to minimize the artifacts on screen some workarounds were needed.

## Working principal VGA
For the VGA to work the DMA 2 is used to transfer data from the main memory blocks in SRAM0 or SRAM1 to the GPIOs.
For this project SRAM1 is too small to hold the framebuffer so SRAM0 is used.
In this transfer DMA2 uses the AHB bus and a bus matrix to move the data. For common peripherals like the USART2 which was used for the UART communication between the terminal and MCU, some compromises are needed for when data is transferred.
Either the transfer for the UART needs to mix in between the VGA transfer.
As the transfer is at 14 MHz this can be done. But the synchronization is difficult as the Bus arbiter (1) still follows a round robin with elevated priority (2), but doesn't block the transfer while it happens.
Using the Cortex M4 core to transfer data from the UART would interfere with the image. Also because of the high baudrate selected when "Hyperdrive" would be enabled would make this even more visible. So if the transfer of the UART is already in progress the VGA needs to wait until the transfer is done. This can delay the image with a couple of nanoseconds but in our findings is enough to distort the image. Multiple tests that were done also showed that even while using the core for transfers at these high speeds, the core couldn't keep up and multiple transfer overrun interrupts would be given to signify the slow transfer speed.

To prevent his from happening, either the DMA can be forced to use another 


> 1: Bus arbiter is a logic device inside the bus matrix which connects the correct host peripheral to device peripheral.</br>
2: Can be found inside STMicroelectronics Reference manual RM0090 p.63 and p.308