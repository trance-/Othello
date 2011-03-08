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
    int game_start();
};

Othello::Othello():board( new TILE[SIZE*SIZE] ) {  
    for (int i = 0; i < SIZE * SIZE; i++) {
        board[i] = X;
    }
    board[27] = B;
    board[28] = W;
    board[35] = W;
    board[36] = B;
}

Othello::Othello( const Othello &tiles ):board( new TILE[SIZE*SIZE] ){
    *this = tiles;
}                              

Othello::~Othello() {
    delete [] board;
}

Othello& Othello::operator=( const Othello &rhs ) {
    if( this != &rhs ) {
        delete [] board;
        board = new TILE[ SIZE*SIZE ];
        memcpy( board, rhs.board, sizeof( TILE ) * SIZE * SIZE );			
    }
    return *this;
}

void Othello::PrintBoard() {
    cout << "  a b c d e f g h";
    for (int i = 0; i < SIZE*SIZE; i++) {
        if (i%SIZE == 0) {
            cout << endl << i/SIZE + 1 ;
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

bool Othello::IsValidMove( bool color, int tile ) {
    int match = 1, opp = 2;
    if (!color) {
        match = 2;
        opp = 1;
    }

    if (board[tile] != 0 && board[tile] != 3) return false;

    int x = tile%SIZE;
    int y = tile/SIZE;
    int tx, ty;
    bool seenOpp, seenSame;

    for (int dir = 0; dir < 8; dir++) {
        tx = x;
        ty = y;
        seenOpp = false;
        seenSame = false;
        while (tx + DIRS[dir][0] >= 0 && ty + DIRS[dir][1] >= 0 && tx + DIRS[dir][0] < SIZE && ty + DIRS[dir][1] < SIZE &&
               (ty + DIRS[dir][1])*SIZE + tx + DIRS[dir][0] < 64 &&
               (board[(ty + DIRS[dir][1])*SIZE + tx + DIRS[dir][0]] != 0 &&
                board[(ty + DIRS[dir][1])*SIZE + tx + DIRS[dir][0]] != 3)) {
            tx += DIRS[dir][0];
            ty += DIRS[dir][1];
            if (board[ty*SIZE + tx] == opp && !seenSame) {
                seenOpp = true;
            } else if (board[ty*SIZE + tx] == match && seenOpp && !seenSame) {
                seenSame = true;
                break;
            }
            if (board[ty*SIZE + tx] == match && !seenOpp) {
                break;
            }
        }
        if (seenOpp && seenSame) {
            return true;
        }
    }

    return false;
}

/* Make a move and return the state of the board after the move. */
Othello Othello::MakeMove( bool color, int tile ) {
    bool valid = false;
    int match = 1, opp = 2;
    if (!color) {
        match = 2;
        opp = 1;
    }

    if (board[tile] != 0 && board[tile] != 3) return *this;

    int x = tile%SIZE;
    int y = tile/SIZE;
    int tx, ty;
    bool seenOpp = false, seenSame = false;
    vector<int> toFlip, toFlipTemp;

    for (int dir = 0; dir < 8; dir++) {
        tx = x;
        ty = y;
        seenOpp = false;
        seenSame = false;
        while (tx + DIRS[dir][0] >= 0 && ty + DIRS[dir][1] >= 0 && tx + DIRS[dir][0] < SIZE && ty + DIRS[dir][1] < SIZE &&
               (ty + DIRS[dir][1])*SIZE + tx + DIRS[dir][0] < 64 &&
               (board[(ty + DIRS[dir][1])*SIZE + tx + DIRS[dir][0]] != 0 &&
                board[(ty + DIRS[dir][1])*SIZE + tx + DIRS[dir][0]] != 3)) {
            tx += DIRS[dir][0];
            ty += DIRS[dir][1];
            if (board[ty*SIZE + tx] == opp && !seenSame) {
                seenOpp = true;
                toFlipTemp.push_back(ty*SIZE + tx);
            } else if (board[ty*SIZE + tx] == match && seenOpp && !seenSame) {
                seenSame = true;
                break;
            }
            if (board[ty*SIZE + tx] == match && !seenOpp) {
                break;
            }
        }
        if (seenOpp && seenSame) {
            toFlip.insert( toFlip.end(), toFlipTemp.begin(), toFlipTemp.end());
            valid = true;
        }
        toFlipTemp.erase( toFlipTemp.begin(), toFlipTemp.end() );
    }

    Othello cpy = *this;
    for (int i = 0; i < SIZE*SIZE; i++) {
        if (board[i] == 3) {
            cpy.board[i] = X;
        }
    }

    if (valid) {
        for (int i = 0; i < toFlip.size(); i++) {
            if (color) {
                cpy.board[toFlip[i]] = B;
            } else {
                cpy.board[toFlip[i]] = W;
            }
        }

        if (color) {
            cpy.board[tile] = B;
        } else {
            cpy.board[tile] = W;
        }
    }

    return cpy;
}

/* Determine what moves are valid and return the list. */
void Othello::FindValidMoves( bool color, vector<int>& retVal ) {
    vector<int> moves;
    for (int i = 0; i < SIZE*SIZE; i++) {
        if (board[i] == 3) {
            board[i] = X;
        }
        if (IsValidMove( color, i )) {
            board[i] = P;
            moves.push_back(i);
        }
    }
    retVal.swap(moves);
}

/* Calculate and print the score. */
bool Othello::GetScore( bool final ) {
    int b = 0, w = 0;
    for ( int i = 0; i < SIZE*SIZE; i++) {
        if (board[i] == 1) {
            b++;
        } else if (board[i] == 2) {
            w++;
        }
    }
    cout << endl << "Score: " << b << "-" << w << endl;
    if (b + w == SIZE*SIZE) return false;
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
    return true;
}

