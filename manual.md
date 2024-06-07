# User Manual
Project group: C++ Enjoyers (Groep 8)

Names:
- Mirko van de Hoef (1805514)
- Tom Selier (1808444)
- Jesse Swildens (1799049)

Class: EV6A

# High Level Design
![image](doxygen\img\HighLevelDesign.png)

## Application layer
The application layer's function is fluid. Any "module" can be placed here, it's important that the output of this layer is human readable strings, as defined by the project requirements.

An example output could be a string `"clearscherm, zwart"`.

In this project a UART application layer is used to send and receive commands. HAL was not employed in this project. Instead, all peripherals were initialized using the necessary registers, thereby reducing overhead that would have otherwise been introduced by HAL. Additionally, to prevent artifacts on the screen, DMA1 was used to retrieve the UART data. The use of DMA is further substantiated in the chapter "Working principal VGA + DMA workaround". 

First, the USART2 is initialized. This consists of enabling USART2 and GPIOA peripheral clocks using the RCC peripheral, setting the GPIOA to alternate function mode in the port mode registers and setting the control registers correctly. Additionally, a priority for the USART2 interrupt is set in the NVIC and the idle line interrupt enable flag is set. 

Second, the DMA1 peripheral is initialized. Specifically the DMA1 channel 4 stream 5 is used as this connects the data transfer request to the USART_RX. The initialization of DMA is similar to that of the USART. It starts by enabling the peripheral clock for the specific stream followed by setting the control registers to the desired settings. Lastly, the DMA can be connected by setting the PAR (source address) to the USART2 data register address and the M0AR (destination address) to any desired output buffer.

Lastly, this layer packages the incoming uart (retrieved from the buffer set in the M0AR register) and packages it into a FIFO. Each command is separated by a line feed.


## Business Logic Layer
The goal of the Business Logic Layer (BL) is to convert the human readable string to something the machine can read and translate to executable code.
Because the output of the application layer is always a human readable string, the BL does not care what kind of application layer is attached to it. For all it cares, it communicates with a user in Morse code.

In this project, the logic layer is solely connected to the previous layer by the FIFO. The main body of the logic layer is a parser. When this parser is called, it will process each entry in the FIFO until empty. 

The processing is as follows: 

First, it only checks the first word of the sentence to retrieve the command. This prevents tokenizing an entire string if the command is invalid. Additionally, it is used by the hyperdrive command to prevent tokenization of an entire video frame as this data is processed differently.  

Secondly, if the command is valid, it tokenizes the string and returns a vector of strings containing each token. Additionally, this removes all white spaces and comma's from each token. 

Thirdly, the tokens and the command is send to a function which iterates through a list of command handlers and if it finds a handler with a command matching the given input it executes the associated function with the provided tokens as arguments.

 ```

 void BL_handle_commands(enum commands command, vector<string> tokens)
{
    for (auto cmd : cmdhandler)
    {
        if (cmd.cmd == command)
            cmd.function(tokens);
    }
}

 std::vector<cmd_struct_t> cmdhandler = 
 {
    { .cmd = clearscherm, .function = BL_clearscherm },
    { .cmd = lijn, .function = BL_lijn },
    { .cmd = rechthoek, .function = BL_rechthoek },
    { .cmd = cirkel, .function = BL_cirkel },
    { .cmd = bitmap, .function = BL_bitmap },
    { .cmd = herhaal, .function = BL_herhaal },
    { .cmd = wacht, .function = BL_wacht },
    { .cmd = tekst, .function = BL_tekst },
};
```

To create a more modular code, each command in the script has its own callback function which all have the same layout as can be seen below. It starts off by creating a template of the arguments. For example `"lijn, 10, 10, 20, 20, groen, 2"` has a template of `{ "0", "0", "0", "0", std::string(), "0" }`. This template is then used to check if the order, length and argument type of the incoming arguments are valid.

If the arguments are valid it is allowed to call the API function associated with the incoming command. 

```
int BL_lijn(vector<string> tokens)
{
#ifdef BL_DEBUG_COMMANDS
    log_message("lijn command");
#endif
    CommandTemplate lijnTemplate = { "lijn", { "0", "0", "0", "0", std::string(), "0" } };

    if (!validateArguments(tokens, lijnTemplate))
    {
        log_message("error: invalid arguments for lijn command");
        return -1;
    }

    auto color = BL_get_valid_color(tokens[5]);
    API_draw_line(stoi(tokens[1]), stoi(tokens[2]), stoi(tokens[3]), stoi(tokens[4]), color, stoi(tokens[6]), 0);
    return 0;
}
```

If one wants to add a command, the command first needs to be added to the enum that can be seen below (which can be found in BL_parser.h).
```
enum commands
{
    nocommand = 0,
    clearscherm = 1,
    lijn = 2,
    rechthoek = 3,
    tekst = 4,
    bitmap = 5,
    wacht = 6,
    herhaal = 7,
    cirkel = 8,
    hyperdrive = 9,
};
```

It can than be added in the cmdhandler vector by adding the follwing line.
```
{ .cmd = command_name, .function = callback_name },
```
Once this is done, the callback needs to be added in the BL_callbacks.cpp. 



