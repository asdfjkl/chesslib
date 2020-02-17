#include <QCoreApplication>
#include <QFile>
#include <QTextStream>
#include <QTextCodec>
#include "pgn_reader.h"
#include "pgn_printer.h"
#include <iostream>
#include <QDebug>
#include "node_pool.h"

int main(int argc, char *argv[])
{
    //QCoreApplication a(argc, argv);
    chess::NodePool::initPool();
    //return a.exec();
    //QFile file("C:\\Users\\user\\MyFiles\\workspace\\test_databases\\kingbase_test.pgn");
    QFile file("C:\\Users\\user\\MyFiles\\workspace\\demo.pgn");
    //QFile file("/home/gast/tmp/demo.pgn");


    if(!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        throw std::invalid_argument("unable to open file w/ supplied filename");
    }
    QTextStream in(&file);
    QTextCodec *codec = QTextCodec::codecForName("UTF-8");
    in.setCodec(codec);

    chess::PgnReader pgnReader;

    auto start = std::chrono::steady_clock::now();

    chess::Game *g = new chess::Game();
    pgnReader.nReadGame(in,g);
    //chess::Game *g = nullptr;

    auto stop = std::chrono::steady_clock::now();
    std::chrono::duration<double> diff = (stop - start);
    std::chrono::milliseconds i_millis = std::chrono::duration_cast<std::chrono::milliseconds>(diff);

    std::cout << "Reading single game           :  " << i_millis.count() <<  "ms" << std::endl;

    chess::PgnPrinter pgnPrinter;
    std::cout << pgnPrinter.printGame(*g).join("\n").toStdString() << std::endl;

    QString kingbase = "C:\\Users\\user\\MyFiles\\workspace\\test_databases\\KingBaseLite2016-03-E60-E99.pgn";
    QVector<qint64> offsets = pgnReader.scanPgn(kingbase, true);

    const char* encoding = pgnReader.detect_encoding(kingbase);

    file.close();
    QFile file1(kingbase);

    if(!file1.open(QIODevice::ReadOnly | QIODevice::Text)) {
        throw std::invalid_argument("unable to open file w/ supplied filename");
    }
    QTextStream in1(&file1);
    QTextCodec *codec1 = QTextCodec::codecForName(encoding);
    in1.setCodec(codec1);

    chess::NodePool::deleteNode(g->getRootNode());
    qDebug() << chess::NodePool::freeList.size();
    g->reset();
//qDebug() << chess::NodePool::freeList.size();
    // new parser
    start = std::chrono::steady_clock::now();
    for(int i=0;i<offsets.size();i++) {
        //qDebug() << "at game i";
        //qDebug() << chess::NodePool::freeList.size();
        in1.seek(offsets[i]);
        pgnReader.nReadGameFromFile(in1, offsets[i], g);
        //qDebug() << pgnPrinter.printGame(*g);
        //delete g;
        chess::NodePool::deleteNode(g->getRootNode());
        g->reset();
        //qDebug() << chess::NodePool::freeList.size();
    }
    stop = std::chrono::steady_clock::now();
    diff = (stop - start);
    i_millis = std::chrono::duration_cast<std::chrono::milliseconds>(diff);

    std::cout << "Reading Kingbase (new parser) :  " << i_millis.count() <<  "ms" << std::endl;

    /*
    // "classic" (jerry < 3.1.0) way of parsing with QRegexp and legal move generation
    start = std::chrono::steady_clock::now();
    for(int i=0;i<offsets.size();i++) {
        chess::Game *g = pgnReader.readGameFromFile(kingbase, encoding, offsets[i]);
        delete g;
        //chess::NodePool::deleteNode(g->getRootNode());
    }
    stop = std::chrono::steady_clock::now();
    diff = (stop - start);
    i_millis = std::chrono::duration_cast<std::chrono::milliseconds>(diff);

    std::cout << "Reading Kingbase (classic)    :  " << i_millis.count() <<  "ms" << std::endl;
*/

    return 1;
}
