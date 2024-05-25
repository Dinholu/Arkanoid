import pygame
import sys
import os
import threading

SCREEN_WIDTH = 1024
SCREEN_HEIGHT = 640
BRICK_WIDTH = 32
BRICK_HEIGHT = 16
NUM_BRICKS_PER_ROW = 16
NUM_ROWS = 16
MARGIN = 2

# Colors for Light Theme
LIGHT_BACKGROUND = (255, 255, 255)
DARK_GRAY = (169, 169, 169)
BLACK = (0, 0, 0)
GRID_COLOR_LIGHT = (200, 200, 200)
BUTTON_COLOR_LIGHT = (240, 240, 240)
BUTTON_HOVER_COLOR_LIGHT = (200, 200, 200)
PALETTE_BORDER_LIGHT = (0, 0, 0)
TEXT_COLOR_LIGHT = (0, 0, 0)

# Colors for Dark Theme
DARK_BACKGROUND = (30, 30, 30)
LIGHT_GRAY = (200, 200, 200)
GRID_COLOR_DARK = (60, 60, 60)
BUTTON_COLOR_DARK = (50, 50, 50)
BUTTON_HOVER_COLOR_DARK = (100, 100, 100)
PALETTE_BORDER_DARK = (255, 255, 255)
TEXT_COLOR_DARK = (255, 255, 255)

# Brick Colors
COLORS = [
    (243, 243, 243),    # White (W)
    (255, 255, 20),     # Yellow (Y)
    (20, 255, 255),     # Blue1 (B)
    (20, 255, 20),      # Green1 (G)
    (20, 20, 204),      # Blue2 (b)
    (255, 152, 20),     # Orange (O)
    (255, 20, 20),      # Red (R)
    (20, 123, 255),     # Blue3 (L)
    (255, 20, 255),     # Pink (P)
    (166, 166, 166),    # Grey (E)
    (194, 180, 20),     # Gold (D)
]

# Brick Characters
BRICK_CHARS = ['-', 'W', 'Y', 'B', 'G', 'b', 'O', 'R', 'L', 'P', 'E', 'D']

pygame.init()
screen = pygame.display.set_mode((SCREEN_WIDTH, SCREEN_HEIGHT))
pygame.display.set_caption('Level Builder')

# Load custom font
custom_font_path = os.path.join(os.path.dirname(os.path.abspath(__file__)), 'retro.ttf')
font = pygame.font.Font(custom_font_path, 10)
title_font = pygame.font.Font(custom_font_path, 24)

# Current theme (starts with light)
current_theme = 'light'

def apply_theme(theme):
    if theme == 'light':
        return LIGHT_BACKGROUND, GRID_COLOR_LIGHT, BUTTON_COLOR_LIGHT, BUTTON_HOVER_COLOR_LIGHT, PALETTE_BORDER_LIGHT, TEXT_COLOR_LIGHT
    else:
        return DARK_BACKGROUND, GRID_COLOR_DARK, BUTTON_COLOR_DARK, BUTTON_HOVER_COLOR_DARK, PALETTE_BORDER_DARK, TEXT_COLOR_DARK

def draw_title(text_color):
    title_text = title_font.render("Arkanoid Level Builder", True, text_color)
    title_rect = title_text.get_rect(center=(SCREEN_WIDTH // 2, 50))
    screen.blit(title_text, title_rect)

def draw_grid(grid, grid_color):
    for row in range(NUM_ROWS):
        for col in range(NUM_BRICKS_PER_ROW):
            rect = pygame.Rect(200 + col * (BRICK_WIDTH + MARGIN), 100 + row * (BRICK_HEIGHT + MARGIN), BRICK_WIDTH, BRICK_HEIGHT)
            pygame.draw.rect(screen, grid_color, rect, 1)
            if grid[row][col] != 0:
                color = COLORS[grid[row][col] - 1]
                pygame.draw.rect(screen, color, rect)

def draw_palette(selected_brick_type, palette_border, text_color):
    for i, color in enumerate(COLORS):
        rect = pygame.Rect(50, 100 + i * (BRICK_HEIGHT + MARGIN), BRICK_WIDTH, BRICK_HEIGHT)
        pygame.draw.rect(screen, color, rect)
        pygame.draw.rect(screen, palette_border, rect, 1)
        if i + 1 == selected_brick_type:
            pygame.draw.rect(screen, text_color, rect, 3)

def draw_buttons(button_color, button_hover_color, text_color):
    buttons = {
        "save": pygame.Rect(SCREEN_WIDTH - 200, 100, 150, 50),
        "clear": pygame.Rect(SCREEN_WIDTH - 200, 170, 150, 50),
        "toggle_theme": pygame.Rect(SCREEN_WIDTH - 200, 240, 150, 50),
    }
    for name, rect in buttons.items():
        color = button_hover_color if rect.collidepoint(pygame.mouse.get_pos()) else button_color
        pygame.draw.rect(screen, color, rect)
        text = font.render(name.replace('_', ' ').capitalize(), True, text_color)
        text_rect = text.get_rect(center=rect.center)
        screen.blit(text, text_rect)
    return buttons

def main():
    global current_theme

    grid = [[0 for _ in range(NUM_BRICKS_PER_ROW)] for _ in range(NUM_ROWS)]
    running = True
    selected_brick_type = 1

    while running:
        background_color, grid_color, button_color, button_hover_color, palette_border, text_color = apply_theme(current_theme)
        screen.fill(background_color)
        draw_title(text_color)
        draw_grid(grid, grid_color)
        draw_palette(selected_brick_type, palette_border, text_color)
        buttons = draw_buttons(button_color, button_hover_color, text_color)

        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                running = False

            elif event.type == pygame.MOUSEBUTTONDOWN:
                x, y = pygame.mouse.get_pos()
                if x < 150:
                    col = 0
                    row = (y - 100) // (BRICK_HEIGHT + MARGIN)
                    if 0 <= row < len(COLORS):
                        selected_brick_type = row + 1
                elif 200 <= x < 200 + NUM_BRICKS_PER_ROW * (BRICK_WIDTH + MARGIN) and 100 <= y < 100 + NUM_ROWS * (BRICK_HEIGHT + MARGIN):
                    col = (x - 200) // (BRICK_WIDTH + MARGIN)
                    row = (y - 100) // (BRICK_HEIGHT + MARGIN)
                    if 0 <= col < NUM_BRICKS_PER_ROW and 0 <= row < NUM_ROWS:
                        grid[row][col] = selected_brick_type if grid[row][col] == 0 else 0
                else:
                    if buttons["save"].collidepoint(x, y):
                        threading.Thread(target=SaveLevel, args=(grid,)).start()
                    elif buttons["clear"].collidepoint(x, y):
                        grid = [[0 for _ in range(NUM_BRICKS_PER_ROW)] for _ in range(NUM_ROWS)]
                    elif buttons["toggle_theme"].collidepoint(x, y):
                        current_theme = 'dark' if current_theme == 'light' else 'light'

        pygame.display.flip()

    pygame.quit()
    sys.exit()

def SaveLevel(grid):
    levels_dir = os.path.join(os.path.dirname(os.path.abspath(__file__)), 'levels')
    if not os.path.exists(levels_dir):
        os.makedirs(levels_dir)
    
    filename = input("Enter the filename to save the level (without extension): ") + ".txt"
    filepath = os.path.join(levels_dir, filename)
    
    with open(filepath, 'w') as f:
        for row in grid:
            f.write(''.join([BRICK_CHARS[brick] for brick in row]) + '\n')
    print(f"Level saved to {filepath}")

if __name__ == '__main__':
    main()
