import pygame
import os

# Initialize Pygame
pygame.init()

# Define constants
font_path = "retro.ttf"  # Ensure this is the correct path to your font file
font_size = 20
sprite_width = 16
sprite_height = 32
chars_per_row = 16
sprite_spacing = 32
output_png = "ascii_sprites.png"
output_bmp = "ascii_sprites.bmp"

# Create a font object
font = pygame.font.Font(font_path, font_size)

# Create a surface for the sprite sheet
sprite_sheet_width = chars_per_row * sprite_spacing
sprite_sheet_height = ((126 - 32) // chars_per_row + 1) * sprite_height
sprite_sheet = pygame.Surface((sprite_sheet_width, sprite_sheet_height), pygame.SRCALPHA, 32)

# Fill the sprite sheet with the characters
for i in range(32, 127):
    char = chr(i)
    char_surface = font.render(char, True, (255, 255, 255))
    x = ((i - 32) % chars_per_row) * sprite_spacing
    y = ((i - 32) // chars_per_row) * sprite_height
    sprite_sheet.blit(char_surface, (x, y))

# Save the sprite sheet as PNG
pygame.image.save(sprite_sheet, output_png)

# Save the sprite sheet as BMP
pygame.image.save(sprite_sheet, output_bmp)

# Quit Pygame
pygame.quit()

print("Sprite sheet saved as PNG and BMP.")
