#include <iostream>
#include <sstream>
#include <cstdlib>
#include <stdio.h>
#include <stdlib.h>

#include <string.h>
#include <time.h>

#include <vector>
#include <iterator>
#include <utility>

#include "gametree.h"

#define SIZE (8)
#define SEARCH_DEPTH (0)
#define VERBOSE (0)
#define AUTO (0)
#define DUMB_AI (false)

int g_auto = AUTO;
int g_searchdepth = SEARCH_DEPTH;
bool g_gofirst = true;
bool g_clearscreen = true;
int g_turn = 0;
int g_verbose = VERBOSE;
double g_timeout = 30;

const char DIRS[8][2] = {{1, 0}, {1, 1}, {0, 1}, {-1, 1}, {-1, 0}, {-1, -1}, {0, -1}, {1, -1}};
 
using namespace std;
namespace std {
template<class A1, class A2>
    ostream& operator<<(ostream& s, vector<A1, A2> const& vec) {
        copy(vec.begin(), vec.end(), ostream_iterator<A1>(s, " "));
        return s;
    }

}

void clear_screen() {
    if (g_clearscreen) {
#ifdef WINDOWS
        system ( "CLS" );
#else
        system ( "clear" );
#endif
    }
}


using namespace Data_structures;
#include "othello.h"

class Othello;

#include "ai.h"

int game_start() {
    Othello *pBoard = new Othello( );
    Othello board = *pBoard;

    cout << "Enter moves in form xy (no space)." << endl;

    bool color = g_gofirst;
    g_turn = 0;
    clock_t g_init, g_final;
    g_init=clock();

    srand ( time(NULL) );

    vector<int> movesW, movesB;
    board.FindValidMoves( true, movesW );
    board.FindValidMoves( false, movesB );

    while (movesW.size() + movesB.size() > 0) {
        cout << endl;
        board.FindValidMoves( true, movesW );
        board.FindValidMoves( false, movesB );

        vector<int> moves;
        board.FindValidMoves( color, moves );
        board.PrintBoard();

        if (board.GetScore()) {
            if (moves.size() > 0) {
                cout << endl << "Valid moves: "; 
                for ( int i = 0; i < moves.size(); i++) {
                    char x = moves[i]%SIZE + 97;
                    int y = moves[i]/SIZE + 1;
                    cout << x << y << "  ";
                }
                cout << endl;
            }

            int pos = 0;

            if (color) {   /* If true, it is player's move. */
                if (moves.size() > 0) {
                    if (g_auto == 0) {
                        bool moveGood = false;
                        do {
                            cout << endl;
                            string input = "";
                            int move = 0;
                            getline(cin, input);
                            //stringstream num(input);
                            //num >> move;
                            int x = (int)input[0] - 97;
                            int y = (int)input[1] - 49;

                            pos = y*SIZE + x;

                            if (board.board[pos] != 3) {
                                cout << endl << "Invalid move." << endl;
                            } else {
                                moveGood = true;
                            }
                        } while (!moveGood);
                    } else if (g_auto == 1) {
                        pos = moves[rand() % moves.size()];
                    } else if (g_auto == 2) {
                        pos = moves[0];
                    } else {
                        pos = ai_initialize( board, true );
                    }
                    clear_screen();
                }
            } else {       /* Otherwise, it is computer's move. */
                if (moves.size() > 0) {
                    if (DUMB_AI) {
                        /* Randomly pick an available move. */
                        pos = moves[rand() % moves.size()];
                    } else {
                        pos = ai_initialize( board, false );
                    }
                    cout << endl << "Opponent moves: " << (char)(pos%SIZE + 97) << pos/SIZE + 1 << endl;
                }
            }
            if (board.IsValidMove( color, pos )) {
                /* Update the board. */
                Othello tmpBoard ( board.MakeMove( color, pos ));
                board = tmpBoard;
            } else {
                if (moves.size() > 0) {
                    cout << "Fuck." << endl;
                }
            }
            color = !color;
        }

    }

    cout << "Game over." << endl;
    g_final=clock()-g_init;
    cout << endl << "Game took " << (double)g_final / ((double)CLOCKS_PER_SEC) << " s." << endl;
    return 0;
}

int main(int argc, char **argv) {
    if (argc >= 2 && (*argv[1]) == 63) {
        cout << "Othello Options: " << endl <<
            "  Play mode: 0 = manual; 1 = random; 2 = fixed; 3 = AI" << endl <<
            "  AI max search depth: 0 = auto; otherwise uses specified value" << endl <<
            "  Player color: 0 = black; 1 = white" << endl <<
            "  Clear screen each turn: 0 = false; 1 = true" << endl <<
            "  AI search timeout: value in seconds" << endl;
    } else {
        if (argc >= 2) {
            g_auto = atoi(argv[1]);
        }
        if (argc >= 3) {
            g_searchdepth = atoi(argv[2]);
        }
        if (argc >= 4) {
            g_gofirst = (atoi(argv[3]) == 1);
        }
        if (argc >= 5) {
            g_clearscreen = (atoi(argv[4]) == 1);
        }
        if (argc >= 6) {
            g_timeout = atoi(argv[5]);
        }
        if (argc >= 7) {
            g_verbose = atoi(argv[6]);
        }

        cout << "OTHELLO" << endl;
        cout << "Settings: " << endl <<
            "  Play mode:    \t"   << (g_auto == 0 ? "manual" : "automatic") << endl <<
            "  Player color: \t"   << (g_gofirst ? "black(@)" : "white(O)") << endl <<
            "  Clear screen: \t"   << (g_clearscreen ? "Yes" : "No" ) << endl <<
            "  Verbosity:    \t"   << g_verbose << endl <<
            "  AI max depth: \t"   << g_searchdepth << (g_searchdepth == 0 ? " (based on move number)" : " (fixed)" ) << endl <<
            "  AI timeout:   \t"   << g_timeout << " seconds." << endl;

        game_start();
    }
    return 0;
}
