/* Jerry - A Chess Graphical User Interface
 * Copyright (C) 2014-2016 Dominik Klein
 * Copyright (C) 2015-2016 Karl Josef Klein
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */


#ifndef BOARD_H
#define BOARD_H


#include <cstdint>
#include <QRegularExpression>
#include <QMap>
#include "constants.h"
#include "move.h"

namespace chess {


class Board
{

public:

    static int alpha_to_pos(QChar alpha);
    static QPoint internal_to_xy(int internal_coord);
    static int xy_to_internal(int x, int y);

    /**
     * @brief turn is either == WHITE or == BLACK
     */
    bool turn;

    /**
     * @brief halfmove_clock number of halfmoves from beginning.
     *                       automatically updated after applying a move
     */
    int halfmove_clock;

    /**
     * @brief fullmove_number
     */
    int fullmove_number;

    /**
     * @brief last_was_null set to true, if last the last move leading to
     *                      this board position was a null move
     */
    bool last_was_null;

    /**
     * @brief Board creates empty board, no castling rights
     */
    Board();

    /**
     * @brief Board creates board w/ initial position, castling rights set if called with true
     *        creates empty board board, no castling rights if called with false
     * @param initial_position triggers wether initial position or empty should be created
     */
    Board(bool initial_position);

    /**
     * @brief Board
     * @param fen_string creates board from FEN string
     */
    Board(const QString &fen_string);

    /**
     * @brief copy constructor to create deep copy of board
     *        TODO: do we really need this?! default might suffice after refactoring
     * @param other
     */
   Board(const Board &other);

    /**
     * @brief fen returns FEN string of current board
     * @return
     */
    QString fen();

    /**
     * @brief apply applies supplied move. doesn't check for legality
     *        no check of legality. always call board.is_legal(m) before applying move
     * @param m move to apply
     */
    void apply(const Move &m);

    /**
     * @brief undo undoes the very last move. undoing can only be done once for the very
     *             last move that was applied before, i.e. apply undo apply undo is ok,
     *             but apply apply undo undo is not. throws logic error if called
     *             in wrong fashion. check with is_undo_available() when in doubt
     */
    void undo();

    /**
     * @brief pseudo_legal_moves returns move list with all pseudo-legal moves of
     *                           current position
     * @return
     */
    QVector<Move> pseudo_legal_moves();

    QVector<Move> pseudo_legal_moves(uint8_t to_square, uint8_t piece_type);

    /**
     * @brief pseudo_legal_moves_from returns move list with pseudo legal moves
     *                                from supplied square index
     * @param from_square_idx         square from which move originates
     *                                must be in internal board representation, i.e.
     *                                in range 21 ... 98
     * @param with_castles            include castling in returned list
     * @param turn_color              either WHITE or BLACK, i.e. the player to move
     * @return pseudo legal move list
     */
    QVector<Move> pseudo_legal_moves_from(int from_square_idx, bool with_castles, bool turn_color);
    QVector<Move> pseudo_legal_moves_to(uint8_t to_square, uint8_t piece_type, bool with_castles, bool turn);

    /**
     * @brief legal_moves returns move list of all legal moves in position
     * @return move list
     */
    QVector<Move> legal_moves();
    QVector<Move> legal_moves(uint8_t to_square, uint8_t piece_type);

    /**
     * @brief legal_moves_from computes all legal moves originating in from square
     * @param from_square  move originates from this square. must be in range 21...98
     * @return move list of legal moves
     */
    QVector<Move> legal_moves_from(int from_square);

    QVector<Move> legals_from_pseudos(QVector<Move> &pseudos);

    /**
     * @brief pseudo_is_legal_move checks whether supplied pseudo legal move is legal
     *              in current position. Does NOT check whether supplied move is pseudo legal!!!
     * @return result of checking legality
     */
    bool pseudo_is_legal_move(const Move &);

    /**
     * @brief is_legal_move checks whether the supplied move is legal in the board
     *                      position. Always call before applying a move on a board!
     * @return true, if the move is legal, otherwise false
     */
    bool is_legal_move(const Move&);

    /**
     * @brief is_legal_and_promotes checks whether supplied move is legal and is
     *                              a pawn move promoting to another piece
     * @return true only if both move is legal and pawn move and promotes. false, otherwise.
     */
    bool is_legal_and_promotes(const Move&);

    /**
     * @brief is_checkmate tests whether player who is on the move in current position
     *                     is in checkmate (i.e. is in check but has not legal move
     *                     escaping thfsane check)
     * @return true, if player in checkmate, false otherwise.
     */
    bool is_checkmate();

