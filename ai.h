/* Timers uses to handle time-limited search. */
clock_t init, final;

/* Counters used to store search statistics. */
int tree_count = 0;
int search_count = 0;

/* Array of search depths, used to determine which search depth to use
 * based on the current turn if no depth is specified in the arguments. */
int searchdepth[60]=	{				
		2, 5, 4, 4, 4, 4, 4, 5, 5, 5, 5, 5, 5, 5, 5,
		5, 5, 5, 5, 5, 5, 5, 6, 6, 6, 6, 6, 6, 6, 6,
		6, 6, 6, 6, 6, 6, 6, 7, 7, 7, 7, 7, 7, 7, 7,
		6, 6, 6, 6, 6, 6, 6, 7, 7, 6, 6, 6, 5, 5, 5
	};
 
/* State class
 *   Stores the calculated value for a given state of the game, and a 
 *   vector containing the possible moves. */
class State {
public:
    int move;
    vector<int> moves;
    int b, w, value;
    bool color;

    State( Othello & argBoard, bool argColor, int argMove, int b = 0, int w = 0);

    friend class Othello;
};

/* State constructor
 *   Calculates the value of the given game state based on the board passed in via the arguments.
 *   Also determines the possible moves that can be made. */
State::State( Othello & argBoard, bool argColor, int argMove, int b, int w):
    move( argMove ), color( argColor ), b( 0 ), w( 0 ), value( 0 ), moves( vector<int>() ) {
        int bx = 0, wx = 0;
        // Determine what moves can be made
        argBoard.FindValidMoves( color, moves );

        // Set up weights for determining value
        static int
            val1[3]={-30,0,0},	/* value of (1,1) if (0,0)=0,1,2 */
            val2[3]={-4,0,0},	/* value of (1,2) if (0,2)=0,1,2 */
            val3[3]={-5,0,0};	/* value of (1,3) if (0,3)=0,1,2 */
        int v1 = val1[argBoard.board[0]];
        int v2 = val1[argBoard.board[7]];
        int v3 = val1[argBoard.board[55]];
        int v4 = val1[argBoard.board[63]];
        int v5 = val2[argBoard.board[2]];
        int v6 = val2[argBoard.board[6]];
        int v7 = val2[argBoard.board[15]];
        int v8 = val2[argBoard.board[22]];
        int v9 = val2[argBoard.board[39]];
        int v10 = val2[argBoard.board[46]];
        int v11 = val2[argBoard.board[58]];
        int v12 = val2[argBoard.board[61]];
        int v13 = val3[argBoard.board[3]];
        int v14 = val3[argBoard.board[4]];
        int v15 = val3[argBoard.board[23]];
        int v16 = val3[argBoard.board[32]];
        int v17 = val3[argBoard.board[31]];
        int v18 = val3[argBoard.board[40]];
        int v19 = val3[argBoard.board[59]];
        int v20 = val3[argBoard.board[60]];

        int weight[SIZE*SIZE] = { 
            50,  4, 16, 12, 12, 16,  4, 50,
            4,  v1, v5,v13,v14, v6, v2,  4,
            16, v7,  1,  0,  0,  1, v8, 16,		
            12,v15,  0,  0,  0,  0,v16, 12,		
            12,v17,  0,  0,  0,  0,v18, 12,		
            16, v9,  1,  0,  0,  1,v10, 16,		
            4,  v3,v11,v19,v20,v12, v4,  4,
            50,  4, 16, 12, 12, 16,  4, 50
        };

        /* Count the pieces. */
        for (int i = 0; i < SIZE*SIZE; i++) {
            if (argBoard.board[i] == 1) {
                bx += weight[i];
                b++;
            } else if (argBoard.board[i] == 2) {
                wx += weight[i];
                w++;
            }
        }

        /* Ensure maximal win by throwing off weighting for perfect game. */
        if (b == 64) bx += 500;
        if (w == 64) wx += 500;

        /* Calculate value. +ve value good for opponent, -ve value good for player. */
        value = wx - bx;
    }

/* printNode
 *   Prints details about the node pointed to by itr. */
void printNode( GameTree<State>::iterator itr, int color ) {
    for ( int i = 0; i < itr.depth(); ++i) {
        std::cout << "\t";
    }
    std::cout << "Move: " << printMove((*itr).move) << ((*itr).color ? " O" : " @")
        << " Value: " << color*(*itr).value << 
        "  (depth " << itr.depth() <<
        ", size " << itr.size() <<
        ", height " << itr.height() <<
        ", degree " << itr.degree() <<
        ", is root " << itr.root() <<
        ", is leaf " << itr.leaf() <<
        ")";
}

/* gensearch
 *   Generates a tree of game states recursively and determines the best state.
 *   It returns a pair of (score, move) for the optimal move.
 *   It also keeps track of time, and terminates the search if it has exceeded the timeout. */
