#include "Sudoku.h"

#include <string> /* string */
#include <cassert> /* assert */

Sudoku::Sudoku(int basesize, SymbolType stype, SUDOKU_CALLBACK callback)
	: board_(nullptr), basesize_(basesize), callback_(callback), sym_type_(stype)
{
    empty_cell_count_ = 0;
    empty_cells_.clear();
    placed_count_ = 0;
    empty_index_ = 0;
    finished_ = false;
    aborted_ = false;
    blocksize_ = basesize_ * basesize_;
	gridsize_ = blocksize_ * blocksize_;
    highest_index_ = gridsize_ - 1;
    num_peers_ = blocksize_ - 1;

    // initialize stats
	stats_.basesize = basesize;
	stats_.placed = 0;
	stats_.moves = 0;
    stats_.backtracks = 0;
}

Sudoku::~Sudoku()
{
	// delete board buffer
	delete[] board_;
}

/*init_board with size
    delete current board buffer
    allocate board buffer of size+1
    fill board buffer with EMPTY_CHAR
    null terminate board buffer
 */
void Sudoku::init_board(int size)
{
	// delete current board buffer
	delete[] board_;

	// allocate board buffer of size+1
	board_ = new char[size + 1];

	// fill board buffer with empty characters
	std::fill_n(board_, size, EMPTY_CHAR);

	// null terminate the board buffer
	board_[size] = '\0';
}

/*SetupBoard with values and size
    init board with size
    for index [0,size)
        if value at index is not '.'
            copy over value to board
 */
void Sudoku::SetupBoard(const char* values, int size)
{
	assert(size != basesize_ * basesize_);

	// initialize board buffer
	init_board(size);

	// copy values into board buffer if the value is not '.'
	for (int i = 0; i < basesize_ * basesize_; ++i)
	{
		if (values[i] != '.')
			board_[i] = values[i];
	}
}

 /*
record empty cells
    for each cell index in board
        if cell is empty at index
            add index to empty cells list
*/
void Sudoku::record_empty_cells()
{
	empty_cell_count_ = 0;
	empty_cells_.clear();
	for (int i = 0; i < gridsize_; ++i)
	{
		if (is_empty(i))
		{
			empty_cells_.push_back(i);
			++empty_cell_count_;
		}
	}
}

/*
Solve
	num of moves <- 0
	aborted <- false
	finished <- false
	empty index <- 0
	place count <- 0

	clear empty cells list

	record empty cells

	send message starting if given a callback

	place value at empty index

	send message finished fail if failed to finish and given a callback
	send message finished ok if finished and given a callback
 */
void Sudoku::Solve()
{
	if (callback_)
		callback_(*this, board_, MSG_STARTING, moves_, stats_.basesize, -1, 0, nullptr);
}

const char* Sudoku::GetBoard() const
{
	return board_;
}

Sudoku::SudokuStats Sudoku::GetStats() const
{
    return stats_;
}

/*
 place value at index
    return moved as true  if finished 
    return moved as false if aborted

    moved <- false
    real index <- get value from empty cells list at index

    peers <- get peers at real index

    for i [0, blocksize)
        update moves count
        update placed_count
        symbol <- ith symbol
        board at real index <- symbol

        if given a callback
            send message abort check
                abort <- true if callback says to
                moved <- true
                return moved
            get duplicates of symbol from peers
            send message placing with duplicate information
        
        if symbol is a duplicate among the peers
            board at real index <- EMPTY_CHAR
            moved <- false
            update placed_count
            send message removing if given callback
        else not a duplicate
            increment empty index
            if placed_count equals empty cells list size
                finished <- true
                moved <- true
                return moved
            moved <- place_value at empty index
            if finished or aborted
                return moved
            if not moved
                board at real index <- EMPTY_CHAR
                decrement empty index
                update placed count
                update backtracks
                send message removing if given a callback
    return moved
 */
bool Sudoku::place_value(int index)
{
    if (finished_)
        return true;
    if (aborted_)
		return false;

    bool moved = false;    

    return moved;
}

/*
 get peers at index
    peers <- {}
    for each cell in row for index
        if cell is not index
            peers <- peers + {peer value, peer index}
    for each cell in column for index
        if cell is not index
            peers <- peers + {peer value, peer index}
    for each cell in basesize by basesize grid for index
        if cell is not index and not in index row and not index column
            peers <- peers + {peer value, peer index}
    return peers
 */
int Sudoku::get_peers(int index, Peer* peers)
{
    return 0;
}

void Sudoku::get_empty_cells(int from)
{

}

bool Sudoku::is_empty(int index)
{
    if (board_[index] == EMPTY_CHAR)
		return true;
}

int Sudoku::next_empty_cell(int from)
{
    for (int i = from; i < gridsize_; ++i)
    {
    	if (is_empty(i))
			return i;
	}
	return -1;
}

int Sudoku::index_to_row(int index)
{
    return index / blocksize_;
}

int Sudoku::index_to_col(int index)
{
    return index % blocksize_;
}

void Sudoku::dump_peers(Peer* peers)
{
}

bool Sudoku::validate_board()
{
    return true;
}

int Sudoku::is_duplicate(char value, Peer* peers)
{
    return -1;
}

/*
 get duplicates of value from peers
    duplicates <- {-1,-1,-1}
    for each peer
        if peer value matches value
            save peer index to duplicates
    sort duplicates
    return duplicates
 */
int Sudoku::get_duplicates(char value, Peer* peers, int* dup_indexes)
{
    return 0;
}
