#include <QCoreApplication>
#include <QFile>
#include <QTextStream>
#include <QTextCodec>
#include "pgn_reader.h"
#include "pgn_printer.h"
#include <iostream>
#include <QDebug>
#include "node_pool.h"
#include <QTimer>
#include "testcases.h"

int main(int argc, char *argv[])
{

    //chess::TestCases cases;
    //cases.run_pertf();




    QCoreApplication a(argc, argv);


    if(a.arguments().size() > 1) {


        chess::NodePool::reserve();

        QString fn_in = a.arguments().at(1);
        QFile file(fn_in);

        if(!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            throw std::invalid_argument("unable to open file w/ supplied filename");
        }
        QTextStream in(&file);
        QTextCodec *codecUtf8 = QTextCodec::codecForName("UTF-8");
        QTextCodec *codecLatin1 = QTextCodec::codecForName("ISO 8859-1");

        chess::PgnReader pgnReader;
        if(pgnReader.isUtf8(fn_in)) {
            in.setCodec(codecUtf8);
        } else {
            in.setCodec(codecLatin1);
        }

        chess::Game *g = new chess::Game();
        pgnReader.readGame(in,g);

        chess::PgnPrinter pgnPrinter;
        std::cout << pgnPrinter.printGame(*g).join("\n").toStdString() << std::endl;
    }


    QTimer::singleShot( 0, &a, &QCoreApplication::quit );
    return a.exec();
}

    /*
    if(argc > 1) {
        qDebug() << argc[1]
    }*/



    /*

    //QCoreApplication a(argc, argv);
    chess::NodePool::reserve();
    qDebug() << "starting up";
    //return a.exec();
    //QFile file("C:\\Users\\user\\MyFiles\\workspace\\test_databases\\kingbase_test.pgn");
    QFile file("C:\\Users\\user\\MyFiles\\workspace\\demo.pgn");
    //QFile file("/home/user/tmp2/demo.pgn");



    if(!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        throw std::invalid_argument("unable to open file w/ supplied filename");
    }
    QTextStream in(&file);
    QTextCodec *codec = QTextCodec::codecForName("UTF-8");
    in.setCodec(codec);

    chess::PgnReader pgnReader;

    qDebug() << "start reading single game";
    auto start = std::chrono::steady_clock::now();

    chess::Game *g = new chess::Game();
    pgnReader.readGame(in,g);
    //chess::Game *g = nullptr;

    qDebug() << "end reading single game";
    auto stop = std::chrono::steady_clock::now();
    std::chrono::duration<double> diff = (stop - start);
    std::chrono::milliseconds i_millis = std::chrono::duration_cast<std::chrono::milliseconds>(diff);

    std::cout << "Reading single game           :  " << i_millis.count() <<  "ms" << std::endl;

    chess::PgnPrinter pgnPrinter;
    std::cout << pgnPrinter.printGame(*g).join("\n").toStdString() << std::endl;
    //pgnPrinter.writeGame(*g, "c:\\temp\\temp.pgn");





    //QString kingbase = "C:\\Users\\user\\MyFiles\\workspace\\test_databases\\KingBaseLite2016-03-E60-E99.pgn";
    QString kingbase = "C:\\Users\\user\\MyFiles\\workspace\\test_databases\\millionbase-2.22.pgn";
    //QString kingbase = "/home/user/tmp2/millionbase-2.22.pgn";
    //QString kingbase = "C:\\Users\\user\\MyFiles\\workspace\\demo_two_games.pgn";


    start = std::chrono::steady_clock::now();
    QVector<qint64> offsets = pgnReader.scanPgn(kingbase, true);
    //for(int i=0;i<offsets.size();i++) {
    for(int i=0;i<10;i++) {
        qDebug() << "offset: " << offsets.at(i);
    }
    stop = std::chrono::steady_clock::now();
    diff = (stop - start);
    i_millis = std::chrono::duration_cast<std::chrono::milliseconds>(diff);
    std::cout << "Scanning Kingbase took :  " << i_millis.count() <<  "ms" << std::endl;

    const char* utf8 = "UTF-8";
    const char* isoLatin1 = "ISO 8859-1";
    bool isUtf8 = pgnReader.isUtf8(kingbase);

    file.close();
    QFile file1(kingbase);

    if(!file1.open(QIODevice::ReadOnly | QIODevice::Text)) {
        throw std::invalid_argument("unable to open file w/ supplied filename");
    }
    QTextStream in1(&file1);
    QTextCodec *codec1;
    if(isUtf8) {
        codec1 = QTextCodec::codecForName(utf8);
    } else {
        codec1 = QTextCodec::codecForName(isoLatin1);
    }
    in1.setCodec(codec1);

    chess::NodePool::deleteNode(g->getRootNode());
    qDebug() << chess::NodePool::freeList.size();
    g->reset();
//qDebug() << chess::NodePool::freeList.size();
    // new parser
    // last pos of last game of millbase
    quint64 pHash = 14269022190383210295UL;

    qDebug() << "# offsets: " << offsets.size();
    start = std::chrono::steady_clock::now();
    //for(int i=0;i<2190000;i++) {
    //for(int i=0;i<2;i++) {
    for(int i=0;i<offsets.size();i++) {
    //qDebug() << "# offsets: " << offsets.size();
    //for(int i=0;i<offsets.size();i++) {
        if(i%50000 == 0) {
            qDebug() << i;
        }
        //qDebug() << "at game i";
        //qDebug() << chess::NodePool::freeList.size();
        in1.seek(offsets[i]);
        pgnReader.readGame(in1, offsets[i], g);

        //qDebug() << "I: " << i;
        if(g->matchesPosition(pHash)) {
            qDebug() << "found pos @: " << i;
        }
        //qDebug() << pgnPrinter.printGame(*g);
        /*

        if(i == 1) {
            qDebug() << "seach,   i=1: " << pHash;
            //g->goToRoot();
            qDebug() << "root,    i=1: " << g->getCurrentNode()->getBoard()->pos_hash();
            //g->goToLeaf();
            qDebug() << "leaf,    i=1: " << g->getCurrentNode()->getBoard()->pos_hash();
            //g->goToParent();
            qDebug() << "leaf -1, i=1: " << g->getCurrentNode()->getBoard()->pos_hash();
            qDebug() << pgnPrinter.printGame(*g);
        }*/
        //if(g->matchesPosition(pHash)) {
        //    qDebug() << "found pos @: " << i;
        //} else {
        //    qDebug() << "no match";
        //}
        //qDebug() << pgnPrinter.printGame(*g);
        //delete g;
        //
/*
        chess::NodePool::deleteNode(g->getRootNode());
        g->reset();
        //qDebug() << chess::NodePool::freeList.size();
    }
    stop = std::chrono::steady_clock::now();
    diff = (stop - start);
    i_millis = std::chrono::duration_cast<std::chrono::milliseconds>(diff);

    std::cout << "Reading Kingbase (new parser) :  " << i_millis.count() <<  "ms" << std::endl;
*/



    // "classic" (jerry < 3.1.0) way of parsing with QRegexp and legal move generation
    //start = std::chrono::steady_clock::now();
    //for(int i=0;i<offsets.size();i++) {
    //    chess::Game *g = pgnReader.readGameFromFile(kingbase, encoding, offsets[i]);
    //    delete g;
        //chess::NodePool::deleteNode(g->getRootNode());
    //}
    //stop = std::chrono::steady_clock::now();
    //diff = (stop - start);
    //i_millis = std::chrono::duration_cast<std::chrono::milliseconds>(diff);

    //std::cout << "Reading Kingbase (classic)    :  " << i_millis.count() <<  "ms" << std::endl;

//return 1;
//}
