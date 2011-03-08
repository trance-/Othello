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

int g_auto = 0;
int g_searchdepth = 0;
bool g_gofirst = true;
bool g_clearscreen = true;
int g_turn = 0;
int g_verbose = 0;
double g_timeout = 30;
bool g_timemode = false;

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

string printMove( int move ) {
    string s;
    s += (char)(move%SIZE + 97);
    s += (char)('0' + move/SIZE + 1);
    return s;
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
                    cout << printMove(moves[i]) << "  ";
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
                        cout << endl << "Player moves: " << printMove(pos) << endl;
                    } else if (g_auto == 2) {
                        pos = moves[0];
                        cout << endl << "Player moves: " << printMove(pos) << endl;
                    } else {
                        pos = ai_initialize( board, true );
                        cout << endl << "Player moves: " << printMove(pos) << endl;
                    }
                    clear_screen();
                }
            } else {       /* Otherwise, it is computer's move. */
                if (moves.size() > 0) {
                    if (g_searchdepth < 0) {
                        /* Randomly pick an available move. */
                        pos = moves[rand() % moves.size()];
                    } else {
                        pos = ai_initialize( board, false );
                    }
                    cout << endl << "Opponent moves: " << printMove(pos) << endl;
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
    board.GetScore( true );
    cout << "Game over." << endl;
    g_final=clock()-g_init;
    cout << endl << "Game took " << (double)g_final / ((double)CLOCKS_PER_SEC) << " s." << endl;
    return 0;
}

int main(int argc, char **argv) {
    int i = 1;
    while ((i < argc) && (argv[i][0]=='-')) {
        string sw = argv[i];
        if (sw=="-m") {
            g_auto = atoi(argv[++i]);
        } else if (sw=="-d") {
            g_searchdepth = atoi(argv[++i]);
        } else if (sw=="-f") {
            g_gofirst = false;
        } else if (sw=="-c") {
            g_clearscreen = true;
        } else if (sw=="-t") {
            g_timeout = atoi(argv[++i]);
        } else if (sw=="-T") {
            g_timemode = true;
        } else if (sw=="-v") {
            g_verbose = atoi(argv[++i]);
        } else if (sw=="-h") {
            cout << "Othello Options: " << endl <<
                "\t-m #" << endl << "\t\tSets the play mode." << endl << 
                                    "\t\t0 = manual; 1 = randomly select a move;" << endl <<
                                    "\t\t2 = always select first move; 3 = AI controlled." << endl << endl <<
                "\t-f"   << endl << "\t\tIf set, the opponent will get the first move. Otherwise, the player will go first." << endl << endl <<
                "\t-d #" << endl << "\t\tSets the AI search depth. If 0, the AI will select a depth based on the current move number." << endl <<
                                    "\t\tIf -1, the AI will randomly pick moves." << endl << endl << 
                "\t-t #" << endl << "\t\tSets the AI search timeout, in seconds. The AI will stop calculating a move if " << endl <<
                                    "\t\tit exceeds this time." << endl << endl <<
                "\t-T"   << endl << "\t\tSets AI to time-limited mode. If set, the AI will continue to increase the " << endl <<
                                    "\t\tsearch depth until it runs out of time." << endl << endl <<
                "\t-c"   << endl << "\t\tIf set, the screen will be cleared after each turn." << endl << endl <<
                "\t-v #" << endl << "\t\tSets the verbosity. A higher number will be more verbose." << endl << endl <<
                "\t-h"   << endl << "\t\tDisplays this help message." << endl;
           return 0;
        } else {
            cout << "Unknown argument: " << argv[i] << endl;
        }
        i++;
    }

    cout << "OTHELLO" << endl;
    cout << "Settings: " << endl <<
        "  Play mode:    \t"   << (g_auto == 0 ? "manual" : "automatic") << endl <<
        "  Player first: \t"   << (g_gofirst ? "Yes" : "No") << endl <<
        "  Clear screen: \t"   << (g_clearscreen ? "Yes" : "No" ) << endl <<
        "  Verbosity:    \t"   << g_verbose << endl <<
        "  AI max depth: \t"   << g_searchdepth << (g_searchdepth == 0 ? " (based on move number)" : " (fixed)" ) << endl <<
        "  AI timeout:   \t"   << g_timeout << " seconds." << endl <<
        "  AI time mode: \t"   << (g_timemode ? "Time-limited" : "Depth-limited") << endl;

    game_start();
    return 0;
}
