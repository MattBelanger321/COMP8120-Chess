import chess
import chess.engine
import random
import copy
import chess.pgn
import numpy as np
import json

import concurrent.futures
from concurrent.futures import ProcessPoolExecutor
import multiprocessing

POPULATION_SIZE = 300
MAX_MOVES = 200 #Each individual will just generate 200 moves. If a victory happens before that, oh well
GENERATIONS = 150

#Note - UCI stands for Universal Chess Interface. It's the chess notation encoding, like e2e4, bd2e3, etc.

#The board is handled via the chess library, and a move is done via the .push() function
stockfish_path = "./stockfish/stockfish-windows-x86-64-avx2"
#stockfish = chess.engine.SimpleEngine.popen_uci(stockfish_path)

def random_weight(center=0.0, spread=0.5):
    return random.uniform(center - spread, center + spread)
    
#Generate the chromosome for the individual, a series of weights for different aspects of chess
def generate_chromosome():
    #Importance of material score
    material_score_bonus = random_weight(1.0, 0.5)

    #Importance of piece mobility, only considering good moves (not shuffling, not hanging a piece, etc)
    piece_mobility_bonus = random_weight(0.1, 0.05)
    
    #Bonus added for having castled (away from danger if applicable), and for keeping castling rights when threatened
    castling_bonus = random_weight(0.5, 0.3)
    
    #Bonus added for developing early and fast
    development_speed_bonus = random_weight(0.2, 0.1)
    
    #Penalty for doubling a pawn
    doubled_pawn_penalty = random_weight(-0.3, 0.2)
    
    #Penalty for isolating a pawn
    isolated_pawn_penalty = random_weight(-0.3, 0.2)
    
    #Bonus for chaining pawns diagonally
    connected_pawn_bonus = random_weight(0.2, 0.1)
    
    #Bonus for having a passed pawn (no opposing pawn on the other side)
    passed_pawn_bonus = random_weight(0.4, 0.2)
    
    #Bonus for pressuring squares around the king
    enemy_king_pressure_bonus = random_weight(0.4, 0.3)
    
    #Bonus for having same-side pieces defended
    piece_defense_bonus = random_weight(0.2, 0.1)
    
    #Bonus for maintaining a bishop pair
    bishop_pair_bonus = random_weight(0.2, 0.1)
    
    #Bonus for keeping rooks connected
    connected_rooks_bonus = random_weight(0.3, 0.1)

    #King should be encouraged to be active towards the end game
    king_centralization_val = random_weight(0.1, 0.05)
    
    #Bonus for having a knight outpost (can't be attacked by an enemy pawn)
    knight_outpost_bonus = random_weight(0.3, 0.2)
    
    #Penalty for having a piece blocked (like a bishop stuck behind pawns)
    blocked_piece_penalty = random_weight(-0.3, 0.2)
    
    #Bonus for how many spaces in the opponents side that are attacked by a piece
    space_control_in_opponent_half_bonus = random_weight(0.2, 0.1)
    
    #Bonus for having pieces in between the king and enemy pieces
    king_shield_bonus = random_weight(0.3, 0.1)
    
    #penalty for pressure on the king from the enemy
    king_pressure_penalty = random_weight(-0.5, 0.2)
    
    #Scores for each piece's position on a board. Ex: Queen is generally better in the center, knight is generally worse on an edge
    pawn_position_weights = np.array([
        [0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00],
        [1.00, 1.00, 1.00, 1.00, 1.00, 1.00, 1.00, 1.00],
        [0.90, 0.90, 0.90, 0.95, 0.95, 0.90, 0.90, 0.90],
        [0.72, 0.72, 0.72, 0.80, 0.80, 0.72, 0.72, 0.72],
        [0.54, 0.54, 0.54, 0.60, 0.60, 0.54, 0.54, 0.54],
        [0.36, 0.36, 0.36, 0.40, 0.40, 0.36, 0.36, 0.36],
        [0.18, 0.18, 0.18, 0.20, 0.20, 0.18, 0.18, 0.18],
        [0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00]
    ]).flatten()
    
    knight_position_weights = np.array([
        [0.10, 0.20, 0.30, 0.30, 0.30, 0.30, 0.20, 0.10],
        [0.20, 0.50, 0.60, 0.60, 0.60, 0.60, 0.50, 0.20],
        [0.30, 0.60, 0.80, 0.80, 0.80, 0.80, 0.60, 0.30],
        [0.30, 0.60, 0.80, 1.00, 1.00, 0.80, 0.60, 0.30],
        [0.30, 0.60, 0.80, 1.00, 1.00, 0.80, 0.60, 0.30],
        [0.30, 0.60, 0.70, 0.70, 0.70, 0.70, 0.60, 0.30],
        [0.20, 0.40, 0.50, 0.50, 0.50, 0.50, 0.40, 0.20],
        [0.10, 0.20, 0.30, 0.20, 0.20, 0.30, 0.20, 0.10]
    ]).flatten()
    
    bishop_position_weights = np.array([
        [0.05, 0.10, 0.20, 0.30, 0.30, 0.20, 0.10, 0.05],
        [0.10, 0.30, 0.40, 0.50, 0.50, 0.40, 0.30, 0.10],
        [0.20, 0.40, 0.60, 0.70, 0.70, 0.60, 0.40, 0.20],
        [0.30, 0.50, 0.70, 1.00, 1.00, 0.70, 0.50, 0.30],
        [0.30, 0.50, 0.70, 1.00, 1.00, 0.70, 0.50, 0.30],
        [0.20, 0.40, 0.60, 0.70, 0.70, 0.60, 0.40, 0.20],
        [0.10, 0.30, 0.40, 0.50, 0.50, 0.40, 0.30, 0.10],
        [0.05, 0.10, 0.20, 0.30, 0.30, 0.20, 0.10, 0.05]
    ]).flatten()
    
    rook_position_weights = np.array([
        [0.50, 0.60, 0.60, 0.60, 0.60, 0.60, 0.60, 0.50],
        [0.60, 0.70, 0.70, 0.70, 0.70, 0.70, 0.70, 0.60],
        [0.60, 0.70, 0.80, 0.80, 0.80, 0.80, 0.70, 0.60],
        [0.60, 0.70, 0.80, 0.90, 0.90, 0.80, 0.70, 0.60],
        [0.60, 0.70, 0.80, 0.90, 0.90, 0.80, 0.70, 0.60],
        [0.60, 0.70, 0.80, 0.80, 0.80, 0.80, 0.70, 0.60],
        [0.60, 0.70, 0.70, 0.70, 0.70, 0.70, 0.70, 0.60],
        [0.50, 0.60, 0.60, 0.60, 0.60, 0.60, 0.60, 0.50]
    ]).flatten()
    
    queen_position_weights = np.array([
        [0.20, 0.30, 0.40, 0.40, 0.40, 0.40, 0.30, 0.20],
        [0.30, 0.50, 0.60, 0.60, 0.60, 0.60, 0.50, 0.30],
        [0.40, 0.60, 0.80, 0.80, 0.80, 0.80, 0.60, 0.40],
        [0.40, 0.60, 0.80, 1.00, 1.00, 0.80, 0.60, 0.40],
        [0.40, 0.60, 0.80, 1.00, 1.00, 0.80, 0.60, 0.40],
        [0.40, 0.60, 0.70, 0.70, 0.70, 0.70, 0.60, 0.40],
        [0.30, 0.50, 0.60, 0.60, 0.60, 0.60, 0.50, 0.30],
        [0.20, 0.30, 0.40, 0.30, 0.30, 0.40, 0.30, 0.20]
    ]).flatten()
    
    king_position_weights = np.array([
        [1.00, 0.90, 0.80, 0.80, 0.80, 0.80, 0.90, 1.00],
        [0.90, 0.70, 0.60, 0.60, 0.60, 0.60, 0.70, 0.90],
        [0.80, 0.60, 0.50, 0.50, 0.50, 0.50, 0.60, 0.80],
        [0.80, 0.60, 0.50, 0.40, 0.40, 0.50, 0.60, 0.80],
        [0.80, 0.60, 0.50, 0.40, 0.40, 0.50, 0.60, 0.80],
        [0.80, 0.60, 0.50, 0.50, 0.50, 0.50, 0.60, 0.80],
        [0.90, 0.70, 0.60, 0.60, 0.60, 0.60, 0.70, 0.90],
        [1.00, 0.90, 0.80, 0.80, 0.80, 0.80, 0.90, 1.00]
    ]).flatten()
    
    #The chromosome itself is just an amalgamation of these weights
    chromosome = [
        material_score_bonus,
        piece_mobility_bonus,
        castling_bonus,
        development_speed_bonus,
        doubled_pawn_penalty,
        isolated_pawn_penalty,
        connected_pawn_bonus,
        passed_pawn_bonus,
        enemy_king_pressure_bonus,
        piece_defense_bonus,
        bishop_pair_bonus,
        connected_rooks_bonus,
        king_centralization_val,
        knight_outpost_bonus,
        blocked_piece_penalty,
        space_control_in_opponent_half_bonus,
        king_shield_bonus,
        king_pressure_penalty,
        pawn_position_weights,
        knight_position_weights,
        bishop_position_weights,
        rook_position_weights,
        queen_position_weights,
        king_position_weights
    ]
    
    return chromosome

