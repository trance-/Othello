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

    int eval( int color );

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
        /*if (color) {
            bx += moves.size() * 2;
            wx += movesOpp.size() * 2;
        } else {
            wx += moves.size() * 2;
            bx += movesOpp.size() * 2;
        }
        */
        /*for ( int i = 0; i < SIZE*SIZE; i++) {
            if (argBoard.board[i] == 1) {
                switch (i) {
                case 0:
                case 7:
                case 63:
                case 56:
                    bx += 20;
                    break;
                case 1:
                case 8:
                case 9:
                    if (argBoard.board[0] != 1) 
                        bx -= 5;
                    break;
                case 6:
                case 14:
                case 15:
                    if (argBoard.board[7] != 1) 
                        bx -= 5;
                    break;
                case 54:
                case 55:
                case 62:
                    if (argBoard.board[63] != 1) 
                        bx -= 5;
                    break;
                case 48:
                case 49:
                case 57:
                    if (argBoard.board[56] != 1) 
                        bx -= 5;
                    break;
                }
                b++;
            } else if (argBoard.board[i] == 2) {
                switch (i) {
                case 0:
                case 7:
                case 63:
                case 56:
                    wx += 20;
                    break;
                case 1:
                case 8:
                case 9:
                    if (argBoard.board[0] != 2) 
                        wx -= 5;
                    break;
                case 6:
                case 14:
                case 15:
                    if (argBoard.board[7] != 2) 
                        wx -= 5;
                    break;
                case 54:
                case 55:
                case 62:
                    if (argBoard.board[63] != 2) 
                        wx -= 5;
                    break;
                case 48:
                case 49:
                case 57:
                    if (argBoard.board[56] != 2) 
                        wx -= 5;
                    break;
                }
                w++;
            }
        }
        */
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
void printNode( GameTree<State>::iterator itr ) {
    for ( int i = 0; i < itr.depth(); ++i) {
        std::cout << "\t";
    }
     std::cout << "Move: " << (char)((*itr).move%SIZE + 97) << (*itr).move/SIZE + 1 << ((*itr).color ? " O" : " @")
        << " Value: " << (*itr).value << 
        "  (depth " << itr.depth() <<
        ", size " << itr.size() <<
        ", height " << itr.height() <<
        ", degree " << itr.degree() <<
        ", is root " << itr.root() <<
        ", is leaf " << itr.leaf() <<
        ")";
}
 
void print( GameTree<State>::iterator itr, int n ) {
    printNode( itr );
    cout << endl;
    for ( GameTree<State>::iterator child = itr.begin(); child != itr.end(); ++child ) {
        print( child, n + 1 );
    }
}

void treegen( GameTree<State>::iterator itr, int d, Othello board ) {
    if (d == 0) return;
    clock_t temp = clock()-init;

    if (!itr.root() && (double)temp / ((double)CLOCKS_PER_SEC) > g_timeout) 
        return; 

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

    for ( GameTree<State>::iterator child = itr.begin(); child != itr.end(); ++child ) {
        treegen( child, d - 1, board.MakeMove( !(*child).color, (*child).move ));
    }
}

pair<int, int> find_best( GameTree<State>::iterator itr, int d, int a, int b, int color) {
    search_count++;
    if (g_verbose == 1) {
        cout << endl;
        printNode(itr);
        cout << " d: " << d << " a: " << a << " b: " << b << " color: " << color;
    }
    if (itr.leaf() || d == 0) {
        return make_pair(color * (*itr).value, (*itr).move);
    } else {
        int move = (*itr.begin()).move;
        for ( GameTree<State>::iterator child = itr.begin(); child != itr.end(); ++child ) {
            pair<int, int> eval = find_best(child, d-1, -b, -a, -color);
            //cout << " : " << a << " " << eval.first;
            if (-eval.first > a) {
                move = (*child).move;
                a = -eval.first;
            }
            //a = max(a, -eval.first);
            if (a >= b) {
                break;
            }
        }
        if (g_verbose == 1) {
            cout << endl;
            for ( int i = 0; i < itr.depth(); ++i) {
                std::cout << "\t";
            }
            cout << " d: " << d << " a: " << a << " b: " << b << " move: " << (char)((*itr).move%SIZE + 97) << (*itr).move/SIZE + 1 ;
        }
        return make_pair(a, move);
    }
}

int ai_initialize( Othello board, bool color ) {
    init=clock();
    GameTree<State> tree ( State( board, color, 0 ) );
    GameTree<State>::iterator itr = tree.begin();
    tree_count = 1;
    search_count = 0;
    int depth = g_searchdepth == 0 ? searchdepth[g_turn++] : g_searchdepth;
    cout << "Depth: " << depth << endl;
    treegen( itr, depth, board );

    if (g_verbose == 2)
        print( itr, depth);

    pair<int, int> optimal;
    optimal = find_best( itr, depth, -10000, 10000, color ? -1 : 1);

    final=clock()-init;
    double totalTime = (double)final / ((double)CLOCKS_PER_SEC);
    cout << endl << "Examined " << search_count << "/" << tree_count << " game states in " << totalTime << " s" << ((totalTime > g_timeout) ? " (timed out)." : ".") << endl;
    return optimal.second;
}

