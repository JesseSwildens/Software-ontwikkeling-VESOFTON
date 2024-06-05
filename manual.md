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
todo