def compute_material_score(board, white):
    color = chess.WHITE if white else chess.BLACK
    score = 0
    
    piece_values = {
        chess.PAWN: 1,
        chess.KNIGHT: 3,
        chess.BISHOP: 3,
        chess.ROOK: 5,
        chess.QUEEN: 9
    }
    for piece_type, value in piece_values.items():
        score += len(board.pieces(piece_type, color)) * value
        score -= len(board.pieces(piece_type, not color)) * value
        
    return score

def compute_piece_mobility(board, white=True):
    color = chess.WHITE if white else chess.BLACK
    
    score = 0
    for move in board.legal_moves:
        board.push(move)
        square = move.to_square
        attackers = board.attackers(not color, square)
        defenders = board.attackers(color, square)
    
        if len(attackers) <= len(defenders):
            score += 1
        
        board.pop()

    return score

def compute_castling_bonus(board, white):
    color = chess.WHITE if white else chess.BLACK
    score = 0
    
    if board.has_castling_rights(color):
        score += 1
    else:
        san_moves = []
        temp_board = chess.Board()
        for move in board.move_stack:
            san_moves.append(temp_board.san(move))
            temp_board.push(move)
        
        if len(san_moves) > 2 and (san_moves[-2] == "O-O" or san_moves[-2] == "O-O-O"):
            score += 3
    
    return score

