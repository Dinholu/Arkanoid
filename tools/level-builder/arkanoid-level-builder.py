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

DARK_BACKGROUND = (30, 30, 30)
LIGHT_GRAY = (200, 200, 200)
BLACK = (0, 0, 0)
GRID_COLOR = (60, 60, 60)
BUTTON_COLOR = (50, 50, 50)
BUTTON_HOVER_COLOR = (100, 100, 100)
PALETTE_BORDER = (255, 255, 255)
TEXT_COLOR = (255, 255, 255)
COLORS = [
    (80, 80, 80),
    (255, 140, 0),
    (0, 0, 205),
    (0, 255, 0),
    (0, 0, 139),
    (34, 139, 34),
    (255, 0, 0),
    (0, 0, 255),
    (255, 20, 147)
]

pygame.init()
screen = pygame.display.set_mode((SCREEN_WIDTH, SCREEN_HEIGHT))
pygame.display.set_caption('Arkanoid Level Generator')
font = pygame.font.SysFont(None, 24)

def DrawGrid(grid):
    for row in range(NUM_ROWS):
        for col in range(NUM_BRICKS_PER_ROW):
            rect = pygame.Rect(200 + col * (BRICK_WIDTH + MARGIN), 50 + row * (BRICK_HEIGHT + MARGIN), BRICK_WIDTH, BRICK_HEIGHT)
            pygame.draw.rect(screen, GRID_COLOR, rect, 1)
            if grid[row][col] != 0:
                color = COLORS[grid[row][col] - 1]
                pygame.draw.rect(screen, color, rect)

def DrawPalette(selected_brick_type):
    for i, color in enumerate(COLORS):
        rect = pygame.Rect(50, 50 + i * (BRICK_HEIGHT + MARGIN), BRICK_WIDTH, BRICK_HEIGHT)
        pygame.draw.rect(screen, color, rect)
        pygame.draw.rect(screen, PALETTE_BORDER, rect, 1)
        if i + 1 == selected_brick_type:
            pygame.draw.rect(screen, TEXT_COLOR, rect, 3)

def draw_buttons():
    buttons = {
        "save": pygame.Rect(SCREEN_WIDTH - 150, 50, 100, 50),
        "clear": pygame.Rect(SCREEN_WIDTH - 150, 120, 100, 50),
    }
    for name, rect in buttons.items():
        color = BUTTON_HOVER_COLOR if rect.collidepoint(pygame.mouse.get_pos()) else BUTTON_COLOR
        pygame.draw.rect(screen, color, rect)
        text = font.render(name.capitalize(), True, TEXT_COLOR)
        text_rect = text.get_rect(center=rect.center)
        screen.blit(text, text_rect)
    return buttons

def main():
    grid = [[0 for _ in range(NUM_BRICKS_PER_ROW)] for _ in range(NUM_ROWS)]
    running = True
    selected_brick_type = 1

    while running:
        screen.fill(DARK_BACKGROUND)
        DrawGrid(grid)
        DrawPalette(selected_brick_type)
        buttons = draw_buttons()

        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                running = False

            elif event.type == pygame.MOUSEBUTTONDOWN:
                x, y = pygame.mouse.get_pos()
                if x < 150:
                    col = 0
                    row = (y - 50) // (BRICK_HEIGHT + MARGIN)
                    if 0 <= row < len(COLORS):
                        selected_brick_type = row + 1
                elif 200 <= x < 200 + NUM_BRICKS_PER_ROW * (BRICK_WIDTH + MARGIN) and 50 <= y < 50 + NUM_ROWS * (BRICK_HEIGHT + MARGIN):
                    col = (x - 200) // (BRICK_WIDTH + MARGIN)
                    row = (y - 50) // (BRICK_HEIGHT + MARGIN)
                    if 0 <= col < NUM_BRICKS_PER_ROW and 0 <= row < NUM_ROWS:
                        grid[row][col] = selected_brick_type if grid[row][col] == 0 else 0
                else:
                    if buttons["save"].collidepoint(x, y):
                        threading.Thread(target=SaveLevel, args=(grid,)).start()
                    elif buttons["clear"].collidepoint(x, y):
                        grid = [[0 for _ in range(NUM_BRICKS_PER_ROW)] for _ in range(NUM_ROWS)]

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
            f.write(''.join(map(str, row)) + '\n')
    print(f"Level saved to {filepath}")

if __name__ == '__main__':
    main()