    /**
     * @brief is_stalemate tests whether player who is on the move in current position
     *                     is in stalemate (i.e. is not in check but all legal moves
     *                     would result in check)
     * @return true, if position is stalemate, false otherwise.
     */
    bool is_stalemate();

    /**
     * @brief is_check checks if the player whose on the move in the current position
     *                 is in check
     * @return true, if player in check, false otherwise.
     */
    bool is_check();

    /**
     * @brief san computes the standard algebraic notation for the supplied move
     *        given the current position. the supplied move MUST be legal on this
     *        board
     * @param m Move to get the san for
     * @return string containing san representation of move (no move number!)
     */
    QString san(const Move &m);

    /**
     * @brief movePromotes checks if the supplied move (ignoring the promotion value stored
     *                     in the move is a pawn move to the 8th / 1st rank, i.e. promoting)
     * @param m move of concern
     * @return true if move promotes, false otherwise
     */
    bool movePromotes(const Move&m);

    /**
     * @brief is_initial_position checks whether the current placement of the pieces
     *                            corresponds to the inital chess position.
     * @return true if initial position, false otherwise.
     */
    bool is_initial_position();

    /**
     * @brief can_castle_wking checks whether the castling rights for the
     *                         position are such that the White king is allowed
     *                         to castle kingside in this position.
     * @return true if king may castle, false otherwise.
     */
    bool can_castle_wking();

    /**
     * @brief can_castle_bking see can_castle_wking
     * @return
     */
    bool can_castle_bking();

    /**
     * @brief can_castle_wqueen see can_castle_wking
     * @return
     */
    bool can_castle_wqueen();

    /**
     * @brief can_castle_bqueen see can_castle_wking
     * @return
     */
    bool can_castle_bqueen();

    /**
     * @brief is_undo_available checks whether the current board position
     *                          has enough information to apply the undo()
     *                          operation, i.e. take back the last move
     *                          and return to the previous board state.
     * @return true if info is available (i.e. undo() may be called), false otherwise
     */
    bool is_undo_available();

    /**
     * @brief set_castle_wking set/unset the right of the white king to castle
     *                         kingside.
     * @param can_do true to allow white to castle, false otherwise.
     */
    void set_castle_wking(bool can_do);

    /**
     * @brief set_castle_bking see set_castle_wking
     * @param can_do
     */
    void set_castle_bking(bool can_do);

    /**
     * @brief set_castle_wqueen see set_castle_wking
     * @param can_do
     */
    void set_castle_wqueen(bool can_do);

    /**
     * @brief set_castle_bqueen see set_castle_wking
     * @param can_do
     */
    void set_castle_bqueen(bool can_do);

    /**
     * @brief set_piece_at sets a piece a the supplied board position (x,y)
     * @param x int in the range (0,7) representing the column (i.e. a - h)
     * @param y int in the range (0,7) representing the row (i.e. 0 - 7)
     * @param piece the piece type (constants such as WHITE_KING, BLACK_QUEEN, EMPTY etc.)
     */
    void set_piece_at(int x, int y, uint8_t piece);

    /**
     * @brief get_piece_at gets piece a the supplied board position (x,y)
     * @param x int in the range (0,7) representing the column (i.e. a - h)
     * @param y int in the range (0,7) representing the row (i.e. 0 - 7)
     * @return piece type encoded as uint8_t (i.e. BLACK_QUEEN or EMPTY)
     */
    uint8_t get_piece_at(int x, int y);

    /**
     * @brief get_piece_type_at get the piece type as uint8_t at supplied
     *                          position. piece type is always the piece
     *                          encoded as if it were are white piece (see
     *                          KING, QUEEN, or empty).
     * @param x int in the range (0,7) representing the column (i.e. a - h)
     * @param y int in the range (0,7) representing the column (i.e. a - h)
     * @return piece encoding
     */
    uint8_t get_piece_type_at(int x, int y);

    /**
     * @brief get_piece_color_at returns color (i.e. WHITE or BLACK)
     *                           at supplied position. Don't call if there is
     *                           is no piece at the position! (check with
     *                           piece_type first)
     * @param x column
     * @param y row
     * @return piece color
     */
    bool get_piece_color_at(int x, int y);

    /**
     * @brief piece_color same as get_piece_color_at but uses here
     *                    the internal position encoding to specify the
     *                    field of the board. see this header file
     * @param idx internal board encoding specifying field position
     * @return piece color
     */
    bool piece_color(uint8_t idx);

    /**
     * @brief piece_at returns the piece at idx in internal board format
     *                    i.e. WHITE_PAWN = 0x01, BLACK_PAWN etc.
     * @param idx internal board encoding specifying field position
     * @return piece in internal format (i.e. 0x00 ... 0x06 and 0x81 ... 0x86
     */
    uint8_t piece_at(uint8_t idx);