def compute_development_speed(board, white):
    if white:
        pawn_start_rank = 1
        piece_start_rank = 0
        color = chess.WHITE
    else:
        pawn_start_rank = 6
        piece_start_rank = 7
        color = chess.BLACK
        
    score = 0
    
    for piece in board.pieces(chess.PAWN, color):
        if chess.square_rank(piece) != piece_start_rank:
            score += 0.5
            
    for piece_type in [chess.KNIGHT, chess.BISHOP, chess.ROOK, chess.QUEEN]:
        for piece in board.pieces(piece_type, color):
            if chess.square_rank(piece) != piece_start_rank:
                score += 1

    return score

def compute_doubled_pawn_score(board, white):
    color = chess.WHITE if white else chess.BLACK
    score = 0
    
    file_counts = [0, 0, 0, 0, 0, 0, 0, 0]
    
    for square in board.pieces(chess.PAWN, color):
        file_index = chess.square_file(square)
        file_counts[file_index] += 1
    
    for count in file_counts:
        if count > 1:
            score -= (count - 1)
    
    return score

def compute_isolated_pawn_score(board, white):
    color = chess.WHITE if white else chess.BLACK
    score = 0
    
    files_with_pawns = set(chess.square_file(piece) for piece in board.pieces(chess.PAWN, color))
    
    for file in files_with_pawns:
        isolated_left = (file == 0) or (file - 1 not in files_with_pawns)
        isolated_right = (file == 7) or (file + 1 not in files_with_pawns)
        
        if isolated_left and isolated_right:
            score -= 1
    
    return score

def compute_connected_pawn_score(board, white):
    color = chess.WHITE if white else chess.BLACK
    score = 0

    direction = -1 if white else 1  # White looks one rank down, black looks one rank up

    pawns = board.pieces(chess.PAWN, color)
    for pawn in pawns:
        file = chess.square_file(pawn)
        rank = chess.square_rank(pawn)
        
        behind_rank = rank + direction
        if 0 <= behind_rank <= 7: #unnecessary check, but not wrong to be here
            if file - 1 >= 0 and chess.square(file - 1, behind_rank) in pawns:
                score += 1
            if file + 1 <= 7 and chess.square(file + 1, behind_rank) in pawns:
                score += 1
    
    return score

def compute_passed_pawn_score(board, white):
    color = chess.WHITE if white else chess.BLACK
    enemy_color = not color
    score = 0

    pawns = board.pieces(chess.PAWN, color)
    for pawn in pawns:
        file = chess.square_file(pawn)
        rank = chess.square_rank(pawn)
        
        #this if/else is to handle rank - white: > rank, black: < rank
        if white:
            enemy_pawns_on_file = [sq for sq in board.pieces(chess.PAWN, enemy_color) if chess.square_file(sq) == file and chess.square_rank(sq) > rank]
        else:
            enemy_pawns_on_file = [sq for sq in board.pieces(chess.PAWN, enemy_color) if chess.square_file(sq) == file and chess.square_rank(sq) < rank]

        if len(enemy_pawns_on_file) == 0:
            score += 1
    
    return score
    
