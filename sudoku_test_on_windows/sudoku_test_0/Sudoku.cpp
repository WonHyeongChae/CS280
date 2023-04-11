#include "Sudoku.h"

#include <string> /* string */
#include <cassert> /* assert */
#include <iostream> /* cout, endl */
#include <algorithm>

Sudoku::Sudoku(int basesize, SymbolType stype, SUDOKU_CALLBACK callback)
	: board_(nullptr), basesize_(basesize), callback_(callback), sym_type_(stype)
{
    empty_cell_count_ = 0;
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

void Sudoku::Solve()
{
    if (board_ == nullptr)
        return;

    moves_ = 0;
    backtracks_ = 0;
    placed_count_ = 0;
    finished_ = false;
    aborted_ = false;

    int count = 0;
    for (int i = 0; i < gridsize_; ++i)
    {
        if (board_[i] == EMPTY_CHAR)
        {
            ++count;
        }
        else
        {
            ++placed_count_;
        }
    }

    empty_cell_count_ = count;
    empty_cells_ = new int[empty_cell_count_];
    get_empty_cells(0);

    if (callback_)
        callback_(*this, board_, MSG_STARTING, moves_, stats_.basesize, -1, 0, nullptr);

    place_value(0);

    if (callback_)
        callback_(*this, board_, MSG_FINISHED_OK, moves_, stats_.basesize, -1, 0, nullptr);

    delete[] empty_cells_;
}

const char* Sudoku::GetBoard() const
{
	return board_;
}

Sudoku::SudokuStats Sudoku::GetStats() const
{
    return stats_;
}

bool Sudoku::place_value(int index)
{
    bool moved = false;
    int real_index = empty_cells_[index];

    Peer* peers = new Peer[blocksize_];
    get_peers(real_index, peers);

    for (int i = 0; i < blocksize_; ++i)
    {
        moves_++;
        placed_count_++;
        char symbol = base_symbol_ + i;
        board_[real_index] = symbol;

        if (callback_)
        {
            int dup_indexes[3];
            int duplicates = get_duplicates(symbol, peers, dup_indexes);
            bool abort = callback_(*this, board_, MSG_ABORT_CHECK, moves_, basesize_, real_index, symbol, dup_indexes);
            if (abort)
            {
                aborted_ = true;
                moved = true;
                return moved;
            }
            callback_(*this, board_, MSG_PLACING, moves_, basesize_, real_index, symbol, dup_indexes);
        }

        if (is_duplicate(symbol, peers) == -1)
        {
            board_[real_index] = EMPTY_CHAR;
            moved = false;
            placed_count_--;

            if (callback_)
                callback_(*this, board_, MSG_REMOVING, moves_, basesize_, real_index, symbol, nullptr);
        }
        else
        {
            empty_index_++;

            if (placed_count_ == empty_cell_count_)
            {
                finished_ = true;
                moved = true;
                return moved;
            }

            moved = place_value(empty_index_);

            if (finished_ || aborted_)
            {
                return moved;
            }

            if (!moved)
            {
                board_[real_index] = EMPTY_CHAR;
                empty_index_--;
                placed_count_--;
                backtracks_++;

                if (callback_)
                    callback_(*this, board_, MSG_REMOVING, moves_, basesize_, real_index, symbol, nullptr);
            }
        }
    }

    stats_.moves = moves_;
    stats_.backtracks = backtracks_;
    stats_.placed = placed_count_;

    return moved;
}

int Sudoku::get_peers(int index, Peer* peers)
{
    int row = index_to_row(index);
    int col = index_to_col(index);
    int count = 0;

    // Peers in the same row
    for (int c = 0; c < blocksize_; ++c)
    {
        if (c != col)
        {
            peers[count].value = board_[row * blocksize_ + c];
            peers[count].index = row * blocksize_ + c;
            ++count;
        }
    }

    // Peers in the same column
    for (int r = 0; r < blocksize_; ++r)
    {
        if (r != row)
        {
            peers[count].value = board_[r * blocksize_ + col];
            peers[count].index = r * blocksize_ + col;
            ++count;
        }
    }

    // Peers in the same subgrid
    int subgrid_row = row / basesize_ * basesize_;
    int subgrid_col = col / basesize_ * basesize_;

    for (int r = subgrid_row; r < subgrid_row + basesize_; ++r)
    {
        for (int c = subgrid_col; c < subgrid_col + basesize_; ++c)
        {
            if (r != row && c != col)
            {
                peers[count].value = board_[r * blocksize_ + c];
                peers[count].index = r * blocksize_ + c;
                ++count;
            }
        }
    }

    return count;
}



void Sudoku::get_empty_cells(int from)
{
    empty_cell_count_ = 0;

    for (int i = from; i < gridsize_; ++i)
    {
	    if (is_empty(i))
        {
	    	empty_cells_[empty_cell_count_] = i;
			++empty_cell_count_;
		}
	}
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
    std::cout << "Peers: " << std::endl;
    for (int i = 0; i < num_peers_; ++i)
    {
        std::cout << "Index: " << peers[i].index << ", Value: " << peers[i].value << std::endl;
    }
}

bool Sudoku::validate_board()
{
    Peer* peers = new Peer[blocksize_];
    int dup_indexes[3];

    for (int index = 0; index < gridsize_; ++index)
    {
        if (is_empty(index))
            continue;

        char value = board_[index];
        int num_peers = get_peers(index, peers);
        int duplicates = get_duplicates(value, peers, dup_indexes);

        if (duplicates > 0)
        {
            return false;
        }
    }

    return true;
}

int Sudoku::is_duplicate(char value, Peer* peers)
{
    for (int i = 0; i < num_peers_; ++i)
    {
        if (peers[i].value == value)
            return i; // Return the index of the duplicate in the peers array
    }
    return -1; // No duplicate found
}

int Sudoku::get_duplicates(char value, Peer* peers, int* dup_indexes)
{
    int duplicate_count = 0;
    for (int i = 0; i < num_peers_; ++i)
    {
        if (peers[i].value == value)
        {
            dup_indexes[duplicate_count++] = peers[i].index;
        }
    }

    if (duplicate_count > 0)
    {
        std::sort(dup_indexes, dup_indexes + duplicate_count);
    }

    return duplicate_count;
}
