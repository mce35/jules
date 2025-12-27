import pygame
import random
from constants import *
from board import Board, Tetromino
from ai import AI

def get_shape():
    return Tetromino(5, 0, random.choice(SHAPES))

def draw_text_middle(text, size, color, surface):
    font = pygame.font.SysFont('comicsans', size, bold=True)
    label = font.render(text, 1, color)
    surface.blit(label, (WIDTH/2 - (label.get_width() / 2), HEIGHT/2 - label.get_height()/2))

def draw_grid(surface, grid, sx, sy):
    for i in range(len(grid)):
        pygame.draw.line(surface, (128,128,128), (sx, sy+ i*BLOCK_SIZE), (sx + PLAY_WIDTH, sy + i * BLOCK_SIZE))  # horizontal lines
        for j in range(len(grid[i])):
            pygame.draw.line(surface, (128,128,128), (sx + j*BLOCK_SIZE, sy), (sx + j*BLOCK_SIZE, sy + PLAY_HEIGHT)) # vertical lines

def draw_window(surface, grid, sx, sy, title, score=0):
    font = pygame.font.SysFont('comicsans', 60)
    label = font.render(title, 1, (255,255,255))
    surface.blit(label, (sx + PLAY_WIDTH / 2 - (label.get_width() / 2), 30))

    # Score
    font = pygame.font.SysFont('comicsans', 30)
    label = font.render('Score: ' + str(score), 1, (255,255,255))
    surface.blit(label, (sx + PLAY_WIDTH + 50, sy + PLAY_HEIGHT/2 - 100))

    for y in range(len(grid)):
        for x in range(len(grid[y])):
            pygame.draw.rect(surface, grid[y][x], (sx + x*BLOCK_SIZE, sy + y*BLOCK_SIZE, BLOCK_SIZE, BLOCK_SIZE), 0)

    draw_grid(surface, grid, sx, sy)
    pygame.draw.rect(surface, (255, 0, 0), (sx, sy, PLAY_WIDTH, PLAY_HEIGHT), 5)

def draw_next_shape(shape, surface, sx, sy):
    font = pygame.font.SysFont('comicsans', 30)
    label = font.render('Next Shape', 1, (255,255,255))
    surface.blit(label, (sx + PLAY_WIDTH + 40, sy + PLAY_HEIGHT/2 - 10))

    format = shape.shape[shape.rotation % len(shape.shape)]
    for i, line in enumerate(format):
        row = list(line)
        for j, column in enumerate(row):
            if column == '0':
                pygame.draw.rect(surface, shape.color, (sx + PLAY_WIDTH + 40 + j*BLOCK_SIZE, sy + PLAY_HEIGHT/2 + i*BLOCK_SIZE, BLOCK_SIZE, BLOCK_SIZE), 0)