def compute_king_pressure_score(board, white):
    color = chess.WHITE if white else chess.BLACK
    enemy_color = not color
    score = 0
    
    king_square = board.king(color)
    directions = [
        (-1, -1), (0, -1), (1, -1),
        (-1,  0),          (1,  0),
        (-1,  1), (0,  1), (1,  1)
    ]
    file = chess.square_file(king_square)
    rank = chess.square_rank(king_square)
    
    for df, dr in directions:
        f = file + df
        r = rank + dr
        if 0 <= f <= 7 and 0 <= r <= 7:
            sq = chess.square(f, r)
            if board.is_attacked_by(enemy_color, sq):
                score -= 1
                
    
    temp_board = board.copy()
    for square in chess.SQUARES:
        piece = temp_board.piece_at(square)
        if piece and piece.color == color and piece.piece_type != chess.KING:
            temp_board.remove_piece_at(square)
    
    xray_attackers = temp_board.attackers(enemy_color, king_square)
    score -= len(xray_attackers)
    
    return score

def compute_piece_defense_score(board, white):
    color = chess.WHITE if white else chess.BLACK
    score = 0

    for piece_type in [chess.PAWN, chess.KNIGHT, chess.BISHOP, chess.ROOK, chess.QUEEN]:
        for square in board.pieces(piece_type, color):
            if len(board.attackers(color, square)) > 0:
                score += 1

    return score

def compute_connected_rooks_score(board, white):
    color = chess.WHITE if white else chess.BLACK
    rooks = list(board.pieces(chess.ROOK, color))
    score = 0

    #Check every unique pair of rooks
    for i in range(len(rooks)):
        for j in range(i + 1, len(rooks)):
            rook1 = rooks[i]
            rook2 = rooks[j]

            rook1_rank = chess.square_rank(rook1)
            rook2_rank = chess.square_rank(rook2)
            rook1_file = chess.square_file(rook1)
            rook2_file = chess.square_file(rook2)
            
            if rook1_rank == rook2_rank:
                step = 1 if rook1_file < rook2_file else -1
                for file in range(rook1_file + step, rook2_file, step):
                    if board.piece_at(chess.square(file, rook1_rank)):
                        break
                else:
                    score += 1
            elif rook1_file == rook2_file:
                step = 1 if rook1_rank < rook2_rank else -1
                for rank in range(rook1_rank + step, rook2_rank, step):
                    if board.piece_at(chess.square(rook1_file, rank)):
                        break
                else:
                    score += 1

    return score

def compute_king_centralization_score(board, white):
    color = chess.WHITE if white else chess.BLACK
    score = 0
    
    king_square = board.king(color)
    king_file = chess.square_file(king_square)
    king_rank = chess.square_rank(king_square)
    
    center_squares = [(3, 3), (4, 3), (3, 4), (4, 4)]

    distances = []
    for cf, cr in center_squares:
        distance = max(abs(king_file - cf), abs(king_rank - cr))  # Chebyshev distance
        distances.append(distance)
    
    min_distance = min(distances)
    #from the corner, the king takes at most 3 moves to get to a center square

    centralization_score = -min_distance
    
    if white:
        distance_from_opposite_side = 7 - king_rank   
    else:
        distance_from_opposite_side = king_rank
        
    score = centralization_score - distance_from_opposite_side * 0.25

    return score

def compute_knight_outpost_score(board, white):
    color = chess.WHITE if white else chess.BLACK
    enemy_color = not color
    score = 0

    pawns = board.pieces(chess.PAWN, color)
    enemy_pawns = board.pieces(chess.PAWN, enemy_color)
    knights = board.pieces(chess.KNIGHT, color)

    for knight in knights:
        knight_rank = chess.square_rank(knight)

        # Check if knight is on enemy's side of the board
        if white and knight_rank < 4:
            continue  # Not on enemy side for white
        if not white and knight_rank > 3:
            continue  # Not on enemy side for black

        # Check if the knight is protected by a friendly pawn
        if any(
            board.is_attacked_by(color, knight) and
            board.piece_at(attacker_sq).piece_type == chess.PAWN
            for attacker_sq in board.attackers(color, knight)
        ):
            # Check if enemy pawns can capture this knight on next move
            knight_file = chess.square_file(knight)

            if color == chess.WHITE:
                enemy_pawn_attack_squares = []
                rank = knight_rank + 1
                for file_delta in [-1, 1]:
                    file = knight_file + file_delta
                    if 0 <= file <= 7 and 0 <= rank <= 7:
                        enemy_pawn_attack_squares.append(chess.square(file, rank))
            else:
                enemy_pawn_attack_squares = []
                rank = knight_rank - 1
                for file_delta in [-1, 1]:
                    file = knight_file + file_delta
                    if 0 <= file <= 7 and 0 <= rank <= 7:
                        enemy_pawn_attack_squares.append(chess.square(file, rank))

            if not any(pawn in enemy_pawns for pawn in enemy_pawn_attack_squares):
                score += 1

    return score