    /**
     * @brief piece_type see piece_color and get_piece_type_at()
     * @param idx
     * @return
     */
    uint8_t piece_type(uint8_t idx);

    /**
     * @brief get_idx_single_piece checks if there is only
     *        one piece (i.e. only one black queen) on the board
     *        if so, returns the board index (internal representation)
     *        otherwise returns -1
     * @param piece to check
     * @return board index or -1
     */
    int get_idx_single_piece(uint8_t piece);

    /**
     * @brief get_king_pos get position of king
     * @param king either white or black king
     * @return position of king in internal board format
     */
    int get_king_pos(uint8_t king);

    /**
     * @brief is_consistent rudimentary check of position consistency
     * @return true if the conditions below are true, false otherwise
     *
     * NOTE: this doesn't capture _all_ invalid positions, but
     *       the most common reasons
     * there exists one white and one black king [ok]
     * kings are >= 1 field apart                [ok]
     * side not to move is not in check          [ok]
     * side to move has less than three attackers who give check
     * if side to move is in check w/ two attackers:
     *     following must not hold:
     *         pawn+(pawn, bishop, knight), bishop+bishop, knight+knight
     * each side has less than 8 pawns          [ok]
     * no pawns in first or last row            [ok]
     * extra pieces = Math.max(0, num_queens-1) + Math.max(0, num_rooks-2)...
     *             and extra_pieces <= (8-num_pawns))
     * no more than 5 pawns in a or h line
     * checks consistency of castling rights. if set, then verify w/
     * is_black_castle_right_lost() and is_white_castle_lost()
     */
    bool is_consistent();

    /**
     * @brief is_black_king_castle_right_lost does not return castling
     *                  rights of current position (for that call can_castle_bking() etc. )
     *                  instead checks whether black king and rook are in initial position
     *                  or have moved
     * @return false, if black king or rook have moved from inital pos, true otherwise.
     */
    bool is_black_king_castle_right_lost();

    /**
     * @brief is_black_queen_castle_right_lost see is_black_king_castle_right_lost()
     * @return
     */
    bool is_black_queen_castle_right_lost();

    /**
     * @brief is_white_king_castle_right_lost see is_black_king_castle_right_lost()
     * @return
     */
    bool is_white_king_castle_right_lost();

    /**
     * @brief is_white_queen_castle_right_lost see is_black_king_castle_right_lost()
     * @return
     */
    bool is_white_queen_castle_right_lost();

    uint8_t get_ep_target();

    bool can_claim_fifty_moves();

    quint64 zobrist();
    quint64 pos_hash();

    QString printRaw();


private:
    /**
     * @brief init_pos
     * is the inital board position
     * for encodings and the general approach
     * see "First Steps in Computer Chess Programming"
     * BYTE Magazine, October 1978
     */
    static const uint8_t init_pos [120];
    /**
     * @brief board stores the current position
     * essentially linearized 10x12 array
     */
    uint8_t board[120];

    /**
     * @brief old_board stores the previous position for undo()
     */
    uint8_t old_board[120];

    uint8_t piece_list[2][7][10];

    /**
     * @brief turn is either WHITE or BLACK
     */
    bool undo_available;

    bool m_zobrist_initialized;
    bool m_pos_hash_initialized;
    quint64 m_zobrist;
    quint64 m_pos_hash;

    /**
     * @brief castling_rights stores the castling rights
     * by using bit positions within byte. Bit positions are
     * CASTLE_WKING_POS, CASTLE_WQUEEN_POS, CASTLE_BKING_POS
     * and CASTLE_BQUEEN_POS
     */
    uint8_t castling_rights;
    uint8_t prev_castling_rights;

    uint8_t en_passent_target;
    uint8_t prev_en_passent_target;

    int prev_halfmove_clock;

    bool is_empty(uint8_t idx);
    bool is_offside(uint8_t idx);
    bool is_white_at(uint8_t idx);
    bool is_attacked(int idx, bool attacker_color);
    bool castles_wking(const Move &m);
    bool castles_bking(const Move &m);
    bool castles_wqueen(const Move &m);
    bool castles_bqueen(const Move &m);
    uint8_t piece_from_symbol(QChar c);
    QChar piece_to_symbol(uint8_t idx);
    QString idx_to_str(int idx);

    void init_piece_list();

    int zobrist_piece_type(uint8_t piece);

    void remove_from_piece_list(bool color, uint8_t piece_type, uint8_t idx);
    void add_to_piece_list(bool color, uint8_t piece_type, uint8_t idx);

    friend std::ostream& operator<<(std::ostream& strm, const Board &b);



};

}
#endif // BOARD_H
