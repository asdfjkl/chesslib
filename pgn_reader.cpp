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


#include "pgn_reader.h"
#include "game.h"
#include "game_node.h"
#include <QFile>
#include <QTextStream>
#include <iostream>
#include <QStack>
#include <QDebug>
#include <QTextCodec>
#include <QDataStream>
#include "node_pool.h"
#include "assert.h"

namespace chess {


bool PgnReader::isUtf8(const QString &filename) {
    // very simple way to detecting majority of encodings:
    // first try ISO 8859-1
    // open the file and read a max of 100 first bytes
    // if conversion to unicode works, try some more bytes (at most 40 * 100)
    // if conversion errors occur, we simply assume UTF-8
    //const char* iso = "ISO 8859-1";
    //const char* utf8 = "UTF-8";
    QFile file(filename);
    if(!file.open(QFile::ReadOnly)) {
        return true;
    }
    QDataStream in(&file);
    // init some char array to read bytes
    char first100arr[100];
    for(int i=0;i<100;i++) {
        first100arr[i] = 0x00;
    }
    char *first100 = first100arr;
    // prep conversion tools
    QTextCodec::ConverterState state;
    QTextCodec *codec = QTextCodec::codecForName("UTF-8");

    int iterations = 40;
    int i=0;
    int l = 100;
    bool isUtf8 = true;
    while(i<iterations && l>=100) {
        l = in.readRawData(first100, 100);
        const QString text = codec->toUnicode(first100, 100, &state);
        if (state.invalidChars > 0) {
            isUtf8 = false;
            break;
        }
        i++;
    }
    return isUtf8;
}



int PgnReader::readGameFromString(QString &pgn_string, chess::Game *g) {
    QTextStream in(&pgn_string);
    return this->readGame(in, g);
}

int PgnReader::readGameFromString(QString &pgn_string, quint64 offset, chess::Game *g) {
    QString substring = QString(pgn_string.mid(offset, pgn_string.size()));
    QTextStream in(&substring);
    return this->readGame(in, g);
}

QVector<qint64> PgnReader::scanPgn(QString &filename, bool is_utf8) {

    QVector<qint64> offsets;
    QFile file(filename);

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return offsets;

    bool inComment = false;

    qint64 game_pos = -1;

    QByteArray byteLine;
    QString line("");
    qint64 last_pos = file.pos();

    int i= 0;
    while(!file.atEnd()) {
        i++;
        byteLine = file.readLine();
        if(!is_utf8) {
            line = QString::fromLatin1(byteLine);
        } else {
            line = QString::fromUtf8(byteLine);
        }

        // skip comments
        if(line.startsWith("%")) {
            continue;
        }

        if(!inComment && line.startsWith("[")) { // from Latin1

            if(game_pos == -1) {
                game_pos = last_pos;
            }
            last_pos = file.pos();
            continue;
        }
        if((!inComment && line.contains("{"))
                || (inComment && line.contains("}"))) {
            inComment = line.lastIndexOf("{") > line.lastIndexOf("}");
        }

        if(game_pos != -1) {
            offsets.append(game_pos);
            game_pos = -1;
        }

        last_pos = file.pos();
    }
    // for the last game
    if(game_pos != -1) {
        offsets.append(game_pos);
        game_pos = -1;
    }

    return offsets;
}



PgnHeader PgnReader::readSingleHeaderFromPgnAt(QString &filename, qint64 offset, bool isUtf8) {

    QFile file(filename);

    if(!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        throw std::invalid_argument("unable to open file w/ supplied filename");
    }
    QTextStream in(&file);
    QTextCodec *codec;
    if(isUtf8) {
        codec = QTextCodec::codecForName("UTF-8");
    } else {
        codec = QTextCodec::codecForName("ISO 8859-1");
    }
    in.setCodec(codec);
    if(offset != 0 && offset > 0) {
        in.seek(offset);
    }

    PgnHeader header;
    bool foundHeader = false;
    bool continueSearch = true;

    QString line = in.readLine();
    while(!in.atEnd() && continueSearch) {
        line = in.readLine();
        if(line.startsWith("%") || line.isEmpty()) {
            line = in.readLine();
            continue;
        }

        QRegularExpressionMatch match_t = TAG_REGEX.match(line);

        if(match_t.hasMatch()) {

            foundHeader = true;

            QString tag = match_t.captured(1);
            QString value = match_t.captured(2);

            if(tag == "Event") {
                header.event = value;
            }
            if(tag == "Site") {
                header.site = value;
            }
            if(tag == "Date") {
                header.date = value;
            }
            if(tag == "Round") {
                header.round = value;
            }
            if(tag == "White") {
                header.white = value;
            }
            if(tag == "Black") {
                header.black = value;
            }
            if(tag == "Result") {
                header.result = value;
            }
            if(tag == "ECO") {
                header.eco = value;
            }
        } else {
            if(foundHeader) {
                continueSearch = false;
                break;
            }
        }
    }

    file.close();
    return header;
}


bool PgnReader::isCol(const QChar &c) {
    if(c >= QChar::fromLatin1('a') && c <= QChar::fromLatin1('h')) {
        return true;
    } else {
        return false;
    }
}

bool PgnReader::isRow(const QChar &c) {
    int row = c.digitValue() - 1;
    if(row >= 0 && row <= 7) {
        return true;
    } else {
        return false;
    }
}

void PgnReader::addMove(GameNode *&node, Move &m) {

    GameNode *next = NodePool::makeNode();
    Board* board = node->getBoard();

    Board b_next = Board(*board);
    b_next.apply(m);
    next->setMove(m);
    next->setBoard(b_next);
    next->setParent(node);
    node->addVariation(next);
    node = next;
}



bool PgnReader::parsePawnMove(QString &line, int &idx, GameNode *&node) {

    int col = Board::alpha_to_pos(line.at(idx));
    Board* board = node->getBoard();
    if(idx+1 < line.size()) {
        if(line.at(idx+1) == QChar::fromLatin1('x')) {
            // after x, next one must be letter denoting column
            // and then digit representing row, like exd4 (white)
            // then parse d, 4, and check wether there is a pawn
            // on e(4-1) = e3
            if(idx+3 < line.size()) {
                if(this->isCol(line.at(idx+2)) && this->isRow(line.at(idx+3))) {
                    int col_to = Board::alpha_to_pos(line.at(idx+2));
                    int row_to = line.at(idx+3).digitValue() - 1;
                    int row_from = -1;

                    if(board->turn == WHITE && row_to - 1 >= 0 &&
                            board->get_piece_at(col, row_to - 1) == WHITE_PAWN) {
                        row_from = row_to - 1;
                    } else if(board->turn == BLACK && row_to + 1 <= 7 &&
                              board->get_piece_at(col, row_to + 1) == BLACK_PAWN) {
                        row_from = row_to + 1;
                    }
                    if(row_from >= 0 && row_from <= 7) {
                        // check wether this is a promotion, i.e. exd8=Q
                        if(idx+5 < line.size() && line.at(idx+4) == QChar::fromLatin1('=') &&
                                (line.at(idx+5) == QChar::fromLatin1('R') ||
                                 line.at(idx+5) == QChar::fromLatin1('B') ||
                                 line.at(idx+5) == QChar::fromLatin1('N') ||
                                 line.at(idx+5) == QChar::fromLatin1('Q'))) {
                            Move m = Move(col, row_from, col_to, row_to, line.at(idx+5));
                            this->addMove(node, m);
                            idx += 6;
                            return true;
                        } else { // just a normal move, like exd4
                            Move m = Move(col, row_from, col_to, row_to);
                            this->addMove(node, m);
                            idx += 4;
                            return true;
                        }
                    } else {
                        idx += 4;
                        return false;
                    }
                } else {
                   idx += 4;
                   return false;
                }
            } else {
                idx += 2;
                return false;
            }
        } else { // only other case: must be a number
            if(this->isRow(line.at(idx+1))) {
                int row = line.at(idx+1).digitValue() - 1;
                int from_row = -1;
                if(board->turn == WHITE) {
                    for(int row_i = row - 1;row_i>= 1;row_i--) {
                        if(board->get_piece_at(col,row_i) == WHITE_PAWN) {
                            from_row = row_i;
                            break;
                        }
                    }
                } else {
                    for(int row_i = row + 1;row_i<= 7;row_i++) {
                        if(board->get_piece_at(col,row_i) == BLACK_PAWN) {
                            from_row = row_i;
                            break;
                        }
                    }
                }
                if(from_row >= 0) { // means we found a from square
                    // check wether this is a promotion
                    if(idx+3 < line.size() && line.at(idx+2) == QChar::fromLatin1('=') &&
                            (line.at(idx+3) == QChar::fromLatin1('R') ||
                             line.at(idx+3) == QChar::fromLatin1('B') ||
                             line.at(idx+3) == QChar::fromLatin1('N') ||
                             line.at(idx+3) == QChar::fromLatin1('Q'))) {
                        Move m = Move(col, from_row, col, row, line.at(idx+3));
                        this->addMove(node, m);
                        idx += 4;
                        return true;
                    } else { // not a promotion, just a standard pawn move
                        Move m = Move(col, from_row, col, row);
                        this->addMove(node, m);
                        idx += 2;
                        return true;
                    }
                } else {
                    idx+=2;
                    return false;
                }
            } else {
                idx+=2;
                return false;
            }
        }
    }
    idx += 2;
    return true;
}


bool PgnReader::createPieceMove(uint8_t piece_type, int to_col, int to_row, GameNode *&node) {

    Board *board = node->getBoard();
    int to_internal = Board::xy_to_internal(to_col, to_row);
    //QVector<Move> pseudos = board->pseudo_legal_moves_to(to_internal, piece_type, false, board->turn);
    QVector<Move> pseudos = board->pseudo_legal_moves(chess::ANY_SQUARE, to_internal, piece_type, false, board->turn);
    if(pseudos.size() == 1) {
        Move m = pseudos.at(0);
        this->addMove(node,m);
        return true;
    } else {
        QVector<Move> legals = board->legals_from_pseudos(pseudos);
        if(legals.size() == 1) {
            Move m = legals.at(0);
            this->addMove(node,m);
            return true;
        } else {
            return false;
        }
    }

}

bool PgnReader::createPieceMove(uint8_t piece_type, int to_col, int to_row, GameNode *&node, QChar qc_from_col) {

    Board *board = node->getBoard();
    int from_col = Board::alpha_to_pos(qc_from_col);
    int to_internal = Board::xy_to_internal(to_col, to_row);
    //QVector<Move> pseudos = board->pseudo_legal_moves_to(to_internal, piece_type, false, board->turn);
    QVector<Move> pseudos = board->pseudo_legal_moves(chess::ANY_SQUARE, to_internal, piece_type, false, board->turn);
    QVector<Move> filter;
    for(int i=0;i<pseudos.size();i++) {
        Move m = pseudos.at(i);
        if((m.from % 10) - 1 == from_col) {
            filter.append(m);
        }
    }
    if(filter.size() == 1) {
        Move m = filter.at(0);
        this->addMove(node,m);
        return true;
    } else {
        QVector<Move> legals = board->legals_from_pseudos(pseudos);
        if(legals.size() == 1) {
            Move m = legals.at(0);
            this->addMove(node,m);
            return true;
        } else {
            return false;
        }
    }
}

bool PgnReader::createPieceMove(uint8_t piece_type, int to_col, int to_row, GameNode *&node, int from_row) {

    Board *board = node->getBoard();
    int to_internal = Board::xy_to_internal(to_col, to_row);
    //QVector<Move> pseudos = board->pseudo_legal_moves_to(to_internal, piece_type, false, board->turn);
    QVector<Move> pseudos = board->pseudo_legal_moves(chess::ANY_SQUARE, to_internal, piece_type, false, board->turn);
    QVector<Move> filter;    
    for(int i=0;i<pseudos.size();i++) {
        Move m = pseudos.at(i);
        if((m.from / 10) - 2 == from_row) {
            filter.append(m);
                    qDebug() << m.uci();
        }
    }
    if(filter.size() == 1) {
        Move m = filter.at(0);
        this->addMove(node,m);
        return true;
    } else {
        QVector<Move> legals = board->legals_from_pseudos(pseudos);
        if(legals.size() == 1) {
            Move m = legals.at(0);
            this->addMove(node,m);
            return true;
        } else {
            return false;
        }
    }
}


bool PgnReader::parsePieceMove(uint8_t piece_type, const QString &line, int &idx, GameNode *&node) {

    //qDebug() << "piece move creation 0";
    if(idx + 4 < line.size()) {
        qDebug() << "parsepiece" << line.mid(idx,5);
    }

    // we have a piece move like "Qxe4" where index points to Q
    // First move idx after piece symbol, i.e. to ">x<e4"
    idx+=1;
    if(idx < line.size() && line.at(idx) == QChar::fromLatin1('x')) {
        idx+=1;
    }
    if(idx < line.size()) {
        if(this->isCol(line.at(idx))) {
            //Qe? or Qxe?, now either digit must follow (Qe4 / Qxe4)
            //or we have a disambiguition (Qee5)
            //qDebug() << "1111111111";
            if(idx+1 < line.size()) {
                if(this->isRow(line.at(idx+1))) {
                    int to_col = Board::alpha_to_pos(line.at(idx));
                    int to_row = line.at(idx+1).digitValue() - 1;
                    idx+=2;
                    // standard move, i.e. Qe4
                    //qDebug() << "piece move creation 1";
                    return createPieceMove(piece_type, to_col, to_row, node);
                } else {
                    int skip_for_take = 0;
                    if(line.at(idx+1) == 'x' && idx + 2 < line.length()) {
                        //System.out.println("fix");
                        skip_for_take = 1;
                        idx+=1;
                    }
                    //qDebug() << "22222222";
                    if(this->isCol(line.at(idx+1))) {
                        //qDebug() << "22222222aaaaaa";
                        // we have a disambiguition, that should resolved by
                        // the column denoted in the san, here in @line[idx]
                        int to_col = Board::alpha_to_pos(line.at(idx+1));
                        if(idx+2 < line.size() && this->isRow(line.at(idx+2))) {
                            int to_row = line.at(idx+2).digitValue() - 1;
                            // move w/ disambig on col, i.e. Qee4
                            // provide line[idx] to cratePieceMove to resolve disamb.
                            idx+=3;
                            //qDebug() << "piece move creation 2";
                            return createPieceMove(piece_type, to_col, to_row, node, line.at(idx-(3+skip_for_take)));
                        } else {
                            idx+=4;
                            return false;
                        }
                    } else {
                        idx+=3;
                        return false;
                    }
                }
           } else {
               idx+=2;
               return false;
           }
       } else {
            qDebug() << "33333333";
            if(idx+1 < line.size() && this->isRow(line.at(idx))) {
                // we have a move with disamb, e.g. Q4xe5 or Q4e5
                int from_row = line.at(idx).digitValue() - 1;
                if(line.at(idx+1) == QChar::fromLatin1('x')) {
                    idx+=1;
                }
                if(idx+2 < line.size() && this->isCol(line.at(idx+1)) && this->isRow(line.at(idx+2))) {
                    int to_col = Board::alpha_to_pos(line.at(idx+1));
                    int to_row = line.at(idx+2).digitValue() - 1;
                    // parse the ambig move
                    idx+=3;
                    qDebug() << "creating MOVE";
                    bool ok = createPieceMove(piece_type, to_col, to_row, node, from_row);
                    std::cout << *node->getBoard() << std::endl;;
                    qDebug() << "success: " << ok;
                    return ok;
                } else {
                    idx+=3;
                    return false;
                }
            } else {
                idx+=2;
                return false;
            }
       }
    } else {
        idx+=2;
        return false;
    }
    idx += 2;
    return false;
}

bool PgnReader::parseCastleMove(QString &line, int &idx, GameNode *&node) {

    int lineSize = line.size();
    if(idx+4 < lineSize && ( line.mid(idx,5) == QString::fromLatin1("O-O-O") || line.mid(idx,5) == QString::fromLatin1("0-0-0")) ) {
        if(node->getBoard()->turn == WHITE) {
            Move m = Move(E1,C1);
            this->addMove(node,m);
            idx += 5;
            return true;
        } else {
            Move m = Move(E8,C8);
            this->addMove(node,m);
            idx += 5;
            return true;
        }
    }
    if(idx+2 < lineSize && ( line.mid(idx,3) == QString::fromLatin1("O-O"))) {  // || line.mid(idx,3) == QString::fromLatin1("0-0"))) {
        if(node->getBoard()->turn == WHITE) {
            Move m = Move(E1,G1);
            this->addMove(node,m);
            idx += 3;
            return true;
        } else {
            Move m = Move(E8,G8);
            this->addMove(node,m);
            idx += 3;
            return true;
        }
    }
    idx+=1;
    return false;
}

void PgnReader::parseNAG(QString &line, int &idx, GameNode *node) {

    int lineSize = line.size();

    if(line[idx] == QChar::fromLatin1('$')) {
        int idx_end = idx;
        while(idx_end < lineSize && (line.at(idx_end) == QChar::fromLatin1('$')
                                     || (line.at(idx_end) >= QChar::fromLatin1('0')
                                         && line.at(idx_end) <= QChar::fromLatin1('9'))))
        {
            idx_end++;
        }
        if(idx_end+1 > idx) {
            bool ok;
            qDebug() << line.mid(idx+1, idx_end - (idx+1));
            int nr = line.mid(idx+1, idx_end - (idx+1)).toInt(&ok, 10);
            if(ok) {
                node->addNag(nr);
                idx = idx_end;
                qDebug() << "after NAG: " << line.mid(idx, 5);
            } else {
                idx += 1;
            }
        } else {
            idx += 1;
        }
        return;
    }
    if(line[idx+1] < lineSize && line.midRef(idx,2) == QString::fromLatin1("??")) {
        node->addNag(NAG_BLUNDER);
        idx += 3;
        return;
    }
    if(line[idx+1] < lineSize && line.midRef(idx,2) == QString::fromLatin1("!!")) {
        node->addNag(NAG_BRILLIANT_MOVE);
        idx += 3;
        return;
    }
    if(line[idx+1] < lineSize && line.midRef(idx,2) == QString::fromLatin1("!?")) {
        node->addNag(NAG_SPECULATIVE_MOVE);
        idx += 3;
        return;
    }
    if(line[idx+1] < lineSize && line.midRef(idx,2) == QString::fromLatin1("?!")) {
        node->addNag(NAG_DUBIOUS_MOVE);
        idx += 3;
        return;
    }
    if(line[idx] == QChar::fromLatin1('?')) {
        node->addNag(NAG_MISTAKE);
        idx += 2;
        return;
    }
    if(line[idx] == QChar::fromLatin1('!')) {
        node->addNag(NAG_GOOD_MOVE);
        idx += 2;
        return;
    }
}



int PgnReader::getNetxtToken(QString &line, int &idx) {

    int lineSize = line.size();
    while(idx < lineSize) {
        QChar ci = line.at(idx);
        if(ci == QChar::fromLatin1(' ') || ci == QChar::fromLatin1('.')) {
            idx += 1;
            continue;
        }
        if(ci >= QChar::fromLatin1('0') && ci <= QChar::fromLatin1('9')) {
            if(ci == QChar::fromLatin1('1')) {
                if(idx+1 < lineSize) {
                    if(line.at(idx+1) == QChar::fromLatin1('-')) {
                        if(idx+2 < lineSize && line.at(idx+2) == QChar::fromLatin1('0')) {
                            return TKN_RES_WHITE_WIN;
                        }
                    }
                    if(idx+2 < lineSize && line.at(idx+1) == QChar::fromLatin1(('/'))) {
                        if(idx+6 < lineSize && line.mid(idx,7) == QString::fromLatin1("1/2-1/2")) {
                            return TKN_RES_DRAW;
                        }
                    }
                }
            }
            // irregular castling like 0-0 or 0-0-0
            if(ci == QChar::fromLatin1('0')) {
                if(idx+1 < lineSize && line.at(idx+1) == QChar::fromLatin1('-')) {
                    if(idx+2 < lineSize && line.at(idx+2) == QChar::fromLatin1('1')) {
                        return TKN_RES_BLACK_WIN;
                    } else {
                        if(idx+2 < lineSize && line.at(idx+2) == QChar::fromLatin1('0')) {
                            return TKN_CASTLE;
                        }
                    }
                }
            }
            // none of the above -> move number, just continue
            idx += 1;
            continue;
        }
        if(ci >= QChar::fromLatin1('a') && ci <= QChar::fromLatin1('h')) {
            return TKN_PAWN_MOVE;
        }
        if(ci == QChar::fromLatin1('O')) {
            return TKN_CASTLE;
        }
        if(ci == QChar::fromLatin1('R')) {
            return TKN_ROOK_MOVE;
        }
        if(ci == QChar::fromLatin1('N')) {
            return TKN_KNIGHT_MOVE;
        }
        if(ci == QChar::fromLatin1('B')) {
            return TKN_BISHOP_MOVE;
        }
        if(ci == QChar::fromLatin1('Q')) {
            return TKN_QUEEN_MOVE;
        }
        if(ci == QChar::fromLatin1('K')) {
            return TKN_KING_MOVE;
        }
        if(ci == QChar::fromLatin1('+')) {
            return TKN_CHECK;
        }
        if(ci == QChar::fromLatin1('(')) {
            return TKN_OPEN_VARIATION;
        }
        if(ci == QChar::fromLatin1(')')) {
            return TKN_CLOSE_VARIATION;
        }
        if(ci == QChar::fromLatin1('$') || ci == QChar::fromLatin1('!') || ci == QChar::fromLatin1('?')) {
            return TKN_NAG;
        }
        if(ci == QChar::fromLatin1('{')) {
            return TKN_OPEN_COMMENT;
        }
        if(ci == QChar::fromLatin1('*')) {
            return TKN_RES_UNDEFINED;
        }
        if(ci == QChar::fromLatin1('-')) {
            if(idx + 1 < lineSize && line.at(idx+1) == QChar::fromLatin1('-')) {
                return TKN_NULL_MOVE;
            }
        }
        // if none of the above match, try to continue until we
        // find another usable token
        idx += 1;
    }
    return TKN_EOL;
}

int PgnReader::readGame(QTextStream& in, chess::Game* g) {



    QString starting_fen = QString("");

    m_game_stack.clear();
    m_game_stack.push(g->getRootNode());
    GameNode* current = g->getRootNode();

    QString line = in.readLine();

    while (!in.atEnd()) {
        if(line.startsWith(QChar::fromLatin1('%')) || line.isEmpty()) {
            if(in.readLineInto(&line)) {;
                continue;
            } else {
                std::cerr << "error reading pgn file";
                return 0;
            }
        }

        if(line.startsWith(QChar::fromLatin1('['))) {
            QRegularExpressionMatch match_t = TAG_REGEX.match(line);

            if(match_t.hasMatch()) {
                QString tag = match_t.captured(1);
                QString value = match_t.captured(2);
                // don't add FEN tag explicitly,
                // will be always automatically generated and added
                // when printing a game later...
                if(tag == QString::fromLatin1("FEN")) {
                    starting_fen = value;
                } else {
                    g->setHeader(tag, value);
                }
            } else {
               break;
            }
        } else {
            break;
        }

        if(in.readLineInto(&line)) {;
            continue;
        } else {
            std::cerr << "error reading pgn file";
            return -1;
        }
    }
    // try to set starting fen, if available
    if(!starting_fen.isEmpty()) {
        try {
            chess::Board b_fen(starting_fen);
            if(!b_fen.is_consistent()) {
                std::cerr << "starting fen position is not consistent" << std::endl;
                m_game_stack.clear();
                return -1;
            } else {
                current->setBoard(b_fen);
            }
        }
        catch(std::invalid_argument a) {
            std::cerr << a.what() << std::endl;
            m_game_stack.clear();
            return -1;
        }
    }
    // we should now have a header, seek first non-empty line
    while(line.trimmed() == QString("") && !line.isEmpty()) {
        if(in.readLineInto(&line)) {;
            continue;
        } else {
            std::cerr << "error reading pgn file";
            return -1;
        }
    }

    bool firstLine = true;

    while (!in.atEnd() && !line.isEmpty()) {
        if(line.trimmed().isEmpty()) {
            return 0;
        }
        // if we are at the first line after skipping
        // all the empty ones, don't read another line
        // otherwise, call readLineInto
        bool lineReadOk = true;
        if(!firstLine) {
            lineReadOk = in.readLineInto(&line);
        } else {
            firstLine = false;
        }
        if(lineReadOk) {
            if(line.startsWith(QString::fromLatin1("%"))) {
                continue;
            }
            int idx = 0;
            while(idx < line.size()) {
                if(idx + 6 << line.size()) {
                    qDebug() << line.mid(idx, 5);
                    //assert(current != nullptr);
                    //qDebug() << "null: "<< +(current == nullptr);
                    //Board temp = *current->getBoard();
                    //qDebug() << "got temp board";
                    //qDebug() << "temp is null: "<< +(temp == nullptr);
                    //std::cout << *current->getBoard() << std::endl;
                }

                int tkn = getNetxtToken(line,idx);
                if(tkn == TKN_EOL) {
                    break;
                }
                if(tkn == TKN_RES_WHITE_WIN) {
                    // 1-0
                    g->setResult(RES_WHITE_WINS);
                    idx += 4;
                }
                if(tkn == TKN_RES_BLACK_WIN) {
                    // 0-1
                    g->setResult(RES_BLACK_WINS);
                    idx += 4;
                }
                if(tkn == TKN_RES_UNDEFINED) {
                    // *
                    g->setResult(RES_UNDEF);
                    idx += 2;
                }
                if(tkn == TKN_RES_DRAW) {
                    // 1/2-1/2
                    g->setResult(RES_DRAW);
                    idx += 8;
                }
                if(tkn == TKN_PAWN_MOVE) {
                    parsePawnMove(line,idx,current);
                }
                if(tkn == TKN_CASTLE) {
                    parseCastleMove(line,idx,current);
                }
                if(tkn == TKN_ROOK_MOVE) {
                    if(idx + 3 << line.size()) {
                        qDebug() << "ROOK: " << line.mid(idx, 4);
                    }
                    parsePieceMove(ROOK,line,idx,current);
                }
                if(tkn == TKN_KNIGHT_MOVE) {
                    //if(idx + 3 << line.size()) {
                    //    qDebug() << "KNIGHT: " << line.mid(idx, 4);
                    //}
                    parsePieceMove(KNIGHT,line,idx,current);
                    //std::cout << "after night move: ";
                    //std::cout << std::endl << *current->getBoard() << std::endl;
                }
                if(tkn == TKN_BISHOP_MOVE) {
                    //if(idx + 3 << line.size()) {
                    //    qDebug() << "BISHOP: " << line.mid(idx, 4);
                    //}
                    parsePieceMove(BISHOP,line,idx,current);
                }
                if(tkn == TKN_QUEEN_MOVE) {
                    parsePieceMove(QUEEN,line,idx,current);
                }
                if(tkn == TKN_KING_MOVE) {
                    parsePieceMove(KING,line,idx,current);
                }
                if(tkn == TKN_CHECK) {
                    idx+=1;
                }
                if(tkn == TKN_NULL_MOVE) {
                    Move m = Move();
                    addMove(current, m);
                    idx+=2;
                }
                if(tkn == TKN_OPEN_VARIATION) {
                    // put current node on stack, so that we don't forget it.
                    // however if we are at the root node, something
                    // is wrong in the PGN. Silently ignore "(" then
                    //if(idx + 3 << line.size()) {
                    //    qDebug() << "VARIATION: " << line.mid(idx, 4);
                    //}
                    if(current != g->getRootNode()) {
                        m_game_stack.push(current);
                        current = current->getParent();
                    }
                    idx+=1;
                }
                if(tkn == TKN_CLOSE_VARIATION) {
                    // pop from stack. but always leave root
                    if(m_game_stack.size() > 1) {
                        current = m_game_stack.pop();
                    }
                    idx+=1;
                }
                if(tkn == TKN_NAG) {
                    //if(idx + 3 << line.size()) {
                    //    qDebug() << "NAG" << line.mid(idx, 3);
                    //}
                    parseNAG(line, idx, current);
                }
                if(tkn == TKN_OPEN_COMMENT) {
                    QStringRef rest_of_line = line.midRef(idx+1, line.size()-(idx+1));
                    int end = rest_of_line.indexOf(QString::fromLatin1("}"));
                    if(end >= 0) {
                        QString comment_line = line.mid(idx+1, end);
                        current->setComment(comment_line);
                        idx = idx+end+1;
                    } else {
                        // get comment over multiple lines
                        QStringList comment_lines;
                        QString comment_line = line.mid(idx+1, line.size() - (idx+1));
                        comment_lines.append(comment_line);
                        // we already have the comment part of the current line,
                        // so read-in the next line, and then loop until we find
                        // the end marker "}"
                        bool readOK = in.readLineInto(&line);
                        if(!readOK) {
                            QString comment_joined = comment_lines.join(QString::fromLatin1("\n"));
                            current->setComment(comment_joined);
                            continue;
                        }
                        while(!line.isEmpty() && !line.contains(QString::fromLatin1("}"))) {
                            comment_lines.append(line.trimmed());
                            bool readOK = in.readLineInto(&line);
                            if(!readOK) {
                                break;
                            }
                        }
                        int end_index = line.indexOf(QString::fromLatin1("}"));
                        if(end_index >= 0) {
                            QString comment_line = line.mid(0, end_index);
                            comment_lines.append(comment_line);
                            QString comment_joined = comment_lines.join(QString::fromLatin1("\n"));
                            current->setComment(comment_joined);
                            idx = end_index+1;
                         } else {
                            QString comment_joined = comment_lines.join(QString::fromLatin1("\n"));
                            current->setComment(comment_joined);
                            continue;
                        }

                    }
                }
            }

        } else {
            std::cerr << "error reading pgn file";
            return -1;
        }
    }
    return 0;
}

int PgnReader::readGame(QTextStream &in, qint64 offset, chess::Game *g) {


    if(offset != 0 && offset > 0) {
        in.seek(offset);
    }
    return this->readGame(in, g);
}





}