def compute_blocked_piece_score(board, white):
    color = chess.WHITE if white else chess.BLACK
    score = 0

    for piece_type in [chess.KNIGHT, chess.BISHOP, chess.ROOK, chess.QUEEN]:
        for square in board.pieces(piece_type, color):
            has_unblocked_move = False
            for move in board.generate_pseudo_legal_moves():
                if move.from_square == square:
                    has_unblocked_move = True
                    break

            if not has_unblocked_move:
                score -= 1  

    return score

def compute_space_control_score(board, white):
    score = 0
    color = chess.WHITE if white else chess.BLACK
    
    if white:
        ranks = [4, 5, 6, 7]
    else:
        ranks = [0, 1, 2, 3]
    
    for rank in ranks:
        for file in range(0, 8):
            if len(board.attackers(color, chess.square(file, rank))) > 0:
                score += 1
    
    return score

def compute_king_shield_score(board, white=True):
    color = chess.WHITE if white else chess.BLACK
    king_square = board.king(color)

    king_file = chess.square_file(king_square)
    king_rank = chess.square_rank(king_square)

    # Direction: forward depends on color
    forward = 1 if white else -1
    shield_rank = king_rank + forward

    score = 0

    for df in [-1, 0, 1]:
        file = king_file + df
        if 0 <= file <= 7 and 0 <= shield_rank <= 7:
            piece = board.piece_at(chess.square(file, shield_rank))
            if piece and piece.color == color:
                if piece.piece_type == chess.PAWN:
                    score += 1
                else:
                    score += 2

    return score
    
