//---------------------------------------------------------------------------
#ifndef SUDOKUH
#define SUDOKUH
//---------------------------------------------------------------------------
#include <cstddef> /* size_t */
#include <list>   /* list */

//! The Sudoku class
class Sudoku
{
public:
    //! Used by the callback function 
    enum MessageType
    {
        MSG_STARTING,      //!< the board is setup, ready to go
        MSG_FINISHED_OK,   //!< finished and found a solution
        MSG_FINISHED_FAIL, //!< finished but no solution found
        MSG_ABORT_CHECK,   //!< checking to see if algorithm should continue
        MSG_PLACING,       //!< placing a symbol on the board
        MSG_REMOVING       //!< removing a symbol (back-tracking)
    };

    //! 1-9 for 9x9, A-P for 16x16, A-Y for 25x25
    enum SymbolType { SYM_NUMBER, SYM_LETTER };

    //! Represents an empty cell (the driver will use a . instead)
    const static char EMPTY_CHAR = ' ';

    //! Implemented in the client and called during the search for a solution
    typedef bool (*SUDOKU_CALLBACK)
        (const Sudoku& sudoku, // the gameboard object itself
         const char* board,    // one-dimensional array of symbols
         MessageType message,  // type of message
         size_t move,          // the move number
         int basesize,         // 3, 4, 5, etc. (for 9x9, 16x16, 25x25, etc.)
         int index,            // index of current cell
         char value,           // symbol (value) in current cell
         int* dup_indexes      // index of each duplicate sorted from lowest
         // index to highest. -1 means no conflict.
         // max of 3 duplicates.
         );

    //! Statistics as the algorithm works
    struct SudokuStats
    {
        int basesize;      //!< 3, 4, 5, etc.
        int placed;        //!< the number of valid values the algorithm has placed 
        size_t moves;      //!< total number of values that have been tried
        size_t backtracks; //!< total number of times the algorithm backtracked

        //!< Default constructor
        SudokuStats() : basesize(0), placed(0), moves(0), backtracks(0) {}
    };

    // Constructor
    Sudoku(int basesize, SymbolType stype = SYM_NUMBER, SUDOKU_CALLBACK callback = 0);

    // Destructor
    ~Sudoku();

    // The client (driver) passed the board in the values parameter
    void SetupBoard(const char* values, int size);

    // Once the board is setup, this will start the search for the solution
    void Solve();

    // For debugging with the driver
    const char* GetBoard() const;
    SudokuStats GetStats() const;

private:
    char* board_;           //!< The 1-D representation of the board
    std::list<int> empty_cells_;         //!< An array of empty cells remaining
    int empty_cell_count_;     //!< The number of empty cells remaining
    int placed_count_;         //!< How many values have been placed
    int empty_index_;          //!< Current index into the empty cell array
    bool finished_;            //!< The algorithm has finished
    bool aborted_;             //!< The algorithm was terminated by the client
    int highest_index_;        //!< The largest index on the board
    int basesize_;             //!< The size of the side of a block (e.g. 3, 4, 5)
    int blocksize_;            //!< The size of a block (e.g. 9, 16, 25)
    int gridsize_;             //!< The size of the grid (e.g. 81, 256, 625_)
    int num_peers_;            //!< The number of peers (e.g. 8, 15, 24)
    SUDOKU_CALLBACK callback_; //!< The callback to communicate with the client
    SymbolType sym_type_;      //!< Numbers (1-9) or letters (A-P, A-Y)
    SudokuStats stats_;        //!< Statistics about the algorithm
    char base_symbol_;         //!< The value of the starting symbol (e.g. 1 or 65)
    size_t moves_;             //!< The number of total attempts
    size_t backtracks_;        //!< The number of times the algorithm backtracked


    /*! Peers are all values in the row, column, and neighborhood of a given cell.
        Makes it easier to determine if there is a duplicate
     */
    struct Peer
    {
        char value; //!< The value in the cell
        int index;  //!< The (0-based) index of the cell
    };

    // index is a 1-D index in the 2-D board

    void record_empty_cells();
    bool place_value(int index);            // Function that does the actual recursion
    void init_board(int size);              // Allocate and initialize the array (board)
    int get_peers(int index, Peer* peers);  // Gets all of the peers of 'index'
    void get_empty_cells(int from);         // Gets all empty cells from 'from' to end of board (populates empty_cells_ array)
    bool is_empty(int index);               // Checks if cell at 'index' is empty
    int next_empty_cell(int from);          // Find next empty cell starting from 'from'
    int index_to_row(int index);            // Gets the row from 'index'
    int index_to_col(int index);            // Gets the column from 'index'
    void dump_peers(Peer* peers);           // Debug: Function to print out peers
    bool validate_board();                  // Debug: Checks that every value on the board is valid (no duplicates)
    int is_duplicate(char value, Peer* peers); // Checks if 'value' is in 'peers' (duplicate)
    int get_duplicates(char value, Peer* peers, int* dup_indexes); // Checks if 'value' is in peers and popluates 'dup_indexes' array
};

#endif  // SUDOKUH