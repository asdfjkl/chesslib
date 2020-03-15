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

const QRegularExpression TAG_REGEX = QRegularExpression("\\[([A-Za-z0-9]+)\\s+\"(.*)\"\\]");
const QRegularExpression MOVETEXT_REGEX =
QRegularExpression("(%.*?[\n\r])|(\{.*)|(\\$[0-9]+)|(\\()|(\\))|(\\*|1-0|0-1|1/2-1/2)|([NBKRQ]?[a-h]?[1-8]?[\\-x]?[a-h][1-8](?:=?[nbrqNBRQ])?|--|O-O(?:-O)?|0-0(?:-0)?)|([\?!]{1,2})");

const int NAG_NULL = 0;

const int NAG_GOOD_MOVE = 1;
//A good move. Can also be indicated by ``!`` in PGN notation."""

const int NAG_MISTAKE = 2;
//A mistake. Can also be indicated by ``?`` in PGN notation."""

const int NAG_BRILLIANT_MOVE = 3;
//A brilliant move. Can also be indicated by ``!!`` in PGN notation."""

const int NAG_BLUNDER = 4;
//A blunder. Can also be indicated by ``??`` in PGN notation."""

const int NAG_SPECULATIVE_MOVE = 5;
//A speculative move. Can also be indicated by ``!?`` in PGN notation."""

const int NAG_DUBIOUS_MOVE = 6;
//A dubious move. Can also be indicated by ``?!`` in PGN notation."""

const int NAG_FORCED_MOVE = 7;

const int NAG_DRAWISH_POSITION = 10;

const int NAG_UNCLEAR_POSITION = 13;

const int NAG_WHITE_MODERATE_ADVANTAGE = 16;
const int NAG_BLACK_MODERATE_ADVANTAGE = 17;
const int NAG_WHITE_DECISIVE_ADVANTAGE = 18;
const int NAG_BLACK_DECISIVE_ADVANTAGE = 19;
const int NAG_WHITE_ZUGZWANG = 22;
const int NAG_BLACK_ZUGZWANG = 23;

const int NAG_WHITE_HAS_ATTACK = 40;
const int NAG_BLACK_HAS_ATTACK = 41;

const int NAG_WHITE_MODERATE_COUNTERPLAY = 132;
const int NAG_BLACK_MODERATE_COUNTERPLAY = 133;

