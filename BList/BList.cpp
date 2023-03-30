// Assignment : BList
// Name : Wonhyeong Chae
// Course : CS280
// Term : Spring 2023

template <typename T, int Size>
BList<T, Size>::BList()
{

}

template <typename T, int Size>
BList<T, Size>::BList(const BList& rhs)
{
}

template <typename T, int Size>
BList<T, Size>::~BList()
{
}

template <typename T, int Size>
BList<T, Size>& BList<T, Size>::operator=(const BList& rhs)
{
}

template <typename T, int Size>
void BList<T, Size>::push_back(const T& value)
{
}

template <typename T, int Size>
void BList<T, Size>::push_front(const T& value)
{
}

template <typename T, int Size>
void BList<T, Size>::insert(const T& value)
{
}

template <typename T, int Size>
void BList<T, Size>::remove(int index)
{
}

template <typename T, int Size>
void BList<T, Size>::remove_by_value(const T& value)
{
}

template <typename T, int Size>
int BList<T, Size>::find(const T& value) const
{
}

template <typename T, int Size>
T& BList<T, Size>::operator[](int index)
{
}

template <typename T, int Size>
const T& BList<T, Size>::operator[](int index) const
{
}

template <typename T, int Size>
size_t BList<T, Size>::size() const
{
}

template <typename T, int Size>
void BList<T, Size>::clear()
{
    while (GetHead() != nullptr)
    {
        head_ = head_->next;
        delete head_->prev;
    }
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
}
