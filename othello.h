/* Array of direction coordinates. */
const char DIRS[8][2] = {{1, 0}, {1, 1}, {0, 1}, {-1, 1}, {-1, 0}, {-1, -1}, {0, -1}, {1, -1}};

/* Overload << for outputting a vector. */
namespace std {
    template<class A1, class A2>
        ostream& operator<<(ostream& s, vector<A1, A2> const& vec) {
            copy(vec.begin(), vec.end(), ostream_iterator<A1>(s, " "));
            return s;
        }
}

/* printMove
 *   Converts an integer representing the move to proper coordinates.
 *   Ex. 0 => a1, 1 => b1, 8 => a2, etc. */
string printMove( int move ) {
    string s;
    s += (char)(move%g_size + 97);
    s += (char)('0' + move/g_size + 1);
    return s;
}

/* Othello class
 *   Stores the game board. */
class Othello {
    public:
        typedef enum { X, B, W, P } TILE;

        TILE * board;

        Othello();

        Othello( const Othello &tiles );

        ~Othello();

        Othello & operator=( const Othello &rhs );

        void PrintBoard();
        bool IsValidMove( bool color, int tile );
        Othello MakeMove( bool color, int tile );
        void FindValidMoves( bool color, vector<int>& retVal );
        bool GetScore( bool final = false );
};

/* Othello constructor.
 *   Sets up the beginning state of the board. */
Othello::Othello():board( new TILE[g_size*g_size] ) {  
    for (int i = 0; i < g_size * g_size; i++) {
        board[i] = X;
    }
    board[27] = (g_gofirst ? B : W);
    board[28] = (g_gofirst ? W : B);
    board[35] = (g_gofirst ? W : B);
    board[36] = (g_gofirst ? B : W);
}

/* Othello copy constructor.
 *   Creates a copy of the board being passed in. */
Othello::Othello( const Othello &tiles ):board( new TILE[g_size*g_size] ){
    *this = tiles;
}                              

/* Othello destructor. */
Othello::~Othello() {
    delete [] board;
}

/* Assignment operator.
 *   Copies the board array from the rhs Object. */
Othello& Othello::operator=( const Othello &rhs ) {
    if( this != &rhs ) {
        delete [] board;
        board = new TILE[ g_size*g_size ];
        memcpy( board, rhs.board, sizeof( TILE ) * g_size * g_size );			
    }
    return *this;
}

/* PrintBoard
 *   Output the current state of the board. */
void Othello::PrintBoard() {
    cout << "  a b c d e f g h";
    for (int i = 0; i < g_size*g_size; i++) {
        if (i%g_size == 0) {
            cout << endl << i/g_size + 1 ;
        }
        char out;
        switch (board[i]) {
            case 0:
                out = '.';
                break;
            case 1:
                out = '@';
                break;
            case 2:
                out = 'O';
                break;
            case 3:
                out = '*';
                break;
        }
        cout << " " << out;
    }
}

/* IsValidMove
 *   Determines whether a given move is valid or not. */
bool Othello::IsValidMove( bool color, int tile ) {
    int match = (color ? 1 : 2),
        opp = (color ? 2 : 1);

    /* If the tile isn't empty, move is invalid. */
    if (board[tile] != 0 && board[tile] != 3) return false;

    int x = tile%g_size;
    int y = tile/g_size;
    int tx, ty;
    bool seenOpp, seenSame;

    /* Search in each direction. */
    for (int dir = 0; dir < 8; dir++) {
        tx = x;
        ty = y;
        seenOpp = false;
        seenSame = false;

        /* Continue searching until we hit an empty tile or run off the board. */
        while ( tx + DIRS[dir][0] >= 0 && ty + DIRS[dir][1] >= 0 && 
                tx + DIRS[dir][0] < g_size && ty + DIRS[dir][1] < g_size &&
                (ty + DIRS[dir][1])*g_size + tx + DIRS[dir][0] < 64 &&
                board[(ty + DIRS[dir][1])*g_size + tx + DIRS[dir][0]] != 0 &&
                board[(ty + DIRS[dir][1])*g_size + tx + DIRS[dir][0]] != 3 ) {
            tx += DIRS[dir][0];
            ty += DIRS[dir][1];

            /* Set flags to check if we've seen a tile of the same or opposite color yet. */
            if (board[ty*g_size + tx] == opp && !seenSame) {
                seenOpp = true;
            } else if (board[ty*g_size + tx] == match && seenOpp && !seenSame) {
                seenSame = true;
                break;
            }

            /* If we find a matching tile but there hasn't been an opponent's tile in between, break. */
            if (board[ty*g_size + tx] == match && !seenOpp) {
                break;
            }
        }
        /* Return true if the move is valid. */
        if (seenOpp && seenSame) {
            return true;
        }
    }

    /* Return false if the move isn't valid. */
    return false;
}

