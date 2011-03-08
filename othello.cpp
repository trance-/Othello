#include <stdlib.h>

#include <string.h>
#include <iostream>
#include <time.h>

#include <vector>
#include <iterator>

#include "gametree.h"

/* Dimensions of the game board. */
#define SIZE (8)

/* Global variables. */
int g_auto = 0;
int g_searchdepth = 0;
bool g_gofirst = true;
bool g_clearscreen = false;
int g_turn = 0;
int g_verbose = 0;
double g_timeout = 30;
bool g_timemode = false;
bool g_shrinkwindow = true;

using namespace std;
using namespace Data_structures;
#include "othello.h"

class Othello;
#include "ai.h"


/* clear_screen
 *   Clears the console. */
void clear_screen() {
    if (g_clearscreen) {
#ifdef WINDOWS
        system ( "CLS" );
#else
        system ( "clear" );
#endif
    }
}

/* game_start
 *   The main function. */
int game_start() {
    /* Create game board. */
    Othello board;

    cout << "Enter moves in form xy (no space)." << endl;

    /* Reset variables. */
    bool color = g_gofirst;
    g_turn = 0;
    clock_t g_init, g_final;
    g_init=clock();

    /* Initialize random seed. */
    srand ( time(NULL) );

    /* Determine moves for each player. */
    vector<int> movesW, movesB;
    board.FindValidMoves( true, movesW );
    board.FindValidMoves( false, movesB );

    /* Main game loop. */
    while (movesW.size() + movesB.size() > 0) {
        cout << endl;
        board.FindValidMoves( true, movesW );
        board.FindValidMoves( false, movesB );

        vector<int> moves;
        board.FindValidMoves( color, moves );

        /* Print the current state of the board. */
        board.PrintBoard();

        /* If the game isn't over, print the score. */
        if (board.GetScore()) {

            /* Print the available moves. */
            if (moves.size() > 0) {
                cout << endl << "Valid moves: "; 
                for ( int i = 0; i < moves.size(); i++) {
                    cout << printMove(moves[i]) << "  ";
                }
                cout << endl;
            }

            int pos = 0;

            if (color) {   /* Player's move. (@) */
                if (moves.size() > 0) { /* Make sure a move can be made. */
                    if (g_auto == 0) {  /* If in manual mode, get input from user. */
                        bool moveGood = false;
                        do { /* Wait until the user inputs a valid move. */
                            cout << endl;
                            string input = "";
                            
                            /* Get the move and convert it to coordinates. */
                            getline(cin, input);
                            int x = (int)input[0] - 97;
                            int y = (int)input[1] - 49;

                            pos = y*SIZE + x;

                            /* Check if the move is valid. */
                            if (board.board[pos] != 3) {
                                cout << endl << "Invalid move." << endl;
                            } else {
                                moveGood = true;
                            }
                        } while (!moveGood);
                    } else if (g_auto == 1) {  /* Random mode, just pick a random move. */
                        pos = moves[rand() % moves.size()];
                        cout << endl << "Player moves: " << printMove(pos) << endl;
                    } else if (g_auto == 2) {  /* Fixed move, just pick the first available move. */
                        pos = moves[0];
                        cout << endl << "Player moves: " << printMove(pos) << endl;
                    } else {                   /* AI mode, determine optimal move using search. */
                        pos = ai_initialize( board, true );
                        cout << endl << "Player moves: " << printMove(pos) << endl;
                    }

                    /* Clear the screen. */
                    clear_screen();
                }
            } else {                          /* Computer's move. (O) */
                if (moves.size() > 0) {       /* Make sure a move can be made. */
                    if (g_searchdepth < 0) {  /* Random mode if depth is negative. */
                        pos = moves[rand() % moves.size()];
                    } else {                  /* Otherwise, call AI. */
                        pos = ai_initialize( board, false );
                    }
                    cout << endl << "Opponent moves: " << printMove(pos) << endl;
                }
            }

            /* Check if the move is valid. */
            if (board.IsValidMove( color, pos )) {
                /* Update the board. */
                Othello tmpBoard ( board.MakeMove( color, pos ));
                board = tmpBoard;
            } else {
                if (moves.size() > 0) { /* WTF? This shouldn't happen. */
                    cout << "Fuck." << endl;
                }
            }
            /* Switch the color for the next move. */
            color = !color;
        }
    }

    /* Show the final score. */
    board.GetScore( true );

    g_final=clock()-g_init;
    cout << "Game over." << endl;
    cout << endl << "Game took " << (double)g_final / ((double)CLOCKS_PER_SEC) << " s." << endl;
    return 0;
}

/* halp
 *   Display the halp message. */
void halp() {
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
        "\t-w"   << endl << "\t\tDisables shrinking search window in time-limited search mode." << endl <<
        "\t-c"   << endl << "\t\tIf set, the screen will be cleared after each turn." << endl << endl <<
        "\t-v #" << endl << "\t\tSets the verbosity. A higher number will be more verbose." << endl << endl <<
        "\t-h"   << endl << "\t\tDisplays this help message." << endl;
}

/* main
 *   Parse command-line args and set global variables. */
int main(int argc, char **argv) {
    int i = 1;
    while ((i < argc) && (argv[i][0]=='-')) {
        string sw = argv[i];
        if (sw=="-m") {         /* Mode. */
            g_auto = atoi(argv[++i]);
        } else if (sw=="-d") {  /* Search depth. */
            g_searchdepth = atoi(argv[++i]);
        } else if (sw=="-f") {  /* Go first. */
            g_gofirst = false;
        } else if (sw=="-c") {  /* Clear screen between moves. */
            g_clearscreen = true;
        } else if (sw=="-t") {  /* AI search timeout. */
            g_timeout = atoi(argv[++i]);
        } else if (sw=="-T") {  /* AI time-limited mode. */
            g_timemode = true;
        } else if (sw=="-w") {  /* Shrink search window. */
            g_shrinkwindow = false;
        } else if (sw=="-v") {  /* Message verbosity. */
            g_verbose = atoi(argv[++i]);
        } else if (sw=="-h") {  /* Display help message. */
            halp();
            return 0;
        } else {                /* The user fucked up. */
            cout << "Unknown argument: " << argv[i] << endl;
            halp();
            return 0;
        }
        i++;
    }
    if (i < argc && argv[i][0] != '-') {
        cout << "Unknown argument: " << argv[i] << endl;
        halp();
        return 0;
    }

    /* Display the current settings. */
    cout << "OTHELLO" << endl;
    cout << "Settings: " << endl <<
        "  Play mode:    \t"   << (g_auto == 0 ? "manual" : "automatic") << endl <<
        "  Player first: \t"   << (g_gofirst ? "Yes" : "No") << endl <<
        "  Clear screen: \t"   << (g_clearscreen ? "Yes" : "No" ) << endl <<
        "  Verbosity:    \t"   << g_verbose << endl <<
        "  AI max depth: \t"   << g_searchdepth << (g_searchdepth == 0 ? " (based on move number)" : " (fixed)" ) << endl <<
        "  AI timeout:   \t"   << g_timeout << " seconds." << endl <<
        "  AI time mode: \t"   << (g_timemode ? "Time-limited" : "Depth-limited") << endl <<
        "  Shrink win.:  \t"   << (g_shrinkwindow ? "Yes" : "No") << endl;

    /* Start the game. */
    return game_start();
}
