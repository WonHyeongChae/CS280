#include "Sudoku.h"

Sudoku::Sudoku(int basesize, SymbolType stype, SUDOKU_CALLBACK callback)
	: board_(nullptr), stype_(stype), callback_(callback), move_(0)
{
	stats_.basesize = basesize;
	board_size_ = basesize * basesize;
	board_ = new char[stats_.basesize];
}

Sudoku::~Sudoku()
{
	delete[] board_;
}

void Sudoku::SetupBoard(const char* values, int size)
{
	if (size != stats_.basesize * stats_.basesize)
		return;

	char* board = new char[size];
	for (int i = 0; i < size; ++i)
	{
		if (values[i] != '.')
			board[i] = values[i];
		else
			board[i] = EMPTY_CHAR;
	}
}

void Sudoku::Solve()
{
	int x = 0, y = 0;
	callback_(*this, board_, MessageType::MSG_STARTING, move_, stats_.basesize, -1, 0, nullptr);
}

const char* Sudoku::GetBoard() const
{
	return	board_;
}

Sudoku::SudokuStats Sudoku::GetStats() const
{
	return stats_;
}

bool Sudoku::place_value(unsigned x, unsigned y)
{
	return true;
}
