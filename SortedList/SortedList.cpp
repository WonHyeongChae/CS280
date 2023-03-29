#include "SortedList.h"

//******************************************************************************
//******************************************************************************
// static
template <typename T, typename Pred>
unsigned SortedList<T, Pred>::nodesize(void)
{
    return sizeof(Node);
}

template <typename T, typename Pred>
SortedList<T, Pred>::SortedList(Pred sorter, ObjectAllocator* Allocator, bool SharedAllocator)
    : head_(nullptr), tail_(nullptr), size_(0), objAllocator_(nullptr), freeAllocator_(false), shareAllocator_(SharedAllocator)
{
    // Use the provided ObjectAllocator, or create a new one if none is given
    if (Allocator != nullptr)
    {
        objAllocator_ = Allocator;
    }
    else
    {
        OAConfig config;
        objAllocator_ = new ObjectAllocator(nodesize(), config);
        freeAllocator_ = true;
    }

    m_sorter = sorter;

    if (!shareAllocator_)
    {
        // Allocate the head and tail nodes using the allocator
        head_ = static_cast<Node*>(objAllocator_->Allocate());
        tail_ = static_cast<Node*>(objAllocator_->Allocate());

        // Check for out of memory exceptions
        if (head_ == nullptr || tail_ == nullptr)
        {
            throw SortedListException(SortedListException::E_NO_MEMORY, "Out of memory");
        }

        // Initialize the head and tail nodes
        head_->Prev = nullptr;
        head_->Next = tail_;
        tail_->Prev = head_;
        tail_->Next = nullptr;
    }
}

template <typename T, typename Pred>
SortedList<T, Pred>::SortedList(const SortedList& rhs)
    : head_(nullptr), tail_(nullptr), size_(0), objAllocator_(nullptr), freeAllocator_(false), shareAllocator_(false)
{
    // Share allocator if requested
    if (rhs.shareAllocator_)
    {
        shareAllocator_ = rhs.shareAllocator_;
        objAllocator_ = new ObjectAllocator(*rhs.objAllocator_);
    }
    else // Allocate our own
    {
        OAConfig config;
        objAllocator_ = new ObjectAllocator(nodesize(), config);
        freeAllocator_ = true;
    }
    this->m_sorter = rhs.m_sorter;
    // Copy each element in the original list

    Node* node = rhs.head_;
    while (node)
    {
        push_back(node->Data);
        node = node->Next;
    }
}

template <typename T, typename Pred>
SortedList<T, Pred>::~SortedList()
{
}

template <typename T, typename Pred>
SortedList<T, Pred>& SortedList<T, Pred>::operator=(const SortedList& rhs)
{
    if (this != &rhs)
    {
        this->m_sorter = rhs.m_sorter;

        // Copy the list contents
        clear();
        size_ = 0;
        Node* node = rhs.head_;

        while (node)
        {
            push_back(node->Data);
            node = node->Next;
        }
    }
    return *this;
}

template <typename T, typename Pred>
const T& SortedList<T, Pred>::operator[](size_t index) const
{
    if (index >= size()) // Check if the index is out of range
    {
        throw SortedListException(SortedListException::E_CLIENT_ERROR, "Index out of range"); // Throw an exception if it is
    }

    Node* curr = head_; // Set the current node to the head
    for (size_t i = 0; i < index; i++) // Traverse the list until the ith node is reached
    {
        curr = curr->Next; // Move to the next node
    }

    return curr->Data; // Return the data in the ith node
}

template <typename T, typename Pred>
void SortedList<T, Pred>::push_back(const T& value)
{
    Node* new_node = nullptr;

    try
    {
        Node* mem = reinterpret_cast<Node*>(objAllocator_->Allocate());
        new_node = new(mem) Node(value);
    }
    catch (const std::bad_alloc& e)
    {
        throw SortedListException(SortedListException::E_NO_MEMORY, "Out of memory");
    }

    if (size_ == 0) {
        head_ = new_node;
        tail_ = new_node;
    }
    else {
        new_node->Prev = tail_;
        tail_->Next = new_node;
        tail_ = new_node;
    }

    size_++;
}

template <typename T, typename Pred>
void SortedList<T, Pred>::insert(const T& value)
{
    if (size_ == 0)
    {
        push_back(value);
    }
    else
    {
        Node* new_node = nullptr;

        try
        {
            Node* mem = reinterpret_cast<Node*>(objAllocator_->Allocate());
            new_node = new(mem) Node(value);
        }
        catch (const std::bad_alloc& e)
        {
            throw SortedListException(SortedListException::E_NO_MEMORY, "Out of memory");
        }

        Node* prev = nullptr;
        Node* curr = head_;
        while (curr && Pred()(curr->Data, value))
        {
            prev = curr;
            curr = curr->Next;
        }
        if (prev)
        {
            prev->Next = new_node;
            new_node->Prev = prev;
        }
        else
        {
            head_ = new_node;
        }

        if (curr)
        {
            curr->Prev = new_node;
            new_node->Next = curr;
        }
        else
        {
            tail_ = new_node;
        }

        size_++;
    }
}

