#include "testcases.h"
#include <QString>
#include <QVector>
#include <QDebug>
#include "board.h"
#include <iostream>

chess::TestCases::TestCases()
{
}

int chess::TestCases::count_moves(Board b, int depth) {

    int count = 0;
    QVector<Move> mvs = b.legal_moves();
    for(int i=0;i<mvs.count();i++) {
        Move mi = mvs.at(i);
        QString mv_uci = mi.uci();
        //qDebug() << "foo: "<< mv_uci;
    }
    if(depth == 0) {
        int cnt = mvs.count();
        return cnt;
    } else {
        // recursive case: for each possible move, apply
        // the move, do the recursive call and undo the move
        for(int i=0;i<mvs.count();i++) {
            Move m = mvs.at(i);
            // /QString old_fen = b.fen();
            b.apply(m);
            int cnt_i = count_moves(b, depth - 1);
            count += cnt_i;
            b.undo();
            // /QString old_fen1 = b.fen();
            // /if(QString::compare(old_fen, old_fen1, Qt::CaseInsensitive) != 0) {
            // /    int foobar = QString::compare(old_fen, old_fen1, Qt::CaseInsensitive);
            // /    std::cout << "MAJOR FOOBAR: " << foobar << std::endl;
            // /    std::cout << "MOVE: " << m.uci().toStdString() << std::endl;
            // /    std::cout << "OLD: " << old_fen.toStdString() << std::endl;
            // /    std::cout << "NEW: " << old_fen1.toStdString() << std::endl;
            // /}
        }
        return count;
    }

}