/* MakeMove
 *   Make a move and return the state of the board after the move. */
Othello Othello::MakeMove( bool color, int tile ) {
    bool valid = false;
    int match = (color ? 1 : 2), 
        opp = (color ? 2 : 1);

    /* If the tile isn't empty, do nothing. */
    if (board[tile] != 0 && board[tile] != 3) return *this;

    int x = tile%g_size;
    int y = tile/g_size;
    int tx, ty;
    bool seenOpp = false, seenSame = false;
    vector<int> toFlip, toFlipTemp;

    /* Search in each direction. */
    for (int dir = 0; dir < 8; dir++) {
        tx = x;
        ty = y;
        seenOpp = false;
        seenSame = false;

        /* Continue searching until we hit an empty tile or run off the board. */
        while ( tx + DIRS[dir][0] >= 0 && ty + DIRS[dir][1] >= 0 && 
                tx + DIRS[dir][0] < g_size && ty + DIRS[dir][1] < g_size &&
                (ty + DIRS[dir][1])*g_size + tx + DIRS[dir][0] < 64 &&
                board[(ty + DIRS[dir][1])*g_size + tx + DIRS[dir][0]] != 0 &&
                board[(ty + DIRS[dir][1])*g_size + tx + DIRS[dir][0]] != 3 ) {
            tx += DIRS[dir][0];
            ty += DIRS[dir][1];

            if (board[ty*g_size + tx] == opp && !seenSame) {
                /* If we hit an opposite-colored tile, add it to the list of pieces to be flipped. */
                seenOpp = true;
                toFlipTemp.push_back(ty*g_size + tx);
            } else if (board[ty*g_size + tx] == match && seenOpp && !seenSame) {
                seenSame = true;
                break;
            }
            if (board[ty*g_size + tx] == match && !seenOpp) {
                break;
            }
        }
        if (seenOpp && seenSame) {
            /* If the move is valid, add the temporary list of tiles to be flipped to the permanent list. */
            toFlip.insert( toFlip.end(), toFlipTemp.begin(), toFlipTemp.end());
            valid = true;
        }
        /* Reset the temporary list of tiles to be flipped. */
        toFlipTemp.erase( toFlipTemp.begin(), toFlipTemp.end() );
    }

    /* Create a copy of the current board and change all * tiles to X. */
    Othello cpy = *this;
    for (int i = 0; i < g_size*g_size; i++) {
        if (board[i] == 3) {
            cpy.board[i] = X;
        }
    }

    /* Add the current tile to the list of tiles to be flipped. */
    toFlip.push_back( tile );

    /* If the move is valid, flip all of the tiles in the list. */
    if (valid) {
        for (int i = 0; i < toFlip.size(); i++) {
            cpy.board[toFlip[i]] = (color ? B : W);
        }
    }

    return cpy;
}

/* FindValidMoves
 *   Determine what moves are valid and return the list. */
void Othello::FindValidMoves( bool color, vector<int>& retVal ) {
    vector<int> moves;
    for (int i = 0; i < g_size*g_size; i++) {
        /* Clear all * tiles. */
        if (board[i] == 3) {
            board[i] = X;
        }
        /* If a move is valid, make it a * tile and add it to the list. */
        if (IsValidMove( color, i )) {
            board[i] = P;
            moves.push_back(i);
        }
    }

    /* Return the list. */
    retVal.swap(moves);
}

/* GetScore
 *   Calculate and print the score. Returns whether the game is over.
 *   The argument final indicates whether the game is over or not. */
bool Othello::GetScore( bool final ) {
    /* Count the tiles. */
    int b = 0, w = 0;
    for ( int i = 0; i < g_size*g_size; i++) {
        if (board[i] == 1) {
            b++;
        } else if (board[i] == 2) {
            w++;
        }
    }

    cout << endl << "Score: " << b << "-" << w << endl;

    /* If the game is over, tell the user if they suck or not. */
    if (final) {
        if (b == w) {
            cout << "DROW GAME" << endl;
        } else if (w > b) {
            cout << "YOU LOSE. PLAY AGAIN" << endl;
        } else {
            cout << "YOU WIN. EXCELLENT." << endl;
        }
        cout << endl << "FINAL SCORE: " << b << "-" << w << endl;
        return false;
    }
    /* Determine whether we've run out of moves or not. */
    if (b == 0 || w == 0 || b + w == g_size*g_size) return false;
    return true;
}

class Othello;