def evaluate_position(board, chromosome, white=True, debug=False):
    material_score_bonus = chromosome[0]
    piece_mobility_bonus = chromosome[1]
    castling_bonus = chromosome[2]
    development_speed_bonus = chromosome[3]
    doubled_pawn_penalty = chromosome[4]
    isolated_pawn_penalty = chromosome[5]
    connected_pawn_bonus = chromosome[6]
    passed_pawn_bonus = chromosome[7]
    enemy_king_pressure_bonus = chromosome[8]
    piece_defense_bonus = chromosome[9]
    bishop_pair_bonus = chromosome[10]
    connected_rooks_bonus = chromosome[11]
    king_centralization_val = chromosome[12]
    knight_outpost_bonus = chromosome[13]
    blocked_piece_penalty = chromosome[14]
    space_control_in_opponent_half_bonus = chromosome[15]
    king_shield_bonus = chromosome[16]
    king_pressure_penalty = chromosome[17]
    
    pawn_position_weights = chromosome[18]
    knight_position_weights = chromosome[19]
    bishop_position_weights = chromosome[20]
    rook_position_weights = chromosome[21]
    queen_position_weights = chromosome[22]
    king_position_weights = chromosome[23]


    score = 0
    
    material_score = compute_material_score(board, white)
    score += material_score * material_score_bonus
    if debug:
        print("\tMaterial score:", material_score)
    
    piece_mobility_score = compute_piece_mobility(board, white)
    score += piece_mobility_score * piece_mobility_bonus
    if debug:
        print("\tPiece mobility score:", piece_mobility_score)
    
    castling_score = compute_castling_bonus(board, white)
    score += castling_score * castling_bonus
    if debug:
        print("\tCastling score:", castling_score)
    
    development_speed_score = compute_development_speed(board, white)
    score += development_speed_score * development_speed_bonus
    if debug:
        print("\tMaterial score:", development_speed_score)
    
    doubled_pawn_score = compute_doubled_pawn_score(board, white)
    score += doubled_pawn_score * doubled_pawn_penalty
    if debug:
        print("\tDevelopment speed score:", doubled_pawn_score)
    
    isolated_pawn_score = compute_isolated_pawn_score(board, white)
    score += isolated_pawn_score * isolated_pawn_penalty
    if debug:
        print("\tIsolated pawn score:", isolated_pawn_score)
    
    connected_pawn_score = compute_connected_pawn_score(board, white)
    score += connected_pawn_score * connected_pawn_bonus
    if debug:
        print("\tConnected pawn score:", connected_pawn_score)
    
    passed_pawn_score = compute_passed_pawn_score(board, white)
    score += passed_pawn_score * passed_pawn_bonus
    if debug:
        print("\tPassed pawn score:", passed_pawn_score)

    enemy_king_pressure_score = compute_king_pressure_score(board, not white)
    score += enemy_king_pressure_score * enemy_king_pressure_bonus
    if debug:
        print("\tEnemy king pressure score:", enemy_king_pressure_score)
    
    piece_defense_score = compute_piece_defense_score(board, white)
    score += piece_defense_score * piece_defense_bonus
    if debug:
        print("\tPiece defense score:", piece_defense_score)
    
    bishop_pair_score = int(len(board.pieces(chess.BISHOP, chess.WHITE if white else chess.BLACK)) > 1)
    score += bishop_pair_score * bishop_pair_bonus
    if debug:
        print("\tBishop pair score:", bishop_pair_score)
    
    connected_rooks_score = compute_connected_rooks_score(board, white)
    score += connected_rooks_score * connected_rooks_bonus
    if debug:
        print("\tConnected rooks score:", connected_rooks_score)
    
    king_centralization_score = compute_king_centralization_score(board, white)
    score += king_centralization_score * king_centralization_val
    if debug:
        print("\tKing centralization score:", king_centralization_score)
    
    knight_outpost_score = compute_knight_outpost_score(board, white)
    score += knight_outpost_score * knight_outpost_bonus
    if debug:
        print("\tKnight outpost score:", knight_outpost_score)
    
    blocked_piece_score = compute_blocked_piece_score(board, white)
    score += blocked_piece_score * blocked_piece_penalty
    if debug:
        print("\tBlocked piece score:", blocked_piece_score)
    
    space_control_score = compute_space_control_score(board, white)
    score += space_control_score * space_control_in_opponent_half_bonus
    if debug:
        print("\tSpace control score:", space_control_score)
    
    king_shield_score = compute_king_shield_score(board, white)
    score += king_shield_score * king_shield_bonus
    if debug:
        print("\tKing shield score:", king_shield_score)
    
    king_pressure_score = -compute_king_pressure_score(board, white)
    score += king_pressure_score * king_pressure_penalty
    if debug:
        print("\tKing pressure score:", king_pressure_score)
    
    pawn_position_score = 0
    for square in board.pieces(chess.PAWN, chess.WHITE):
        pawn_position_score += pawn_position_weights[square]
    for square in board.pieces(chess.PAWN, chess.BLACK):
        pawn_position_score -= pawn_position_weights[chess.square_mirror(square)]
    score += pawn_position_score
    if debug:
        print("\tPawn position score:", pawn_position_score)
    
    knight_position_score = 0
    for square in board.pieces(chess.KNIGHT, chess.WHITE):
        knight_position_score += knight_position_weights[square]
    for square in board.pieces(chess.KNIGHT, chess.BLACK):
        knight_position_score -= knight_position_weights[chess.square_mirror(square)]
    score += knight_position_score
    if debug:
        print("\tKnight position score:", knight_position_score)
    
    bishop_position_score = 0
    for square in board.pieces(chess.BISHOP, chess.WHITE):
        bishop_position_score += bishop_position_weights[square]
    for square in board.pieces(chess.BISHOP, chess.BLACK):
        bishop_position_score -= bishop_position_weights[chess.square_mirror(square)]
    score += bishop_position_score
    if debug:
        print("\tBishop position score:", bishop_position_score)
    
    rook_position_score = 0
    for square in board.pieces(chess.ROOK, chess.WHITE):
        rook_position_score += rook_position_weights[square]
    for square in board.pieces(chess.ROOK, chess.BLACK):
        rook_position_score -= rook_position_weights[chess.square_mirror(square)]
    score += rook_position_score
    if debug:
        print("\tRook position score:", rook_position_score)
    
    queen_position_score = 0
    for square in board.pieces(chess.QUEEN, chess.WHITE):
        queen_position_score += queen_position_weights[square]
    for square in board.pieces(chess.QUEEN, chess.BLACK):
        queen_position_score -= queen_position_weights[chess.square_mirror(square)]
    score += queen_position_score
    if debug:
        print("\tQueen position score:", queen_position_score)
    
    king_position_score = 0
    for square in board.pieces(chess.KING, chess.WHITE):
        king_position_score += king_position_weights[square]
    for square in board.pieces(chess.KING, chess.BLACK):
        king_position_score -= king_position_weights[chess.square_mirror(square)]
    score += king_position_score
    if debug:
        print("\tKing position score:", king_position_score)
        
    return score