def main():
    win = pygame.display.set_mode((WIDTH, HEIGHT))
    pygame.display.set_caption('Tetris AI')

    # Player setup
    player_board = Board()
    player_change_piece = False
    player_current_piece = get_shape()
    player_next_piece = get_shape()

    # AI setup
    ai_board = Board()
    ai_change_piece = False
    ai_current_piece = get_shape()
    ai_next_piece = get_shape()
    ai = AI(ai_board)
    ai_target_x, ai_target_rotation = None, None
    best_move = ai.get_best_move(ai_current_piece)
    if best_move:
        ai_target_x, ai_target_rotation = best_move

    clock = pygame.time.Clock()
    fall_time = 0
    fall_speed = 0.27
    level_time = 0
    run = True

    while run:
        player_board.grid = player_board.create_grid(player_board.locked_positions)
        ai_board.grid = ai_board.create_grid(ai_board.locked_positions)

        fall_time += clock.get_rawtime()
        level_time += clock.get_rawtime()
        clock.tick()

        if level_time/1000 > 5:
            level_time = 0
            if fall_speed > 0.12:
                fall_speed -= 0.005

        # AI Paced Movement
        if ai_target_rotation is not None and ai_current_piece.rotation != ai_target_rotation:
            ai_current_piece.rotate()
            if not ai_board.valid_space(ai_current_piece):
                ai_current_piece.rotate()
                ai_current_piece.rotate()
                ai_current_piece.rotate()
        elif ai_target_x is not None:
            if ai_current_piece.x < ai_target_x:
                ai_current_piece.x += 1
                if not ai_board.valid_space(ai_current_piece):
                    ai_current_piece.x -= 1
            elif ai_current_piece.x > ai_target_x:
                ai_current_piece.x -= 1
                if not ai_board.valid_space(ai_current_piece):
                    ai_current_piece.x += 1

        if fall_time/1000 >= fall_speed:
            fall_time = 0
            player_current_piece.y += 1
            if not (player_board.valid_space(player_current_piece)) and player_current_piece.y > 0:
                player_current_piece.y -= 1
                player_change_piece = True

            ai_current_piece.y += 1
            if not (ai_board.valid_space(ai_current_piece)) and ai_current_piece.y > 0:
                ai_current_piece.y -= 1
                ai_change_piece = True

        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                run = False
                pygame.display.quit()
                quit()

            if event.type == pygame.KEYDOWN:
                if event.key == pygame.K_LEFT:
                    player_current_piece.x -= 1
                    if not player_board.valid_space(player_current_piece):
                        player_current_piece.x += 1
                elif event.key == pygame.K_RIGHT:
                    player_current_piece.x += 1
                    if not player_board.valid_space(player_current_piece):
                        player_current_piece.x -= 1
                elif event.key == pygame.K_UP:
                    player_current_piece.rotate()
                    if not player_board.valid_space(player_current_piece):
                        player_current_piece.rotate()
                        player_current_piece.rotate()
                        player_current_piece.rotate()
                if event.key == pygame.K_DOWN:
                    player_current_piece.y += 1
                    if not player_board.valid_space(player_current_piece):
                        player_current_piece.y -= 1

        # Player piece logic
        player_shape_pos = player_board.convert_shape_format(player_current_piece)
        for i in range(len(player_shape_pos)):
            x, y = player_shape_pos[i]
            if y > -1:
                player_board.grid[y][x] = player_current_piece.color

        if player_change_piece:
            for pos in player_shape_pos:
                p = (pos[0], pos[1])
                player_board.locked_positions[p] = player_current_piece.color
            player_current_piece = player_next_piece
            player_next_piece = get_shape()
            player_change_piece = False
            player_board.clear_rows(player_board.grid, player_board.locked_positions)

        # AI piece logic
        ai_shape_pos = ai_board.convert_shape_format(ai_current_piece)
        for i in range(len(ai_shape_pos)):
            x, y = ai_shape_pos[i]
            if y > -1:
                ai_board.grid[y][x] = ai_current_piece.color

        if ai_change_piece:
            for pos in ai_shape_pos:
                p = (pos[0], pos[1])
                ai_board.locked_positions[p] = ai_current_piece.color
            ai_current_piece = ai_next_piece
            ai_next_piece = get_shape()
            ai_change_piece = False
            ai_board.clear_rows(ai_board.grid, ai_board.locked_positions)

            best_move = ai.get_best_move(ai_current_piece)
            if best_move:
                ai_target_x, ai_target_rotation = best_move
            else:
                ai_target_x, ai_target_rotation = None, None

        win.fill(BLACK)
        draw_window(win, player_board.grid, PLAYER_TOP_LEFT_X, PLAYER_TOP_LEFT_Y, 'Player', player_board.score)
        draw_window(win, ai_board.grid, AI_TOP_LEFT_X, AI_TOP_LEFT_Y, 'AI', ai_board.score)
        draw_next_shape(player_next_piece, win, PLAYER_TOP_LEFT_X, PLAYER_TOP_LEFT_Y)
        draw_next_shape(ai_next_piece, win, AI_TOP_LEFT_X, AI_TOP_LEFT_Y)
        pygame.display.update()

        if player_board.check_lost(player_board.locked_positions):
            winner = "AI"
            run = False
        elif ai_board.check_lost(ai_board.locked_positions):
            winner = "Player"
            run = False

    if winner == "Player":
        draw_text_middle("You Won!", 80, (0, 255, 0), win)
    else:
        draw_text_middle("You Lost", 80, (255, 0, 0), win)
    pygame.display.update()
    pygame.time.delay(3000)

def main_menu():
    pygame.init()
    run = True
    win = pygame.display.set_mode((WIDTH, HEIGHT))
    while run:
        win.fill((0,0,0))
        draw_text_middle('Press any key to begin.', 60, (255, 255, 255), win)
        pygame.display.update()
        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                run = False
            if event.type == pygame.KEYDOWN:
                main()
    pygame.quit()

if __name__ == '__main__':
    main_menu()
