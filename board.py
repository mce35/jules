import pygame
from constants import *
import random

class Tetromino:
    def __init__(self, x, y, shape):
        self.x = x
        self.y = y
        self.shape = shape
        self.color = SHAPE_COLORS[SHAPES.index(shape)]
        self.rotation = 0

    def image(self):
        return self.shape[self.rotation % len(self.shape)]

    def rotate(self):
        self.rotation = (self.rotation + 1) % len(self.shape)

class Board:
    def __init__(self):
        self.locked_positions = {}
        self.grid = self.create_grid(self.locked_positions)
        self.score = 0

    def create_grid(self, locked_positions={}):
        grid = [[(0,0,0) for _ in range(10)] for _ in range(20)]
        for y in range(len(grid)):
            for x in range(len(grid[y])):
                if (x,y) in locked_positions:
                    c = locked_positions[(x,y)]
                    grid[y][x] = c
        return grid

    def convert_shape_format(self, shape):
        positions = []
        format = shape.image()

        for i, line in enumerate(format):
            row = list(line)
            for j, column in enumerate(row):
                if column == '0':
                    positions.append((shape.x + j, shape.y + i))

        for i, pos in enumerate(positions):
            positions[i] = (pos[0] - 2, pos[1] - 4)
        return positions

    def valid_space(self, shape):
        accepted_positions = [[(j, i) for j in range(10) if self.grid[i][j] == (0,0,0)] for i in range(20)]
        accepted_positions = [j for sub in accepted_positions for j in sub]
        formatted = self.convert_shape_format(shape)

        for pos in formatted:
            if pos not in accepted_positions:
                if pos[1] > -1:
                    return False
        return True

    def check_lost(self, positions):
        for pos in positions:
            x, y = pos
            if y < 1:
                return True
        return False

    def clear_rows(self, grid, locked):
        inc = 0
        for i in range(len(grid)-1,-1,-1):
            row = grid[i]
            if (0,0,0) not in row:
                inc += 1
                ind = i
                for j in range(len(row)):
                    try:
                        del locked[(j, i)]
                    except KeyError:
                        continue
        if inc > 0:
            for key in sorted(list(locked), key=lambda x: x[1])[::-1]:
                x, y = key
                if y < ind:
                    newKey = (x, y + inc)
                    locked[newKey] = locked.pop(key)
        self.score += inc * 10
