import chess
import numpy as np
import chess.pgn

chromosome = [3.681323372733834, 1.2199358111967356, 3.4749116314889616, 0.21464572903608664, 2.2975685652049744, 3.3003943905601685, 4.440464181755182, 3.0460535872924677, 1.088666773890384, np.array([1.97946098, 3.83649238, 3.00723801, 3.12463434, 4.95788776,
       4.28692043, 2.31758424, 4.39998272, 4.63055802, 2.48499279,
       1.71787535, 2.97689258, 2.63493989, 1.3936035 , 3.52489919,
       4.38591915, 1.86086153, 2.31659338, 4.23477523, 2.71860866,
       1.93433567, 4.09049523, 1.89283685, 4.17406757, 3.6483174 ,
       3.19161592, 1.77016814, 4.32707386, 4.37058391, 3.002198  ,
       2.36929851, 2.98317571, 2.54068385, 4.2018582 , 4.01285315,
       3.55229655, 2.30122415, 1.56887469, 1.96744729, 4.25678325,
       4.13425136, 3.37489401, 1.71762804, 2.15052578, 2.3705397 ,
       4.9134985 , 3.04537121, 2.26365454, 1.95783016, 1.48348551,
       4.84753063, 2.15260748, 4.85563224, 3.49954742, 3.48278384,
       3.31166908, 3.51086723, 4.81900886, 0.94154943, 1.55643123,
       1.28378902, 4.39919848, 1.97815924, 4.70590714]), np.array([4.7333691 , 1.86994278, 3.70837214, 2.88408563, 1.40612272,
       4.49024629, 3.93576859, 2.10247554, 4.66305631, 1.71749011,
       2.15022553, 1.51835233, 1.02771049, 3.46046463, 3.17302353,
       3.84364168, 3.13286578, 3.03072405, 2.3309688 , 4.25354462,
       3.68747419, 3.59150078, 3.40208523, 2.49550588, 3.03131174,
       4.31990608, 3.69563012, 3.42501339, 1.8972703 , 3.7956317 ,
       3.60364885, 1.88134093, 1.95682566, 4.25093956, 2.0463051 ,
       4.29161083, 3.67185844, 4.30812318, 1.26283625, 4.51807491,
       3.33243911, 3.0118029 , 3.48275484, 2.93496796, 3.53819724,
       1.23624932, 2.03336069, 3.8144416 , 1.76685349, 2.53373055,
       1.06039441, 4.41163267, 1.45856523, 2.52518415, 2.42037849,
       4.7301291 , 3.00523003, 1.4443044 , 3.38952684, 2.66502894,
       3.92599758, 4.12138539, 4.09298255, 4.14429755]), np.array([1.38511545, 2.84555463, 4.14939289, 3.73037537, 4.40083074,
       3.01417967, 2.33921082, 3.86332418, 3.90319543, 1.18431298,
       2.86934797, 2.08610203, 4.86517128, 2.91265899, 1.61886373,
       2.77598426, 2.81676652, 4.53550403, 1.98006663, 4.21485627,
       1.42766682, 3.93687645, 4.3485257 , 4.93383106, 3.08168032,
       2.60386137, 3.50522159, 3.1380107 , 3.41851775, 4.25335947,
       1.53418783, 1.91446542, 4.07830455, 1.61942403, 4.24658829,
       1.15092481, 1.65858618, 2.28975991, 4.4699268 , 1.25658044,
       2.54094922, 3.73861159, 1.8244042 , 2.42022632, 1.04444808,
       3.0023863 , 4.80212546, 1.31000086, 4.68714725, 3.64230138,
       3.42108404, 2.77171938, 2.0385665 , 3.18300129, 3.21267715,
       4.4174114 , 4.79590165, 2.19935641, 3.16606704, 3.60129233,
       3.19219523, 1.50700558, 3.96275036, 4.48763786]), np.array([4.78353034, 4.15761615, 1.14208298, 3.53734   , 2.95225075,
       3.05861628, 4.96958671, 3.00423385, 3.54983167, 2.6791743 ,
       3.85166524, 3.05158038, 1.58409168, 1.23129049, 4.48036964,
       3.91587353, 3.48274476, 2.35054885, 4.89396113, 3.07381464,
       1.00095934, 3.33915416, 2.2859916 , 3.51590473, 1.19837426,
       1.5470051 , 1.68698676, 4.18978111, 3.0082368 , 4.79238191,
       1.78291829, 1.28435099, 1.43427537, 1.75991073, 2.17292947,
       1.87865323, 4.05648018, 2.2415916 , 4.75867247, 4.82050362,
       1.43468221, 3.11705363, 1.81883517, 2.1124426 , 3.05748589,
       4.41692176, 4.80852715, 3.42451864, 3.43905601, 1.7494331 ,
       1.06933777, 1.31804173, 5.00675677, 4.57783696, 2.16655819,
       1.87225988, 2.64002448, 2.94400716, 1.36686266, 2.1842574 ,
       1.1510711 , 1.13487701, 3.43030187, 2.73179296]), np.array([4.28372608, 5.03110657, 1.49196859, 3.69236259, 1.8900494 ,
       3.91099198, 3.96479479, 1.3377019 , 4.80419659, 4.78187361,
       4.53856177, 4.6181443 , 2.38366096, 1.40080174, 1.82730586,
       3.38064626, 3.82993745, 1.25820177, 1.87222538, 1.0972005 ,
       2.42082845, 2.90998449, 2.86938924, 4.64316188, 0.92078196,
       2.05911844, 4.65671441, 1.31376624, 1.31875554, 3.9970901 ,
       2.27811097, 0.9552742 , 4.19679216, 3.12098195, 3.8397111 ,
       4.64492365, 4.09223485, 4.44491244, 1.64597874, 3.61704408,
       1.69194124, 3.86052381, 4.6478502 , 4.00160696, 2.36098243,
       1.20345702, 4.14023461, 3.44566015, 4.91166603, 1.86710786,
       3.25251381, 4.11909214, 2.80985452, 4.26204284, 4.27443671,
       3.15041597, 4.83708724, 3.07236   , 4.8517261 , 4.7832492 ,
       3.17356656, 2.35889499, 3.03721097, 2.98641649]), np.array([2.72830846, 1.24037869, 1.61598717, 1.8131966 , 3.77308706,
       3.4862386 , 2.54423751, 4.61989528, 3.6076079 , 4.40818048,
       3.68909796, 3.52624095, 4.55908581, 1.32269912, 4.80721894,
       3.76727297, 5.07835248, 3.38630819, 4.36614762, 1.25979047,
       4.91659931, 4.81248587, 3.89720277, 1.44523653, 2.72082795,
       4.57126552, 1.27471153, 3.3797561 , 1.70574902, 4.4733053 ,
       4.48566902, 3.43784607, 1.82878145, 1.13028281, 3.28349395,
       3.87621591, 2.34544475, 4.49290339, 1.81795198, 4.68328604,
       2.35761808, 2.86612128, 2.93240349, 3.30601796, 1.37862069,
       2.67844435, 2.21438468, 4.67253158, 1.76236226, 4.41853987,
       3.58000125, 4.72021441, 1.94175622, 2.00600494, 3.17228946,
       3.81452108, 4.39007472, 2.57561532, 1.11697297, 1.67167657,
       1.09725188, 2.08079614, 2.78901302, 1.03752416])]
   