'''
chromosome = generate_chromosome()

with open("chesscom_game_maria.pgn") as pgn:
    game = chess.pgn.read_game(pgn)

board = game.board()
for move in game.mainline_moves():
    print(move)
    board.push(move)
    if move.uci() == "h7h3":
        break
        
print(board)

print(evaluate_position(board, chromosome))

exit()
'''
def crossover(parent1, parent2):
    child_chromosome = []

    # Scalars first (0 to 17)
    for i in range(18):
        alpha = random.uniform(0.4, 0.6)
        child_gene = alpha * parent1[i] + (1 - alpha) * parent2[i]
        child_chromosome.append(child_gene)
    
    # Piece-square tables
    for i in range(18, len(parent1)):
        alpha = random.uniform(0.4, 0.6)
        child_gene = alpha * parent1[i] + (1 - alpha) * parent2[i]
        child_chromosome.append(child_gene)

    return child_chromosome

def mutate(chromosome, mutation_rate=0.5, mutation_strength=0.1):
    new_chromosome = copy.deepcopy(chromosome)
    
    # Mutate scalars
    for i in range(18):
        if random.random() < mutation_rate:
            delta = np.random.normal(-mutation_strength, mutation_strength)
            new_chromosome[i] += delta
    
    # Mutate piece-square tables
    for i in range(18, len(new_chromosome)):
        table = new_chromosome[i]
        for j in range(len(table)):
            if random.random() < mutation_rate:
                delta = np.random.normal(-mutation_strength, mutation_strength)
                table[j] += delta

    return new_chromosome


def move_score(board, move):
    PIECE_VALUES = {
        chess.PAWN: 1,
        chess.KNIGHT: 3,
        chess.BISHOP: 3,
        chess.ROOK: 5,
        chess.QUEEN: 9,
        chess.KING: 0
    }

    if board.is_capture(move):
        victim = board.piece_at(move.to_square)
        attacker = board.piece_at(move.from_square)
        if victim and attacker:
            return 10 * PIECE_VALUES[victim.piece_type] - PIECE_VALUES[attacker.piece_type]
    if board.gives_check(move):
        return 1  # Very lightweight bonus
    return 0


transposition_table = {}
    
def order_moves(board):
    # Captures and checks first
    return sorted(board.legal_moves, key=lambda m: move_score(board, m), reverse=True)

def minimax(board, chromosome, depth, alpha, beta, maximizing_player, cache=transposition_table):
    fen = board.fen()
    
    if (fen, depth, maximizing_player) in cache:
        return cache[(fen, depth, maximizing_player)]
    
    if depth == 0 or board.is_game_over():
        score = evaluate_position(board, chromosome, white=maximizing_player)
        cache[(fen, depth, maximizing_player)] = score
        return score

    if maximizing_player:
        max_eval = -float('inf')
        for move in order_moves(board):
            board.push(move)
            eval = minimax(board, chromosome, depth - 1, alpha, beta, False)
            board.pop()
            max_eval = max(max_eval, eval)
            alpha = max(alpha, eval)
            if beta <= alpha:
                break
            
        cache[(fen, depth, maximizing_player)] = max_eval
        return max_eval
    else:
        min_eval = float('inf')
        for move in order_moves(board):
            board.push(move)
            eval = minimax(board, chromosome, depth - 1, alpha, beta, True)
            board.pop()
            min_eval = min(min_eval, eval)
            beta = min(beta, eval)
            if beta <= alpha:
                break
        cache[(fen, depth, maximizing_player)] = min_eval
        return min_eval
        
def select_best_move(board, chromosome, depth=2):
    legal_moves = list(board.legal_moves)
    best_move = None
    best_score = -float('inf') if board.turn == chess.WHITE else float('inf')

    for move in legal_moves:
        board.push(move)
        #start = time.time()
        score = minimax(board, chromosome, depth - 1, -float('inf'), float('inf'), board.turn == chess.WHITE)
        #print(f"Minimax took: {time.time() - start:.4f} seconds")
        board.pop()

        if board.turn == chess.WHITE:
            if score > best_score:
                best_score = score
                best_move = move
        else:
            if score < best_score:
                best_score = score
                best_move = move

    return best_move
    
