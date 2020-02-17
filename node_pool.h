#ifndef NODE_POOL_H
#define NODE_POOL_H

#include "game_node.h"
#include <QVector>

namespace chess {

const int MAX_NODE_COUNT = 512;

class NodePool
{
public:
    NodePool();
    static void initPool();
    static GameNode* makeNode();
    static void deleteNode(GameNode* node);
    static QVector<GameNode*> freeList;


private:

};

}

#endif // NODE_POOL_H
