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


#ifndef GAME_H
#define GAME_H

#include "game_node.h"
#include "ecocode.h"

namespace chess {

const int RES_UNDEF = 0;
const int RES_WHITE_WINS = 1;
const int RES_BLACK_WINS = 2;
const int RES_DRAW = 3;
const int RES_ANY = 4;

class Game
{

public:

    /**
     * @brief returns true if a tree operation was recently carried
     *                out that changed the tree structure (i.e. then
     *                a redraw is required)
     */
    bool isTreeChanged();

    void reset();

    /**
     *  @brief call this function with true if an operation was carried
     *         out that fundamentally changed the tree structure (i.e.
     *         deleting nodes, adding comments etc.). Used to remember
     *         if a GUI representation of the tree needs to be redrawn
     *         call with false, after updating GUI representation
     */
    void setTreeWasChanged(bool status);


    void setHeader(QString tag, QString value);
    QString getHeader(QString tag);
    void resetHeaders();
    QStringList getTags();

    /**
     * @brief Game essentially a tree of GameNode objects that
     *             represents a game. Default root node has a
     *             board position which is empty
     */
    Game();
    ~Game();

    /**
     * @brief getRootNode returns the root node of the game
     * @return
     */
    GameNode* getRootNode();

    /**
     * @brief getEndNode returns end of mainline
     * @return
     */
    GameNode* getEndNode();


    /**
     * @brief getCurrentNode returns the current node. The current
     *                       node is a pointer to a node in the tree
     *                       and used e.g. for the node of the last move
     * @return
     */
    GameNode* getCurrentNode();

    /**
     * @brief getResult returns the result of the game
     * @return RES_BLACK_WINS or RES_DRAW or RES_WHITE_WINS or RES_UNDEF
     */
    int getResult();

    /**
     * @brief setResult sets the result of the game
     * @param r see getResult()
     */
    void setResult(int r);

    /**
     * @brief applyMove apply a move to the current node, and change
     *                  the node to the resulting new node (or existing node)
     *                  if a child node for this move already exists.
     *                  There is no check if the supplied move is legal!
     * @param m the move to apply on the current board.
     */
    void applyMove(Move &m);

    /**
     * @brief findNodeById each GameNode has a unique id (see class definition)
     *                     this searches for and find the node given the supplied id
     *                     throw std::invalid_argument if there exists no node
     *                     with the id
     * @param id the node id
     * @return gamenode with the supplied id
     */
    GameNode* findNodeById(int id);

    /**
     * @brief setCurrent set the current pointer to the supplied node. There is
     *                   no validity check whether the node is actually a node
     *                   in the game tree.
     * @param new_current pointer to the node
     */
    void setCurrent(GameNode *new_current);

    /**
     * @brief setRoot sets the root node pointer to the supplied node. Really just that.
     *                Beware of memory leaks when using this function, as nodes below
     *                the old root might become inaccessible
     * @param new_root
     */
    void setRoot(GameNode *new_root);

    /**
     * @brief goToMainLineChild moves the current node pointer one down to the mainline (zeroth)
     *                          variation (if it exists), otherwise keeps the pointer
     *                          at the current node
     */
    void goToMainLineChild();

    /**
     * @brief goToChild moves the current node poiner to the child at index idx_child.
     *                  If the index is out of range, keeps the pointer at the current node.
     * @param idx_child the index of the variation of the child node
     */
    void goToChild(int idx_child);

    /**
     * @brief goToParent moves current node pointer to the parent node (if exists).
     *                   keeps pointer at existing node if already at root
     */
    void goToParent();

    /**
     * @brief goToRoot moves current pointer to the root node of the game
     */
    void goToRoot();

    /**
     * @brief goToEnd starting at the current node, moves the current node pointer
     *                down among all mainline until reaching a leaf
     */
    void goToEnd();

    /**
     * @brief moveUp Suppose the supplied node is a child referenced at parent with index i,
     *               and there is another child of the parent with index i-1. Then this
     *               function switches these indexes. In other word, moves the supplied node
     *               variation one up. Has no effect, if node is root (i.e. has no parent)
     *               or is already the mainline (i.e. the zeroth) child of parent.
     * @param node The node that should be moved up
     */
    void moveUp(GameNode *node);

    /**
     * @brief moveDown Reverse of moveUp().
     * @param node
     */
    void moveDown(GameNode *node);

    /**
     * @brief delVariant deletes the whole variation on which the supplied node exists. I.e.
     *        moves up the tree to the root of the variation, and deletes everything below.
     *        sets current node pointer to the root of the variation.
     * @param node
     */
    void delVariant(GameNode *node);

    /**
     * @brief delBelow delete the subtree below the supplied node. Afterwards sets current
     *        node pointer to the supplied node.
     * @param node
     */
    void delBelow(GameNode *node);

    /**
     * @brief removeCommentRec removes comment at supplied node and recursively removes
     *        at all comments at nodes below the supplied node.
     * @param node the node to start with.
     */
    void removeCommentRec(GameNode *node);

    /**
     * @brief goToLeaf from the current node pointer, go down the mainlines until
     *                 there are no more childs.
     */
    void goToLeaf();

    /**
     * @brief resetWithNewRootBoard delete the whole game tree, and set a new
     *        root node constructed with the supplied move. Essentially call this, if
     *        a new game has been requrested by the user, especially if the user
     *        has set up a custom board position. The supplied board MUST be
     *        a valid board position.
     * @param new_root_board The chess board to construct the root node.
     */
    void resetWithNewRootBoard(Board new_root_board);

    /**
     * @brief removeAllComments iterates through the tree, and removes every
     *                          comment from each GameNode
     */
    void removeAllComments();

    /**
     * @brief removeAllVariants iterates through the tree and keeps only the
     *                          mainlines (i.e. zeroth) variations if there
     *                          are more than one child in a GameNode
     */
    void removeAllVariants();

    /*
    bool hasCommentSubstring(QString &s, bool caseSensitive);

    bool hasCommentSubstringMainline(QString &s, bool caseSensitive);
    */

    bool matchesPosition(quint64 posHash);

    /**
     * @brief clearHeaders deletes all headers entries.
     */
    void clearHeaders();

    void findEco();
    EcoInfo getEcoInfo();
    bool wasEcoClassified;

    /**
     * @brief countHalfmoves counts halfmoves of mainline
     * @return
     */
    int countHalfmoves();

private:

    QMap<QString, QString> headers;
    bool treeWasChanged;
    GameNode* root;
    GameNode* current;
    int result;
    GameNode* findNodeByIdRec(int id, GameNode* node);

    bool hasCommentSubstringBelow(QString &s, GameNode* node, bool caseSensitive);

    EcoInfo ecoInfo;

};

}

#endif // GAME_H
