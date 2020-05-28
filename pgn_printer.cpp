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


#include <QStringList>
#include <QHashIterator>
#include <iostream>
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include "game.h"
#include "pgn_printer.h"

namespace chess {

PgnPrinter::PgnPrinter() {
    this->pgn = QStringList();
    this->currentLine = QString("");
    this->variationDepth = 0;
    this->forceMoveNumber = true;
}

void PgnPrinter::reset() {
    this->pgn.clear();
    this->currentLine = QString("");
    this->variationDepth = 0;
    this->forceMoveNumber = true;
}

void PgnPrinter::flushCurrentLine() {
    if(!this->currentLine.isEmpty()) {
        QString line_copy = QString(this->currentLine);
        this->pgn.append(line_copy.trimmed());
    }
    this->currentLine = QString("");
}

void PgnPrinter::writeToken(const QString &token) {
    if(80 - this->currentLine.length() < token.length()) {
        this->flushCurrentLine();
    }
    this->currentLine.append(token);
}

void PgnPrinter::writeLine(const QString &line) {
    this->flushCurrentLine();
    this->pgn.append(line.trimmed());
}

void PgnPrinter::writeGame(Game &g, const QString &filename) {

    QStringList pgn = this->printGame(g);
    QFile fOut(filename);
    bool success = false;
    if(fOut.open(QFile::WriteOnly | QFile::Text)) {
      QTextStream s(&fOut);
      for (int i = 0; i < pgn.size(); ++i) {
          s << pgn.at(i) << '\n';
      }
      success = true;
    } else {
      std::cerr << "error opening output file\n";
    }
    fOut.close();
    if(!success) {
        throw std::invalid_argument("Error writing file");
    }
}

void PgnPrinter::printHeaders(QStringList &pgn, Game &g) {
    QString tag = "[Event \"" + g.getHeader("Event") + "\"]";
    pgn.append(tag);
    tag = "[Site \"" + g.getHeader("Site") + "\"]";
    pgn.append(tag);
    tag = "[Date \"" + g.getHeader("Date") + "\"]";
    pgn.append(tag);
    tag = "[Round \"" + g.getHeader("Round") + "\"]";
    pgn.append(tag);
    tag = "[White \"" + g.getHeader("White") + "\"]";
    pgn.append(tag);
    tag = "[Black \"" + g.getHeader("Black") + "\"]";
    pgn.append(tag);
    tag = "[Result \"" + g.getHeader("Result") + "\"]";
    pgn.append(tag);
    QStringList all_tags = g.getTags();
    for(int i=0;i<all_tags.count();i++) {
        QString tag_i = all_tags.at(i);
        if(tag_i != "Event" && tag_i != "Site" && tag_i != "Date" && tag_i != "Round"
                && tag_i != "White" && tag_i != "Black" && tag_i != "Result" )
        {
            QString value_i = g.getHeader(tag_i);
            QString tag_val = "[" + tag_i + " \"" + value_i + "\"]";
            pgn.append(tag_val);
        }
    }
    // add fen string tag if root is not initial position
    chess::Board *root = g.getRootNode()->getBoard();
    if(!root->is_initial_position()) {
        QString tag = "[FEN \"" + root->fen() + "\"]";
        pgn.append(tag);
    }

}

QStringList PgnPrinter::printGame(Game &g) {

    this->reset();

    //pgn = new QStringList();

    // first print the headers
    //qDebug() << "print headers";
    this->printHeaders(pgn, g);

    this->writeLine(QString(""));
    GameNode *root = g.getRootNode();

    // special case if the root node has
    // a comment before the actual game starts
    if(!root->getComment().isEmpty()) {
        this->printComment(root->getComment());
    }

    //qDebug() << "print game content";
    this->printGameContent((*root));
    this->printResult(g.getResult());
    this->pgn.append(this->currentLine);

    return this->pgn;

}

void PgnPrinter::printMove(Board &b, Move &m) {
    if(b.turn == WHITE) {
        QString tkn = QString::number(b.fullmove_number);
        tkn.append(QString(". "));
        this->writeToken(tkn);
    }
    else if(this->forceMoveNumber) {
        QString tkn = QString::number(b.fullmove_number);
        tkn.append(QString("... "));
        this->writeToken(tkn);
    }
    this->writeToken((b.san(m)).append(QString(" ")));
    this->forceMoveNumber = false;
}

void PgnPrinter::printNag(int nag) {
    QString tkn = QString("$").append(QString::number(nag)).append(" ");
    this->writeToken(tkn);
}

void PgnPrinter::printResult(int result) {
    QString res = "";
    if(result == RES_WHITE_WINS) {
        res = QString("1-0");
    } else if(result == RES_BLACK_WINS) {
        res = QString("0-1");
    } else if(result == RES_DRAW) {
        res = QString("1/2-1/2");
    } else {
        res = QString("*");
    }
    this->writeToken(res.append(" "));
}

void PgnPrinter::beginVariation() {
    this->variationDepth++;
    QString tkn = QString("( ");
    this->writeToken(tkn);
    this->forceMoveNumber = true;
}

void PgnPrinter::endVariation() {
    this->variationDepth--;
    QString tkn = QString(") ");
    this->writeToken(tkn);
    this->forceMoveNumber = true;
}

void PgnPrinter::printComment(const QString &comment) {
    QString temp_c = QString(comment);
    QString write = QString("{ ").append(temp_c.replace("}","").trimmed()).append(" } ");
    this->writeToken(write);
    //this->forceMoveNumber = false;
}



void PgnPrinter::printGameContent(GameNode &g) {

    Board *b = g.getBoard();

    // first write mainline move, if there are variations
    int cntVar = g.getVariations().count();
    if(cntVar > 0) {
        GameNode* main_variation = g.getVariation(0);
        //qDebug() << "1";
        Move m = main_variation->getMove();
        //qDebug() << m.uci();
        //qDebug() << m.from << " " << m.to;
        //qDebug() << "1.5";
        this->printMove(*b,m);
        // write nags
        //qDebug() << "2";
        QVector<int> nags = main_variation->getNags();
        for(int j=0;j<nags.count();j++) {
            int n = nags.at(j);
            this->printNag(n);
        }
        //qDebug() << "3";
        // write comments
        if(!main_variation->getComment().isEmpty()) {
            this->printComment(main_variation->getComment());
        }
        //qDebug() << "4";
    }

    // now handle all variations (sidelines)
    for(int i=1;i<cntVar;i++) {
        // first create variation start marker, and print the move
        GameNode *var_i = g.getVariation(i);
        this->beginVariation();
        chess::Move m_temp = var_i->getMove();
        this->printMove(*b,m_temp);
        // next print nags
        QVector<int> nags = var_i->getNags();
        for(int j=0;j<nags.count();j++) {
            int n = nags.at(j);
            this->printNag(n);
        }
        // finally print comments
        if(!var_i->getComment().isEmpty()) {
            this->printComment(var_i->getComment());
        }

        // recursive call for all childs
        this->printGameContent(*var_i);

        // print variation end
        this->endVariation();
    }

    // finally do the mainline
    if(cntVar > 0) {
        GameNode* main_variation = g.getVariation(0);
        this->printGameContent((*main_variation));
    }
}


}
