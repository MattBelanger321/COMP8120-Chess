import chess
import chess.engine
import random
import copy
import chess.pgn
import numpy as np

POPULATION_SIZE = 20
MAX_MOVES = 200 #Each individual will just generate 200 moves. If a victory happens before that, oh well
GENERATIONS = 20

#Note - UCI stands for Universal Chess Interface. It's the chess notation encoding, like e2e4, bd2e3, etc.

#The board is handled via the chess library, and a move is done via the .push() function
stockfish_path = "./stockfish/stockfish-windows-x86-64-avx2"
stockfish = chess.engine.SimpleEngine.popen_uci(stockfish_path)

#Generate the chromosome for the individual, a series of weights for different aspects of chess
def generate_chromosome():
    #Piece value for the individual
    pawn_val = np.random.uniform(0.1, 5)
    knight_val = np.random.uniform(0.1, 5)
    bishop_val = np.random.uniform(0.1, 5)
    rook_val = np.random.uniform(0.1, 5)
    queen_val = np.random.uniform(0.1, 5)
    
    #Importance of chess mechanics like king safety
    material_score_val = np.random.uniform(2, 5)
    piece_mobility_val = np.random.uniform(1, 5)
    center_control_val = np.random.uniform(1, 5)
    king_safety_val = np.random.uniform(0.5, 2)
    
    #Scores for each piece's position on a board. Ex: Queen is generally better in the center, knight is generally worse on an edge
    pawn_position_weights = np.random.uniform(1, 5, 64)
    knight_position_weights = np.random.uniform(1, 5, 64)
    bishop_position_weights = np.random.uniform(1, 5, 64)
    rook_position_weights = np.random.uniform(1, 5, 64)
    queen_position_weights = np.random.uniform(1, 5, 64)
    king_position_weights = np.random.uniform(1, 5, 64)
    
    #The chromosome itself is just an amalgamation of these weights
    chromosome = [
        pawn_val, knight_val, bishop_val, rook_val, queen_val,
        material_score_val, piece_mobility_val, center_control_val, king_safety_val,
        
        pawn_position_weights, 
        knight_position_weights, 
        bishop_position_weights, 
        rook_position_weights, 
        queen_position_weights, 
        king_position_weights
    ]
    
    return chromosome

def king_safety(board):
    king_safety_penalty = 0
    
    penalty = 1
    king_square = board.king(chess.WHITE)
    king_file = chess.square_file(king_square)
    king_rank = chess.square_rank(king_square)
    
    #Define an open file as no white piece before a black piece or the end of the board
    #If the king is on an open file, apply full penalty
    for rank in range(king_rank + 1, 8):
        piece = board.piece_at(chess.square(king_file, rank))
        if piece:
            if piece.color == chess.WHITE:
                break
            elif piece.color == chess.BLACK or rank == 7:
                king_safety_penalty += 0.8 * penalty
    
    #If the king is next to an open file, apply reduced penalty
    for file in [king_file - 1, king_file + 1]:
        if file < 0 or file > 7:
            continue
        
        for rank in range(king_rank, 8):
            piece = board.piece_at(chess.square(file, rank))
            if piece:
                if piece.color == chess.WHITE:
                    break
                elif piece.color == chess.BLACK or rank == 7:
                    king_safety_penalty += 0.8 * penalty
    
    adjacent_squares = [
        (king_file - 1, king_rank - 1), (king_file, king_rank - 1), (king_file + 1, king_rank - 1),
        (king_file - 1, king_rank),                                 (king_file + 1, king_rank),
        (king_file - 1, king_rank + 1), (king_file, king_rank + 1), (king_file + 1, king_rank + 1)
    ]
    #If the king isn't next to white pieces, penalize by each square not occupied by a white piece
    for pair in adjacent_squares:
        if 0 <= pair[0] <= 7 and 0 <= pair[1] <= 7:
            piece = board.piece_at(chess.square(pair[0], pair[1]))
            if not piece:
                king_safety_penalty += 0.1 * penalty
            elif piece.color == chess.BLACK:
                if piece.piece_type == chess.PAWN:
                    king_safety_penalty += 1
                elif piece.piece_type == chess.KNIGHT:
                    king_safety_penalty += 3
                elif piece.piece_type == chess.BISHOP:
                    king_safety_penalty += 3
                elif piece.piece_type == chess.ROOK:
                    king_safety_penalty += 5
                elif piece.piece_type == chess.QUEEN:
                    king_safety_penalty += 9
    
    #If king not on edge of board, penalize
    #Penalize harder if not on rank 0
    if king_rank != 0:
        if king_file != 0 and king_file != 7 and king_rank != 7:
            king_safety_penalty += 0.5 * penalty
        else:
            king_safety_penalty += 0.3 * penalty
    
    #If black's next move can put the king in check, it's not safe
    board_copy = board.copy()
    board_copy.turn = chess.BLACK
    for move in board_copy.legal_moves:
        board_copy.push(move)
        if board_copy.is_checkmate():
            king_safety_penalty += 10 * penalty
        elif board_copy.is_check():
            king_safety_penalty += 2 * penalty
    
        board_copy.pop()
    
    #If a king move is blocked because of check, penalize
    king_pseudo_moves = [move for move in board.generate_pseudo_legal_moves() if move.from_square == king_square]

    board_copy = board.copy()
    for move in king_pseudo_moves:
        board_copy.push(move)
        if board_copy.is_check():
            king_safety_penalty += penalty
        board_copy.pop()
    
    return -king_safety_penalty


