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
    this->castling_rights = 0;
    this->en_passent_target = 0;
    this->halfmove_clock = 0;
    this->fullmove_number = 1;
    this->undo_available = false;
    this->last_was_null = false;
    this->prev_halfmove_clock = 0;
    this->m_zobrist_initialized = false;
    this->m_pos_hash_initialized = false;
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
        this->castling_rights = 0x0F;
    } else {
        for(int i=0;i<120;i++) {
            this->board[i] = EMPTY_POS[i];
        }
        this->castling_rights = 0;
    }
    this->init_piece_list();
    this->en_passent_target = 0;
    this->halfmove_clock = 0;
    this->fullmove_number = 1;
    this->undo_available = false;
    this->last_was_null = false;
    this->prev_halfmove_clock = 0;
    this->m_zobrist_initialized = false;
    this->m_pos_hash_initialized = false;
}

bool Board::is_initial_position() {
    if(!this->turn == WHITE) {
        return false;
    }
    for(int i=0;i<120;i++) {
        if(this->board[i] != chess::INIT_POS[i]) {
            return false;
        }
    }
    if(this->castling_rights != 0x0F) {
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
uint8_t Board::get_ep_target() {
    return this->en_passent_target;
}


// returns 'empty' if c is no valid piece symbol in
// fen notation
uint8_t Board::piece_from_symbol(QChar c) {
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
    return 0x00;
}

void Board::set_piece_at(int x, int y, uint8_t piece) {
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

uint8_t Board::get_piece_at(int x, int y) {
    if(x>=0 && x<8 && y>=0 && y <8) {
        int idx = ((y+2)*10) + (x+1);
        return this->board[idx];
    } else {
        throw std::invalid_argument("called get_piece_at with invalid paramters");
    }
}

uint8_t Board::get_piece_type_at(int x, int y) {
    if(x>=0 && x<8 && y>=0 && y <8) {
        int idx = ((y+2)*10) + (x+1);
        uint8_t piece = this->board[idx];
        if(piece >= 0x80) {
            return piece - 0x80;
        } else {
            return piece;
        }
    } else {
        throw std::invalid_argument("called get_piece_type_at with invalid paramters");
    }
}

bool Board::get_piece_color_at(int x, int y) {
    if(x>=0 && x<8 && y>=0 && y <8) {
        int idx = ((y+2)*10) + (x+1);
        return this->piece_color(idx);
    } else {
        throw std::invalid_argument("called get_piece_color_at with invalid paramters");
    }
}


// returns 'empty' if c is no valid piece symbol in
// fen notation
QChar Board::piece_to_symbol(uint8_t piece) {
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
            if(rj == QChar('1') || rj == QChar('2') || rj == QChar('3') || rj == QChar('4')
                    || rj == QChar('5') || rj == QChar('6') || rj == QChar('7') || rj == QChar('8')) {
                if(previous_was_digit) {
                    throw std::invalid_argument("fen: two consecutive digits in rows");
                } else {
                    field_sum += rj.digitValue();
                    previous_was_digit = true;
                }
            } else if(rjl == QChar('p') || rjl == QChar('n') || rjl == QChar('b')
                      || rjl == QChar('r') || rjl == QChar('q') || rjl == QChar('k')) {
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
    if(!(fen_parts.at(1) == QString("w") || fen_parts.at(1) == QString("b"))) {
        throw std::invalid_argument("turn part is invalid");
    }
    // check that castles part in correctly encoded using regex
    QRegularExpressionMatch match = FEN_CASTLES_REGEX.match(fen_parts[2]);
    if(!match.hasMatch()) {
        throw std::invalid_argument("castles encoding is invalid");
    }
    // check correct encoding of en passent squares
    if(fen_parts.at(3) != QChar('-')) {
        if(fen_parts.at(1) == QChar('w')) {
            // should be something like "e6" etc. if white is to move
            // check that int value part is sixth rank
            if(fen_parts.at(3).size() != 2 || fen_parts.at(3).at(1) != QChar('6')) {
                throw std::invalid_argument("invalid e.p. encoding (white to move)");
            }
        } else {
            if(fen_parts.at(3).size() != 2 || fen_parts.at(3).at(1) != QChar('3')) {
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
            if(rj == QChar('1') || rj == QChar('2') || rj == QChar('3') || rj == QChar('4')
                    || rj == QChar('5') || rj == QChar('6') || rj == QChar('7') || rj == QChar('8')) {
                square_index += rj.digitValue();
            } else if(rjl == QChar('p') || rjl == QChar('n') || rjl == QChar('b')
                      || rjl == QChar('r') || rjl == QChar('q') || rjl == QChar('k')) {
                uint8_t piece = this->piece_from_symbol(rj);
                this->board[square_index] = piece;
                square_index += 1;
            }
        }
    }
    // set turn
    if(fen_parts.at(1) == QString("w")) {
        this->turn = WHITE;
    }
    if(fen_parts.at(1) == QString("b")) {
        this->turn = BLACK;
    }
    this->castling_rights = 0x00;
    for(int i=0;i<fen_parts.at(2).size();i++) {
        QChar ci = fen_parts.at(2).at(i);
        if(ci == QChar('K')) {
            this->set_castle_wking(true);
        }
        if(ci == QChar('Q')) {
            this->set_castle_wqueen(true);
        }
        if(ci == QChar('k')) {
            this->set_castle_bking(true);
        }
        if(ci == QChar('q')) {
            this->set_castle_bqueen(true);
        }
    }
    // set en passent square
    if(fen_parts.at(3) == QString('-')) {
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
    if(!this->is_consistent()) {
        throw std::invalid_argument("board position from supplied fen is inconsistent");
    }
    this->init_piece_list();

    this->m_zobrist_initialized = false;
    this->m_pos_hash_initialized = false;
}

QString Board::idx_to_str(int idx) {
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

QString Board::fen() {
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
    if(this->castling_rights == 0x00) {
        fen_string.append(" -");
    } else {
        fen_string.append(' ');
        if(this->can_castle_wking()) {
            fen_string.append(('K'));
        }
        if(this->can_castle_wqueen()) {
            fen_string.append(('Q'));
        }
        if(this->can_castle_bking()) {
            fen_string.append(('k'));
        }
        if(this->can_castle_bqueen()) {
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


bool Board::can_castle_wking() {
    IntBits cstle = IntBits(this->castling_rights);
    if(cstle.test(CASTLE_WKING_POS)) {
        return true;
    } else {
        return false;
    }
}

bool Board::can_castle_bking() {
    IntBits cstle = IntBits(this->castling_rights);
    if(cstle.test(CASTLE_BKING_POS)) {
        return true;
    } else {
        return false;
    }
}

bool Board::can_castle_wqueen() {
    IntBits cstle = IntBits(this->castling_rights);
    if(cstle.test(CASTLE_WQUEEN_POS)) {
        return true;
    } else {
        return false;
    }
}

bool Board::can_castle_bqueen() {
    IntBits cstle = IntBits(this->castling_rights);
    if(cstle.test(CASTLE_BQUEEN_POS)) {
        return true;
    } else {
        return false;
    }
}

void Board::set_castle_wking(bool can_do) {
    IntBits cstle = IntBits(this->castling_rights);
    if(can_do) {
        cstle.set(CASTLE_WKING_POS);
    } else {
        cstle.reset(CASTLE_WKING_POS);
    }
    this->castling_rights = static_cast<uint8_t>(cstle.to_ulong());
}

void Board::set_castle_bking(bool can_do) {
    IntBits cstle = IntBits(this->castling_rights);
    if(can_do) {
        cstle.set(CASTLE_BKING_POS);
    } else {
        cstle.reset(CASTLE_BKING_POS);
    }
    this->castling_rights = static_cast<uint8_t>(cstle.to_ulong());
}

void Board::set_castle_wqueen(bool can_do) {
    IntBits cstle = IntBits(this->castling_rights);
    if(can_do) {
        cstle.set(CASTLE_WQUEEN_POS);
    } else {
        cstle.reset(CASTLE_WQUEEN_POS);
    }
    this->castling_rights = static_cast<uint8_t>(cstle.to_ulong());
}

void Board::set_castle_bqueen(bool can_do) {
    IntBits cstle = IntBits(this->castling_rights);
    if(can_do) {
        cstle.set(CASTLE_BQUEEN_POS);
    } else {
        cstle.reset(CASTLE_BQUEEN_POS);
    }
    this->castling_rights = static_cast<uint8_t>(cstle.to_ulong());
}

QVector<Move> Board::pseudo_legal_moves() {

    return this->pseudo_legal_moves_from(0,true,this->turn);
}

QVector<Move> Board::pseudo_legal_moves(uint8_t to_square, uint8_t piece_type) {
    if(piece_type == KING) {
        return this->pseudo_legal_moves_to(to_square, piece_type, true,this->turn);
    } else {
        return this->pseudo_legal_moves_to(to_square, piece_type, false,this->turn);
    }
}

bool Board::castles_wking(const Move &m) {
    if(this->piece_type(m.from) == KING && this->piece_color(m.from) == WHITE &&
            m.from == E1 && m.to == G1) {
        return true;
    } else {
        return false;
    }
}


bool Board::castles_wqueen(const Move &m) {
    if(this->piece_type(m.from) == KING && this->piece_color(m.from) == WHITE &&
            m.from == E1 && m.to == C1) {
        return true;
    } else {
        return false;
    }
}


bool Board::castles_bking(const Move &m) {
    if(this->piece_type(m.from) == KING && this->piece_color(m.from) == BLACK &&
            m.from == E8 && m.to == G8) {
        return true;
    } else {
        return false;
    }
}

bool Board::castles_bqueen(const Move &m) {
    if(this->piece_type(m.from) == KING && this->piece_color(m.from) == BLACK &&
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
QVector<Move> Board::legal_moves(uint8_t to_square, uint8_t piece_type) {

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

    QVector<Move> pseudo_legals = this->pseudo_legal_moves_from(from_square, true,this->turn);
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
    QVector<Move> pseudo_legals = this->pseudo_legal_moves_from(m.from, true,this->turn);
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
    bool color = this->piece_color(m.from);
    // find king with that color
    for(int i= 21;i<99;i++) {
        if(this->piece_type(i) == KING && this->piece_color(i) == color) {
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
                if(this->castles_wking(m)) {
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
                if(this->castles_bking(m)) {
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
                if(this->castles_wqueen(m)) {
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
                if(this->castles_bqueen(m)) {
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
            && (this->piece_color(idx-9)==WHITE) && (this->piece_type(idx-9)==PAWN)) {
        return true;
    }
    // lower right
    if(attacker_color == WHITE && (this->board[idx-11]!=0xFF)
            && (this->piece_color(idx-11)==WHITE) && (this->piece_type(idx-11)==PAWN)) {
        return true;
    }
    // check black, upper right
    if(attacker_color == BLACK && (this->board[idx+11]!=0xFF)
            && (this->piece_color(idx+11)==BLACK) && (this->piece_type(idx+11)==PAWN)) {
        return true;
    }
    // check black, upper left
    if(attacker_color == BLACK && (this->board[idx+9]!=0xFF)
            && (this->piece_color(idx+9)==BLACK) && (this->piece_type(idx+9)==PAWN)) {
        return true;
    }
    // check all squares (except idx itself)
    // for potential attackers
    for(int i=21;i<99;i++) {
        // skip empty squares
        if(i!=idx && this->board[i] != 0x00) {
            // can't attack yourself
            if(this->piece_color(i) == attacker_color) {
                uint8_t piece = this->piece_type(i);
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
                    QVector<Move> targets = this->pseudo_legal_moves_from(i,false,attacker_color);
                    for(int j=0;j<targets.size();j++) {
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

// calling with from_square = 0 means all possible moves
// will find all pseudo legal move for supplied player (turn must be
// either WHITE or BLACK)
QVector<Move> Board::pseudo_legal_moves_from(int from_square, bool with_castles, bool turn) {

    QVector<Move> moves;

    for(int i=21;i<99;i++) {
        if(from_square == 0 || from_square == i) {
            // skip offboard's left and right
            if(!(this->board[i] == 0xFF)) {
                // get piece type & color
                bool color = this->piece_color(i);
                if(color == turn) {
                    uint8_t piece = this->piece_type(i);
                    // handle case of PAWN
                    if(piece == PAWN) {
                        uint8_t piece_idx = IDX_WPAWN;
                        if(color == BLACK) {
                            piece_idx = IDX_BPAWN;
                        }
                        // take up right, or up left
                        for(int j=3;j<=4;j++) {
                            uint8_t idx = i + DIR_TABLE[piece_idx][j];
                            if(!this->is_offside(idx)) {
                                if((!this->is_empty(idx) && color==BLACK && this->is_white_at(idx)) ||
                                        (!this->is_empty(idx) && color==WHITE && !this->is_white_at(idx))) {
                                    // if it's a promotion square, add four moves
                                    if((color==WHITE && (idx / 10 == 9)) || (color==BLACK && (idx / 10 == 2))) {
                                        moves.append(Move(i,idx,QUEEN));
                                        moves.append(Move(i,idx,ROOK));
                                        moves.append(Move(i,idx,BISHOP));
                                        moves.append(Move(i,idx,KNIGHT));
                                    } else {
                                        moves.append(Move(i,idx));
                                    }
                                }
                            }
                        }
                        // move one (j=1) or two (j=2) up (or down in the case of black)
                        for(int j=1;j<=2;j++) {
                            uint8_t idx = i + DIR_TABLE[piece_idx][j];
                            if(!this->is_offside(idx)) {
                                if(j==2 && ((color == WHITE && (i/10==3)) || (color==BLACK && (i/10==8)))) {
                                    // means we have a white/black pawn in inital position, direct square
                                    // in front is empty => allow to move two forward
                                    if(this->is_empty(idx)) {
                                        moves.append(Move(i,idx));
                                    }
                                }
                                else if(j==1) {
                                    // case of one-step move forward
                                    if(!this->is_empty(idx)) {
                                        break;
                                    } else {
                                        // if it's a promotion square, add four moves
                                        if((color==WHITE && (idx / 10 == 9)) || (color==BLACK && (idx / 10 == 2))) {
                                            moves.append(Move(i,idx,QUEEN));
                                            moves.append(Move(i,idx,ROOK));
                                            moves.append(Move(i,idx,BISHOP));
                                            moves.append(Move(i,idx,KNIGHT));
                                        } else {
                                            moves.append(Move(i,idx));
                                        }
                                    }
                                }
                            }
                        }
                        // finally, potential en-passent capture is handled
                        // left up
                        if(color == WHITE && (this->en_passent_target - i)==9) {
                            Move m = (Move(i,this->en_passent_target));
                            moves.append(m);
                        }
                        // right up
                        if(color == WHITE && (this->en_passent_target - i)==11) {
                            Move m = (Move(i,this->en_passent_target));
                            moves.append(m);
                        }
                        // left down
                        if(color == BLACK && (this->en_passent_target - i)==-9) {
                            Move m = (Move(i,this->en_passent_target));
                            moves.append(m);
                        }
                        if(color == BLACK && (this->en_passent_target - i)==-11) {
                            Move m = (Move(i,this->en_passent_target));
                            moves.append(m);
                        }
                    }
                    // handle case of knight
                    if(piece == KNIGHT || piece == KING) {
                        int lookup_idx;
                        if(piece == KNIGHT) {
                            lookup_idx = IDX_KNIGHT;
                        } else {
                            lookup_idx = IDX_KING;
                        }
                        for(int j=1;j<=DIR_TABLE[lookup_idx][0];j++) {
                            uint8_t idx = i + DIR_TABLE[lookup_idx][j];
                            if(!this->is_offside(idx)) {
                                if(this->is_empty(idx) ||
                                        (this->piece_color(idx) != color)) {
                                    moves.append(Move(i,idx));
                                }
                            }
                        }
                    }
                    // handle case of bishop, rook, queen
                    if(piece == ROOK || piece == BISHOP || piece == QUEEN) {
                        int lookup_idx = IDX_ROOK;
                        if(piece == QUEEN) {
                            lookup_idx = IDX_QUEEN;
                        }
                        if(piece == BISHOP) {
                            lookup_idx = IDX_BISHOP;
                        }
                        for(int j=1;j<=DIR_TABLE[lookup_idx][0] ;j++) {
                            uint8_t idx = i + DIR_TABLE[lookup_idx][j];
                            bool stop = false;
                            while(!stop) {
                                if(!this->is_offside(idx)) {
                                    if(this->is_empty(idx)) {
                                        moves.append(Move(i,idx));
                                    } else {
                                        stop = true;
                                        if(this->piece_color(idx) != color) {
                                            moves.append(Move(i,idx));
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
            }
            if(with_castles) {
                if(this->turn == WHITE) {
                    // check for castling
                    // white kingside
                    if(i==E1 && !this->is_empty(E1) && this->can_castle_wking() &&
                            this->piece_color(E1) == WHITE && this->piece_color(H1) == WHITE
                            && this->piece_type(E1) == KING && this->piece_type(H1) == ROOK
                            && this->is_empty(F1) && this->is_empty(G1)) {
                        moves.append(Move(E1,G1));
                    }
                    // white queenside
                    if(i==E1 && !this->is_empty(E1) && this->can_castle_wqueen() &&
                            this->piece_color(E1) == WHITE && this->piece_color(A1) == WHITE
                            && this->piece_type(E1) == KING && this->piece_type(A1) == ROOK
                            && this->is_empty(D1) && this->is_empty(C1) && this->is_empty(B1)) {
                        moves.append(Move(E1,C1));
                    }
                }
                if(this->turn == BLACK) {
                    // black kingside
                    if(i==E8 && !this->is_empty(E8) && this->can_castle_bking() &&
                            this->piece_color(E8) == BLACK && this->piece_color(H8) == BLACK
                            && this->piece_type(E8) == KING && this->piece_type(H8) == ROOK
                            && this->is_empty(F8) && this->is_empty(G8)) {
                        moves.append(Move(E8,G8));
                    }
                    // black queenside
                    if(i==E8 && !this->is_empty(E8) && this->can_castle_bqueen() &&
                            this->piece_color(E8) == BLACK && this->piece_color(A8) == BLACK
                            && this->piece_type(E8) == KING && this->piece_type(A8) == ROOK
                            && this->is_empty(D8) && this->is_empty(C8) && this->is_empty(B8)) {
                        moves.append(Move(E8,C8));
                    }
                }
            }
        }
    }
    return moves;
}


QVector<Move> Board::pseudo_legal_moves_to(uint8_t to_square, uint8_t piece_type, bool with_castles, bool turn) {

    QVector<Move> moves;
    // pawn moves
    if(piece_type == PAWN) {
        for(int i=0;i<10;i++) {
            int from = this->piece_list[turn][PAWN][i];
            if(from == EMPTY) {
                break;
            }
            assert(this->board[from] != 0xff);
            uint8_t piece_idx = IDX_WPAWN;
            if(turn == BLACK) {
                piece_idx = IDX_BPAWN;
            }
            // take up right, or up left
            for(int j=3;j<=4;j++) {
                uint8_t idx = from + DIR_TABLE[piece_idx][j];
                if(idx == to_square && !this->is_offside(idx)) {
                    if((!this->is_empty(idx) && turn==BLACK && this->is_white_at(idx)) ||
                            (!this->is_empty(idx) && turn==WHITE && !this->is_white_at(idx))) {
                        // if it's a promotion square, add four moves
                        if((turn==WHITE && (idx / 10 == 9)) || (turn==BLACK && (idx / 10 == 2))) {
                            assert(this->board[from] != 0xff);

                            moves.append(Move(from,idx,QUEEN));
                            moves.append(Move(from,idx,ROOK));
                            moves.append(Move(from,idx,BISHOP));
                            moves.append(Move(from,idx,KNIGHT));
                        } else {
                            assert(this->board[from] != 0xff);
                            moves.append(Move(from,idx));
                        }
                    }
                }
            }
            // move one (j=1) or two (j=2) up (or down in the case of black)
            uint8_t idx_1up = from + DIR_TABLE[piece_idx][1];
            uint8_t idx_2up = from + DIR_TABLE[piece_idx][2];
            if(idx_2up == to_square && !this->is_offside(idx_2up)) {
                if((turn == WHITE && (i/10==3)) || (turn==BLACK && (i/10==8))) {
                    // means we have a white/black pawn in inital position, direct square
                    // in front is empty => allow to move two forward
                    if(this->is_empty(idx_1up) && this->is_empty(idx_2up)) {
                        assert(this->board[from] != 0xff);

                        moves.append(Move(from,idx_2up));
                    }
                }
            }
            if(idx_1up == to_square && !this->is_offside(idx_1up) && this->is_empty(idx_1up)) {
                // if it's a promotion square, add four moves
                if((turn==WHITE && (idx_1up / 10 == 9)) || (turn==BLACK && (idx_1up / 10 == 2))) {
                    assert(this->board[from] != 0xff);

                    moves.append(Move(from,idx_1up,QUEEN));
                    moves.append(Move(from,idx_1up,ROOK));
                    moves.append(Move(from,idx_1up,BISHOP));
                    moves.append(Move(from,idx_1up,KNIGHT));
                } else {
                    assert(this->board[from] != 0xff);

                    moves.append(Move(from,idx_1up));
                }
            }
            // finally, potential en-passent capture is handled
            // left up
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
    if(piece_type == KNIGHT) {
        for(int i=0;i<10;i++) {
            int from = this->piece_list[turn][KNIGHT][i];
            if(from == EMPTY) {
                break;
            }
            assert(this->board[from] != 0xff);
            int lookup_idx = IDX_KNIGHT;
            for(int j=1;j<=DIR_TABLE[lookup_idx][0];j++) {
                uint8_t idx = from + DIR_TABLE[lookup_idx][j];
                if(idx == to_square && !this->is_offside(idx)) {
                    if(this->is_empty(idx) ||
                            (this->piece_color(idx) != turn)) {
                        moves.append(Move(from,idx));
                    }
                }
            }
        }
    }
    if(piece_type == KING) {
        for(int i=0;i<10;i++) {
            int from = this->piece_list[turn][KING][i];
            if(from == EMPTY) {
                break;
            }
            assert(this->board[from] != 0xff);
            int lookup_idx = IDX_KING;
            for(int j=1;j<=DIR_TABLE[lookup_idx][0];j++) {
                uint8_t idx = from + DIR_TABLE[lookup_idx][j];
                if(idx == to_square && !this->is_offside(idx)) {
                    if(this->is_empty(idx) ||
                            (this->piece_color(idx) != turn)) {
                        moves.append(Move(from,idx));
                    }
                }
            }
        }
    }
    if(piece_type == ROOK) {
        for(int i=0;i<10;i++) {
            int from = this->piece_list[turn][ROOK][i];
            if(from == EMPTY) {
                break;
            }
            assert(this->board[from] != 0xff);
            int lookup_idx = IDX_ROOK;
            for(int j=1;j<=DIR_TABLE[lookup_idx][0] ;j++) {
                uint8_t idx = from + DIR_TABLE[lookup_idx][j];
                bool stop = false;
                while(!stop) {
                    if(!this->is_offside(idx)) {
                        if(this->is_empty(idx)) {
                            if(to_square == idx) {
                                moves.append(Move(from,idx));
                            }
                        } else {
                            stop = true;
                            if(this->piece_color(idx) != turn) {
                                if(to_square == idx) {
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
    if(piece_type == BISHOP) {
        for(int i=0;i<10;i++) {
            int from = this->piece_list[turn][BISHOP][i];
            if(from == EMPTY) {
                break;
            }
            assert(this->board[from] != 0xff);
            int lookup_idx = IDX_BISHOP;
            for(int j=1;j<=DIR_TABLE[lookup_idx][0] ;j++) {
                uint8_t idx = from + DIR_TABLE[lookup_idx][j];
                bool stop = false;
                while(!stop) {
                    if(!this->is_offside(idx)) {
                        if(this->is_empty(idx)) {
                            if(to_square == idx) {
                                moves.append(Move(from,idx));
                            }
                        } else {
                            stop = true;
                            if(this->piece_color(idx) != turn) {
                                if(to_square == idx) {
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
    if(piece_type == QUEEN) {
        for(int i=0;i<10;i++) {
            int from = this->piece_list[turn][QUEEN][i];
            if(from == EMPTY) {
                break;
            }
            assert(this->board[from] != 0xff);
            int lookup_idx = IDX_QUEEN;
            for(int j=1;j<=DIR_TABLE[lookup_idx][0] ;j++) {
                uint8_t idx = from + DIR_TABLE[lookup_idx][j];
                bool stop = false;
                while(!stop) {
                    if(!this->is_offside(idx)) {
                        if(this->is_empty(idx)) {
                            if(to_square == idx) {
                                moves.append(Move(from,idx));
                            }
                        } else {
                            stop = true;
                            if(this->piece_color(idx) != turn) {
                                if(to_square == idx) {
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
    if(with_castles) {
        if(turn == WHITE) {
            // check for castling
            // white kingside
            if(!this->is_empty(E1) && this->can_castle_wking() &&
                    this->piece_color(E1) == WHITE && this->piece_color(H1) == WHITE
                    && this->piece_type(E1) == KING && this->piece_type(H1) == ROOK
                    && this->is_empty(F1) && this->is_empty(G1)) {
                moves.append(Move(E1,G1));
            }
            // white queenside
            if(!this->is_empty(E1) && this->can_castle_wqueen() &&
                    this->piece_color(E1) == WHITE && this->piece_color(A1) == WHITE
                    && this->piece_type(E1) == KING && this->piece_type(A1) == ROOK
                    && this->is_empty(D1) && this->is_empty(C1) && this->is_empty(B1)) {
                moves.append(Move(E1,C1));
            }
        }
        if(turn == BLACK) {
            // black kingside
            if(!this->is_empty(E8) && this->can_castle_bking() &&
                    this->piece_color(E8) == BLACK && this->piece_color(H8) == BLACK
                    && this->piece_type(E8) == KING && this->piece_type(H8) == ROOK
                    && this->is_empty(F8) && this->is_empty(G8)) {
                moves.append(Move(E8,G8));
            }
            // black queenside
            if(!this->is_empty(E8) && this->can_castle_bqueen() &&
                    this->piece_color(E8) == BLACK && this->piece_color(A8) == BLACK
                    && this->piece_type(E8) == KING && this->piece_type(A8) == ROOK
                    && this->is_empty(D8) && this->is_empty(C8) && this->is_empty(B8)) {
                moves.append(Move(E8,C8));
            }
        }
    }
    return moves;
}


bool Board::movePromotes(const Move&m) {
    if(this->piece_type(m.from) == chess::PAWN) {
        if(this->piece_color(m.from) == chess::WHITE && ((m.to / 10)==9)) {
            return true;
        }
        if(this->piece_color(m.from) == chess::BLACK && ((m.to / 10)==2)) {
            return true;
        }
    }
    return false;
}


bool Board::piece_color(uint8_t idx) {
    IntBits piece = IntBits(this->board[idx]);
    if(piece.test(COLOR_FLAG) == WHITE) {
        return WHITE;
    } else {
        return BLACK;
    }
}

uint8_t Board::piece_type(uint8_t idx) {
    IntBits piece = IntBits(this->board[idx]);
    piece.set(7,0);
    return static_cast<uint8_t>(piece.to_ulong());
}

uint8_t Board::piece_at(uint8_t idx) {
    if(idx >= 21 && idx <= 98) {
        return this->board[idx];
    } else {
        throw std::invalid_argument("called get_piece_at with invalid paramters");
    }
}

// returns true if square is not empty
bool Board::is_empty(uint8_t idx) {
    if(this->board[idx] == 0x00) {
        return true;
    } else {
        return false;
    }
}


// returns true if square is in fringe
bool Board::is_offside(uint8_t idx) {
    if(this->board[idx] == 0xFF) {
        return true;
    } else {
        return false;
    }
}

bool Board::can_claim_fifty_moves() {
    return this->halfmove_clock >= 100;
}



// returns true (== Black) if not occupied!
bool Board::is_white_at(uint8_t idx) {
    IntBits square = IntBits(this->board[idx]);
    if(square.test(COLOR_FLAG)) {
        return false;
    } else {
        return true;
    }
}

void Board::remove_from_piece_list(bool color, uint8_t piece_type, uint8_t idx) {

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

void Board::add_to_piece_list(bool color, uint8_t piece_type, uint8_t idx) {

    for(int i=0;i<10;i++) {
        if(this->piece_list[color][piece_type][i] == EMPTY) {
            this->piece_list[color][piece_type][i] = idx;
            break;
        }
    }
}

// doesn't check legality
void Board::apply(const Move &m) {

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
        this->last_was_null = false;
    this->turn = !this->turn;
    this->prev_en_passent_target = this->en_passent_target;
    this->prev_castling_rights = this->castling_rights;
    this->en_passent_target = 0;
    if(this->turn == WHITE) {
        this->fullmove_number++;
    }
    for(int i=0;i<120;i++) {
        this->old_board[i] = this->board[i];
    }

    uint8_t old_piece_type = this->piece_type(m.from);
    bool color = this->piece_color(m.from);
    // if target field is not empty, remove from piece list
    // this must be of oppsite color than the currently moving piece
    if(this->board[m.to] != EMPTY) {
        uint8_t current_target_piece = this->piece_type(m.to);
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
                this->board[m.to-10] = 0x00;
                this->remove_from_piece_list(BLACK, PAWN, m.to-10);
            }
            if(color == BLACK && ((m.from -m.to == 9) || (m.from - m.to)==11)) {
                // remove captured pawn
                this->board[m.to+10] = 0x00;
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
}

void Board::undo() {
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
            this->castling_rights = this->prev_castling_rights;
            this->turn = !this->turn;
            this->halfmove_clock = this->prev_halfmove_clock;
            this->prev_halfmove_clock = 0;
            if(this->turn == BLACK) {
                this->fullmove_number--;
            }
        }
    }
    this->init_piece_list();
}


Board::Board(const Board &other) {
    turn = other.turn;
    castling_rights = other.castling_rights;
    en_passent_target = other.en_passent_target;
    halfmove_clock = other.halfmove_clock;
    fullmove_number = other.fullmove_number;
    undo_available = other.undo_available;
    last_was_null = other.last_was_null;
    prev_halfmove_clock = other.prev_halfmove_clock;
    m_zobrist_initialized = other.m_zobrist_initialized;
    m_pos_hash_initialized = other.m_pos_hash_initialized;
    m_zobrist = other.m_zobrist;
    m_pos_hash = other.m_pos_hash;
    for(int i=0;i<120;i++) {
        board[i] = other.board[i];
        old_board[i] = other.old_board[i];
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
        if((this->piece_type(i)==KING) && (this->piece_color(i)==this->turn)){
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
        if((this->piece_type(i)==KING) && (this->piece_color(i)==this->turn)){
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
        if((this->piece_type(i)==KING) && (this->piece_color(i)==this->turn)){
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
    return GENERAL_ERROR;
}

QPoint Board::internal_to_xy(int internal_coord) {

    if(internal_coord < 21 || internal_coord > 98) {
        return QPoint(-1,-1);
    } else {
        int col = internal_coord % 10 - 1;
        int row = (internal_coord / 10) - 2;
        return QPoint(col,row);
    }
}

int Board::xy_to_internal(int x, int y) {

    if(x < 0 || x > 7 || y < 0 || y > 7) {
        return GENERAL_ERROR;
    } else {
        return (x+1) + ((y*10)+20);
    }
}

// assumes supplied move is correct
// otherwise might mess up the whole
// current board
QString Board::san(const Move &m) {

    QString san = QString("");
    // first check for null move
    if(m.is_null) {
        san = QString("--");
        return san;
    }

    // first test for checkmate and check (to be appended later)
    // create temp board, since appyling move and
    // testing for checkmate (which again needs
    // application of a move) makes it impossible
    // to undo (undo can only be done once, not twice in a row)

    /*
    Board* b_temp = this->copy_and_apply(m);
    bool is_check = b_temp->is_check();
    bool is_checkmate = b_temp->is_checkmate();
    delete b_temp;
    */
    Board b_temp = Board(*this);
    b_temp.apply(m);
    bool is_check = b_temp.is_check();
    bool is_checkmate = b_temp.is_checkmate();

    if(this->castles_wking(m) || this->castles_bking(m)) {
        san.append("O-O");
        if(is_checkmate) {
            san.append("#");
        }
        if(is_check) {
            san.append("+");
        }
        return san;
    } else if(this->castles_wqueen(m) || this->castles_bqueen(m)) {
        san.append("O-O-O");
        if(is_checkmate) {
            san.append("#");
        } else if(is_check) {
            san.append("+");
        }
        return san;
    } else {
        uint8_t piece_type = this->piece_type(m.from);
        QVector<Move> legals = this->legal_moves(m.to, piece_type);
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

        // find amibguous moves (except for pawns)
        if(piece_type != PAWN) {
            for(int i=0;i<legals.count();i++) {
                Move mi = legals.at(i);
                if(this->piece_type(mi.from) == piece_type && mi.to == m.to && mi.from != m.from) {
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
        // handle a capture, i.e. if destination field
        // is not empty
        // in case of an en-passent capture, the destiation field
        // is empty. But then the destination field is the e.p. square
        if(this->piece_type(m.to) != EMPTY || m.to == this->en_passent_target) {
            if(piece_type == PAWN) {
                san.append(QChar(this_col + 96));
            }
            san.append(QString("x"));
        }
        san.append(this->idx_to_str(m.to));
        if(m.promotion_piece == KNIGHT) {
            san.append(("=N"));
        }
        if(m.promotion_piece == BISHOP) {
            san.append(("=B"));
        }
        if(m.promotion_piece == ROOK) {
            san.append(("=R"));
        }
        if(m.promotion_piece == QUEEN) {
            san.append(("=Q"));
        }
    }
    if(is_checkmate) {
        san.append("#");
    } else if(is_check) {
        san.append("+");
    }
    return san;
}



int Board::get_idx_single_piece(uint8_t piece) {

    int count = 0;
    int idx = -1;
    for(int i=21;i<99;i++) {
        if(this->board[i] == piece) {
            count += 1;
            idx = i;
        }
    }
    if(count == 1) {
        return idx;
    } else {
        return -1;
    }
}

int Board::get_king_pos(uint8_t king) {
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
bool Board::is_black_king_castle_right_lost() {
    if(this->board[E8] == BLACK_KING &&
            this->board[H8] == BLACK_ROOK) {
        return false;
    } else {
        return true;
    }
}

bool Board::is_black_queen_castle_right_lost() {
    if(this->board[E8] == BLACK_KING &&
            this->board[A8] == BLACK_ROOK) {
        return false;
    } else {
        return true;
    }
}

bool Board::is_white_king_castle_right_lost() {
    if(this->board[E1] == WHITE_KING &&
            this->board[H1] == WHITE_ROOK) {
        return false;
    } else {
        return true;
    }
}

bool Board::is_white_queen_castle_right_lost() {
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
        uint8_t piece_type = this->piece_type(i);
        bool piece_color = this->piece_color(i);
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
        return false;
    }
    // each side has 8 pawns or less
    if(cnt_white_pawns > 8 || cnt_black_pawns > 8) {
        return false;
    }
    // check whether no. of promotions and pawn count fits for white
    int white_extra_pieces = std::max(0, cnt_white_queens-1) + std::max(0, cnt_white_rooks-2)
            + std::max(0, cnt_white_bishops - 2) + std::max(0, cnt_white_knights - 2);
    if(white_extra_pieces > (8-cnt_white_pawns)) {
        return false;
    }
    // ... for black
    int black_extra_pieces = std::max(0, cnt_black_queens-1) + std::max(0, cnt_black_rooks-2)
            + std::max(0, cnt_black_bishops - 2) + std::max(0, cnt_black_knights - 2);
    if(black_extra_pieces > (8-cnt_black_pawns)) {
        return false;
    }
    // compare encoded castling rights of this board w/ actual
    // position of king and rook
    if(this->can_castle_wking() && this->is_white_king_castle_right_lost()) {
        return false;
    }
    if(this->can_castle_wqueen() && this->is_white_queen_castle_right_lost()) {
        return false;
    }
    if(this->can_castle_bking() && this->is_black_king_castle_right_lost()) {
        return false;
    }
    if(this->can_castle_bqueen() && this->is_black_queen_castle_right_lost()) {
        return false;
    }
    return true;
}

QString Board::printRaw() {
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

int Board::zobrist_piece_type(uint8_t piece) {
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

quint64 Board::pos_hash() {

    if(this->m_pos_hash_initialized) {
        return this->m_pos_hash;
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
        this->m_pos_hash = piece;
        return this->m_pos_hash;
    }
}

quint64 Board::zobrist() {

    if(this->m_zobrist_initialized) {
        return this->m_zobrist;
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
                uint8_t left = this->piece_at(ep_target-11);
                uint8_t right = this->piece_at(ep_target-9);
                if(left == WHITE_PAWN || right == WHITE_PAWN) {
                    en_passent = POLYGLOT_RANDOM_64[RANDOM_EN_PASSENT + file];
                }
            } else {
                uint8_t left = this->piece_at(ep_target+11);
                uint8_t right = this->piece_at(ep_target+9);
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

        this->m_zobrist = piece^castle^en_passent^turn;
        return this->m_zobrist;
    }
}



}
