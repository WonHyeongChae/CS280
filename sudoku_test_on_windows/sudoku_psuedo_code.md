

SetupBoard with values and size
    init board with size
    for index [0,size)
        if value at index is not '.'
            copy over value to board

init_board with size
    delete current board buffer
    allocate board buffer of size+1
    fill board buffer with EMPTY_CHAR
    null terminate board buffer


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


record empty cells
    for each cell index in board
        if cell is empty at index
            add index to empty cells list

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


get duplicates of value from peers
    duplicates <- {-1,-1,-1}
    for each peer
        if peer value matches value
            save peer index to duplicates
    sort duplicates
    return duplicates