// tokens for move parsing
const int TKN_ERROR = -1;
const int TKN_PAWN_MOVE = 1;
const int TKN_CASTLE = 2;
const int TKN_ROOK_MOVE = 3;
const int TKN_KNIGHT_MOVE = 4;
const int TKN_BISHOP_MOVE = 5;
const int TKN_QUEEN_MOVE = 6;
const int TKN_KING_MOVE = 7;
const int TKN_OPEN_VARIATION = 8;
const int TKN_CLOSE_VARIATION = 9;
const int TKN_NAG = 10;
const int TKN_OPEN_COMMENT = 11;
const int TKN_RES_WHITE_WIN = 12;
const int TKN_RES_BLACK_WIN = 13;
const int TKN_RES_DRAW = 14;
const int TKN_RES_UNDEFINED = 15;
const int TKN_CHECK = 16;
const int TKN_EOL = 17;

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

    /**
     * @brief detect_encoding tries to heuristically detect the encoding of a text file
     *                        this function is only able to distinguish UTF8 (with or
     *                        without BOM from ISO 8859-1. However this should handle
     *                        most available PGN files - modern chess programs usually
     *                        store PGNs in UTF-8.
     * @param filename        filename
     * @return                const char* = "ISO 8859-1" or "UTF-8". Can be used to
     *                        set encoding for QTextStream
     */
    const char* detect_encoding(const QString &filename);

    bool detectUtf8(const QString &filename);


    /**
     * @brief readGameFromFile read the (first) game from the PGN filename
     *                throws std::invalid_argument if impossible to read from
     *                that PGN a valid game
     * @param filename name of the file
     * @return pointer to the generated game
     */
    chess::Game* readGameFromFile(const QString &filename, const char* encoding);

    /**
     * @brief readGameFromFile read the game at supplied offset from the PGN filename
     *                throws std::invalid_argument if impossible to read from
     *                that PGN or if the offset leads to an invalid position.
     * @param filename name of the file
     * @param offset integer denoting the offset position to seek to prior reading the file
     * @return pointer to generated game
     */
    chess::Game* readGameFromFile(const QString &filename, const char* encoding, qint64 offset);

    QList<HeaderOffset> scan_headers_fast(const QString &filename, const char* encoding);

    int readNextHeader(const QString &filename, const char* encoding,
                                  quint64 offset, HeaderOffset &headerOffset);

    QVector<PgnHeaderOffset> scan_headers_foo(const QString &filename, const char* encoding);


    /**
     * @brief readFileIntoString read the PGN file from disk into memory
     *                    as a string. throws std::invalid_argument if
     *                    something goes wrong
     * @param filename name of the file
     * @return pointer to string
     */
    QString readFileIntoString(const QString &filename, const char* encoding);

    /**
     * @brief readGameFromString reads (first) PGN game from supplied pgn string
     * @param pgn_string string containing pgn file
     * @return pointer to generated game
     */
    chess::Game* readGameFromString(QString &pgn_string);

    /**
     * @brief readGameFromString read game from string, but first
     *              seeks to position in string, and starts reading
     *              the first encountered game at that position
     * @param pgn_string string with one or more pgn games
     * @param offset denoting the offset in the string to start from
     * @return generated game
     */
    chess::Game* readGameFromString(QString &pgn_string, quint64 offset);

    /**
     * @brief readGame reads a game from supplied textstream
     * @param in the textstream to read from
     * @return generated game
     */
    chess::Game* readGame(QTextStream& in);

    /**
     * @brief scan_headers scans a PGN file, reads the headers and
     *         remembers the offsets on which the games start. skips
     *         parsing the actual games.
     * @param filename name of the PGN file
     * @return list of headers and offset pairs
     */
    QVector<PgnHeaderOffset> scan_headers(const QString &filename, const char* encoding);

    /**
     * @brief scan_headersFromString scans a PGN string, reads the headers and
     *         remembers the offsets on which the games start. skips
     *         parsing the actual games.
     * @param content pointer the pgn string to read from
     * @return list of headers and offset pairs
     */
    QList<HeaderOffset> scan_headersFromString(QString &content);

    QVector<PgnHeaderOffset> readMultipleHeadersFromPgnAround(QString &filename, QVector<qint64> &offsets, bool isUtf8);

    QVector<qint64> scanPgn(QString &filename, bool isUtf8);
    chess::Game* readGameFromPgnAt(QString &filename, qint64 offset, bool isUtf8);
    PgnHeader readSingleHeaderFromPgnAt(QString &filename, qint64 offset, bool isUtf8);

    int nReadGame(QTextStream& in, chess::Game *g);
    //chess::Game* nReadGameFromFile(const QString &filename, const char* encoding, qint64 offset);
    void nReadGameFromFile(QTextStream &in, qint64 offset, chess::Game *g);


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

    bool parsePieceMove(uint8_t piece_type, const QString &line, int &idx, GameNode *&node);
    bool createPieceMove(uint8_t piece_type, int to_col, int to_row, GameNode *&node);

    bool createPieceMove(uint8_t piece_type, int to_col, int to_row, GameNode *&node, QChar col_from);
    bool createPieceMove(uint8_t piece_type, int to_col, int to_row, GameNode *&node, int from_row);

    bool parsePawnMove(QString &line, int &idx, GameNode *&node);

    bool parseCastleMove(QString &line, int &idx, GameNode *&node);

    void parseNAG(QString &line, int &idx, GameNode *node);

    // seeks to the next token in line, and sets
    // idx to the start of the new token
    // returns the token type, i.e. one of TKN_*
    int getNetxtToken(QString &line, int &idx);

};

}

#endif // PGN_READER_H
