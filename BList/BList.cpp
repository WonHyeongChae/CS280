// Assignment : BList
// Name : Wonhyeong Chae
// Course : CS280
// Term : Spring 2023


// Default constructor
// Initializes an empty BList with null head and tail pointers, and the default BListStats.
template <typename T, int Size>
BList<T, Size>::BList()
    : head_(nullptr), tail_(nullptr)
{
    m_stats_ = BListStats(nodesize(), 0, Size, 0);
}

// Copy constructor
// Creates a new BList as a deep copy of the input BList (rhs).
template <typename T, int Size>
BList<T, Size>::BList(const BList& rhs)
{
    // Copy BListStats from rhss
    m_stats_ = rhs.m_stats_;

    // Initialize pointers for traversing source and destination lists
    BNode* srcNode = rhs.head_;
    BNode* dstNode = nullptr;

    // Iterate through source nodes to create a deep copy of rhs
    while (srcNode != nullptr)
    {
        // Create a new node and copy the count and values from the source node
        BNode* newNode = new BNode;
        newNode->count = srcNode->count;

        for (int i = 0; i < srcNode->count; ++i)
        {
            newNode->values[i] = srcNode->values[i];
        }

        // Link the new node to the destination list
        if (dstNode != nullptr)
        {
            dstNode->next = newNode;
            newNode->prev = dstNode;
        }
        else
        {
            head_ = newNode;
        }

        // Move to the next node in both source and destination lists
        dstNode = newNode;
        srcNode = srcNode->next;
    }
}

// Destructor
// Deallocates the memory occupied by the BList.
template <typename T, int Size>
BList<T, Size>::~BList()
{
}

template <typename T, int Size>
BList<T, Size>& BList<T, Size>::operator=(const BList& rhs)
{
    // If the object is being assigned to itself, return *this to avoid self-assignment
    if (this == &rhs)
    {
        return *this;
    }

    // Clear the current list
    clear();

    // Initialize pointers for the rhs list and the new list
    const BNode* rhead = rhs.GetHead();
    BNode* node = nullptr;
    BNode* prev = nullptr;

    // Iterate through the rhs list and copy the nodes to the new list
    while (rhead)
    {
        node = new BNode(); // Create a new node
        node->count = rhead->count; // Copy the count from the rhs node

        // Copy the values from the rhs node
        for (auto i = 0; i < rhead->count; ++i)
        {
            node->values[i] = rhead->values[i];
        }

        // Link the new node to the previous node in the new list
        if (prev)
        {
            prev->next = node;
            node->prev = prev;
        }
        else
        {
            head_ = node; // If this is the first node, set it as the head of the new list
        }
        
        prev = node; // Move the previous pointer to the current node
        rhead = rhead->next; // Move to the next node in the rhs list
    }

    // Set the tail of the new list
    tail_ = prev; 

    // Copy the statistics from the rhs list
    m_stats_ = rhs.GetStats();

    return *this; // Return the new list
}

template <typename T, int Size>
void BList<T, Size>::push_back(const T& value)
{
    // Case 1: If the BList is empty, create a new node and set the value
    if (head_ == nullptr)
    {
        head_ = tail_ = new BNode();
        head_->values[0] = value;
        ++head_->count;
        ++m_stats_.NodeCount;
    }
    // Case 2: If the tail node is not full, add the value to the tail node
    else if (tail_->count < Size)
    {
        tail_->values[tail_->count++] = value;
    }
    // Case 3: If the tail node is full, create a new node, set the value, and update the tail pointer
    else
    {
        tail_->next = new BNode();
        tail_->next->prev = tail_;
        tail_ = tail_->next;
        tail_->values[0] = value;
        ++tail_->count;
        ++m_stats_.NodeCount;
    }
    // Increment the total item count in the BList
    ++m_stats_.ItemCount;
}

template <typename T, int Size>
void BList<T, Size>::push_front(const T& value)
{
    // If the BList is empty, create a new node and set head and tail pointers
    if (head_ == nullptr)
    {
        head_ = tail_ = new BNode();
        head_->values[0] = value;
        head_->count = 1;
        ++m_stats_.NodeCount;
        ++m_stats_.ItemCount;
    }
    // If head node is full, create a new node and set it as the new head
    else if (head_->count == Size)
    {
        BNode* new_node = new BNode();
        new_node->values[0] = value;
        new_node->count = 1;
        new_node->next = head_;
        head_->prev = new_node;
        head_ = new_node;
        ++m_stats_.NodeCount;
        ++m_stats_.ItemCount;
    }
    // If head node is not full, shift the elements to the right and insert the value
    else
    {
        for (int i = head_->count; i > 0; --i)
        {
            head_->values[i] = head_->values[i - 1];
        }
        head_->values[0] = value;
        ++head_->count;
        ++m_stats_.ItemCount;
    }
}