def evaluate_position(board, chromosome):
    #Unpack the chromosome
    pawn_val = chromosome[0]
    knight_val = chromosome[1]
    bishop_val = chromosome[2]
    rook_val = chromosome[3]
    queen_val = chromosome[4]
    
    material_score_val = chromosome[5]
    piece_mobility_val = chromosome[6]
    center_control_val = chromosome[7]
    king_safety_val = chromosome[8]
    
    pawn_position_weights = chromosome[9]
    knight_position_weights = chromosome[10]
    bishop_position_weights = chromosome[11]
    rook_position_weights = chromosome[12]
    queen_position_weights = chromosome[13]
    king_position_weights = chromosome[14]
    
    score = 0
    
    #Total piece scores based on individual's weightings
    #white
    score += len(board.pieces(chess.PAWN, chess.WHITE)) * pawn_val
    score += len(board.pieces(chess.KNIGHT, chess.WHITE)) * knight_val
    score += len(board.pieces(chess.BISHOP, chess.WHITE)) * bishop_val
    score += len(board.pieces(chess.ROOK, chess.WHITE)) * rook_val
    score += len(board.pieces(chess.QUEEN, chess.WHITE)) * queen_val
    #Black
    score -= len(board.pieces(chess.PAWN, chess.BLACK)) * pawn_val
    score -= len(board.pieces(chess.KNIGHT, chess.BLACK)) * knight_val
    score -= len(board.pieces(chess.BISHOP, chess.BLACK)) * bishop_val
    score -= len(board.pieces(chess.ROOK, chess.BLACK)) * rook_val
    score -= len(board.pieces(chess.QUEEN, chess.BLACK)) * queen_val
    
    
    #Compute material score
    true_material_score = 0
    #White
    true_material_score += len(board.pieces(chess.PAWN, chess.WHITE)) * 1
    true_material_score += len(board.pieces(chess.KNIGHT, chess.WHITE)) * 3
    true_material_score += len(board.pieces(chess.BISHOP, chess.WHITE)) * 3
    true_material_score += len(board.pieces(chess.ROOK, chess.WHITE)) * 5
    true_material_score += len(board.pieces(chess.QUEEN, chess.WHITE)) * 9
    #Black
    true_material_score -= len(board.pieces(chess.PAWN, chess.BLACK)) * 1
    true_material_score -= len(board.pieces(chess.KNIGHT, chess.BLACK)) * 3
    true_material_score -= len(board.pieces(chess.BISHOP, chess.BLACK)) * 3
    true_material_score -= len(board.pieces(chess.ROOK, chess.BLACK)) * 5
    true_material_score -= len(board.pieces(chess.QUEEN, chess.BLACK)) * 9
    
    score += true_material_score * material_score_val
    
    
    #Compute mobility, defined simply as the number of available moves
    board_copy = board.copy()
    board_copy.turn = chess.WHITE
    score += piece_mobility_val * board.legal_moves.count()
    board_copy.turn = chess.BLACK
    score -= piece_mobility_val * board.legal_moves.count()
    
    #Compute center control. Applies center_control_val to each square where a piece is, otherwise applies half the val for
    #each piece that can attack a black piece at the center
    
    central_squares = [chess.E4, chess.D4, chess.E5, chess.D5]
    for square in central_squares:
        piece = board.piece_at(square)
        if piece:
            if piece.color == chess.WHITE:
                score += center_control_val
            elif piece.color == chess.BLACK:
                score -= center_control_val
    
    for square in chess.SQUARES:
        piece = board.piece_at(square)
        if piece:
            legal_moves = board.attacks(square)
            for center_square in central_squares:
                if center_square in legal_moves:
                    score += (0.5 * center_control_val if piece.color == chess.WHITE else 0.5 * -center_control_val)

    #King safety penalty
    score += king_safety_val * king_safety(board)
    
    #Score increases if the enemy king is being pressured
    #Not implementing this for now
    #score += enemy_king_pressure_val * enemy_king_pressure(board)
    for square in board.pieces(chess.PAWN, chess.WHITE):
        score += pawn_position_weights[square]
    for square in board.pieces(chess.PAWN, chess.BLACK):
        score -= pawn_position_weights[chess.square_mirror(square)]
    
    for square in board.pieces(chess.KNIGHT, chess.WHITE):
        score += knight_position_weights[square]
    for square in board.pieces(chess.KNIGHT, chess.BLACK):
        score -= knight_position_weights[chess.square_mirror(square)]
    
    for square in board.pieces(chess.BISHOP, chess.WHITE):
        score += bishop_position_weights[square]
    for square in board.pieces(chess.BISHOP, chess.BLACK):
        score -= bishop_position_weights[chess.square_mirror(square)]
    
    for square in board.pieces(chess.ROOK, chess.WHITE):
        score += rook_position_weights[square]
    for square in board.pieces(chess.ROOK, chess.BLACK):
        score -= rook_position_weights[chess.square_mirror(square)]
    
    for square in board.pieces(chess.QUEEN, chess.WHITE):
        score += queen_position_weights[square]
    for square in board.pieces(chess.QUEEN, chess.BLACK):
        score -= queen_position_weights[chess.square_mirror(square)]
    
    return score

