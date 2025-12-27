import random
from board import Tetromino

class AI:
    def __init__(self, board):
        self.board = board
        # Heuristic weights from an online article
        self.w_height = -0.510066
        self.w_lines = 0.760666
        self.w_holes = -0.35663
        self.w_bumpiness = -0.184483

    def get_best_move(self, piece):
        best_move = None
        best_score = -1000000  # Start with a very low score

        # Iterate through all possible rotations
        for rotation in range(len(piece.shape)):
            # Create a copy of the piece to manipulate for each rotation
            temp_piece = Tetromino(5, 0, piece.shape)
            temp_piece.rotation = rotation

            # Iterate through all possible horizontal positions
            for x in range(-2, 11):
                temp_piece.x = x
                temp_piece.y = 0  # Reset y for each new position test

                # First, check if the starting position is valid
                if not self.board.valid_space(temp_piece):
                    continue

                # Now, find the final y position by dropping the piece
                while self.board.valid_space(temp_piece):
                    temp_piece.y += 1
                temp_piece.y -= 1  # Go back to the last valid position

                # Score the move now that the piece is at its final resting place
                score = self.score_move(temp_piece)
                if score > best_score:
                    best_score = score
                    # The "move" is the starting x and rotation
                    best_move = (x, rotation)

        return best_move

    def score_move(self, piece):
        temp_locked = self.board.locked_positions.copy()
        shape_pos = self.board.convert_shape_format(piece)

        # Add the piece to the temporary board state
        for pos in shape_pos:
            p = (pos[0], pos[1])
            temp_locked[p] = piece.color

        # Create a grid representation for easy calculation
        grid = [[(0,0,0) for _ in range(10)] for _ in range(20)]
        for (x, y), color in temp_locked.items():
            if 0 <= x < 10 and 0 <= y < 20:
                grid[y][x] = color

        # Calculate metrics
        lines_cleared = self._calculate_lines_cleared(grid)
        aggregate_height = self._calculate_aggregate_height(grid)
        holes = self._calculate_holes(grid)
        bumpiness = self._calculate_bumpiness(grid)

        # Combine metrics with weights
        score = (self.w_height * aggregate_height) + \
                (self.w_lines * lines_cleared) + \
                (self.w_holes * holes) + \
                (self.w_bumpiness * bumpiness)
        return score

    def _get_column_heights(self, grid):
        heights = [0] * 10
        for x in range(10):
            for y in range(20):
                if grid[y][x] != (0,0,0):
                    heights[x] = 20 - y
                    break
        return heights

    def _calculate_aggregate_height(self, grid):
        return sum(self._get_column_heights(grid))

    def _calculate_lines_cleared(self, grid):
        lines = 0
        for y in range(20):
            if all(cell != (0,0,0) for cell in grid[y]):
                lines += 1
        return lines * lines # Squaring makes clearing more lines much more rewarding

    def _calculate_holes(self, grid):
        holes = 0
        for x in range(10):
            col_has_block = False
            for y in range(20):
                if grid[y][x] != (0,0,0):
                    col_has_block = True
                elif col_has_block:
                    holes += 1
        return holes

    def _calculate_bumpiness(self, grid):
        bumpiness = 0
        heights = self._get_column_heights(grid)
        for x in range(9):
            bumpiness += abs(heights[x] - heights[x+1])
        return bumpiness