template <typename T, int Size>
void BList<T, Size>::insert(const T& value)
{
    // If the list is not empty
    if (head_)
    {
        BNode* node = head_;
        int i = 0;
        // Iterate through the list to find the correct position for the new value
        while (node)
        {
            while (i < Size && node->values[i] < value)
            {
                ++i;
            }

            if (i < node->count)
                break;

            node = node->next;
            i = 0;
        }
        // If the position is found within the list
        if (node)
        {
            if (i == 0)
            {
                // If there's room in the previous node, insert the value there
                if (node->prev && node->prev->count < Size)
                {
                    node->prev->values[node->prev->count] = value;
                    inc_count(node->prev);
                    ++m_stats_.ItemCount;
                }
                // If there's room in the current node, insert the value and shift the rest to the right
                else if (node->count < Size)
                {
                    auto j = node->count;
                    while (j > i)
                    {
                        node->values[j] = node->values[j - 1];
                        --j;
                    }
                    node->values[i] = value;
                    inc_count(node);
                    ++m_stats_.ItemCount;
                }
                // If no room in the previous node, split the current node and insert the value
                else if (node->prev)
                {
                    split_node(node->prev, value, Size);
                }
                // If no previous node, split the current node and insert the value
                else
                {
                    split_node(node, value, i);
                }
            }
            // If position is not at the beginning of the node
            else
            {
                // If there's room in the current node, insert the value and shift the rest to the right
                if (node->count < Size)
                {
                    auto j = node->count;
                    while (j > i)
                    {
                        node->values[j] = node->values[j - 1];
                        --j;
                    }
                    node->values[i] = value;
                    inc_count(node);
                    ++m_stats_.ItemCount;
                }
                // If no room in the current node, split the node and insert the value
                else
                {
                    split_node(node, value, i);
                }
            }
        }
        // If the position is not found within the list (insert at the end)
        else
        {
            if (tail_->count < Size)
            {
                tail_->values[tail_->count] = value;
                inc_count(tail_);
                ++m_stats_.ItemCount;
            }
            else
            {
                split_node(tail_, value, tail_->count);
            }
        }
    }
    // If the list is empty, create a new node with the value
    else
    {
        BNode* node = new BNode();
        node->values[0] = value;
        inc_count(node);

        tail_ = head_ = node;
        ++m_stats_.ItemCount;
        ++m_stats_.NodeCount;
    }
}

template <typename T, int Size>
void BList<T, Size>::remove(int index)
{
    // 1. Get the node containing the element at the specified index
    BNode* node = get_node(index);

    // 2. Calculate the position of the element within the node
    int i = index % Size;

    // 3. Shift all the elements to the right of the target element to the left by one position
    for (int j = i; j < node->count; ++j)
    {
        node->values[j] = node->values[j + 1];
    }

    // 4. Decrement the element count in the node
    --node->count;
    // 5. Decrement the total item count in the list
    --m_stats_.ItemCount;

    // 6. If the node is empty after removing the element, delete the node and update the node count
    if (node->count == 0)
    {
        delete_node(node);
        --m_stats_.NodeCount;
    }
}

template <typename T, int Size>
void BList<T, Size>::remove_by_value(const T& value)
{
    // Initialize the deletion_index to 0
    int deletion_index = 0;
    // Start with the head node
    BNode* node = head_;
    // A flag to determine if the value is found and the loop should be broken
    bool is_broken = false;

    // Iterate through the nodes
    while (node)
    {
        // Iterate through the values in the current node
        for (int i = 0; i < node->count; ++i)
        {
            // If the value is found, set the deletion_index and break the loop
            if (node->values[i] == value)
            {
                deletion_index = i;
                is_broken = true;
                break;
            }
        }
        // If the value is found, break the outer loop
        if (is_broken)
        {
            break;
        }

        // Move on to the next node
        node = node->next;
    }

    // If the value is found in a node
    if (node)
    {
        // Shift the remaining values to the left, overwriting the value to be removed
        for (int j = deletion_index; j < node->count; ++j)
        {
            node->values[j] = node->values[j + 1];
        }

        // Decrease the count of values in the node
        --node->count;
        // Decrease the total item count
        --m_stats_.ItemCount;

        // If the node is empty after the removal, delete the node and decrease the node count
        if (node->count == 0)
        {
            delete_node(node);
            --m_stats_.NodeCount;
        }
    }
}

template <typename T, int Size>
int BList<T, Size>::find(const T& value) const
{
    BNode* node = head_; // Start at the head node of the BList.
    int sum = 0; // Initialize the sum of counts to 0.

    // Traverse the BList until the end is reached or the value is found.
    while (node)
    {
        // Iterate through the values in the current node.
        for (int i = 0; i < node->count; ++i)
        {
            // If the value is found, return the sum of counts plus the index.
            if (node->values[i] == value)
            {
                return sum + i;
            }
        }
        // Increment the sum by the number of items in the current node.
        sum += node->count;
        // Move on to the next node.
        node = node->next;
    }
    // If the value is not found, return -1.
    return -1;
}

