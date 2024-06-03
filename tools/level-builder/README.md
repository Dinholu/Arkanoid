# Arkanoid Level Generator

This is a simple level generator for the Arkanoid game. You can create custom levels by placing bricks on a grid and saving your designs to a file.

## Requirements

- Python 3.x
- `pygame` library

## Installation

1. Clone this repository or download the `arkanoid-level-builder.py` file.
2. Install the required Python packages using pip:

```sh
pip install -r requirements.txt
```
## Usage

Run the arkanoid_level_generator.py script:

```sh
python arkanoid_level_generator.py
```
The window will open with a grid where you can place bricks.

Use the left mouse button to place bricks. Select a brick type from the palette on the left.

To save your design, click the `Save` button. A dialog will prompt you to enter a filename.

The level will be saved in the levels directory located in the same directory as the script.

To clear the grid, click the `Clear` button.

## Controls
- Left Mouse Button: Place or remove a brick.
- Save Button: Save the current level design.
- Clear Button: Clear the grid.