def generate_midgame_position(stockfish, moves=20, depth=4):
    board = chess.Board()
    for _ in range(moves):
        if board.is_game_over():
            break  # avoid weird positions
        result = stockfish.play(board, chess.engine.Limit(depth=depth))
        board.push(result.move)
        
    return board
    
def play_game(board, chromosome_white, stockfish_engine, max_moves=200):
    for move_number in range(max_moves):
        if board.is_game_over():
            result = board.result()
            if result == '1-0':
                print()
                return 1000
            elif result == '0-1':
                print()
                return -1000 + move_number
            else:
                print()
                return compute_material_score(board, board.turn == chess.WHITE)

        if board.turn == chess.WHITE:
            start = time.time()
            move = select_best_move(board, chromosome_white)

        else:
            move = play_stockfish_move(board, stockfish_engine)

        if move is None:
            break

        board.push(move)
    print()
    return compute_material_score(board, board.turn == chess.WHITE)

import time
def play_stockfish_move(board, stockfish):
    #start = time.time()
    result = stockfish.play(board, chess.engine.Limit(depth=1))
    #print(f"Stockfish took: {time.time() - start:.4f} seconds")
    return result.move

'''
def evaluate_fitness(board, chromosome_white, games_per_opponent=1):
    fitness = 0
    #for _ in range(games_per_opponent):
    #    print("game")
    fitness = play_game(board, chromosome_white, use_stockfish_black=True)
    print("Resultant fitness:", fitness)
    return fitness
'''

def tournament_selection(fitness_scores, k=3):
    #Randomly select k individuals and return the best among them
    selected = random.sample(fitness_scores, k)
    selected.sort(key=lambda x: x[1], reverse=True)
    return selected[0][0]


def evaluate_fitness_wrapper(chromosome):
    # Do NOT use any global stockfish object here
    engine = chess.engine.SimpleEngine.popen_uci(stockfish_path)
    # Generate new board inside process
    board = generate_midgame_position(engine)  
    
    score = play_game(board, chromosome, stockfish_engine=engine)
    print("Resultant fitness:", score)
    engine.quit()
    return chromosome, score
    
def genetic_algorithm():
    #Initialize the population
    population = [generate_chromosome() for _ in range(POPULATION_SIZE)]

    for gen in range(0, GENERATIONS):
        with ProcessPoolExecutor(max_workers=multiprocessing.cpu_count() - 1) as executor:
            futures = [executor.submit(evaluate_fitness_wrapper, chromo) for chromo in population]
            fitness_scores = [future.result() for future in concurrent.futures.as_completed(futures)]

        
        fitness_scores.sort(key=lambda x: x[1], reverse=True) #Sort based on fitness

        print(f"Generation {gen}, Best Fitness: {fitness_scores[0][1]}")

        #Keep the best two guaranteed, sample the others
        next_gen = [fitness_scores[0][0], fitness_scores[1][0]]
        output_chromosome = []
        for element in fitness_scores[0][0]:
            if type(element) == np.ndarray:
                output_chromosome.append(element.tolist())
            else:
                output_chromosome.append(element)
        json.dump(output_chromosome, open(f"best/generation_{gen}.json", "w"))
        #Selection/Crossover
        while len(next_gen) < POPULATION_SIZE:
            parent1 = tournament_selection(fitness_scores)
            parent2 = tournament_selection(fitness_scores)
            child = crossover(parent1, parent2)
            child = mutate(child, mutation_rate = max(0.01, 0.3 * (1 - gen / GENERATIONS)))
            next_gen.append(child)

        population = next_gen

    fitness_scores = [(chromosome, evaluate_fitness(chromosome)) for chromosome in population]
    fitness_scores.sort(key=lambda x: x[1], reverse=True)
    best_white = fitness_scores[0][0]
    second_best_white = fitness_scores[1][0]

    print("\nBest move sequence:")
    print(best_white)
    print("\nSecond best move sequence:")
    print(second_best_white)

    return best_white, second_best_white

if __name__ == "__main__":
    best_white, best_black = genetic_algorithm()

    board = chess.Board()
    max_moves = 200
    result = "Draw"
    for move_number in range(max_moves):
        if board.is_game_over():
            result = board.result()
            if result == '1-0':
                print("White won")
            elif result == '0-1':
                print("Black won")
            else:
                print("No victory, material score:", material_balance(board))

        if board.turn == chess.WHITE:
            move = select_best_move(board, best_white)
        else:
            move = play_stockfish_move(board)

        if move is None:
            break

        board.push(move)

    print(f"\nFinal game result (white advantage): {result}")
    
    exit()
    