'''
output_chromosome = []
for element in chromosome:
    if type(element) == np.ndarray:
        output_chromosome.extend(element.tolist())
    else:
        output_chromosome.append(element)

#output_chromosome = np.array(output_chromosome, dtype='float64').flatten().tolist()
import json
json.dump({"chromosome": output_chromosome}, open("chromosome_worse.json", "w"))
exit()
'''

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

    print("\tPost open file penalty:", king_safety_penalty)
    
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

    print("\tPost adjacency penalty:", king_safety_penalty)
    
    #If king not on edge of board, penalize
    #Penalize harder if not on rank 0
    if king_rank != 0:
        if king_file != 0 and king_file != 7 and king_rank != 7:
            king_safety_penalty += 0.5 * penalty
        else:
            king_safety_penalty += 0.3 * penalty
    print("\tPost edge of board penalty:", king_safety_penalty)
    
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
    print("\tPost threat from black penalty:", king_safety_penalty)
    
    #If a king move is blocked because of check, penalize
    king_pseudo_moves = [move for move in board.generate_pseudo_legal_moves() if move.from_square == king_square]

    board_copy = board.copy()
    for move in king_pseudo_moves:
        board_copy.push(move)
        if board_copy.is_check():
            king_safety_penalty += penalty
        board_copy.pop()
    print("\tPost king moves in check penalty:", king_safety_penalty)
    
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
    
    print("Post chromosome material score:", score)
    
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

    print("Post true material score:", score)
    
    
    #Compute mobility, defined simply as the number of available moves
    board_copy = board.copy()
    board_copy.turn = chess.WHITE
    score += piece_mobility_val * board.legal_moves.count()
    board_copy.turn = chess.BLACK
    score -= piece_mobility_val * board.legal_moves.count()
    
    print("Post mobility score:", score)
    
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
    print("Post center control score:", score)

    #King safety penalty
    print(king_safety_val)
    score += king_safety_val * king_safety(board)
    print("Post king safety score:", score)
    
    #Score increases if the enemy king is being pressured
    #Not implementing this for now
    #score += enemy_king_pressure_val * enemy_king_pressure(board)
    
    
    #Piece-based weight matrices
    for square in board.pieces(chess.PAWN, chess.WHITE):
        score += pawn_position_weights[square]

    for square in board.pieces(chess.PAWN, chess.BLACK):
        score -= pawn_position_weights[chess.square_mirror(square)]
    print("Post pawn position score:", score)

    
    for square in board.pieces(chess.KNIGHT, chess.WHITE):
        score += knight_position_weights[square]
    for square in board.pieces(chess.KNIGHT, chess.BLACK):
        score -= knight_position_weights[chess.square_mirror(square)]
    print("Post knight position score:", score)
    
    for square in board.pieces(chess.BISHOP, chess.WHITE):
        score += bishop_position_weights[square]
    for square in board.pieces(chess.BISHOP, chess.BLACK):
        score -= bishop_position_weights[chess.square_mirror(square)]
    print("Post bishop position score:", score)
    
    for square in board.pieces(chess.ROOK, chess.WHITE):
        score += rook_position_weights[square]
    for square in board.pieces(chess.ROOK, chess.BLACK):
        score -= rook_position_weights[chess.square_mirror(square)]
    print("Post rook position score:", score)
    
    for square in board.pieces(chess.QUEEN, chess.WHITE):
        score += queen_position_weights[square]
    for square in board.pieces(chess.QUEEN, chess.BLACK):
        score -= queen_position_weights[chess.square_mirror(square)]
    print("Post queen position score:", score)
    
    return score

