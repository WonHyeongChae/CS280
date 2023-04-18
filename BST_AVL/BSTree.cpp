#include "BSTree.h"

template <typename T>
BSTree<T>::BSTree(ObjectAllocator* oa, bool ShareOA)
    : oa_(oa), share_oa_(ShareOA)
{
}

template <typename T>
BSTree<T>::BSTree(const BSTree& rhs)
{
}

template <typename T>
BSTree<T>::~BSTree()
{
}

template <typename T>
BSTree<T>& BSTree<T>::operator=(const BSTree& rhs)
{
    if (this != &rhs)
    {
        root_ = rhs.get_root();
    }
    return *this;
}

template <typename T>
const typename BSTree<T>::BinTreeNode* BSTree<T>::operator[](int index) const
{
}

template <typename T>
void BSTree<T>::insert(const T& value)
{
}

template <typename T>
void BSTree<T>::remove(const T& value)
{
}

template <typename T>
void BSTree<T>::clear()
{
}

template <typename T>
bool BSTree<T>::find(const T& value, unsigned& compares) const
{
    return false;
}

template <typename T>
bool BSTree<T>::empty() const
{
    if (root == nullptr)
        return true;
    return false;
}

template <typename T>
unsigned BSTree<T>::size() const
{
}

template <typename T>
int BSTree<T>::height() const
{
    int left_count = 0, right_count = 0;
    auto left_height = root_->left;
    auto right_height = root_->right;

    while (left_height)
    {
        left_height = left_height->left;
        left_count++;
    }
    while (right_height)
    {
        right_height = right_height->right;
        right_count++;
    }

    return left_count > right_count ? left_count : right_count;
}

template <typename T>
typename BSTree<T>::BinTree BSTree<T>::root() const
{
}

template <typename T>
bool BSTree<T>::ImplementedIndexing()
{
}

template <typename T>
typename BSTree<T>::BinTree& BSTree<T>::get_root()
{
    return root_;
}

template <typename T>
typename BSTree<T>::BinTree BSTree<T>::make_node(const T& value) const
{
    try
    {
        BinTree* node = reinterpret_cast<BinTree>(oa_->Allocate()); // Allocate memory for the object.
        node = new (node) BinTreeNode(value); // Construct the object in the memory.
    }
    catch (const OAException& e)
    {
        throw(BSTException(BSTException::E_NO_MEMORY, e.what()));
    }
}

template <typename T>
void BSTree<T>::free_node(BinTree node)
{
}

template <typename T>
int BSTree<T>::tree_height(BinTree tree) const
{
}

template <typename T>
void BSTree<T>::find_predecessor(BinTree tree, BinTree& predecessor) const
{
}