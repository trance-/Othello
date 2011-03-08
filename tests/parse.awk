BEGIN {
    moves = "";
    score = "";
    time = "";
}
/Score/ { score = $2; }
/Game took/ { time = $3; }
/Player moves:/ { moves = moves "@ " $3 " "; }
/Opponent moves:/ { moves = moves "O " $3 " "; }
END {
    print FILENAME;
    print "Score: " score;
    print "Time: " time;
    print "Moves: " moves;
}