void chess::TestCases::run_pertf() {

    // initial position tests, perft 1 - 6
    Board b = Board(QString("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"));
    std::cout << "Testing " << b.fen().toStdString() << std::endl;
    std::cout << "Perft 1, expected: 20" << std::endl;
    int c = count_moves(b,0);
    std::cout << "         computed: " << c << std::endl;

    b = Board(QString("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"));
    std::cout << "Testing " << b.fen().toStdString() << std::endl;
    std::cout << "Perft 2, expected: 400" << std::endl;
    c = count_moves(b,1);
    std::cout << "         computed: " << c << std::endl;

    b = Board(QString("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"));
    std::cout << "Testing " << b.fen().toStdString() << std::endl;
    std::cout << "Perft 3, expected: 8902" << std::endl;
    c = count_moves(b,2);
    std::cout << "         computed: " << c << std::endl;

    b = Board(QString("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"));
    std::cout << "Testing " << b.fen().toStdString() << std::endl;
    std::cout << "Perft 4, expected: 197281" << std::endl;
    c = count_moves(b,3);
    std::cout << "         computed: " << c << std::endl;

    b = Board(QString("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"));
    std::cout << "Testing " << b.fen().toStdString() << std::endl;
    std::cout << "Perft 5, expected: 4865609" << std::endl;
    c = count_moves(b,4);
    std::cout << "         computed: " << c << std::endl;

    // "Kiwipete" by Peter McKenzie, great for identifying bugs
    // perft 1 - 5
    b = Board(QString("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 0"));
    std::cout << "Testing " << b.fen().toStdString() << std::endl;
    std::cout << "Perft 1, expected: 48" << std::endl;
    c = count_moves(b,0);
    std::cout << "         computed: " << c << std::endl;

    b = Board(QString("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 0"));
    std::cout << "Testing " << b.fen().toStdString() << std::endl;
    std::cout << "Perft 2, expected: 2039" << std::endl;
    c = count_moves(b,1);
    std::cout << "         computed: " << c << std::endl;

    b = Board(QString("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 0"));
    std::cout << "Testing " << b.fen().toStdString() << std::endl;
    std::cout << "Perft 3, expected: 97862" << std::endl;
    c = count_moves(b,2);
    std::cout << "         computed: " << c << std::endl;

    b = Board(QString("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 0"));
    std::cout << "Testing " << b.fen().toStdString() << std::endl;
    std::cout << "Perft 4, expected: 4085603" << std::endl;
    c = count_moves(b,3);
    std::cout << "         computed: " << c << std::endl;

    // some more pos. for bug-hunting, taken from chessprogramming wiki and
    // Sharper by Albert Bertilsson's homepage
    // perft 1 - 2
    b = Board(QString("8/3K4/2p5/p2b2r1/5k2/8/8/1q6 b - - 1 67"));
    std::cout << "Testing " << b.fen().toStdString() << std::endl;
    std::cout << "Perft 1, expected: 50" << std::endl;
    c = count_moves(b,0);
    std::cout << "         computed: " << c << std::endl;

    b = Board(QString("8/3K4/2p5/p2b2r1/5k2/8/8/1q6 b - - 1 67"));
    std::cout << "Testing " << b.fen().toStdString() << std::endl;
    std::cout << "Perft 2, expected: 279" << std::endl;
    c = count_moves(b,1);
    std::cout << "         computed: " << c << std::endl;

    // perft 5
    b = Board(QString("rnbqkb1r/ppppp1pp/7n/4Pp2/8/8/PPPP1PPP/RNBQKBNR w KQkq f6 0 3"));
    std::cout << "Testing " << b.fen().toStdString() << std::endl;
    std::cout << "Perft 5, expected: 11139762" << std::endl;
    c = count_moves(b,4);
    std::cout << "         computed: " << c << std::endl;

    // perft 1 - 5
    b = Board(QString("rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8"));
    std::cout << "Testing " << b.fen().toStdString() << std::endl;
    std::cout << "Perft 1, expected: 44" << std::endl;
    c = count_moves(b,0);
    std::cout << "         computed: " << c << std::endl;

    b = Board(QString("rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8"));
    std::cout << "Testing " << b.fen().toStdString() << std::endl;
    std::cout << "Perft 2, expected: 1486" << std::endl;
    c = count_moves(b,1);
    std::cout << "         computed: " << c << std::endl;

    b = Board(QString("rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8"));
    std::cout << "Testing " << b.fen().toStdString() << std::endl;
    std::cout << "Perft 3, expected: 62379" << std::endl;
    c = count_moves(b,2);
    std::cout << "         computed: " << c << std::endl;

    b = Board(QString("rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8"));
    std::cout << "Testing " << b.fen().toStdString() << std::endl;
    std::cout << "Perft 4, expected: 2103487" << std::endl;
    c = count_moves(b,3);
    std::cout << "         computed: " << c << std::endl;

    // most computational intensive (i.e. deepest) of the above
    // are at the end here
    b = Board(QString("rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8"));
    std::cout << "Testing " << b.fen().toStdString() << std::endl;
    std::cout << "Perft 5, expected: 89941194" << std::endl;
    c = count_moves(b,4);
    std::cout << "         computed: " << c << std::endl;

    b = Board(QString("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"));
    std::cout << "Testing " << b.fen().toStdString() << std::endl;
    std::cout << "Perft 6, expected: 119060324" << std::endl;
    c = count_moves(b,5);
    std::cout << "         computed: " << c << std::endl;

    // perft 6
    b = Board(QString("8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - 0 0"));
    std::cout << "Testing " << b.fen().toStdString() << std::endl;
    std::cout << "Perft 6, expected: 11030083" << std::endl;
    c = count_moves(b,5);
    std::cout << "         computed: " << c << std::endl;

    // perft 7
    qDebug() << "perft7";
    b = Board(QString("8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - 0 0"));
    std::cout << "Testing " << b.fen().toStdString() << std::endl;
    std::cout << "Perft 7, expected: 178633661" << std::endl;
    c = count_moves(b,6);
    std::cout << "         computed: " << c << std::endl;

    // perft 6
    b = Board(QString("8/7p/p5pb/4k3/P1pPn3/8/P5PP/1rB2RK1 b - d3 0 28"));
    std::cout << "Testing " << b.fen().toStdString() << std::endl;
    std::cout << "Perft 6, expected: 38633283" << std::endl;
    c = count_moves(b,5);
    std::cout << "         computed: " << c << std::endl;

    b = Board(QString("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 0"));
    std::cout << "Testing " << b.fen().toStdString() << std::endl;
    std::cout << "Perft 5, expected: 193690690" << std::endl;
    c = count_moves(b,4);
    std::cout << "         computed: " << c << std::endl;

}