template <typename T, int Size>
T& BList<T, Size>::operator[](int index)
{
    // Initialize the current index and current node pointer
    auto i = index;
    auto node = head_;
    auto sum = node->count;

    // Iterate through the list until the desired index is found
    while (sum <= index)
    {
        i -= node->count;

        node = node->next;

        // Check if the next node exists, then add its count to the sum
        if (node)
        {
            sum += node->count;
        }
    }

    // Return the value at the found index within the node
    return node->values[i];
}

template <typename T, int Size>
const T& BList<T, Size>::operator[](int index) const
{
    // Initialize the current index and current node pointer
    auto i = index;
    auto node = head_;
    auto sum = node->count;

    // Iterate through the list until the desired index is found
    while (sum <= index)
    {
        i -= node->count;

        node = node->next;

        // Check if the next node exists, then add its count to the sum
        if (node)
        {
            sum += node->count;
        }
    }

    // Return the value at the found index within the node
    return node->values[i];
}

template <typename T, int Size>
size_t BList<T, Size>::size() const
{
    return GetStats().ItemCount;
}

template <typename T, int Size>
void BList<T, Size>::clear()
{
    while (m_stats_.ItemCount)
        remove(0);
}

template <typename T, int Size>
size_t BList<T, Size>::nodesize(void)
{
    return sizeof(BNode);
}

template <typename T, int Size>
const typename BList<T, Size>::BNode* BList<T, Size>::GetHead() const
{
    return head_;
}

template <typename T, int Size>
BListStats BList<T, Size>::GetStats() const
{
    return m_stats_;
}


// Splits a node in the BList when a node has reached its maximum size.
// Inserts a new value at the given insertion index, splitting the node in two
// and adjusting the values and links between the two nodes.
template <typename T, int Size>
void BList<T, Size>::split_node(BNode* node, T value, int insertion_index)
{
    // Create a new node and link it to the existing node
    BNode* newnode = new BNode();
    newnode->prev = node;

    if (node->next)
    {
        node->next->prev = newnode;
        newnode->next = node->next;
    }

    node->next = newnode;

    // If the BList only allows one value per node
    if (Size == 1)
    {
        if (insertion_index == 0)
        {
            newnode->values[0] = node->values[0];
            node->values[0] = value;
        }
        else
        {
            newnode->values[0] = value;
        }
        inc_count(newnode);
    }
    // For BLists with nodes that allow multiple values
    else
    {
        int mid = Size / 2;

        // Move values from the original node to the new node
        int j = 0;
        for (int k = mid; k < Size; ++k)
        {
            newnode->values[j++] = node->values[k];
            inc_count(newnode);
        }

        // Update the original node's count
        node->count = mid;

        // Insert the new value into the appropriate node
        if (insertion_index <= mid)
        {
            for (int k = node->count; k > insertion_index; --k)
                node->values[k] = node->values[k - 1];
            node->values[insertion_index] = value;
            inc_count(node);
        }
        else
        {
            if (insertion_index == Size)
            {
                newnode->values[newnode->count] = value;
                inc_count(newnode);
            }
            else
            {
                insertion_index -= mid;
                for (int k = newnode->count; k > insertion_index; --k)
                {
                    newnode->values[k] = newnode->values[k - 1];
                }
                newnode->values[insertion_index] = value;
                inc_count(newnode);
            }
        }
    }
    // Update the tail if necessary
    if (node == tail_)
        tail_ = newnode;

    // Update the BList's statistics
    ++m_stats_.ItemCount;
    ++m_stats_.NodeCount;
}

template <typename T, int Size>
void BList<T, Size>::inc_count(BNode* node)
{
    ++node->count;
    if (node->count > Size)
        node->count = Size;
}

template <typename T, int Size>
void BList<T, Size>::delete_node(BNode* node)
{
    // Update the 'next' pointer of the previous node to bypass the node being deleted
    if (node->prev)
    {
        node->prev->next = node->next;
    }
    else // If there's no previous node, the head should point to the next node
    {
        head_ = node->next;
    }

    // Update the 'prev' pointer of the next node to bypass the node being deleted
    if (node->next)
    {
        node->next->prev = node->prev;
    }
    else // If there's no next node, the tail should point to the previous node
    {
        tail_ = node->prev;
    }

    // Call the destructor for the node being deleted
    node->~BNode();
    // Deallocate memory for the node being deleted
    delete node;
}

template <typename T, int Size>
typename BList<T, Size>::BNode* BList<T, Size>::get_node(const int& index) const
{
    // Initialize a variable to keep track of the cumulative count of nodes
    int sum = 0;

    // Initialize a pointer to the head of the list
    BNode* node = head_;

    // Traverse the list until the sum of node counts exceeds the given index
    while (sum <= index)
    {
        sum += node->count;
        node = node->next;
    }

    // If the current node is the head of the list, return the head node
    if (node == head_)
    {
        return node;
    }
    // If the current node is not null, return the previous node
    else if (node)
    {
        return node->prev;
    }
    // If the current node is null, return the tail node
    else
    {
        return tail_;
    }
}