def select_best_move(board, chromosome, depth=1):
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

# Set up game board and PGN recorder
board = chess.Board()
'''
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
game = chess.pgn.Game()
game.headers["White"] = "Agent"
game.headers["Black"] = "Human"
node = game

# Main game loop
while not board.is_game_over():
    print(board.unicode(borders=True))
    print()

    if board.turn == chess.BLACK:
        # Human move
        move_uci = input("Your move (in UCI, e.g. e2e4): ")
        try:
            move = chess.Move.from_uci(move_uci)
            if move in board.legal_moves:
                board.push(move)
                node = node.add_variation(move)
            else:
                print("Illegal move!")
        except Exception as e:
            print("Invalid input!", e)
    else:
        # Agent move
        move = select_best_move(board, chromosome)
        print(f"Agent plays: {move.uci()}")
        board.push(move)
        node = node.add_variation(move)

print(board.unicode(borders=True))
print("Game over!")
print("Result: ", board.result())
game.headers["Result"] = board.result()

# Ask to save PGN
save = input("Save game as PGN? (y/n): ")
if save.lower() == 'y':
    filename = input("Enter filename (example: mygame.pgn): ")
    with open(filename, "w") as pgn_file:
        print(game, file=pgn_file)
    print(f"Game saved as {filename}")
    