pair<int, int> gensearch( GameTree<State>::iterator itr, Othello board, int d, int a, int b, int color) {
    search_count++;

    /* Print the current node if verbosity is set. */
    if (g_verbose == 1) {
        cout << endl;
        printNode(itr, color);
        cout << " d: " << d << " a: " << a << " b: " << b << " color: " << color;
    }

    /* Initialize the clock. */
    clock_t temp = clock() - init;

    /* If at a leaf or out of time, return the value of the current node. */
    if (d == 0 || (double)temp / ((double)CLOCKS_PER_SEC) > g_timeout) return make_pair(color*(*itr).value, (*itr).move);

    /* Otherwise, create children for each possible move. */
    for (int i = 0; i < (*itr).moves.size(); i++) {
        Othello tmp (board.MakeMove( (*itr).color, (*itr).moves[i] ));
        State newState ( tmp, !(*itr).color, (*itr).moves[i] );
        itr.insert( newState );
        tree_count++;
    }
    /* If there are no possible moves, create a state for "pass". */
    if ((*itr).moves.size() == 0) {
        Othello tmp (board.MakeMove( (*itr).color, 0 ));
        State newState ( tmp, !(*itr).color, 0 );
        itr.insert( newState );
        tree_count++;
    }

    int move = (*itr.begin()).move;
    for ( GameTree<State>::iterator child = itr.begin(); child != itr.end(); ++child ) {
        /* Print information about the tree traveral if verbosity is set. */
        if (g_verbose == 1) {
            cout << endl;
            for ( int i = 0; i < itr.depth()+1; ++i) {
                std::cout << "\t";
            }
            cout << "Entering " << printMove((*child).move) << ": a = " << a << " b = " << b ;
        }

        /* Recursively call the function on the children. */
        pair<int, int> eval = gensearch(child, board.MakeMove( !(*child).color, (*child).move), d-1, -b, -a, -color);

        /* Print information about the tree traveral if verbosity is set. */
        if (g_verbose == 1) {
            cout << endl;
            for ( int i = 0; i < itr.depth()+1; ++i) {
                std::cout << "\t";
            }
            cout << "Leaving " << printMove((*child).move) << ": ret = " << eval.first << " a = " << a << " b = " << b << endl;
        }

        /* If the child has a better score than the current max, set it to the new best. */
        if (-eval.first > a) {
            move = (*child).move;
            a = -eval.first;
        }

        /* If we've exceeded the worst-case scenario, there is no need to search the rest of the children. */
        if (a >= b) {
            break;
        }
    }

    /* Print some more shit if verbosity is set. */
    if (g_verbose == 1) {
        cout << endl;
        for ( int i = 0; i < itr.depth(); ++i) {
            std::cout << "\t";
        }
        cout << " d: " << d << " a: " << a << " b: " << b << " move: " << printMove((*itr).move);
    }

    /* Return the best move. */
    return make_pair(a, move);
}

/* timesearch
 *   Does a time-limited search. It starts with a depth of 1, and continues to increase the depth
 *   and repeat the search until it runs out of time. */
pair<int, int> timesearch( GameTree<State>::iterator itr, Othello board, int color) {
    /* Initialize the clock. */
    clock_t temp = clock() - init;

    int d = (g_shrinkwindow ? 1 : 1);
    pair<int, int> optimal;
    
    int window = 1000;

    double curtime = (double)temp / ((double)CLOCKS_PER_SEC);
    while (curtime < g_timeout) { /* Loop until we run out of time. */
        /* Run the search again and increase depth. */
        pair<int, int> tempmove = gensearch( itr, board, d++, -window, 1000, color);

        temp = clock() - init;
        curtime = (double)temp / ((double)CLOCKS_PER_SEC);

        /* Only update the optimal move if the previous search finished. */
        if (curtime < g_timeout) { 
            optimal = tempmove;

            /* If shrink window is enabled, set the cutoff for the score to the current best, so we only search
             * branches of the tree if they are better than the current best. */
            if (g_shrinkwindow) {
                window = min(window, abs(tempmove.first));
                if (g_verbose >= 1) 
                    cout << "Time remaining. Increasing depth to " << d << " and shrinking window to " << window << "." << endl;
            } else {
                if (g_verbose >= 1) 
                    cout << "Time remaining. Increasing depth to " << d << "." << endl;
            }
        }
    }
    cout << "Final search depth: " << (g_shrinkwindow ? d - 2 : d - 1) << endl;
    return optimal;
}

/* ai_initialize
 *   Run the AI on the current game state, and return the optimal move. */
int ai_initialize( Othello board, bool color ) {
    /* Reset clock and counters. */
    init=clock();
    tree_count = 1;
    search_count = 0;

    /* Initialize the tree with the current game state and create the iterator. */
    GameTree<State> tree ( State( board, color, 0 ) );
    GameTree<State>::iterator itr = tree.begin();

    int depth = g_searchdepth == 0 ? searchdepth[g_turn++] : g_searchdepth;

    pair<int, int> optimal;
    if (!g_timemode) { /* Depth-limited search. */
        cout << "Depth: " << depth << endl;
        optimal = gensearch( itr, board, depth, -1000, 1000, color ? -1 : 1);
    } else {           /* Time-limited search. */
        optimal = timesearch( itr, board, color ? -1 : 1);
    }

    final=clock()-init;
    double totalTime = (double)final / ((double)CLOCKS_PER_SEC);

    cout << endl << "Examined " << search_count << "/" << tree_count << " game states in " << totalTime << " s" << ((totalTime > g_timeout && !g_timemode) ? " (timed out)." : ".") << endl;

    return optimal.second;
}

