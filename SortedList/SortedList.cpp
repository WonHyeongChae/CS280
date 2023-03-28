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
    : m_sorter(sorter), shareAllocator(SharedAllocator)
{
    if (Allocator != nullptr)
    {
        this->objAllocator = Allocator;
    }
    else
    {
        
    }
}

template <typename T, typename Pred>
SortedList<T, Pred>::SortedList(const SortedList& rhs)
{
    if (shareAllocator_)
    {
        this->objAllocator_ = rhs.objAllocator_;
    }
    std::copy(*this, rhs);
}

template <typename T, typename Pred>
SortedList<T, Pred>::~SortedList()
{
}

template <typename T, typename Pred>
SortedList<T, Pred>& SortedList<T, Pred>::operator=(const SortedList& rhs)
{
    
    *this = std::move(rhs);
    return *this;
}

template <typename T, typename Pred>
const T& SortedList<T, Pred>::operator[](size_t index) const
{
    auto node = head_;

    for (size_t i = 0; i < index; i++)
    {
        node = node->Next;
    }

    return *node;
}
    
template <typename T, typename Pred>
void SortedList<T, Pred>::push_back(const T& value)
{
    if (head_ == nullptr)
    {
        try
        {
            Node* node = reinterpret_cast<Node*>(objAllocator_->Allocate());
            head_ = new(node) Node(value);
            tail_ = head_;
        }
        catch (const OAException& e)
        {
            throw(SortedListException(SortedListException::E_NO_MEMORY, e.what()));
        }
    }
    else
    {
        auto node = head_;
        while (node->Next)
        {
            node = node->Next;
        }

    }
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
        Node node(value);
        while (Node* temp = head_)
        {
            if (temp->Data)
            temp = temp->Next;
        }
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
void SortedList<T, Pred>::selection_sort(Sorter fn)
{

}

template <typename T, typename Pred>
template <typename Sorter>
void SortedList<T, Pred>::merge_sort(Sorter fn)
{
}

template <typename T, typename Pred>
bool SortedList<T, Pred>::ImplementedExtraCredit()
{
    return false;
}
