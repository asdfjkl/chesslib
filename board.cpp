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


#include <QString>
#include <QList>
#include <QDebug>
#include <QStringList>
#include <QVector>
#include "board.h"
#include <iostream>
#include <exception>
#include <algorithm>
#include <assert.h>
#include "move.h"

using namespace std;

namespace chess {


Board::Board() {

    this->turn = WHITE;
    for(int i=0;i<120;i++) {
        this->board[i] = EMPTY_POS[i];
        this->old_board[i] = 0xFF;
    }
    this->init_piece_list();
    this->castle_wking_ok = false;
    this->castle_wqueen_ok = false;
    this->castle_bking_ok = false;
    this->castle_bqueen_ok = false;
    this->prev_castle_wking_ok = false;
    this->prev_castle_wqueen_ok = false;
    this->prev_castle_bking_ok = false;
    this->prev_castle_bqueen_ok = false;
    this->en_passent_target = 0;
    this->halfmove_clock = 0;
    this->fullmove_number = 1;
    this->undo_available = false;
    this->last_was_null = false;
    this->prev_halfmove_clock = 0;
    this->zobrist_initialized = false;
    this->pos_hash_initialized = false;
}

void Board::init_piece_list() {

    for(int i=0;i<7;i++) {
        for(int j=0;j<10;j++) {
            this->piece_list[WHITE][i][j] = EMPTY;
            this->piece_list[BLACK][i][j] = EMPTY;
        }
    }
    for(uint8_t i=21;i<99;i++) {
        uint8_t piece = this->board[i];
        if(!(piece == EMPTY) && !(piece == 0xFF)) {
            bool color = WHITE;
            if(piece > 0x80) {
                piece = piece - 0x80;
                color = BLACK;
            }
            // piece contains now the piece type
            for(int j=0;j<10;j++) {
                if(this->piece_list[color][piece][j] == EMPTY) {
                   this->piece_list[color][piece][j] = i;
                   break;
                }
            }
        }
    }
}


Board::Board(bool initial_position) {
    this->turn = WHITE;
    if(initial_position) {
        for(int i=0;i<120;i++) {
            this->board[i] = chess::INIT_POS[i];
            this->old_board[i] = 0xFF;
        }
        this->castle_wking_ok = true;
        this->castle_wqueen_ok = true;
        this->castle_bking_ok = true;
        this->castle_bqueen_ok = true;
    } else {
        for(int i=0;i<120;i++) {
            this->board[i] = EMPTY_POS[i];
        }
        this->castle_wking_ok = false;
        this->castle_wqueen_ok = false;
        this->castle_bking_ok = false;
        this->castle_bqueen_ok = false;
    }
    this->init_piece_list();
    this->en_passent_target = 0;
    this->halfmove_clock = 0;
    this->fullmove_number = 1;
    this->undo_available = false;
    this->last_was_null = false;
    this->prev_halfmove_clock = 0;
    this->prev_castle_wking_ok = false;
    this->prev_castle_wqueen_ok = false;
    this->prev_castle_bking_ok = false;
    this->prev_castle_bqueen_ok = false;
    this->zobrist_initialized = false;
    this->pos_hash_initialized = false;
}

bool Board::is_initial_position() const {
    if(!this->turn == WHITE) {
        return false;
    }
    for(int i=0;i<120;i++) {
        if(this->board[i] != chess::INIT_POS[i]) {
            return false;
        }
    }
    if(!this->castle_wking_ok ||
            !this->castle_wqueen_ok ||
            !this->castle_bking_ok ||
            !this->castle_bqueen_ok) {
        return false;
    }
    if(this->en_passent_target != 0) {
        return false;
    }
    if(this->halfmove_clock != 0) {
        return false;
    }
    if(this->fullmove_number != 1) {
        return false;
    }
    if(this->undo_available) {
        return false;
    }
    return true;
}

// returns 0 if no e.p. field in current position
int Board::get_ep_target() const {
    return this->en_passent_target;
}


// returns 'empty' if c is no valid piece symbol in
// fen notation
int Board::piece_from_symbol(QChar c) const {
    if(c == QChar('K')) {
        return 0x06;
    }
    if(c == QChar('Q')) {
        return 0x05;
    }
    if(c == QChar('R')) {
        return 0x04;
    }
    if(c == QChar('B')) {
        return 0x03;
    }
    if(c == QChar('N')) {
        return 0x02;
    }
    if(c == QChar('P')) {
        return 0x01;
    }
    if(c == QChar('k')) {
        return 0x86;
    }
    if(c == QChar('q')) {
        return 0x85;
    }
    if(c == QChar('r')) {
        return 0x84;
    }
    if(c == QChar('b')) {
        return 0x83;
    }
    if(c == QChar('n')) {
        return 0x82;
    }
    if(c == QChar('p')) {
        return 0x81;
    }
    throw new std::invalid_argument("piece_from_symbol: " + QString(c).toStdString());
}

void Board::set_piece_at(int x, int y, int piece) {
    // check wether x,y is a valid location on chess board
    // and wether piece is a valid piece
    if(x>=0 && x<8 && y>=0 && y <8 &&
            ((piece >= 0x01 && piece <= 0x07) ||  // white piece
             (piece >= 0x81 && piece <= 0x87) || (piece == 0x00))) { // black piece or empty
        int idx = ((y+2)*10) + (x+1);
        this->board[idx] = piece;
    } else {
        throw std::invalid_argument("called set_piece_at with invalid paramters");
    }
}

int Board::get_piece_at(int x, int y) const {
    if(x>=0 && x<8 && y>=0 && y <8) {
        int idx = ((y+2)*10) + (x+1);
        return this->board[idx];
    } else {
        throw std::invalid_argument("called get_piece_at with invalid paramters");
    }
}

int Board::get_piece_type_at(int x, int y) const {
    if(x>=0 && x<8 && y>=0 && y <8) {
        int idx = ((y+2)*10) + (x+1);
        int piece = this->board[idx];
        if(piece == EMPTY) {
            throw std::invalid_argument("called get_piece_type_at, but field is empty");
        }
        if(piece == FRINGE) {
            throw std::invalid_argument("called get_piece_type_at, but field is in fringe");
        }
        if(piece > 0x80) {
            return piece - 0x80;
        } else {
            return piece;
        }
    } else {
        throw std::invalid_argument("called get_piece_type_at with invalid paramters");
    }
}

bool Board::get_piece_color_at(int x, int y) const {
    if(x>=0 && x<8 && y>=0 && y <8) {
        int idx = ((y+2)*10) + (x+1);
        return this->get_piece_color(idx);
    } else {
        throw std::invalid_argument("called get_piece_color_at with invalid paramters");
    }
}


// returns 'empty' if c is no valid piece symbol in
// fen notation
QChar Board::piece_to_symbol(int piece) const {
    if(piece == 0x06) {
        return QChar('K');
    }
    if(piece == 0x05) {
        return QChar('Q');
    }
    if(piece == 0x04) {
        return QChar('R');
    }
    if(piece == 0x03) {
        return QChar('B');
    }
    if(piece == 0x02) {
        return QChar('N');
    }
    if(piece == 0x01) {
        return QChar('P');
    }
    if(piece == 0x86) {
        return QChar('k');
    }
    if(piece == 0x85) {
        return QChar('q');
    }
    if(piece == 0x84) {
        return QChar('r');
    }
    if(piece == 0x83) {
        return QChar('b');
    }
    if(piece == 0x82) {
        return QChar('n');
    }
    if(piece == 0x81) {
        return  QChar('p');
    }
    throw std::invalid_argument("called piece to symbol, but square contains no piece!");
}

Board::Board(const QString &fen_string) {
    for(int i=0;i<120;i++) {
        this->board[i] = EMPTY_POS[i];
        this->old_board[i] = 0xFF;
    }

    // check that we have six parts in fen, each separated by space
    // if last two parts are missing (fullmove no. + halfmove clock)
    // try to still parse the game
    QStringList fen_parts = fen_string.split(QChar(' '));
    if(fen_parts.size() < 4) {
        throw std::invalid_argument("fen: parts missing 6 fen parts");
    }
    // check that the first part consists of 8 rows, each sep. by /
    QStringList rows = fen_parts.at(0).split(QChar('/'));
    if(rows.size() != 8) {
        throw std::invalid_argument("fen: not 8 rows in 0th part");
    }
    // check that in each row, there are no two consecutive digits
    for(int i=0;i<rows.size();i++) {
        QString row = rows.at(i);
        int field_sum = 0;
        bool previous_was_digit = false;
        for(int j=0;j<row.size();j++) {
            QChar rj = row.at(j);
            QChar rjl = rj.toLower();
            if(rj == QChar::fromLatin1('1') ||
                    rj == QChar::fromLatin1('2') ||
                    rj == QChar::fromLatin1('3') ||
                    rj == QChar::fromLatin1('4') ||
                    rj == QChar::fromLatin1('5') ||
                    rj == QChar::fromLatin1('6') ||
                    rj == QChar::fromLatin1('7') ||
                    rj == QChar::fromLatin1('8')) {
                if(previous_was_digit) {
                    throw std::invalid_argument("fen: two consecutive digits in rows");
                } else {
                    field_sum += rj.digitValue();
                    previous_was_digit = true;
                }
            } else if(rjl == QChar::fromLatin1('p') ||
                      rjl == QChar::fromLatin1('n') ||
                      rjl == QChar::fromLatin1('b') ||
                      rjl == QChar::fromLatin1('r') ||
                      rjl == QChar::fromLatin1('q') ||
                      rjl == QChar::fromLatin1('k')) {
                field_sum += 1;
                previous_was_digit = false;
            } else {
                throw std::invalid_argument("fen: two consecutive chars in rows");
            }
        }
        // validate that there are 8 alphanums in each row
        if(field_sum != 8) {
            throw std::invalid_argument("fen: field sum is not 8");
        }
    }
    // check that turn part is valid
    if(!(fen_parts.at(1) == QString::fromLatin1("w") || fen_parts.at(1) == QString::fromLatin1("b"))) {
        throw std::invalid_argument("turn part is invalid");
    }
    // check that castles part in correctly encoded using regex
    QRegularExpressionMatch match = FEN_CASTLES_REGEX.match(fen_parts[2]);
    if(!match.hasMatch()) {
        throw std::invalid_argument("castles encoding is invalid");
    }
    // check correct encoding of en passent squares
    if(fen_parts.at(3) != QChar::fromLatin1('-')) {
        if(fen_parts.at(1) == QChar::fromLatin1('w')) {
            // should be something like "e6" etc. if white is to move
            // check that int value part is sixth rank
            if(fen_parts.at(3).size() != 2 || fen_parts.at(3).at(1) != QChar::fromLatin1('6')) {
                throw std::invalid_argument("invalid e.p. encoding (white to move)");
            }
        } else {
            if(fen_parts.at(3).size() != 2 || fen_parts.at(3).at(1) != QChar::fromLatin1('3')) {
                throw std::invalid_argument("invalid e.p. encoding (black to move)");
            }
        }
    }
    // half-move counter validity (if half-move is present)
    if((fen_parts.size() >= 5) && fen_parts.at(4).toInt() < 0) {
        throw std::invalid_argument("negative half move clock or not a number");
    }
    // full move number validity (if full move number is present)
    if((fen_parts.size() >= 6) && fen_parts.at(5).toInt() < 0) {
        throw std::invalid_argument("fullmove number not positive");
    }
    // set pieces
    for(int i=0;i<rows.size();i++) {
        int square_index = 91 - (i*10);
        QString row = rows.at(i);
        for(int j=0;j<row.size();j++) {
            QChar rj = row.at(j);
            QChar rjl = rj.toLower();
            if(rj == QChar::fromLatin1('1') ||
                    rj == QChar::fromLatin1('2') ||
                    rj == QChar::fromLatin1('3') ||
                    rj == QChar::fromLatin1('4') ||
                    rj == QChar::fromLatin1('5') ||
                    rj == QChar::fromLatin1('6') ||
                    rj == QChar::fromLatin1('7') ||
                    rj == QChar::fromLatin1('8')) {
                square_index += rj.digitValue();
            } else if(rjl == QChar::fromLatin1('p') ||
                      rjl == QChar::fromLatin1('n') ||
                      rjl == QChar::fromLatin1('b') ||
                      rjl == QChar::fromLatin1('r') ||
                      rjl == QChar::fromLatin1('q') ||
                      rjl == QChar::fromLatin1('k')) {
                uint8_t piece = this->piece_from_symbol(rj);
                this->board[square_index] = piece;
                square_index += 1;
            }
        }
    }
    // set turn
    if(fen_parts.at(1) == QString::fromLatin1("w")) {
        this->turn = WHITE;
    }
    if(fen_parts.at(1) == QString::fromLatin1("b")) {
        this->turn = BLACK;
    }
    this->castle_wking_ok = false;
    this->castle_wqueen_ok = false;
    this->castle_bking_ok = false;
    this->castle_bqueen_ok = false;
    for(int i=0;i<fen_parts.at(2).size();i++) {
        QChar ci = fen_parts.at(2).at(i);
        if(ci == QChar::fromLatin1('K')) {
            this->castle_wking_ok = true;
        }
        if(ci == QChar::fromLatin1('Q')) {
            this->castle_wqueen_ok = true;
        }
        if(ci == QChar::fromLatin1('k')) {
            this->castle_bking_ok = true;
        }
        if(ci == QChar::fromLatin1('q')) {
            this->castle_bqueen_ok = true;
        }
    }
    // set en passent square
    if(fen_parts.at(3) == QString::fromLatin1("-")) {
        this->en_passent_target = 0;
    } else {
        int row = 10 + (fen_parts.at(3).at(1).digitValue() * 10);
        int col = 0;
        QChar c = fen_parts.at(3).at(0).toLower();
        if(c == 'a') {
            col = 1;
        }
        if(c == 'b') {
            col = 2;
        }
        if(c == 'c') {
            col = 3;
        }
        if(c == 'd') {
            col = 4;
        }
        if(c == 'e') {
            col = 5;
        }
        if(c == 'f') {
            col = 6;
        }
        if(c == 'g') {
            col = 7;
        }
        if(c == 'h') {
            col = 8;
        }
        this->en_passent_target = row + col;
    }
    if(fen_parts.size() >= 5) {
        this->halfmove_clock = fen_parts.at(4).toInt();
    } else {
        this->halfmove_clock = 0;
    }
    if(fen_parts.size() >= 6) {
        this->fullmove_number = fen_parts.at(5).toInt();
    } else {
        this->fullmove_number = 1;
    }
    this->undo_available = false;
    this->last_was_null = false;
    this->init_piece_list();
    if(!this->is_consistent()) {
        throw std::invalid_argument("board position from supplied fen is inconsistent");
    }

    this->zobrist_initialized = false;
    this->pos_hash_initialized = false;
}

QString Board::idx_to_str(int idx) const {
    if(idx<21 || idx>98) {
        throw std::invalid_argument("called idx_to_str but idx is in fringe!");
    } else {
        QChar row = QChar ((idx / 10) + 47);
        QChar col = QChar ((idx % 10) + 96);
        QString str = "";
        str.append(col);
        str.append(row);
        return str;
    }
}

QString Board::fen() const {
    // first build board
    QString fen_string = QString("");
    for(int i=90;i>=20;i-=10) {
        int square_counter = 0;
        for(int j=1;j<9;j++) {
            if(this->board[i+j] != 0x00) {
                uint8_t piece = this->board[i+j];
                fen_string.append(this->piece_to_symbol(piece));
                square_counter = 0;
            } else {
                square_counter += 1;
                if(j==8) {
                    fen_string.append(QChar((char) (48+square_counter)));
                } else {
                    if(this->board[i+j+1] != 0x00) {
                        fen_string.append(QChar((char) (48+square_counter)));
                    }
                }
            }
        }
        if(i!=20) {
            fen_string.append(QChar('/'));
        }
    }
    // write turn
    if(this->turn == WHITE) {
        fen_string.append(" w");
    } else {
        fen_string.append(" b");
    }
    // write castling rights
    if(!this->castle_wking_ok &&
            !this->castle_wqueen_ok &&
            !this->castle_bking_ok &&
            !this->castle_bqueen_ok) {
        fen_string.append(" -");
    } else {
        fen_string.append(' ');
        if(this->castle_wking_ok) {
            fen_string.append(('K'));
        }
        if(this->castle_wqueen_ok) {
            fen_string.append(('Q'));
        }
        if(this->castle_bking_ok) {
            fen_string.append(('k'));
        }
        if(this->castle_bqueen_ok) {
            fen_string.append(('q'));
        }
    }
    // write ep target if exists
    if(this->en_passent_target != 0x00) {
        fen_string.append(" "+this->idx_to_str(this->en_passent_target));
    } else {
        fen_string.append(" -");
    }
    // add halfmove clock and fullmove counter
    fen_string.append(" ").append(QString::number(this->halfmove_clock));
    fen_string.append(" ").append(QString::number(this->fullmove_number));
    return fen_string;
}


bool Board::can_castle_wking() const {
    return castle_wking_ok;
}

bool Board::can_castle_bking() const {
    return castle_bking_ok;
}

bool Board::can_castle_wqueen() const {
    return castle_wqueen_ok;
}

bool Board::can_castle_bqueen() const {
    return castle_bqueen_ok;
}

void Board::set_castle_wking(bool can_do) {
    this->castle_wking_ok = can_do;
}

void Board::set_castle_bking(bool can_do) {
    this->castle_bking_ok = can_do;
}

void Board::set_castle_wqueen(bool can_do) {
    this->castle_wqueen_ok = can_do;
}

void Board::set_castle_bqueen(bool can_do) {
    this->castle_bqueen_ok = can_do;
}

QVector<Move> Board::pseudo_legal_moves() {
    //return this->pseudo_legal_moves_from(0,true,this->turn);
    return this->pseudo_legal_moves(chess::ANY_SQUARE, chess::ANY_SQUARE, chess::ANY_PIECE, true, this->turn);
}

QVector<Move> Board::pseudo_legal_moves(int to_square, int piece_type) {
    if(piece_type == KING) {
        //return this->pseudo_legal_moves_to(to_square, piece_type, true,this->turn);
        return this->pseudo_legal_moves(chess::ANY_SQUARE, to_square, piece_type, true, this->turn);
    } else {
        return this->pseudo_legal_moves(chess::ANY_SQUARE, to_square, piece_type, false, this->turn);
    }
}

bool Board::is_castles_wking(const Move &m) const {
    if(this->get_piece_type(m.from) == KING && this->get_piece_color(m.from) == WHITE &&
            m.from == E1 && m.to == G1) {
        return true;
    } else {
        return false;
    }
}


bool Board::is_castles_wqueen(const Move &m) const {
    if(this->get_piece_type(m.from) == KING && this->get_piece_color(m.from) == WHITE &&
            m.from == E1 && m.to == C1) {
        return true;
    } else {
        return false;
    }
}


bool Board::is_castles_bking(const Move &m) const {
    if(this->get_piece_type(m.from) == KING && this->get_piece_color(m.from) == BLACK &&
            m.from == E8 && m.to == G8) {
        return true;
    } else {
        return false;
    }
}

bool Board::is_castles_bqueen(const Move &m) const {
    if(this->get_piece_type(m.from) == KING && this->get_piece_color(m.from) == BLACK &&
            m.from == E8 && m.to == C8) {
        return true;
    } else {
        return false;
    }
}

// to get legal moves, just get list of pseudo
// legals and then filter by checking each move's
// legality
QVector<Move> Board::legal_moves() {

    QVector<Move> pseudo_legals = this->pseudo_legal_moves();
    QVector<Move> legals;
    for(int i=0;i<pseudo_legals.size();i++) {
        Move m = pseudo_legals.at(i);
        if(this->pseudo_is_legal_move(m)) {
            legals.append(m);
        }
    }
    return legals;
}

// to speed up san parsing, check here
// only moves where destination is hit.
QVector<Move> Board::legal_moves(int to_square, int piece_type) {

    QVector<Move> pseudo_legals = this->pseudo_legal_moves(to_square, piece_type);
    QVector<Move> legals;
    for(int i=0;i<pseudo_legals.size();i++) {
        Move m = pseudo_legals.at(i);
        if(m.to == to_square) {
            if(this->pseudo_is_legal_move(m)) {
                legals.append(m);
            }
        }
    }
    return legals;
}

QVector<Move> Board::legal_moves_from(int from_square) {

    QVector<Move> pseudo_legals = this->pseudo_legal_moves(from_square, chess::ANY_SQUARE, chess::ANY_PIECE, true,this->turn);

    QVector<Move> legals;
    for(int i=0;i<pseudo_legals.size();i++) {
        Move m = pseudo_legals.at(i);
        if(this->pseudo_is_legal_move(m)) {
            legals.append(m);
        }
    }
    return legals;
}

bool Board::is_legal_and_promotes(const Move &m) {
    QVector<Move> legals = this->legal_moves_from(m.from);
    for(int i=0;i<legals.size();i++) {
        Move mi = legals.at(i);
        if(mi.from == m.from && mi.to == m.to && mi.promotion_piece != 0) {
            return true;
        }
    }
    return false;
}

bool Board::is_legal_move(const Move &m) {
    QVector<Move> pseudo_legals = this->pseudo_legal_moves(m.from, m.to, this->get_piece_type(m.from), true, this->turn);
    for(int i=0;i<pseudo_legals.size();i++) {
        Move mi = pseudo_legals.at(i);
        if(mi == m && this->pseudo_is_legal_move(m)) {
            return true;
        }
    }
    return false;
}

QVector<Move> Board::legals_from_pseudos(QVector<Move> &pseudos) {
    QVector<Move> legals;
    for(int i=0;i<pseudos.size();i++) {
        Move mi = pseudos.at(i);
        if(pseudo_is_legal_move(mi)) {
            legals.append(mi);
        }
    }
    return legals;
}

bool Board::pseudo_is_legal_move(const Move &m) {

    // a pseudo legal move is a legal move if
    // a) doesn't put king in check
    // b) if castle, must ensure that 1) king is not currently in check
    //                                2) castle over squares are not in check
    //                                3) doesn't castle into check
    // first find color of mover
    bool color = this->get_piece_color(m.from);
    // find king with that color
    for(int i= 21;i<99;i++) {
        if(this->get_piece_type(i) == KING && this->get_piece_color(i) == color) {
            // if the move is not by the king
            if(i!=m.from) {
                // apply the move, check if king is attacked, and decide
                bool legal = false;
                this->apply(m);
                legal = !this->is_attacked(i,!color);
                this->undo();
                return legal;
            } else {
                // means we move the king
                // first check castle cases
                if(this->is_castles_wking(m)) {
                    if(!this->is_attacked(E1,BLACK) && !this->is_attacked(F1,BLACK)
                            && !this->is_attacked(G1,BLACK)) {
                        bool legal = false;
                        this->apply(m);
                        legal = !this->is_attacked(G1,BLACK);
                        this->undo();
                        return legal;
                    } else {
                        return false;
                    }
                }
                if(this->is_castles_bking(m)) {
                    if(!this->is_attacked(E8,WHITE) && !this->is_attacked(F8,WHITE)
                            && !this->is_attacked(G8,WHITE)) {
                        bool legal = false;
                        this->apply(m);
                        legal = !this->is_attacked(G8,WHITE);
                        this->undo();
                        return legal;
                    } else {
                        return false;
                    }
                }
                if(this->is_castles_wqueen(m)) {
                    if(!this->is_attacked(E1,BLACK) && !this->is_attacked(D1,BLACK)
                            && !this->is_attacked(C1,BLACK) ) {
                        bool legal = false;
                        this->apply(m);
                        legal = !this->is_attacked(C1,BLACK);
                        this->undo();
                        return legal;
                    } else {
                        return false;
                    }
                }
                if(this->is_castles_bqueen(m)) {
                    if(!this->is_attacked(E8,WHITE) && !this->is_attacked(D8,WHITE)
                            && !this->is_attacked(C8,WHITE) ) {
                        bool legal = false;
                        this->apply(m);
                        legal = !this->is_attacked(C8,WHITE);
                        this->undo();
                        return legal;
                    } else {
                        return false;
                    }
                }
                // if none of the castles cases triggered, we have a standard king move
                // just check if king isn't attacked after applying the move
                bool legal = false;
                this->apply(m);
                legal = !this->is_attacked(m.to,!color);
                this->undo();
                return legal;
            }
        }
    }
    return false;
}

// doesn't account for attacks via en-passent
bool Board::is_attacked(int idx, bool attacker_color) {
    // first check for potential pawn attackers
    // attacker color white, pawn must be white.
    // lower left
    if(attacker_color == WHITE && (this->board[idx-9]!=0xFF)
            && (this->get_piece_color(idx-9)==WHITE) && (this->get_piece_type(idx-9)==PAWN)) {
        return true;
    }
    // lower right
    if(attacker_color == WHITE && (this->board[idx-11]!=0xFF)
            && (this->get_piece_color(idx-11)==WHITE) && (this->get_piece_type(idx-11)==PAWN)) {
        return true;
    }
    // check black, upper right
    if(attacker_color == BLACK && (this->board[idx+11]!=0xFF)
            && (this->get_piece_color(idx+11)==BLACK) && (this->get_piece_type(idx+11)==PAWN)) {
        return true;
    }
    // check black, upper left
    if(attacker_color == BLACK && (this->board[idx+9]!=0xFF)
            && (this->get_piece_color(idx+9)==BLACK) && (this->get_piece_type(idx+9)==PAWN)) {
        return true;
    }
    // check all squares (except idx itself)
    // for potential attackers
    for(int i=21;i<99;i++) {
        // skip empty squares
        if(i!=idx && this->board[i] != 0x00) {  // ??? fringe??
            // can't attack yourself
            if(this->get_piece_color(i) == attacker_color) {
                uint8_t piece = this->get_piece_type(i);
                int distance = idx - i;
                if(distance < 0) {
                    distance = -distance;
                }
                IntBits pot_attackers = IntBits(ATTACK_TABLE[distance]);
                if((piece == KNIGHT && pot_attackers.test(0)) ||
                        (piece == BISHOP && pot_attackers.test(1)) ||
                        (piece == ROOK && pot_attackers.test(2)) ||
                        (piece == QUEEN && pot_attackers.test(3)) ||
                        (piece == KING && pot_attackers.test(4))) {
                    // the target could be a potential attacker
                    // now just get all pseudo legal moves from i,
                    // excluding castling. If a move contains
                    // target idx, then we have an attacker
                    //QVector<Move> targets = this->pseudo_legal_moves_from(i,false,attacker_color);
                    QVector<Move> targets =  this->pseudo_legal_moves(i, chess::ANY_SQUARE, chess::ANY_PIECE, false, attacker_color);
                    for(int j=0;j<targets.size();j++) {
                        //qDebug() << "is attacked psl: " << targets.at(j).uci();
                        if(targets.at(j).to == idx) {
                            return true;
                        }
                    }
                }
            }
        }
    }
    return false;
}


QVector<Move> Board::pseudo_legal_moves(int from_square, int to_square,
                                        int piece_type, bool generate_castles, bool turn)
{

    //qDebug() << "psl";
    QVector<Move> moves;
    // pawn moves
    if(piece_type == ANY_PIECE || piece_type == PAWN) {
        for(int i=0;i<10;i++) {
            int from = this->piece_list[turn][PAWN][i];
            if(from == EMPTY) {
                break;
            }
            // skip if we generate only moves from a certain square
            if(from_square != ANY_SQUARE && from_square != from) {
                continue;
            }
            int piece_idx = IDX_WPAWN;
            if(turn == BLACK) {
                piece_idx = IDX_BPAWN;
            }
            // take up right, or up left
            for(int j=3;j<=4;j++) {
                int idx = from + DIR_TABLE[piece_idx][j];
                if((to_square == ANY_SQUARE || idx == to_square) && !this->is_offside(idx)) {
                    if((!this->is_empty(idx) && turn==BLACK && this->is_white_at(idx)) ||
                            (!this->is_empty(idx) && turn==WHITE && !this->is_white_at(idx))) {
                        // if it's a promotion square, add four moves
                        if((turn==WHITE && (idx / 10 == 9)) || (turn==BLACK && (idx / 10 == 2))) {
                            //qDebug() << "pawn take, promotion: " << from << " "  << idx;
                            assert(this->board[from] != 0xff);
                            moves.append(Move(from,idx,QUEEN));
                            moves.append(Move(from,idx,ROOK));
                            moves.append(Move(from,idx,BISHOP));
                            moves.append(Move(from,idx,KNIGHT));
                        } else {
                            assert(this->board[from] != 0xff);
                            /*
                            qDebug() << "pawn take: " << from << " "  << idx;
                            qDebug() << "is white at: " << +(this->is_white_at(idx));
                            qDebug() << "content at: " << this->board[idx];
*/
                            moves.append(Move(from,idx));
                        }
                    }
                }
            }
            // move one (j=1) or two (j=2) up (or down in the case of black)
            int idx_1up = from + DIR_TABLE[piece_idx][1];
            int idx_2up = from + DIR_TABLE[piece_idx][2];
            if((to_square == ANY_SQUARE || idx_2up == to_square) && !this->is_offside(idx_2up)) {
                if((turn == WHITE && (from/10==3)) || (turn==BLACK && (from/10==8))) {
                    // means we have a white/black pawn in inital position, direct square
                    // in front is empty => allow to move two forward
                    if(this->is_empty(idx_1up) && this->is_empty(idx_2up)) {
                        assert(this->board[from] != 0xff);
                        //qDebug() << "pawn move, 2up: " << from << " "  << idx_2up;
                        moves.append(Move(from,idx_2up));
                    }
                }
            }
            if((to_square == ANY_SQUARE || idx_1up == to_square) && !this->is_offside(idx_1up) && this->is_empty(idx_1up)) {
                // if it's a promotion square, add four moves
                if((turn==WHITE && (idx_1up / 10 == 9)) || (turn==BLACK && (idx_1up / 10 == 2))) {
                    assert(this->board[from] != 0xff);
                    //qDebug() << "pawn move, 1up, promotion: " << from << " "  << idx_1up;
                    moves.append(Move(from,idx_1up,QUEEN));
                    moves.append(Move(from,idx_1up,ROOK));
                    moves.append(Move(from,idx_1up,BISHOP));
                    moves.append(Move(from,idx_1up,KNIGHT));
                } else {
                    assert(this->board[from] != 0xff);
                    //qDebug() << "pawn move, 1up: " << from << " "  << idx_2up;
                    moves.append(Move(from,idx_1up));
                }
            }
            // finally, potential en-passent capture is handled
            // left up
            if(to_square == ANY_SQUARE || to_square == this->en_passent_target) {
                if(turn == WHITE && (this->en_passent_target - from)==9) {
                    assert(this->board[from] != 0xff);
                    Move m = (Move(from,this->en_passent_target));
                    moves.append(m);
                }
                // right up
                if(turn == WHITE && (this->en_passent_target - from)==11) {
                    assert(this->board[from] != 0xff);
                    Move m = (Move(from,this->en_passent_target));
                    moves.append(m);
                }
                // left down
                if(turn == BLACK && (this->en_passent_target - from)==-9) {
                    assert(this->board[from] != 0xff);
                    Move m = (Move(from,this->en_passent_target));
                    moves.append(m);
                }
                if(turn == BLACK && (this->en_passent_target - from)==-11) {
                    assert(this->board[from] != 0xff);
                    Move m = (Move(from,this->en_passent_target));
                    moves.append(m);
                }
            }
        }
    }
    if(piece_type == ANY_PIECE || piece_type == KNIGHT) {
        for(int i=0;i<10;i++) {
            int from = this->piece_list[turn][KNIGHT][i];
            //qDebug() << "knight from: " << from;
            if(from == EMPTY) {
                break;
            }
            assert(this->board[from] != 0xff);
            int lookup_idx = IDX_KNIGHT;
            for(int j=1;j<=DIR_TABLE[lookup_idx][0];j++) {
                int idx = from + DIR_TABLE[lookup_idx][j];
                if((to_square == ANY_SQUARE || idx == to_square) && !this->is_offside(idx)) {
                    if(this->is_empty(idx) ||
                            (this->get_piece_color(idx) != turn)) {
                        moves.append(Move(from,idx));
                    }
                }
            }
        }
    }
    if(piece_type == ANY_PIECE || piece_type == KING) {
        for(int i=0;i<10;i++) {
            int from = this->piece_list[turn][KING][i];
            if(from == EMPTY) {
                break;
            }
            assert(this->board[from] != 0xff);
            int lookup_idx = IDX_KING;
            for(int j=1;j<=DIR_TABLE[lookup_idx][0];j++) {
                int idx = from + DIR_TABLE[lookup_idx][j];
                if((to_square == ANY_SQUARE || idx == to_square) && !this->is_offside(idx)) {
                    if(this->is_empty(idx) ||
                            (this->get_piece_color(idx) != turn)) {
                        moves.append(Move(from,idx));
                    }
                }
            }
        }
    }
    if(piece_type == ANY_PIECE || piece_type == ROOK) {
        for(int i=0;i<10;i++) {
            int from = this->piece_list[turn][ROOK][i];
            if(from == EMPTY) {
                break;
            }
            assert(this->board[from] != 0xff);
            int lookup_idx = IDX_ROOK;
            for(int j=1;j<=DIR_TABLE[lookup_idx][0] ;j++) {
                int idx = from + DIR_TABLE[lookup_idx][j];
                bool stop = false;
                while(!stop) {
                    if(!this->is_offside(idx)) {
                        if(this->is_empty(idx)) {
                            if(to_square == ANY_SQUARE || to_square == idx) {
                                moves.append(Move(from,idx));
                            }
                        } else {
                            stop = true;
                            if(this->get_piece_color(idx) != turn) {
                                if(to_square == ANY_SQUARE || to_square == idx) {
                                    moves.append(Move(from,idx));
                                }
                            }
                        }
                        idx = idx + DIR_TABLE[lookup_idx][j];
                    } else {
                        stop = true;
                    }
                }
            }
        }
    }
    if(piece_type == ANY_PIECE || piece_type == BISHOP) {
        for(int i=0;i<10;i++) {
            int from = this->piece_list[turn][BISHOP][i];
            if(from == EMPTY) {
                break;
            }
            assert(this->board[from] != 0xff);
            int lookup_idx = IDX_BISHOP;
            for(int j=1;j<=DIR_TABLE[lookup_idx][0] ;j++) {
                int idx = from + DIR_TABLE[lookup_idx][j];
                bool stop = false;
                while(!stop) {
                    if(!this->is_offside(idx)) {
                        if(this->is_empty(idx)) {
                            if(to_square == ANY_SQUARE || to_square == idx) {
                                moves.append(Move(from,idx));
                            }
                        } else {
                            stop = true;
                            if(this->get_piece_color(idx) != turn) {
                                if(to_square == ANY_SQUARE || to_square == idx) {
                                    moves.append(Move(from,idx));
                                }
                            }
                        }
                        idx = idx + DIR_TABLE[lookup_idx][j];
                    } else {
                        stop = true;
                    }
                }
            }
        }
    }
    if(piece_type == ANY_PIECE || piece_type == QUEEN) {
        for(int i=0;i<10;i++) {
            int from = this->piece_list[turn][QUEEN][i];
            if(from == EMPTY) {
                break;
            }
            assert(this->board[from] != 0xff);
            int lookup_idx = IDX_QUEEN;
            for(int j=1;j<=DIR_TABLE[lookup_idx][0] ;j++) {
                int idx = from + DIR_TABLE[lookup_idx][j];
                bool stop = false;
                while(!stop) {
                    if(!this->is_offside(idx)) {
                        if(this->is_empty(idx)) {
                            if(to_square == ANY_SQUARE || to_square == idx) {
                                moves.append(Move(from,idx));
                            }
                        } else {
                            stop = true;
                            if(this->get_piece_color(idx) != turn) {
                                if(to_square == ANY_SQUARE || to_square == idx) {
                                    moves.append(Move(from,idx));
                                }
                            }
                        }
                        idx = idx + DIR_TABLE[lookup_idx][j];
                    } else {
                        stop = true;
                    }
                }
            }
        }
    }
    if(generate_castles) {
        if(turn == WHITE) {
            // check for castling
            // white kingside
            if(!this->is_empty(E1) && this->can_castle_wking() &&
                    this->get_piece_color(E1) == WHITE && this->get_piece_color(H1) == WHITE
                    && this->get_piece_type(E1) == KING && this->get_piece_type(H1) == ROOK
                    && this->is_empty(F1) && this->is_empty(G1)) {
                moves.append(Move(E1,G1));
            }
            // white queenside
            if(!this->is_empty(E1) && this->can_castle_wqueen() &&
                    this->get_piece_color(E1) == WHITE && this->get_piece_color(A1) == WHITE
                    && this->get_piece_type(E1) == KING && this->get_piece_type(A1) == ROOK
                    && this->is_empty(D1) && this->is_empty(C1) && this->is_empty(B1)) {
                moves.append(Move(E1,C1));
            }
        }
        if(turn == BLACK) {
            // black kingside
            if(!this->is_empty(E8) && this->can_castle_bking() &&
                    this->get_piece_color(E8) == BLACK && this->get_piece_color(H8) == BLACK
                    && this->get_piece_type(E8) == KING && this->get_piece_type(H8) == ROOK
                    && this->is_empty(F8) && this->is_empty(G8)) {
                moves.append(Move(E8,G8));
            }
            // black queenside
            if(!this->is_empty(E8) && this->can_castle_bqueen() &&
                    this->get_piece_color(E8) == BLACK && this->get_piece_color(A8) == BLACK
                    && this->get_piece_type(E8) == KING && this->get_piece_type(A8) == ROOK
                    && this->is_empty(D8) && this->is_empty(C8) && this->is_empty(B8)) {
                moves.append(Move(E8,C8));
            }
        }
    }
    //qDebug() << "psl fin";
    //for(const Move mi: moves) {
    //    qDebug() << "psl: " << mi.uci();
    //}
    return moves;

}


bool Board::move_promotes(const Move&m) const {
    if(this->get_piece_type(m.from) == chess::PAWN) {
        if(this->get_piece_color(m.from) == chess::WHITE && ((m.to / 10)==9)) {
            return true;
        }
        if(this->get_piece_color(m.from) == chess::BLACK && ((m.to / 10)==2)) {
            return true;
        }
    }
    return false;
}


bool Board::get_piece_color(int idx) const {
    if(this->board[idx] > 0x80) {
        return BLACK;
    } else {
        return WHITE;
    }
}

int Board::get_piece_type(int idx) const {
    int piece = this->board[idx];
    /*
    if(piece == EMPTY) {
        throw std::invalid_argument("get piece type: empty!");
    }
    if(piece == FRINGE) {
        throw std::invalid_argument("get piece type: fringe!");
    }*/
    if(piece > 0x80) {
        return piece - 0x80;
    } else {
        return piece;
    }
}

int Board::get_piece_at(int idx) const {
    if(idx >= 21 && idx <= 98) {
        return this->board[idx];
    } else {
        throw std::invalid_argument("called get_piece_at with invalid paramters");
    }
}

// returns true if square is not empty
bool Board::is_empty(int idx) const {
    if(this->board[idx] == EMPTY) {
        return true;
    } else {
        return false;
    }
}


// returns true if square is in fringe
bool Board::is_offside(int idx) const {
    if(this->board[idx] == FRINGE) {
        return true;
    } else {
        return false;
    }
}

bool Board::can_claim_fifty_moves() const {
    return this->halfmove_clock >= 100;
}



// returns true (== Black) if not occupied!
bool Board::is_white_at(int idx) const {
    if(this->board[idx] > 0x80) {
        return false;
    } else {
        return true;
    }
}

void Board::remove_from_piece_list(bool color, int piece_type, int idx) {

    assert(piece_type >= 0 && piece_type < 7);
    assert(idx >= 21 && idx <= 98);
    int j = -1;
    for(int i=0;i<10;i++) {
        if(this->piece_list[color][piece_type][i] == idx) {
            j = i;
            break;
        }
    }
    if(j>=0) {
        // move all other one step further
        for(int i=j+1;i<10;i++) {
            this->piece_list[color][piece_type][i-1] = this->piece_list[color][piece_type][i];
        }
        // empty last one in list
        this->piece_list[color][piece_type][9] = EMPTY;
    }
}

void Board::add_to_piece_list(bool color, int piece_type, int idx) {

    assert(piece_type >= 0 && piece_type < 7);
    assert(idx >= 21 && idx <= 98);
    for(int i=0;i<10;i++) {
        if(this->piece_list[color][piece_type][i] == EMPTY) {
            this->piece_list[color][piece_type][i] = idx;
            break;
        }
    }
}

// doesn't check legality
void Board::apply(const Move &m) {

    //qDebug() << "apply";
    if(m.is_null) {
        this->turn = !this->turn;
        this->prev_en_passent_target = this->en_passent_target;
        this->en_passent_target = 0;
        this->last_was_null = true;
        this->undo_available = true;
        if(this->turn == WHITE) {
            this->fullmove_number++;
        }
    } else {
        //qDebug() << "apply else";
        this->last_was_null = false;
    this->turn = !this->turn;
    this->prev_en_passent_target = this->en_passent_target;
    this->prev_castle_wking_ok = this->castle_wking_ok;
    this->prev_castle_wqueen_ok = this->castle_wqueen_ok;
    this->prev_castle_bking_ok = this->castle_bking_ok;
    this->prev_castle_bqueen_ok = this->castle_bqueen_ok;
    this->en_passent_target = 0;
    if(this->turn == WHITE) {
        this->fullmove_number++;
    }
    for(int i=0;i<120;i++) {
        this->old_board[i] = this->board[i];
    }

    int old_piece_type = this->get_piece_type(m.from);
    bool color = this->get_piece_color(m.from);
    // if target field is not empty, remove from piece list
    // this must be of oppsite color than the currently moving piece
    if(this->board[m.to] != EMPTY) {
        int current_target_piece = this->get_piece_type(m.to);
        this->remove_from_piece_list(!color, current_target_piece, m.to);
    }
    // also remove the currently moving piece from the list
    this->remove_from_piece_list(color, old_piece_type, m.from);
    // increase halfmove clock only if no capture or pawn advance
    // happend
    this->prev_halfmove_clock = this->halfmove_clock;
    if(old_piece_type == PAWN || this->board[m.to] != EMPTY) {
        this->halfmove_clock = 0;
    } else {
        this->halfmove_clock++;
    }
    //qDebug() << "apply 2 ";
    // if we move a pawn two steps up, set the en_passent field
    if(old_piece_type == PAWN) {
        // white pawn moved two steps up
        if((m.to - m.from) == 20) {
            this->en_passent_target = m.from + 10;
        }
        // black pawn moved two steps up (down)
        if((m.to - m.from == -20)) {
            this->en_passent_target = m.from - 10;
        }
    }
    // if the move is an en-passent capture,
    // remove the (non-target) corresponding pawn
    // move is an en passent move, if
    // a) color is white, piece type is pawn, target
    // is up left or upright and empty
    // b) color is black, piece type is pawn, target
    // is down right or down left and empty
    // also set last_move_was_ep to true
    if(old_piece_type == PAWN) {
        if(this->board[m.to] == EMPTY) {
            if(color == WHITE && ((m.to-m.from == 9) || (m.to-m.from)==11)) {
                // remove captured pawn
                this->board[m.to-10] = EMPTY;
                this->remove_from_piece_list(BLACK, PAWN, m.to-10);
            }
            if(color == BLACK && ((m.from -m.to == 9) || (m.from - m.to)==11)) {
                // remove captured pawn
                this->board[m.to+10] = EMPTY;
                this->remove_from_piece_list(WHITE, PAWN, m.to+10);
            }
        }
    }
    // if the move is a promotion, the target
    // field becomes the promotion choice
    if(m.promotion_piece != EMPTY) {
        // true means black
        if(color == BLACK) {
            // +128 sets 7th bit to true (means black)            
            this->board[m.to] = m.promotion_piece +128;
            // add to piece list
            this->add_to_piece_list(BLACK, m.promotion_piece, m.to);
        }
        else {
            this->board[m.to] = m.promotion_piece;
            this->add_to_piece_list(WHITE, m.promotion_piece, m.to);
        }
    } else {
        // otherwise the target is the piece on the from field
        this->board[m.to] = this->board[m.from];
        // add to piece list
        this->add_to_piece_list(color, old_piece_type, m.to);
    }
    this->board[m.from] = EMPTY;
    // check if the move is castles, i.e. 0-0 or 0-0-0
    // then we also need to move the rook
    // white kingside
    if(old_piece_type == KING) {
        if(color==WHITE) {
            if(m.from == E1 && m.to == G1) {
                this->board[F1] = this->board[H1];
                this->board[H1] = EMPTY;
                this->set_castle_wking(false);
                this->remove_from_piece_list(WHITE, ROOK, H1);
                this->add_to_piece_list(WHITE, ROOK, F1);
            }
            // white queenside
            if(m.from == E1 && m.to == C1) {
                this->board[D1] = this->board[A1];
                this->board[A1] = EMPTY;
                this->set_castle_wqueen(false);
                this->remove_from_piece_list(WHITE, ROOK, A1);
                this->add_to_piece_list(WHITE, ROOK, D1);
            } }
        else if(color==BLACK) {
            // black kingside
            if(m.from == E8 && m.to == G8) {
                this->board[F8] = this->board[H8];
                this->board[H8] = EMPTY;
                this->set_castle_bking(false);
                this->remove_from_piece_list(BLACK, ROOK, H8);
                this->add_to_piece_list(BLACK, ROOK, F8);
            }
            // black queenside
            if(m.from == E8 && m.to == C8) {
                this->board[D8] = this->board[A8];
                this->board[A8] = EMPTY;
                this->set_castle_bqueen(false);
                this->remove_from_piece_list(BLACK, ROOK, A8);
                this->add_to_piece_list(BLACK, ROOK, D8);
            }
        }
    }
    // check if someone loses castling rights
    // by moving king or by moving rook
    // or if one of the rooks is captured by the
    // opposite side
    if(color == WHITE) {
        if(old_piece_type == KING) {
            if(m.from == E1 && m.to !=G1) {
                this->set_castle_wking(false);
            }
            if(m.from == E1 && m.to != C1) {
                this->set_castle_wqueen(false);
            }
        }
        if(old_piece_type == ROOK) {
            if(m.from == A1) {
                this->set_castle_wqueen(false);
            }
            if(m.from == H1) {
                this->set_castle_wking(false);
            }
        }
        // white moves a piece to H8 or A8
        // means either white captures rook
        // or black has moved rook prev.
        // [even though: in the latter case, should be already
        // done by check above in prev. moves]
        if(m.to == H8) {
            this->set_castle_bking(false);
        }
        if(m.to == A8) {
            this->set_castle_bqueen(false);
        }
    }
    // same for black
    if(color == BLACK) {
        if(old_piece_type == KING) {
            if(m.from == E8 && m.to !=G8) {
                this->set_castle_bking(false);
            }
            if(m.from == E8 && m.to != C8) {
                this->set_castle_bqueen(false);
            }
        }
        if(old_piece_type == ROOK) {
            if(m.from == A8) {
                this->set_castle_bqueen(false);
            }
            if(m.from == H8) {
                this->set_castle_bking(false);
            }
        }
        // black moves piece to A1 or H1
        if(m.to == H1) {
            this->set_castle_wking(false);
        }
        if(m.to == A1) {
            this->set_castle_wqueen(false);
        }
    }
    // after move is applied, can revert to the previous position
    this->undo_available = true;
    }
    //qDebug() << "apply fin";
}

void Board::undo() {
    //qDebug() << "undo";
    if(!this->undo_available) {
        throw std::logic_error("must call board.apply(move) each time before calling undo() ");
    } else {
        if(this->last_was_null) {
            this->turn = !this->turn;
            this->en_passent_target = this->prev_en_passent_target;
            this->prev_en_passent_target = 0;
            this->last_was_null = false;
            this->undo_available = true;
        } else {
            for(int i=0;i<120;i++) {
                this->board[i] = this->old_board[i];
            }
            this->undo_available = false;
            this->en_passent_target = this->prev_en_passent_target;
            this->prev_en_passent_target = 0;
            this->castle_wking_ok = this->prev_castle_wking_ok;
            this->castle_wqueen_ok = this->prev_castle_wqueen_ok;
            this->castle_bking_ok = this->prev_castle_bking_ok;
            this->castle_bqueen_ok = this->prev_castle_bqueen_ok;
            this->turn = !this->turn;
            this->halfmove_clock = this->prev_halfmove_clock;
            this->prev_halfmove_clock = 0;
            if(this->turn == BLACK) {
                this->fullmove_number--;
            }
        }
    }
    this->init_piece_list();
    //qDebug() << "undo fin";
}


Board::Board(const Board &other) {
    turn = other.turn;
    this->castle_wking_ok = other.castle_wking_ok;
    this->castle_wqueen_ok = other.castle_wqueen_ok;
    this->castle_bking_ok = other.castle_bking_ok;
    this->castle_bqueen_ok = other.castle_bqueen_ok;
    this->prev_castle_wking_ok = other.prev_castle_wking_ok;
    this->prev_castle_wqueen_ok = other.prev_castle_wqueen_ok;
    this->prev_castle_bking_ok = other.prev_castle_bking_ok;
    this->prev_castle_bqueen_ok = other.prev_castle_bqueen_ok;
    this->en_passent_target = other.en_passent_target;
    this->halfmove_clock = other.halfmove_clock;
    this->fullmove_number = other.fullmove_number;
    this->undo_available = other.undo_available;
    this->last_was_null = other.last_was_null;
    this->prev_halfmove_clock = other.prev_halfmove_clock;
    this->zobrist_initialized = other.zobrist_initialized;
    this->pos_hash_initialized = other.pos_hash_initialized;
    this->zobrist = other.zobrist;
    this->pos_hash = other.pos_hash;
    for(int i=0;i<120;i++) {
        this->board[i] = other.board[i];
        this->old_board[i] = other.old_board[i];
    }
    for(int i=0;i<7;i++) {
        for(int j=0;j<10;j++) {
            this->piece_list[WHITE][i][j] = other.piece_list[WHITE][i][j];
            this->piece_list[BLACK][i][j] = other.piece_list[BLACK][i][j];
        }
    }
}

bool Board::is_stalemate() {
    // search for king of player with current turn
    // check whether king is attacked
    for(int i=21;i<99;i++) {
        if((this->get_piece_type(i)==KING) && (this->get_piece_color(i)==this->turn)){
            if(!this->is_attacked(i,!this->turn)) {
                QVector<Move> legals = this->legal_moves();
                int c = legals.count();
                if(c==0) {
                    return true;
                } else {
                    return false;
                }
            } else {
                return false;
            }
        }
    }
    return false;
}


bool Board::is_checkmate() {
    // search for king of player with current turn
    // check whether king is attacked
    for(int i=21;i<99;i++) {
        if((this->get_piece_type(i)==KING) && (this->get_piece_color(i)==this->turn)){
            if(this->is_attacked(i,!this->turn)) {
                QVector<Move> legals = this->legal_moves();
                int c = legals.count();
                if(c==0) {
                    return true;
                } else {
                    return false;
                }
            } else {
                return false;
            }
        }
    }
    return false;
}

bool Board::is_check() {
    for(int i=21;i<99;i++) {
        if((this->get_piece_type(i)==KING) && (this->get_piece_color(i)==this->turn)){
            if(this->is_attacked(i,!this->turn)) {
                return true;
            } else {
                return false;
            }
        }
    }
    return false;
}


int Board::alpha_to_pos(QChar alpha) {
    if(alpha == QChar::fromLatin1('A') || alpha == QChar::fromLatin1('a')) {
        return 0;
    } else if(alpha == QChar::fromLatin1('B') || alpha == QChar::fromLatin1('b')) {
        return 1;
    } else if(alpha == QChar::fromLatin1('C') || alpha == QChar::fromLatin1('c')) {
        return 2;
    } else if(alpha == QChar::fromLatin1('D') || alpha == QChar::fromLatin1('d')) {
        return 3;
    } else if(alpha == QChar::fromLatin1('E') || alpha == QChar::fromLatin1('e')) {
        return 4;
    } else if(alpha == QChar::fromLatin1('F') || alpha == QChar::fromLatin1('f')) {
        return 5;
    } else if(alpha == QChar::fromLatin1('G') || alpha == QChar::fromLatin1('g')) {
        return 6;
    } else if(alpha == QChar::fromLatin1('H') || alpha == QChar::fromLatin1('h')) {
        return 7;
    }
    throw new std::invalid_argument("alpha_to_pos: "+QString(alpha).toStdString());
}

QPoint Board::internal_to_xy(int internal_coord) {

    if(internal_coord < 21 || internal_coord > 98) {
        throw new std::invalid_argument("internal_to_xy: "+std::to_string(internal_coord));
    } else {
        int col = internal_coord % 10 - 1;
        int row = (internal_coord / 10) - 2;
        return QPoint(col,row);
    }
}

int Board::xy_to_internal(int x, int y) {

    if(x < 0 || x > 7 || y < 0 || y > 7) {
        throw new std::invalid_argument("xy_to_internal: "+std::to_string(x)+" "+std::to_string(y));
    } else {
        return (x+1) + ((y*10)+20);
    }
}

// assumes supplied move is correct
// otherwise might mess up the whole
// current board
QString Board::san(const Move &m) {

    //qDebug() << "SAN 1";
    QString san = QString("");
    // first check for null move
    if(m.is_null) {
        san = QString("--");
        return san;
    }

    //qDebug() << "SAN 2";
    // first test for checkmate and check (to be appended later)
    // create temp board, since appyling move and
    // testing for checkmate (which again needs
    // application of a move) makes it impossible
    // to undo (undo can only be done once, not twice in a row)

    //qDebug() << "SAN 3";
    Board b_temp = Board(*this);
    b_temp.apply(m);
    bool is_check = b_temp.is_check();
    bool is_checkmate = b_temp.is_checkmate();

    if(this->is_castles_wking(m) || this->is_castles_bking(m)) {
        san.append("O-O");
        if(is_checkmate) {
            san.append("#");
        }
        if(is_check) {
            san.append("+");
        }
        return san;
    } else if(this->is_castles_wqueen(m) || this->is_castles_bqueen(m)) {
        san.append("O-O-O");
        if(is_checkmate) {
            san.append("#");
        } else if(is_check) {
            san.append("+");
        }
        //qDebug() << "SAN 4";
        return san;
    } else {
        //qDebug() << "SAN 5";
        int piece_type = this->get_piece_type(m.from);
        int piece = this->get_piece_at(m.from);
        if(piece_type == KNIGHT) {
            san.append("N");
        }
        if(piece_type == BISHOP) {
            san.append("B");
        }
        if(piece_type == ROOK) {
            san.append("R");
        }
        if(piece_type == QUEEN) {
            san.append("Q");
        }
        if(piece_type == KING) {
            san.append("K");
        }
        QVector<Move> col_disambig;
        QVector<Move> row_disambig;
        int this_row = (m.from / 10) - 1;
        int this_col = m.from % 10;

        //qDebug() << "SAN 6";
        // find amibguous moves (except for pawns)
        if(piece_type != PAWN) {
            // if piece list contains only one piece, skip move generation
            // for testing disambiguity
            //qDebug() << "piece: " << piece;
            if(this->piece_list[this->turn][piece_type][1] == EMPTY) {
                goto ambig_check_finished;
            }
            //qDebug() << "SAN 6a";
            //QVector<Move> pseudo_legals = this->pseudo_legal_moves_from(m.from, false, this->turn);
            QVector<Move> pseudo_legals = this->pseudo_legal_moves(chess::ANY_SQUARE, m.to, piece_type, false, this->turn);
            //qDebug() << "SAN 6b";
            if(pseudo_legals.size() == 1) {
                goto ambig_check_finished;
            }
            //qDebug() << "SAN 6c";
            QVector<Move> legals = this->legals_from_pseudos(pseudo_legals);
            //qDebug() << "SAN 6d";
            if(legals.size() == 1) {
                goto ambig_check_finished;
            }
            for(int i=0;i<legals.count();i++) {
                Move mi = legals.at(i);
                if(this->get_piece_type(mi.from) == piece_type && mi.to == m.to && mi.from != m.from) {
                    // found pontential amibg. move
                    if((mi.from % 10) != this_col) {
                        // can be resolved via row
                        col_disambig.append(mi);
                    } else { // otherwise resolve by col
                        row_disambig.append(mi);
                    }
                }
            }
            int cnt_col_disambig = col_disambig.count();
            //cout << "ambig cols: " << +cnt_col_disambig << endl;
            int cnt_row_disambig = row_disambig.count();
            //cout << "ambig rows: " << +cnt_row_disambig << endl;
            // if there is an ambiguity
            if(cnt_col_disambig != 0 || cnt_row_disambig != 0) {
                // preferred way: resolve via column
                if(cnt_col_disambig>0 && cnt_row_disambig==0) {
                    san.append(QChar(this_col + 96));
                    // if not try to resolve via row
                } else if(cnt_row_disambig>0 && cnt_col_disambig==0) {
                    san.append(QChar(this_row + 48));
                } else {
                    // if that also fails (think three queens)
                    // resolve via full coordinate
                    san.append(QChar(this_col + 96));
                    san.append(QChar(this_row + 48));
                }
            }
        }
        ambig_check_finished:
        //qDebug() << "SAN 7";
        // handle a capture, i.e. if destination field
        // is not empty
        // in case of an en-passent capture, the destiation field
        // is empty. But then the destination field is the e.p. square
        if(this->get_piece_type(m.to) != EMPTY || m.to == this->en_passent_target) {
            if(piece_type == PAWN) {
                san.append(QChar(this_col + 96));
            }
            san.append(QString::fromLatin1("x"));
        }
        san.append(this->idx_to_str(m.to));
        if(m.promotion_piece == KNIGHT) {
            san.append(QString::fromLatin1("=N"));
        }
        if(m.promotion_piece == BISHOP) {
            san.append(QString::fromLatin1("=B"));
        }
        if(m.promotion_piece == ROOK) {
            san.append(QString::fromLatin1("=R"));
        }
        if(m.promotion_piece == QUEEN) {
            san.append(QString::fromLatin1("=Q"));
        }
    }
    if(is_checkmate) {
        san.append(QString::fromLatin1("#"));
    } else if(is_check) {
        san.append(QString::fromLatin1("+"));
    }
    return san;
}


int Board::get_king_pos(int king) const {
    for(int i=21;i<99;i++) {
        if(this->board[i] == king) {
            return i;
        }
    }
    return -1;
}


/**
 * @brief Board::is_black_castle_right_lost
 * @return true if black king and kingside rook
 *         are on initial position, false otherwise
 *         i.e. checks the _possibility_ whether
 *         castling could be possible (to check consistency when
 *         entering a board position)
 *         to call board status, use can_castle_* functions
 */
bool Board::is_black_king_castle_right_lost() const {
    if(this->board[E8] == BLACK_KING &&
            this->board[H8] == BLACK_ROOK) {
        return false;
    } else {
        return true;
    }
}

bool Board::is_black_queen_castle_right_lost() const {
    if(this->board[E8] == BLACK_KING &&
            this->board[A8] == BLACK_ROOK) {
        return false;
    } else {
        return true;
    }
}

bool Board::is_white_king_castle_right_lost() const {
    if(this->board[E1] == WHITE_KING &&
            this->board[H1] == WHITE_ROOK) {
        return false;
    } else {
        return true;
    }
}

bool Board::is_white_queen_castle_right_lost() const {
    if(this->board[E1] == WHITE_KING &&
            this->board[A1] == WHITE_ROOK) {
        return false;
    } else {
        return true;
    }
}

bool Board::is_consistent() {
    int white_king_pos = -1;
    int black_king_pos = -1;

    int cnt_white_king = 0;
    int cnt_black_king = 0;

    int cnt_white_queens = 0;
    int cnt_white_rooks = 0;
    int cnt_white_bishops = 0;
    int cnt_white_knights = 0;
    int cnt_white_pawns = 0;

    int cnt_black_queens = 0;
    int cnt_black_rooks = 0;
    int cnt_black_bishops = 0;
    int cnt_black_knights = 0;
    int cnt_black_pawns = 0;

    for(int i=21;i<99;i++) {
        uint8_t piece_type = this->get_piece_type(i);
        bool piece_color = this->get_piece_color(i);
        if(piece_type != EMPTY) {
            if(piece_type == KING) {
                if(piece_color == WHITE) {
                    white_king_pos = i;
                    cnt_white_king++;
                } else {
                    black_king_pos = i;
                    cnt_black_king++;
                }
            } else if(piece_type == QUEEN) {
                if(piece_color == WHITE) {
                    cnt_white_queens++;
                } else {
                    cnt_black_queens++;
                }
            } else if(piece_type == ROOK) {
                if(piece_color == WHITE) {
                    cnt_white_rooks++;
                } else {
                    cnt_black_rooks++;
                }
            } else if(piece_type == BISHOP) {
                if(piece_color == WHITE) {
                    cnt_white_bishops++;
                } else {
                    cnt_black_bishops++;
                }
            } else if(piece_type == KNIGHT) {
                if(piece_color == WHITE) {
                    cnt_white_knights++;
                } else {
                    cnt_black_knights++;
                }
            } else if(piece_type == PAWN) {
                if(piece_color == WHITE) {
                    if((i / 10) == 2) { // white pawn in first rank
                        return false;
                    } else {
                        cnt_white_pawns++;
                    }
                } else {
                    if((i / 10) == 9) { // black pawn in 8th rank
                        return false;
                    } else {
                        cnt_black_pawns++;
                    }
                }
            }
        }
    }
    // exactly one white and black king exist on board
    if(white_king_pos < 21 || white_king_pos >= 99
            || black_king_pos < 21 || black_king_pos >= 99
            || cnt_white_king != 1 || cnt_black_king != 1) {
        //qDebug() << "more than one king";
        return false;
    }
    // white and black king at least on field apart
    int larger = white_king_pos;
    int smaller = black_king_pos;
    if(black_king_pos > white_king_pos) {
        larger = black_king_pos;
        smaller = white_king_pos;
    }
    int diff = larger - smaller;
    if(diff == 10 || diff == 1 || diff == 11 || diff == 9) {
        //qDebug() << "diff of kings";
        return false;
    }
    // side not to move must not be in check
    bool not_to_move = !this->turn;
    bool to_move = this->turn;
    int idx_king_not_to_move = white_king_pos;
    if(not_to_move == BLACK) {
        idx_king_not_to_move = black_king_pos;
    }
    if(this->is_attacked(idx_king_not_to_move, to_move)) {
        //qDebug() << "idx of king not to move: " << idx_king_not_to_move;
        //qDebug() << "piece at 56" << this->board[56];
        //qDebug() << "king in check";
        return false;
    }
    // each side has 8 pawns or less
    if(cnt_white_pawns > 8 || cnt_black_pawns > 8) {
        //qDebug() << "pawn count";
        return false;
    }
    // check whether no. of promotions and pawn count fits for white
    int white_extra_pieces = std::max(0, cnt_white_queens-1) + std::max(0, cnt_white_rooks-2)
            + std::max(0, cnt_white_bishops - 2) + std::max(0, cnt_white_knights - 2);
    if(white_extra_pieces > (8-cnt_white_pawns)) {
        //qDebug() << "promo pieces";
        return false;
    }
    // ... for black
    int black_extra_pieces = std::max(0, cnt_black_queens-1) + std::max(0, cnt_black_rooks-2)
            + std::max(0, cnt_black_bishops - 2) + std::max(0, cnt_black_knights - 2);
    if(black_extra_pieces > (8-cnt_black_pawns)) {
        //qDebug() << "promo pieces black";
        return false;
    }
    // compare encoded castling rights of this board w/ actual
    // position of king and rook
    if(this->can_castle_wking() && this->is_white_king_castle_right_lost()) {
        //qDebug() << "castle 1";
        return false;
    }
    if(this->can_castle_wqueen() && this->is_white_queen_castle_right_lost()) {
        //qDebug() << "castle 2";
        return false;
    }
    if(this->can_castle_bking() && this->is_black_king_castle_right_lost()) {
        //qDebug() << "castle 3";
        return false;
    }
    if(this->can_castle_bqueen() && this->is_black_queen_castle_right_lost()) {
        //qDebug() << "castle 4";
        return false;
    }
    return true;
}

QString Board::print_raw() {
    QString s = QString("");
    for(int i=0;i<120;i++) {
        s += QString(" ").append(QString::number(this->board[i], 16 ));
        if((i+1)%10 == 0) {
            s.append(QString("\n"));
        }
    }
    return s;
}

/*
 * \brief operator <<
 * \param strm
 * \param b
 * \return stream prints board representation into stream
 */
/**
 * @brief operator <<
 * @param strm
 * @param b
 * @return
 *
 * prints board as ascii output.
 *
 * Example:
 * Board b();
 * std::cout << b << std::endl;
 *
 */
std::ostream& operator<<(std::ostream &strm, const Board &b) {
    for(uint8_t i=90;i>=20;i-=10) {
        for(uint8_t j=1;j<=9;j++) {
            //uint8_t piece = b.piece_type(i+j);
            IntBits piece = IntBits(b.board[i+j]);
            bool color = WHITE;
            if(piece.test(COLOR_FLAG) == WHITE) {
                color = WHITE;
            } else {
                color = BLACK;
            }
            piece.set(3,0);
            piece.set(4,0);
            piece.set(7,0);
            if(piece == PAWN) {
                if(color == WHITE) {
                    strm << "P ";
                }
                else {
                    strm << "p ";
                }
            }
            if(piece == KNIGHT) {
                if(color == WHITE) {
                    strm << "N ";
                }
                else {
                    strm << "n ";
                }
            }
            if(piece == BISHOP) {
                if(color == WHITE) {
                    strm << "B ";
                }
                else {
                    strm << "b ";
                }
            }
            if(piece == ROOK) {
                if(color == WHITE) {
                    strm << "R ";
                }
                else {
                    strm << "r ";
                }
            }
            if(piece == QUEEN) {
                if(color == WHITE) {
                    strm << "Q ";
                }
                else {
                    strm << "q ";
                }
            }
            if(piece == KING) {
                if(color == WHITE) {
                    strm << "K ";
                }
                else {
                    strm << "k ";
                }
            }
            if(b.en_passent_target == (i+j)) {
                strm << ": ";
            } else if(piece == EMPTY) {
                strm << ". ";
            }
        }
        strm << std::endl;
    }
    return strm;
}

int Board::zobrist_piece_type(int piece) const {
    switch (piece)
    {
        case BLACK_PAWN:
            return 0;
        case WHITE_PAWN:
            return 1;
        case BLACK_KNIGHT:
            return 2;
        case WHITE_KNIGHT:
            return 3;
        case BLACK_BISHOP:
            return 4;
        case WHITE_BISHOP:
            return 5;
        case BLACK_ROOK:
            return 6;
        case WHITE_ROOK:
            return 7;
        case BLACK_QUEEN:
            return 8;
        case WHITE_QUEEN:
            return 9;
        case BLACK_KING:
            return 10;
        case WHITE_KING:
            return 11;
    }
    throw std::invalid_argument("piece type out of range in ZobristHash:kind_of_piece");
}

quint64 Board::get_pos_hash() {

    if(this->pos_hash_initialized) {
        return this->pos_hash;
    } else {
        quint64 piece = Q_UINT64_C(0);
        for(int i=0;i<8;i++) {
            for(int j=0;j<8;j++) {
                uint8_t piece_at_ij = this->get_piece_at(i,j);
                //qDebug() << i << j << " " <<  piece_at_ij;
                if(piece_at_ij != EMPTY) {
                    int kind_of_piece = this->zobrist_piece_type(piece_at_ij);
                    int offset_piece = 64 * kind_of_piece + 8 * j + i;
                    piece = piece^POLYGLOT_RANDOM_64[offset_piece];
                }
            }
        }
        this->pos_hash = piece;
        return this->pos_hash;
    }
}

quint64 Board::get_zobrist() {

    if(this->zobrist_initialized) {
        return this->zobrist;
    } else {
        quint64 piece = Q_UINT64_C(0);
        for(int i=0;i<8;i++) {
            for(int j=0;j<8;j++) {
                uint8_t piece_at_ij = this->get_piece_at(i,j);
                if(piece_at_ij != EMPTY) {
                    int kind_of_piece = this->zobrist_piece_type(piece_at_ij);
                    int offset_piece = 64 * kind_of_piece + 8 * j + i;
                    piece = piece^POLYGLOT_RANDOM_64[offset_piece];
                }
            }
        }
        //std::cout << "pieces:" << std::endl;
        //std::cout << std::hex << piece << std::endl;
        quint64 en_passent = Q_UINT64_C(0);
        uint8_t ep_target = this->get_ep_target();
        if(ep_target != 0) {
            int file = (ep_target % 10) - 1;
            // check if left or right is a pawn from player to move
            if(this->turn == WHITE) {
                uint8_t left = this->get_piece_at(ep_target-11);
                uint8_t right = this->get_piece_at(ep_target-9);
                if(left == WHITE_PAWN || right == WHITE_PAWN) {
                    en_passent = POLYGLOT_RANDOM_64[RANDOM_EN_PASSENT + file];
                }
            } else {
                uint8_t left = this->get_piece_at(ep_target+11);
                uint8_t right = this->get_piece_at(ep_target+9);
                if(left == BLACK_PAWN || right == BLACK_PAWN) {
                    en_passent = POLYGLOT_RANDOM_64[RANDOM_EN_PASSENT + file];
                }
            }
        }
        quint64 castle = Q_UINT64_C(0);
        if(this->can_castle_wking()) {
            castle = castle^POLYGLOT_RANDOM_64[RANDOM_CASTLE];
        }
        if(this->can_castle_wqueen()) {
            castle = castle^POLYGLOT_RANDOM_64[RANDOM_CASTLE+1];
        }
        if(this->can_castle_bking()) {
            castle = castle^POLYGLOT_RANDOM_64[RANDOM_CASTLE+2];
        }
        if(this->can_castle_bqueen()) {
            castle = castle^POLYGLOT_RANDOM_64[RANDOM_CASTLE+3];
        }

        quint64 turn = Q_UINT64_C(0);
        if(this->turn == WHITE) {
            turn = POLYGLOT_RANDOM_64[RANDOM_TURN];
        }

        this->zobrist = piece^castle^en_passent^turn;
        return this->zobrist;
    }
}



}
