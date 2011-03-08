clock_t init, final;
int tree_count = 0;
int search_count = 0;
int searchdepth[60]=	{				
		2, 5, 4, 4, 4, 4, 4, 5, 5, 5, 5, 5, 5, 5, 5,
		5, 5, 5, 5, 5, 5, 5, 6, 6, 6, 6, 6, 6, 6, 6,
		6, 6, 6, 6, 6, 6, 6, 7, 7, 7, 7, 7, 7, 7, 7,
		6, 6, 6, 6, 6, 6, 6, 7, 7, 6, 6, 6, 5, 5, 5
	};
 
class State {
public:
    int move;
    vector<int> moves;
    int b, w, value;
    bool color;

    State( Othello & argBoard, bool argColor, int argMove, int b = 0, int w = 0);

    friend class Othello;
};

State::State( Othello & argBoard, bool argColor, int argMove, int b, int w):
    move( argMove ),
    color( argColor ),
    b( 0 ),
    w( 0 ),                        
    value( 0 ),
    moves( vector<int>() ) {
        int bx = 0, wx = 0;
        vector<int> movesOpp;
        argBoard.FindValidMoves( color, moves );
        argBoard.FindValidMoves( !color, movesOpp );
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
        weight[9] = val1[argBoard.board[0]];
        weight[1*SIZE + SIZE - 1] = val1[argBoard.board[7]];
        weight[6*SIZE + 1] = val1[argBoard.board[7*SIZE]];
        weight[6*SIZE + SIZE - 1] = val1[argBoard.board[63]];
        for (int i = 0; i < SIZE*SIZE; i++) {
            if (argBoard.board[i] == 1) {
                bx += weight[i];
                b++;
            } else if (argBoard.board[i] == 2) {
                wx += weight[i];
                w++;
            }
        }
        if (b == 64) bx += 500;
        if (w == 64) wx += 500;
        value = wx - bx;
    }


using namespace Data_structures;
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
 
pair<int, int> gensearch( GameTree<State>::iterator itr, Othello board, int d, int a, int b, int color) {
    search_count++;
    if (g_verbose == 1) {
        cout << endl;
        printNode(itr, color);
        cout << " d: " << d << " a: " << a << " b: " << b << " color: " << color;
    }
    clock_t temp = clock() - init;
    if (d == 0 || (double)temp / ((double)CLOCKS_PER_SEC) > g_timeout) return make_pair(color*(*itr).value, (*itr).move);

    for (int i = 0; i < (*itr).moves.size(); i++) {
        Othello tmp (board.MakeMove( (*itr).color, (*itr).moves[i] ));
        State newState ( tmp, !(*itr).color, (*itr).moves[i] );
        itr.insert( newState );
        tree_count++;
    }
    if ((*itr).moves.size() == 0) {
        Othello tmp (board.MakeMove( (*itr).color, 0 ));
        State newState ( tmp, !(*itr).color, 0 );
        itr.insert( newState );
        tree_count++;
    }

    int move = (*itr.begin()).move;
    for ( GameTree<State>::iterator child = itr.begin(); child != itr.end(); ++child ) {
        if (g_verbose == 1) {
            cout << endl;
            for ( int i = 0; i < itr.depth()+1; ++i) {
                std::cout << "\t";
            }
            cout << "Entering " << printMove((*child).move) << ": a = " << a << " b = " << b ;
        }

        pair<int, int> eval = gensearch(child, board.MakeMove( !(*child).color, (*child).move), d-1, -b, -a, -color);

        if (g_verbose == 1) {
            cout << endl;
            for ( int i = 0; i < itr.depth()+1; ++i) {
                std::cout << "\t";
            }
            cout << "Leaving " << printMove((*child).move) << ": ret = " << eval.first << " a = " << a << " b = " << b << endl;
        }

        if (-eval.first > a) {
            move = (*child).move;
            a = -eval.first;
        }
        if (a >= b) {
            break;
        }
    }

    if (g_verbose == 1) {
        cout << endl;
        for ( int i = 0; i < itr.depth(); ++i) {
            std::cout << "\t";
        }
        cout << " d: " << d << " a: " << a << " b: " << b << " move: " << printMove((*itr).move);
    }

    return make_pair(a, move);
}

pair<int, int> timesearch( GameTree<State>::iterator itr, Othello board, int color) {
    clock_t temp = clock() - init;
    int d = 1;
    pair<int, int> optimal;
    double curtime = (double)temp / ((double)CLOCKS_PER_SEC);
    while (curtime < g_timeout) {
        cout <<"Time remaining. Increasing depth to " << d + 1 << endl;
        pair<int, int> tempmove = gensearch( itr, board, d++, -1000, 1000, color);
        temp = clock() - init;
        curtime = (double)temp / ((double)CLOCKS_PER_SEC);
        if (curtime < g_timeout) {
            optimal = tempmove;
        }
    }
    return optimal;
}

int ai_initialize( Othello board, bool color ) {
    init=clock();
    tree_count = 1;
    search_count = 0;

    GameTree<State> tree ( State( board, color, 0 ) );
    GameTree<State>::iterator itr = tree.begin();

    int depth = g_searchdepth == 0 ? searchdepth[g_turn++] : g_searchdepth;

    pair<int, int> optimal;
    if (!g_timemode) {
        cout << "Depth: " << depth << endl;
        optimal = gensearch( itr, board, depth, -1000, 1000, color ? -1 : 1);
    } else {
        optimal = timesearch( itr, board, color ? -1 : 1);
    }

    final=clock()-init;
    double totalTime = (double)final / ((double)CLOCKS_PER_SEC);

    cout << endl << "Examined " << search_count << "/" << tree_count << " game states in " << totalTime << " s" << ((totalTime > g_timeout && !g_timemode) ? " (timed out)." : ".") << endl;

    return optimal.second;
}

