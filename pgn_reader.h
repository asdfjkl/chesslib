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


#ifndef PGN_READER_H
#define PGN_READER_H

#include <QTextStream>
#include <memory>
#include <QStack>
#include "game.h"

namespace chess {

struct HeaderOffset
{
    qint64 offset;
    QMap<QString, QString> headers;
};


struct PgnHeader
{
    QString eco;
    QString event;
    QString site;
    QString date;
    QString round;
    QString white;
    QString black;
    QString result;
};

struct PgnHeaderOffset
{
    qint64 offset;
    PgnHeader header;
};

class PgnReader
{

public:

    bool isUtf8(const QString &filename);

    QVector<qint64> scanPgn(QString &filename, bool isUtf8);
    QVector<qint64> scanPgn1(QString &filename, bool is_utf8);
    PgnHeader readSingleHeaderFromPgnAt(QString &filename, qint64 offset, bool isUtf8);

    int readGameFromString(QString &pgn_string, chess::Game *g);
    int readGameFromString(QString &pgn_string, quint64 offset, chess::Game *g);  // might be not needed

    int readGame(QTextStream& in, chess::Game* g);
    int readGame(QTextStream &in, qint64 offset, chess::Game *g);


private:

    QStack<GameNode*> m_game_stack;

    inline void addMove(GameNode *&node, Move &m);

    // these functions expect a qstring and an offset
    // where the (move) token start. they will parse
    // the token, return true, and set idx to the offset
    // after the token. If the token cannot be parsed
    // they will return false
    inline bool isCol(const QChar &c);
    inline bool isRow(const QChar &c);

    bool parsePawnMove(QString &line, int &idx, GameNode *&node);
    bool parsePieceMove(uint8_t piece_type, const QString &line, int &idx, GameNode *&node);
    bool parseCastleMove(QString &line, int &idx, GameNode *&node);

    bool createPieceMove(uint8_t piece_type, int to_col, int to_row, GameNode *&node);
    bool createPieceMove(uint8_t piece_type, int to_col, int to_row, GameNode *&node, QChar col_from);
    bool createPieceMove(uint8_t piece_type, int to_col, int to_row, GameNode *&node, int from_row);

    void parseNAG(QString &line, int &idx, GameNode *node);

    // seeks to the next token in line, and sets
    // idx to the start of the new token
    // returns the token type, i.e. one of TKN_*
    int getNetxtToken(QString &line, int &idx);

};

}

#endif // PGN_READER_H
