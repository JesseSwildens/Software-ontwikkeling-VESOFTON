import pygame
from pygame.locals import *
from OpenGL.GL import *
from OpenGL.GLUT import *
import math

def display():
    # Clear previous frame
    glClear(GL_COLOR_BUFFER_BIT)
    
    # Set drawing color to white
    glColor3f(1, 1, 1)  

    angle = 0

    # Begin drawing a polygon
    glBegin(GL_POLYGON)

    # Loop through 100 points to form the circle
    for i in range(100):
        angle = i * 2 * (math.pi / 100)
        
        # Calculate x and y coordinates based on the angle and radius
        glVertex2f(xo + (math.cos(angle) * rad),
                   yo + (math.sin(angle) * rad))

    glEnd()

    # Update the display
    pygame.display.flip()

if __name__ == "__main__":
    # Initialize pygame
    pygame.init()

    # Take input values for x, y coordinates and radius
    xo, yo, rad = map(float, input("Enter x y radius: ").split())

    display_mode = pygame.OPENGL | pygame.DOUBLEBUF
    
    # Set up display mode and window size
    pygame.display.set_mode((500, 500), display_mode)
    
    # Set the window title
    pygame.display.set_caption("GeeksforGeeks")
    
    # Set background color to green
    glClearColor(0, 1, 0, 1)  
    
    # Setting the coordinate limits for drawing
    gluOrtho2D(-500, 500, -500, 500)  

    running = True
    
    # Event loop to keep the window running and listen for events
    while running:
        for event in pygame.event.get():
            # Check for window close event
            if event.type == pygame.QUIT:
                running = False
        
        # Draw the circle in each frame
        display()

    # Close the pygame window
    pygame.quit()

# This Code Is Contributed By Shubham Tiwari