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


#include "move.h"
#include <assert.h>
#include <iostream>
#include <QDebug>

namespace chess {

Move::Move(uint8_t from, uint8_t to) {

    // create ascii (latin1) code numbers from
    // uint8_t board pos numbers
    QChar col_from = QChar((from % 10) + 96);
    QChar row_from = QChar((from / 10) + 47);

    QChar col_to = QChar((to % 10) + 96);
    QChar row_to = QChar((to / 10) + 47);

    this->from = from;
    this->to = to;
    this->promotion_piece = 0;
    //this->uci_string = QString(col_from) + row_from + col_to + row_to;
    this->is_null = false;
}

Move::Move(int from_col, int from_row, int to_col, int to_row) {

    // convert to mailbox index
    this->from = ((from_row+2)*10) + (from_col+1);
    this->to = ((to_row+2)*10) + (to_col+1);

    // create ascii (latin1) code numbers from
    // uint8_t board pos numbers
    QChar c_col_from = QChar::fromLatin1(from_col + 97);
    QChar c_row_from = QChar::fromLatin1(from_row + 49);
    QChar c_col_to = QChar::fromLatin1(to_col + 97);
    QChar c_row_to = QChar::fromLatin1(to_row + 49);

    this->promotion_piece = 0;
    //this->uci_string = QString(c_col_from) + c_row_from + c_col_to + c_row_to;
    this->is_null = false;
}

/*
 * creates a null move
 */
Move::Move() {
    this->from = 0x00;
    this->to = 0x00;
    this->promotion_piece = 0;
    //this->uci_string = "0000";
    this->is_null = true;
}

Move::Move(uint8_t from, uint8_t to, uint8_t promotion_piece) {
    QChar col_from = QChar((from % 10) + 96);
    QChar row_from = QChar((from / 10) + 47);

    QChar col_to = QChar((to % 10) + 96);
    QChar row_to = QChar((to / 10) + 47);

    QChar prom_piece;
    if(promotion_piece == KNIGHT) {
        prom_piece = QChar('N');
    } else if (promotion_piece == BISHOP) {
        prom_piece = QChar('B');
    } else if (promotion_piece == ROOK) {
        prom_piece = QChar('R');
    } else if (promotion_piece == QUEEN) {
        prom_piece = QChar('Q');
    } else {
        prom_piece = QChar(' ');
    }
    this->from = from;
    this->to = to;
    this->promotion_piece = promotion_piece;
    if(prom_piece == QChar(' ')) {
        //this->uci_string = QString(col_from) + row_from + col_to + row_to;
    } else {
        //this->uci_string = QString(col_from) + row_from + col_to + row_to + prom_piece;
    }
    this->is_null = false;
}

Move::Move(int from_col, int from_row, int to_col, int to_row, QChar cProm) {

    // convert to mailbox index
    this->from = ((from_row+2)*10) + (from_col+1);
    this->to = ((to_row+2)*10) + (to_col+1);

    // create ascii (latin1) code numbers from
    // uint8_t board pos numbers
    QChar c_col_from = QChar::fromLatin1(from_col + 97);
    QChar c_row_from = QChar::fromLatin1(from_row + 49);
    QChar c_col_to = QChar::fromLatin1(to_col + 97);
    QChar c_row_to = QChar::fromLatin1(to_row + 49);

    this->promotion_piece = 0;
    if(cProm == QChar::fromLatin1('N')) {
        this->promotion_piece = KNIGHT;
    } else if (cProm == QChar::fromLatin1('B')) {
        this->promotion_piece = BISHOP;
    } else if (cProm == QChar::fromLatin1('R')) {
        this->promotion_piece = ROOK;
    } else if (cProm == QChar::fromLatin1('Q')) {
        this->promotion_piece = QUEEN;
    }
    //this->uci_string = QString(c_col_from) + c_row_from + c_col_to + c_row_to + cProm;
    this->is_null = false;
}

Move::Move(QString uci) {
    assert((uci.size()==4) || (uci.size()==5));
    //this->uci_string = uci;
    QString up = uci.toUpper();
    uint8_t from_col = (uint8_t) this->alpha_to_pos(up.at(0));
    // -49 for ascii(1) -> int 0, *10 + 20 is to get board coord
    uint8_t from_row = (uint8_t) ((up.at(1).toLatin1()-49) * 10)+20;
    this->from = from_row + from_col;
    uint8_t to_col = (uint8_t) this->alpha_to_pos(up.at(2));
    uint8_t to_row = (uint8_t) ((up.at(3).toLatin1() -49) * 10) + 20;
    this->to = to_row + to_col;
    if(uci.size() == 5) {
        QChar piece = up.at(4);
        assert(piece == QChar('N') || piece == QChar('B') ||
               piece == QChar('R') || piece == QChar('Q'));
        if(piece == QChar('N')) {
            this->promotion_piece = KNIGHT;
        }
        if(piece == QChar('B')) {
            this->promotion_piece = BISHOP;
        }
        if(piece == QChar('R')) {
            this->promotion_piece = ROOK;
        }
        if(piece == QChar('Q')) {
            this->promotion_piece = QUEEN;
        }
    } else {
        this->promotion_piece = 0;
    }
    this->is_null = false;
}

QString Move::uci() {
    if(this->is_null) {
        return "0000";
    } else {
        //return this->uci_string;
        QChar col_from = QChar((this->from % 10) + 96);
        QChar row_from = QChar((this->from / 10) + 47);

        QChar col_to = QChar((this->to % 10) + 96);
        QChar row_to = QChar((this->to / 10) + 47);

        QString uci = QString(col_from) + row_from + col_to + row_to;
        if(this->promotion_piece==BISHOP) {
            uci.append("=B");
        } else if(this->promotion_piece==KNIGHT) {
            uci.append("=K");
        } else if(this->promotion_piece==ROOK) {
            uci.append("=R");
        } else if(this->promotion_piece==QUEEN) {
            uci.append("=Q");
        } else {
            return uci;
        }
        return uci;
    }
}

QPoint Move::fromAsXY() {
    int col_from = (this->from % 10) - 1;
    int row_from = (this->from / 10) - 2;
    return QPoint(col_from, row_from);
}

QPoint Move::toAsXY() {
    int col_to = (this->to % 10) - 1;
    int row_to = (this->to / 10) - 2;
    return QPoint(col_to, row_to);
}

// create deep copy
Move::Move(const Move& m) {
    this->from = m.from;
    this->to = m.to;
    this->promotion_piece = m.promotion_piece;
    this->is_null = m.is_null;
    //this->uci_string = this->uci(); // QString::fromUtf16(m.uci_string.utf16());
}


uint8_t Move::alpha_to_pos(QChar alpha) {
    if(alpha == QChar('A')) {
        return 1;
    } else if(alpha == QChar('B')) {
        return 2;
    } else if(alpha == QChar('C')) {
        return 3;
    } else if(alpha == QChar('D')) {
        return 4;
    } else if(alpha == QChar('E')) {
        return 5;
    } else if(alpha == QChar('F')) {
        return 6;
    } else if(alpha == QChar('G')) {
        return 7;
    } else if(alpha == QChar('H')) {
        return 8;
    }

    return 0;
}

bool Move::operator==(const Move &other) const {
    return (this->from == other.from
            && this->to == other.to
            && this->promotion_piece == other.promotion_piece
            && this->is_null == other.is_null);
}

bool Move::operator!=(const Move &other) const {
  return !(*this == other);
}

/**
 * @brief operator <<
 * @param strm
 * @param m
 * @return
 *
 * prints move as uci string
 *
 * Example:
 * Move m = Move("g1f3");
 * std::cout << m << std::endl;
 */
std::ostream& operator<<(std::ostream &strm, const Move &m) {

    //return strm << m.uci_string.toStdString();
    return strm << QString("foo").toStdString();

}

}