## API-layer
For the API layer the functionality has strict requirements to allow the interchange ability between different groups and therefore gets a specification that needs to be followed. This specification gives the outline of the function calls and what key parameters should do, this is to prevents arguments being mixed. Extra commands are allowed, but the required commands are given with extra reserved arguments for specific implementations that are group specific which allows for customization. The api calls available are listed in the section "Scripts".

This layer including the VGA Driver is also available in this project as a separate library which can be statically linked into any project which requires this library.
Example usage of this functionality can be found in the repository under `/example_project/`.

Also most functions have error codes and a callback that hooks into a logging interface like UART to convey messages back to the user.
These have 3 levels of severity: Info, Warning and Error.
Info is basic information that is to tell something is working and send information that isn't critical to the functionality.
Warning is errors that aren't blocking to the functionality of the API layer.
An example would be something like a color depth that is larger than supported then the image is capped at white, as this is the largest value possible.
Error is critical impediments to the working of the driver, and therefore in most functions returning error codes with not performing the task that gets called.
An example would be drawing outside the screen.

# Scripts
The implemented scripts are as follows:
- text, x, y, kleur, tekst, fontnaam\*, fontgrootte, fontstijl
- lijn, x1, y1, x2, y2, kleur, dikte
- rechthoek, x, y, breedte, hoogte, kleur, gevuld
- clearscherm, kleur
- bitmap, nr, x, y
- wacht, msec
- herhaal, aantal, hoe vaak
- cirkel, x, y, radius
- hyperdrive

*/ Arial, Consolas, FreeMono, FreeSans, FreeSerif

## Hyperdrive
Hyperdrive is a custom command made to upgrade the bandwidth between the MCU and the terminal.
To be more specific the command upgrades the default 115200 Baud to 2000000 Baud. This is required to achieve the required bitrate for the run length encoded pixel data to allow of video streaming over the UART connection.

# Tips 'n Tricks
During the development of this project some tricks were designed in order to get better performance while not interfering with the display. As we tried to minimize the artifacts on screen some workarounds were needed.
Also some improvements to quality and guarantees to the functionality of the product.

## Working principal VGA + DMA workaround
For the VGA to work the DMA 2 is used to transfer data from the main memory blocks in SRAM0 or SRAM1 to the GPIOs.
For this project SRAM1 is too small to hold the framebuffer so SRAM0 is used.
In this transfer DMA2 uses the AHB bus and a bus matrix to move the data. For common peripherals like the USART2 which was used for the UART communication between the terminal and MCU, some compromises are needed for when data is transferred.
Either the transfer for the UART needs to mix in between the VGA transfer.
As the transfer is at 14 MHz this can be done. But the synchronization is difficult as the Bus arbiter (1) still follows a round robin with elevated priority (2), but doesn't block the transfer while it happens.

Using the Cortex M4 core to transfer data from the UART would interfere with the image as this used the AHB1 to APB1 bridge.
Also because of the high baudrate selected when "Hyperdrive" would be enabled would make this even more visible.
So if the transfer of the UART is already in progress the VGA needs to wait until the transfer is done.
This can delay the image with a couple of nanoseconds but in our findings is enough to distort the image.
Multiple tests that were done also showed that even while using the core for transfers at these high speeds, the core couldn't keep up and multiple transfer overrun interrupts would be given to signify the slow transfer speed.

In this project DMA 1 was used to connect the UART data register to the memory block so the transfers are done over the APB1 bridge of the DMA peripheral as it has it's own to avoid specifically this issue.
This creates another issue as the DMA only gives an interrupt when it's buffer is over the set threshold.
But with UART when idling on the line is when processing can be done as otherwise the transfer is still ongoing.
Therefore in this project the IdleLine interrupt is enabled to give us this information.
So to know when to read the DMA buffer it sacrifices some small artifacts (single pixel) on screen (3).

By using this trick this project achieved a total throughput of around 250 KB/s of realtime image processing bandwidth. With the same time streaming of 4.6 MB/s of video bandwidth.
All using a system clock of 168 MHz, which means every 36 clock cycles a byte of data is send and 670 cycles a byte of run length encoded pixel data is decoded and send to the framebuffer.

> 1: Bus arbiter is a logic device inside the bus matrix which connects the correct host peripheral to device peripheral.</br>
2: Can be found inside STMicroelectronics Reference manual RM0090 p.63 and p.308</br>
3: There is an open issue for the polling of the pointer address of the DMA so the same effect can be achieved as the IdleLine interrupt but without going through the APB1 bridge</br>

## Unit testing
In this project initial attempts were made to make it possible to unit test the project as this verifies that what is made also functions in the same way it is expected of the functionality.
For this purpose and to prevent getting a high barrier to entry the framework chosen is Catch2.
This is a easy to setup framework with all the functionality this project needed to have unit testing.</br>
This framework uses OS functionality to perform the testing, and to simplify the application, the testing applications are designed to use mocks for MCU functionality and run on the Host PC.

For the ability of dynamic switching (Dynamically, as in a define in the CLI changes behavior) using `Set()` variables in CMake CLI commands and `target_compile_options()` to enable and disable functionality in the source code and change files with function sources from MCU specific to Mock functions.