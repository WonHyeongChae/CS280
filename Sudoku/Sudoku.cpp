// Assignment : Sudoku
// Name : Wonhyeong Chae
// Course : CS280
// Term : Spring 2023

#include "Sudoku.h"

Sudoku::Sudoku(int basesize, SymbolType stype, SUDOKU_CALLBACK callback)
{

}

Sudoku::~Sudoku()
{
}

void Sudoku::SetupBoard(const char* values, int size)
{
}

void Sudoku::Solve()
{
}

const char* Sudoku::GetBoard() const
{
    return "OK";
}

Sudoku::SudokuStats Sudoku::GetStats() const
{
    SudokuStats stats;
    return  stats;
}

//Pseudocode for a place_value Function
//
//This function takes a position(row / column or an index) and is called everytime you want to place a value on the board.
//The function is initially called from the public Solve method with the position as the first empty cell(from the upper - left) on the board.
//You'll need to add the appropriate calls to the callback function in various places in your code.
//Pseudocode: (assume 9x9)
//
//Place a value at the specified location. (The first time you will be placing the value 1 in the first empty position)
//Increment the number of moves taken.
//Increment the current move number.
//If there is no conflict in any row / column / neighborhood values then
//If this was the last position left on the board :
//The algorithm has finished, nothing left to do but to perform the proper callback.
//If there are more empty cells :
//call place_move recursively with the location of the next empty cell. (This will put you at step #1 again.)
//If there is a conflict :
//If the value is less than 9 :
//    Remove that value, increment it, and place it back in the cell and check for conflicts.
//    If the value is 9 :
//    Backtrack by removing the 9
//    Decrement the current move number.
//    Return from the function(It will return to the previous call and will likely be ready to try the next value in the previous cell)
//    The details are left for you.Of course, this is not the only way, or necessarily the "right" way to approach the algorithm.However, it is straight - forward and relatively simple.Feel free to modify or tweak the algorithm to suit your needs.Make sure to send the appropriate messages via the callback function so the driver can update the display properly.