def crossover(parent1, parent2):
    child_chromosome = []

    # Scalars first (0 to 8)
    for i in range(9):
        if random.random() < 0.5:
            child_chromosome.append(parent1[i])
        else:
            child_chromosome.append(parent2[i])
    
    # Piece-square tables
    for i in range(9, len(parent1)):
        if random.random() < 0.5:
            child_chromosome.append(np.copy(parent1[i]))
        else:
            child_chromosome.append(np.copy(parent2[i]))

    return child_chromosome

def mutate(chromosome, mutation_rate=0.5, mutation_strength=0.1):
    new_chromosome = copy.deepcopy(chromosome)
    
    # Mutate scalars
    for i in range(9):
        if random.random() < mutation_rate:
            delta = random.uniform(-mutation_strength, mutation_strength)
            new_chromosome[i] += delta
    
    # Mutate piece-square tables
    for i in range(9, len(new_chromosome)):
        table = new_chromosome[i]
        for j in range(len(table)):
            if random.random() < mutation_rate:
                delta = random.uniform(-mutation_strength, mutation_strength)
                table[j] += delta

    return new_chromosome

def select_best_move(board, chromosome, depth=2):
    def minimax(board, depth, alpha, beta, maximizing_player):
        if depth == 0 or board.is_game_over():
            return evaluate_position(board, chromosome)

        if maximizing_player:
            max_eval = -float('inf')
            for move in board.legal_moves:
                board.push(move)
                eval = minimax(board, depth - 1, alpha, beta, False)
                board.pop()
                max_eval = max(max_eval, eval)
                alpha = max(alpha, eval)
                if beta <= alpha:
                    break
            return max_eval
        else:
            min_eval = float('inf')
            for move in board.legal_moves:
                board.push(move)
                eval = minimax(board, depth - 1, alpha, beta, True)
                board.pop()
                min_eval = min(min_eval, eval)
                beta = min(beta, eval)
                if beta <= alpha:
                    break
            return min_eval

    legal_moves = list(board.legal_moves)
    best_move = None
    best_score = -float('inf') if board.turn == chess.WHITE else float('inf')

    for move in legal_moves:
        board.push(move)
        score = minimax(board, depth - 1, -float('inf'), float('inf'), board.turn == chess.BLACK)
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


