import chess
import chess.engine
import random
import copy
import chess.pgn

POPULATION_SIZE = 20
MAX_MOVES = 200 #Each individual will just generate 200 moves. If a victory happens before that, oh well
GENERATIONS = 30
MUTATION_RATE = 0.1

#Note - UCI stands for Universal Chess Interface. It's the chess notation encoding, like e2e4, bd2e3, etc.

#The board is handled via the chess library, and a move is done via the .push() function

#Generate the actual moves for the individual
def random_chromosome():
    board = chess.Board()
    chromosome = []

    while not board.is_game_over() and len(chromosome) < MAX_MOVES:
        move = random.choice(list(board.legal_moves))
        chromosome.append(move.uci())
        board.push(move)

    return chromosome

def evaluate_fitness(chromosome):
    #Push all of the chromosomes move's to the board, and evaluate the position afterwards
    board = chess.Board()
    for move_uci in chromosome:
        try:
            move = chess.Move.from_uci(move_uci)
            if move in board.legal_moves:
                board.push(move)
            else:
                return -float('inf') #If any moves are illegal, the individual is not fit
        except:
            return -float('inf')

    #Evaluate fitness if the game is concluded
    result = board.result(claim_draw=True)
    if result == "1-0":
        return 1000 #white won
    elif result == "0-1":
        return -1000 #black won
    elif result == "1/2-1/2":
        return 0 #draw
    else:
        #Otherwise fall back to material score
        return material_score(board)

def material_score(board):
    piece_values = {
        chess.PAWN: 1,
        chess.KNIGHT: 3,
        chess.BISHOP: 3,
        chess.ROOK: 5,
        chess.QUEEN: 9,
    }
    
    #Compute material score: positive for white, negative for black
    score = 0
    for piece_type, value in piece_values.items():
        score += len(board.pieces(piece_type, chess.WHITE)) * value
        score -= len(board.pieces(piece_type, chess.BLACK)) * value
    return score

def crossover(parent1, parent2):
    #Alternating move crossover
    
    child = []
    for i in range(min(len(parent1), len(parent2))):
        if i % 2 == 0:
            child.append(parent1[i])
        else:
            child.append(parent2[i])
            
    #Pad with remaining moves from the longer parent, parent length is not different in the current implementation, this is to guard for the future
    longer = parent1 if len(parent1) > len(parent2) else parent2
    child.extend(longer[len(child):])
    return child

def mutate(chromosome):
    #Mutate approximately MUTATION_RATE fraction of genes (moves)
    new_chromosome = copy.deepcopy(chromosome)
    board = chess.Board()

    for idx in range(len(new_chromosome)):
        # Build up the board to current move
        try:
            move = chess.Move.from_uci(new_chromosome[idx])
            if move in board.legal_moves:
                board.push(move)
            else:
                break  #If any move is illegal, no need to mutate, this individual will likely not be selected anyways and should be purged eventually regardless
        except:
            break

        #Grab a random legal move and overwrite the current move
        if random.random() < MUTATION_RATE:
            legal_moves = list(board.legal_moves)
            if legal_moves:
                new_move = random.choice(legal_moves).uci()
                new_chromosome[idx] = new_move
                board.push(chess.Move.from_uci(new_move))  # Advance the board
            else:
                break

    return new_chromosome

def genetic_algorithm():
    #Initialize the population
    population = [random_chromosome() for _ in range(POPULATION_SIZE)]

    
    for gen in range(0, GENERATIONS):
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
            child = mutate(child)
            next_gen.append(child)

        population = next_gen

    best_chromosome = max(population, key=evaluate_fitness)
    print("\nBest move sequence:")
    print(best_chromosome)
    return best_chromosome

def tournament_selection(fitness_scores, k=3):
    #Randomly select k individuals and return the best among them
    selected = random.sample(fitness_scores, k)
    selected.sort(key=lambda x: x[1], reverse=True)
    return selected[0][0]

#PGN is a format chess.com can use to import a game, convert the chromosome/sequence to PGN format
def sequence_to_pgn(move_sequence):
    board = chess.Board()
    game = chess.pgn.Game()
    node = game

    for move_uci in move_sequence:
        move = chess.Move.from_uci(move_uci)
        if move in board.legal_moves:
            board.push(move)
            node = node.add_main_variation(move)
        else:
            break

    return str(game)
    
if __name__ == "__main__":
    best_sequence = genetic_algorithm()
    
    #Create the final state to visualize
    board = chess.Board()
    for move_uci in best_sequence:
        board.push(chess.Move.from_uci(move_uci))
    
    print(board)
    
    pgn_string = sequence_to_pgn(best_sequence)

    with open("genetic_game.pgn", "w") as f:
        f.write(pgn_string)
        