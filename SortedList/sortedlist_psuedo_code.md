Ctor
	if given allocator
		save provided allocator
		save if we should share this allocator
	else
		create our own allocator
		do not share this allocator
	create dummy
	save compare function
	init data fields

Dtor
	clear the container
	free allocator if we created our own

Copy Ctor
	init data fields
	if should share allocator
		shallow copy allocator
		share this allocator
	else
		create our own allocator
		do not share this allocator
	copy compare function
	for each element in original list
		push back into our list

Copy Assignment
	clear the container
	for each element in original list
		push back into our list

operator []
	start at the head node
	walk i amount of times into the list
	return ith nodes data

clear
	while not empty
		remove and free head node

push_back
	make a node with the provided value
	if size is zero
		update head and tail pointers
	else
		use tail pointer to insert at end
	increase size count

insert (sorted)
	if size is zero
		push back value
	else
		make a node with the provided value
		walk the list while compare function returns true
		insert node after last node that returns true

selection sort
	if size is less than 2
		ret since we are already sorted
	for current node in all nodes
		min <- current node
		for each node test range (current, tail]
			if test node is "less than" min
				min <- test node
		if min is not current
			remove min from list
			insert min at current node location
			current node <- min


merge sort
	if size is less than 2
		ret since we are already sorted
	create local left, right and tail node pointers
	right <- this head

	allocate an empty dummy node

	chunk_size    <- 1
	number merges <- any number bigger than 1

	while number of merges is greater than 1
		local tail <- dummy node
		choice node <- null
		number of merges <- 0
		left size <- 0
		right size <- 0

		while right is not null
			set left node ptr to point to next starting "list" 
			increment number of merges
			left size <- 0

			attempt walking the right node pointer chunk_size times
				stop if we get right as null
			left size <- number of times right pointer walked

			right size <- chunk_size
			# note that a local list is empty if it's size is 0 or we reach a null
			while left is not empty and right is not empty
				if left data is "less than" right data
					choose left
				else
					choose right
			while left is not empty
				choose left
			while right is not empty
				choose right

		use choice to null terminate the merged lists so far
		double the size of chunk_size
		reset right pointer back to the front of the list

	reset this head to new front of the list
	make sure head prev is null
	reset this tail to new end of the list
	free dummy node

choose left
	choice <- left
	walk left to next node
	decrement left size
	use local tail pointer to insert choice at end
	update local tail pointer

choose right
	choice <- right
	walk right to next node
	decrement right size
	use local tail pointer to insert choice at end
	update local tail pointer























