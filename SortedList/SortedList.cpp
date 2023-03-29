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
	clear();
	if (freeAllocator_)
		objAllocator_->Free(objAllocator_);
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
		// Already sorted if there are 0 or 1 elements
		return;
	}

	Node* current = head_;

	while (current->Next != nullptr)
	{
		Node* min = current;
		Node* test = current->Next;

		while (test != nullptr)
		{
			if (sorter(test->Data, min->Data))
			{
				min = test;
			}
			test = test->Next;
		}

		if (min != current)
		{
			// remove min from its current position
			if (min->Prev != nullptr)
			{
				min->Prev->Next = min->Next;
			}
			else
			{
				head_ = min->Next;
			}
			if (min->Next != nullptr)
			{
				min->Next->Prev = min->Prev;
			}
			else
			{
				tail_ = min->Prev;
			}

			// insert min at the current node location
			if (current->Prev != nullptr)
			{
				current->Prev->Next = min;
				min->Prev = current->Prev;
			}
			else
			{
				head_ = min;
				min->Prev = nullptr;
			}
			current->Prev = min;
			min->Next = current;
			current = min;
		}

		current = current->Next;
	}
}

template <typename T, typename Pred>
template <typename Sorter>
void SortedList<T, Pred>::merge_sort([[maybe_unused]] Sorter fn)
{
	m_sorter = fn;

    // empty or one node
    if (size() < 2)
        return;

    Node* left, *right = head_, *tail, *dummy;
    int chunk_size = 1, num_merges = 2;

    dummy = reinterpret_cast<Node*>(objAllocator_->Allocate());
    dummy->Next = head_;

    while (num_merges > 1)
    {
        tail = dummy;
        Node* choice = nullptr;
        num_merges = 0;
        int left_size = 0, right_size = 0;

        while (right != nullptr)
        {
            left = right;
			num_merges++;
            left_size = 0;

            for (unsigned i = 0; i < chunk_size; i++)
            {
                if (right)
                {
                    right = right->Next;
                    left_size++;
                }
                else
                {
                    break;
                }
            }

            right_size = chunk_size;

			// note that a local list is empty if it's size is 0 or we reach a null
            while (left_size > 0 && right_size > 0)
            {
                if (m_sorter(left->Data, right->Data))
                {
					choice = left;
					left = left->Next;
					left_size--;
					tail = left;
                }
                else
                {
                    choice = right;
					right = right->Next;
					right_size--;
					tail = right;
                }
            }

            while (left_size > 0)
            {
					choice = left;
					left = left->Next;
					left_size--;
					tail = left;
            }

            while (right_size > 0)
            {
                    choice = right;
					right = right->Next;
					right_size--;
					tail = right;
            }
        }

		tail->Next = choice;

        chunk_size *= 2;
        right = dummy->Next;
    }
	head_ = dummy->Next;
	head_->Prev = nullptr;
	tail_ = tail;
    objAllocator_->Free(dummy);
}

template <typename T, typename Pred>
bool SortedList<T, Pred>::ImplementedExtraCredit()
{
	return false;
}
