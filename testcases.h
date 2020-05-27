#ifndef TESTCASES_H
#define TESTCASES_H

#include "board.h"

namespace chess {

class TestCases
{
public:
    TestCases();
    void run_pertf();

private:
    int count_moves(Board b, int depth);

};

}

#endif // TESTCASES_H
