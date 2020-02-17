#include "node_pool.h"
#include <QDebug>

namespace chess {

QVector<GameNode*> NodePool::freeList;

NodePool::NodePool()
{

}

void NodePool::initPool() {

    for(int i=0;i<MAX_NODE_COUNT;i++) {
        GameNode *node = new GameNode();
        NodePool::freeList.append(node);
    }
    qDebug() << "pool init";

}

GameNode* NodePool::makeNode() {

    //assert(NodePool::freeList.size() > 0);
    if(NodePool::freeList.size() == 0) {
        qDebug() << "EMPTY STORE!";
    }
    return NodePool::freeList.takeLast();
}

void NodePool::deleteNode(GameNode* node) {

    // recursively put back all child nodes
    QVector<GameNode*> children = node->getVariations();
    for(int i=0;i<children.size();i++) {
        GameNode* child_i = children.at(i);
        NodePool::deleteNode(child_i);
    }
    NodePool::freeList.append(node);
    node->reset();
}


}