//******************************************************************************
//******************************************************************************
template <typename T, typename Pred>
unsigned SortedList<T, Pred>::size(void) const
{
    return size_;
}

template <typename T, typename Pred>
void SortedList<T, Pred>::clear()
{
    while (head_)
    {
        //head_->~Node();           // Call the object's destructor.
        objAllocator_->Free(head_); // Free the object's memory.
        head_ = head_->Next;
    }
}

template <typename T, typename Pred>
template <typename Sorter>
void SortedList<T, Pred>::selection_sort([[maybe_unused]] Sorter fn)
{
    m_sorter = fn;

    if (size_ < 2)
    {
        return; // already sorted
    }

    Node* current_node = head_;

    while (current_node->Next != nullptr)
    {
        Node* min_node = current_node;
        Node* test_node = current_node->Next;

        while (test_node != nullptr)
        {
            if (m_sorter(test_node->Data, min_node->Data))
            {
                min_node = test_node;
            }
            test_node = test_node->Next;
        }

        if (min_node != current_node)
        {
            // remove min_node from the list
            if (min_node->Prev != nullptr)
            {
                min_node->Prev->Next = min_node->Next;
            }
            if (min_node->Next != nullptr) 
            {
                min_node->Next->Prev = min_node->Prev;
            }

            // insert min_node before current_node
            if (current_node->Prev != nullptr)
            {
                current_node->Prev->Next = min_node;
            }
            else
            {
                head_ = min_node;
            }
            min_node->Prev = current_node->Prev;
            min_node->Next = current_node;
            current_node->Prev = min_node;

            // reset current_node to the beginning of the list
            current_node = head_;
        }
        else
        {
            // min_node is already in its proper place, move on to the next node
            current_node = current_node->Next;
        }
    }
}

template <typename T, typename Pred>
template <typename Sorter>
void SortedList<T, Pred>::merge_sort([[maybe_unused]] Sorter fn)
{
    m_sorter = fn;
    if (size_ < 2) {
        // Nothing to sort
        return;
    }

    // Allocate local pointers and dummy node
    Node* left;
    Node* right;
    Node* tail;
    Node* dummy = new Node();

    // Initialize dummy node
    dummy->Next = head_;
    head_->Prev = dummy;
    tail_ = tail_->Next;

    // Initialize variables
    int chunk_size = 1;
    int num_merges = size_ / 2;


    while (num_merges > 0) {
        left = dummy->Next;
        right = left;
        tail = dummy;

        while (right != nullptr) {
            // Merge chunk_size number of nodes from left and right
            int left_size = 0;
            int right_size = 0;

            // Traverse chunk_size number of nodes on right side
            for (int i = 0; i < chunk_size && right != nullptr; i++) {
                right = right->Next;
                right_size++;
            }

            // Merge left and right nodes
            while (left_size < chunk_size && right_size < chunk_size && right != nullptr) {
                if (m_sorter(left->Data, right->Data)) {
                    tail->Next = left;
                    left->Prev = tail;
                    left = left->Next;
                    left_size++;
                }
                else {
                    tail->Next = right;
                    right->Prev = tail;
                    right = right->Next;
                    right_size++;
                }
                tail = tail->Next;
            }

            // Add remaining nodes to merged list
            while (left_size < chunk_size && left != right) {
                tail->Next = left;
                left->Prev = tail;
                left = left->Next;
                left_size++;
                tail = tail->Next;
            }

            while (right_size < chunk_size && right != nullptr) {
                tail->Next = right;
                right->Prev = tail;
                right = right->Next;
                right_size++;
                tail = tail->Next;
            }

            // Traverse to next chunk on right side
            dummy->Next->Prev = nullptr;
            dummy->Next = right;
            tail = right;
        }

        // Double the chunk size and reset right pointer
        chunk_size *= 2;
        num_merges /= 2;
        dummy->Next->Prev = nullptr;
        dummy->Next = head_;
        tail_ = tail;
    }

    // Reset head and tail pointers
    head_ = dummy->Next;
    head_->Prev = nullptr;
    tail_->Next = nullptr;
    tail_ = tail;

    // Free dummy node
    delete dummy;
}

template <typename T, typename Pred>
bool SortedList<T, Pred>::ImplementedExtraCredit()
{
    return false;
}