def play_game(chromosome_white, use_stockfish_black=True, max_moves=200):
    board = chess.Board()

    for move_number in range(max_moves):
        if board.is_game_over():
            result = board.result()
            if result == '1-0':
                return float("inf")
            elif result == '0-1':
                return -float("inf")
            else:
                return material_balance(board)

        if board.turn == chess.WHITE:
            move = select_best_move(board, chromosome_white)
        else:
            if use_stockfish_black:
                move = play_stockfish_move(board)
            else:
                move = select_best_move(board, chromosome_white)

        if move is None:
            break

        board.push(move)

    return material_balance(board)

def play_stockfish_move(board):
    result = stockfish.play(board, chess.engine.Limit(depth=1))
    return result.move

def material_balance(board):
    material_values = {
        chess.PAWN: 1,
        chess.KNIGHT: 3,
        chess.BISHOP: 3,
        chess.ROOK: 5,
        chess.QUEEN: 9
    }
    score = 0
    for piece_type in material_values.keys():
        score += len(board.pieces(piece_type, chess.WHITE)) * material_values[piece_type]
        score -= len(board.pieces(piece_type, chess.BLACK)) * material_values[piece_type]
    return score

def evaluate_fitness(chromosome_white, games_per_opponent=3):
    fitness = 0
    for _ in range(games_per_opponent):
        print("game")
        fitness = max(fitness, play_game(chromosome_white, use_stockfish_black=True))
    return fitness

def tournament_selection(fitness_scores, k=3):
    #Randomly select k individuals and return the best among them
    selected = random.sample(fitness_scores, k)
    selected.sort(key=lambda x: x[1], reverse=True)
    return selected[0][0]

def genetic_algorithm():
    #Initialize the population
    population = [generate_chromosome() for _ in range(POPULATION_SIZE)]


    for gen in range(0, GENERATIONS):
        chromosome_black = population[0]
        
        fitness_scores = [(chromosome, evaluate_fitness(chromosome)) for chromosome in population]
        fitness_scores.sort(key=lambda x: x[1], reverse=True) #Sort based on fitness

        print(f"Generation {gen}, Best Fitness: {fitness_scores[0][1]}")

        #Keep the best two guaranteed, sample the others
        next_gen = [fitness_scores[0][0], fitness_scores[1][0]]

        #Selection/Crossover
        while len(next_gen) < POPULATION_SIZE:
            parent1 = tournament_selection(fitness_scores)
            parent2 = tournament_selection(fitness_scores)
            child = crossover(parent1, parent2)
            child = mutate(child, mutation_rate = max(0.02, 0.1 * (1 - gen / GENERATIONS)))
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
            move = select_best_move(board, chromosome_white)
        else:
            if use_stockfish_black:
                move = play_stockfish_move(board)
            else:
                move = select_best_move(board, chromosome_white)

        if move is None:
            break

        board.push(move)

    print(f"\nFinal game result (white advantage): {result}")
    
    exit()
    
