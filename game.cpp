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


#include "game.h"
#include <QDebug>
#include <iostream>
#include "node_pool.h"

namespace chess {

Game::Game() {

    //this->root = new GameNode();
    this->root = NodePool::makeNode();
    this->result = RES_UNDEF;
    this->current = root;
    this->treeWasChanged = false;

    this->wasEcoClassified = false;
    //this->ecoInfo("","");

}

void Game::reset() {

    NodePool::deleteNode(this->root);
    this->root = NodePool::makeNode();
    this->result = RES_UNDEF;
    this->current = root;
    this->treeWasChanged = true;
    this->headers.clear();

    this->wasEcoClassified = false;
}

Game::~Game() {
    //this->delBelow(this->root);
    //delete this->root;
}

GameNode* Game::getRootNode() {
    return this->root;
}

GameNode* Game::getCurrentNode() {
    return this->current;
}

void Game::setCurrent(GameNode *new_current) {
    this->current = new_current;
}

void Game::setRoot(GameNode *new_root) {
    this->root = new_root;
}

int Game::getResult() {
    return this->result;
}

void Game::setResult(int r) {
    this->result = r;
}

void Game::setHeader(QString tag, QString value) {
    this->headers[tag] = value;
}

QString Game:: getHeader(QString tag) {
    return this->headers[tag];
}

QStringList Game::getTags() {
    QStringList tags;
    QMapIterator<QString, QString> i(this->headers);
    while (i.hasNext()) {
        i.next();
        tags.append(i.key());
    }
    return tags;
}

/*
bool Game::hasCommentSubstring(QString &s, bool caseSensitive) {
    GameNode *root = this->getRootNode();
    return this->hasCommentSubstringBelow(s, root, caseSensitive);
}

bool Game::hasCommentSubstringBelow(QString &s, GameNode* temp, bool caseSensitive) {

    if(caseSensitive) {
        if(temp->getComment().contains(s)) {
            return true;
        }
    } else {
        if(temp->getComment().contains(s, Qt::CaseInsensitive)) {
            return true;
        }
    }
    for(int i=0;i < temp->variations.size(); i++) {
        GameNode* child_i = temp->variations.at(i);
        if(hasCommentSubstringBelow(s, child_i, caseSensitive)) {
            return true;
        }
    }
    return false;
}
*/

bool Game::matchesPosition(quint64 posHash) {

    GameNode* temp = this->getRootNode();
    Board *b = temp->getBoard();
    //std::cout << *b << std::endl;
    if(temp->getBoard()->get_pos_hash() == posHash) {
        return true;
    }
    while(temp->variations.size() > 0) {
        temp = temp->getVariation(0);
        b = temp->getBoard();
        //std::cout << *b << std::endl;
        //qDebug() << "";
        //qDebug().noquote() << b->printRaw();
        //qDebug() << "";
        if(temp->getBoard()->get_pos_hash() == posHash) {
            return true;
        }
    }
    return false;
}

bool isThreefoldRepetition() {
    //TODO
    return false;
}


GameNode* Game::findNodeByIdRec(int id, GameNode *node) {
    if(node->getId() == id) {
        return node;
    } else {
        for(int i=0;i < node->variations.size();i++) {
            GameNode *child_i = node->variations.at(i);
            GameNode *result = this->findNodeByIdRec(id, child_i);
            if(result != 0) {
                return result;
            }
        }
    }
    return 0;
}

GameNode* Game::findNodeById(int id) {
    GameNode *current = this->getRootNode();
    GameNode *result = this->findNodeByIdRec(id, current);
    if(result == 0) {
        throw std::invalid_argument("node doesn't exist");
    } else {
        return result;
    }
}

void Game::goToMainLineChild() {
    if(this->current->variations.count() > 0) {
        this->current = this->current->getVariation(0);
    }
}

int Game::countHalfmoves() {
    int halfmoves = 0;
    GameNode *temp = this->root;
    while(temp->variations.count() > 0) {
        temp = temp->getVariation(0);
        halfmoves += 1;
    }
    return halfmoves;
}


GameNode* Game::getEndNode() {
    GameNode *temp = this->getRootNode();
    while(temp->variations.count() > 0) {
        temp = temp->getVariation(0);
    }
    return temp;
}

void Game::goToChild(int idx_child) {
    if(this->current->variations.count() > idx_child) {
        this->current = this->current->getVariation(idx_child);
    }
}

void Game::goToParent() {
    if(this->current->getParent() != 0) {
        this->current = this->current->getParent();
    }
}

void Game::goToEnd() {
    GameNode *temp = this->root;
    while(temp->variations.count() > 0) {
        temp = temp->getVariation(0);
    }
    this->current = temp;
}

void Game::goToRoot() {
    this->current = this->root;
}

void Game::resetWithNewRootBoard(chess::Board new_root_board) {
    chess::GameNode* old_root = this->getRootNode();
    this->delBelow(old_root);
    chess::GameNode* new_root = new chess::GameNode();
    new_root->setBoard(new_root_board);
    this->setRoot(new_root);
    this->setCurrent(new_root);
    this->result = RES_UNDEF;
    this->clearHeaders();
    this->treeWasChanged = true;
    delete old_root;
}

void Game::clearHeaders() {
    this->headers.clear();
    this->headers.insert(("Event"), "");
    this->headers.insert("Site","");
    this->headers.insert("Date","");
    this->headers.insert("Round","");
    this->headers.insert("White","");
    this->headers.insert("Black","");
    this->headers.insert("Result","*");
}

void Game::goToLeaf() {
    while(!current->isLeaf()) {
        this->goToChild(0);
    }
}

void Game::applyMove(Move &m) {
    bool exists_child = false;
    for(int i=0;i<this->current->variations.size();i++) {
        Move mi = this->current->variations.at(i)->getMove();
        if(m == mi) {
            exists_child = true;
            this->current = this->current->variations.at(i);
            break;
        }
    }
    if(!exists_child) {
        GameNode *current = this->getCurrentNode();
        Board *b_current = current->getBoard();
        Board b_child = Board(*b_current);
        b_child.apply(m);
        GameNode *new_current = NodePool::makeNode();
        new_current->setBoard(b_child);
        new_current->setMove(m);
        new_current->setParent(current);
        current->variations.append(new_current);
        this->current = new_current;
        this->treeWasChanged = true;
    }
}

void Game::moveUp(GameNode *node) {
    if(node->getParent() != 0) {
        GameNode *parent = node->getParent();
        int i = parent->variations.indexOf(node);
        if(i > 0) {
            parent->variations.removeAt(i);
            parent->variations.insert(i-1,node);
        }
        this->treeWasChanged = true;
    }
}

void Game::moveDown(GameNode *node) {
    if(node->getParent() != 0) {
        GameNode *parent = node->getParent();
        int i = parent->variations.indexOf(node);
        if(i < parent->variations.size() -1) {
            parent->variations.removeAt(i);
            parent->variations.insert(i+1,node);
        }
        this->treeWasChanged = true;

    }
}

void Game::delVariant(GameNode *node) {
    // go up the variation until we
    // find the root of the variation
    GameNode *child = node;
    GameNode *var_root = node;
    while(var_root->getParent() != 0 && var_root->getParent()->variations.size() == 1) {
        child = var_root;
        var_root = var_root->getParent();
    }
    int idx = -1;
    // one more to get the actual root
    if(var_root->getParent() !=0) {
        child = var_root;
        var_root = var_root->getParent();
        idx = var_root->variations.indexOf(child);
    }
    if(idx != -1) {
        var_root->variations.removeAt(idx);
        delete child;
        this->current = var_root;
    }
}

void Game::delBelow(GameNode *node) {
    for(int i=0;i<node->variations.size();i++) {
        GameNode *child_i = node->variations.at(i);
        node->variations.removeAt(i);
        delete child_i;
    }
    this->current = node;
}

void Game::removeCommentRec(GameNode *node) {
    QString empty = QString("");
    node->setComment(empty);
    for(int i=0;i<node->variations.size();i++) {
        GameNode *var_i = node->variations.at(i);
        this->removeCommentRec(var_i);
    }
}

void Game::removeAllComments() {
    this->removeCommentRec(this->getRootNode());
}

void Game::removeAllVariants() {
    GameNode *temp = this->getRootNode();
    int size = temp->variations.size();
    while(size > 0) {
        GameNode *main = temp->variations.at(0);
        // delete all variants
        for(int i=1;i<size;i++) {
            GameNode *ni = temp->variations.at(i);
            delete ni;
        }
        temp->variations.clear();
        temp->addVariation(main);
        temp = temp->variations.at(0);
        size = temp->variations.size();
    }
    this->current = this->getRootNode();
}

void Game::findEco() {

    EcoCode *ec = new EcoCode();
    GameNode* temp = this->getRootNode();
    int depth = 0;
    while(depth < 29 && temp->variations.count() > 0) {
        temp = temp->variations.at(0);
        depth++;
    }
    int maxdepth = depth;
    while(depth >= 2)  {
        Board b_temp = temp->getBoard();
        EcoInfo e_temp = ec->classify(b_temp);
        if(!e_temp.code.isEmpty()) {
            this->ecoInfo = EcoInfo(e_temp);
            this->wasEcoClassified = true;
            this->headers.insert("ECO", e_temp.code);
            delete ec;
            break;
        } else {
            temp = temp->getParent();
            depth--;
        }
    }
    // we consider this a failed attempt
    // if we haven't found a code, and the game
    // is sufficiently long
    if(maxdepth > 4 && !this->wasEcoClassified) {
        this->wasEcoClassified = true;
        this->ecoInfo.code = "A00";
        this->ecoInfo.info = "Unknown";
    }
}

bool Game::isTreeChanged() {
    return this->treeWasChanged;
}

void Game::setTreeWasChanged(bool status) {
    this->treeWasChanged = status;
}

EcoInfo Game::getEcoInfo() {
    return this->ecoInfo;
}

}

