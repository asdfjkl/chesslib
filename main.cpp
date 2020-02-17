#include <QCoreApplication>
#include <QFile>
#include <QTextStream>
#include <QTextCodec>
#include "pgn_reader.h"
#include "pgn_printer.h"
#include <iostream>
#include <QDebug>

int main(int argc, char *argv[])
{
    //QCoreApplication a(argc, argv);

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

    chess::Game *g = pgnReader.nReadGame(in);

    auto stop = std::chrono::steady_clock::now();
    std::chrono::duration<double> diff = (stop - start);
    std::chrono::milliseconds i_millis = std::chrono::duration_cast<std::chrono::milliseconds>(diff);

    std::cout << "Reading single game           :  " << i_millis.count() <<  "ms" << std::endl;

    chess::PgnPrinter pgnPrinter;
    std::cout << pgnPrinter.printGame(*g).join("\n").toStdString() << std::endl;

    QString kingbase = "C:\\Users\\user\\MyFiles\\workspace\\test_databases\\KingBaseLite2016-03-E60-E99.pgn";
    QVector<qint64> offsets = pgnReader.scanPgn(kingbase, true);

    const char* encoding = pgnReader.detect_encoding(kingbase);

    // new parser
    start = std::chrono::steady_clock::now();
    for(int i=0;i<offsets.size();i++) {
        chess::Game *g = pgnReader.nReadGameFromFile(kingbase, encoding, offsets[i]);
        //qDebug() << pgnPrinter.printGame(*g);
        delete g;
    }
    stop = std::chrono::steady_clock::now();
    diff = (stop - start);
    i_millis = std::chrono::duration_cast<std::chrono::milliseconds>(diff);

    std::cout << "Reading Kingbase (new parser) :  " << i_millis.count() <<  "ms" << std::endl;

    // "classic" (jerry < 3.1.0) way of parsing with QRegexp and legal move generation
    start = std::chrono::steady_clock::now();
    for(int i=0;i<offsets.size();i++) {
        chess::Game *g = pgnReader.readGameFromFile(kingbase, encoding, offsets[i]);
        delete g;
    }
    stop = std::chrono::steady_clock::now();
    diff = (stop - start);
    i_millis = std::chrono::duration_cast<std::chrono::milliseconds>(diff);

    std::cout << "Reading Kingbase (classic)    :  " << i_millis.count() <<  "ms" << std::endl;


    return 1;
}
