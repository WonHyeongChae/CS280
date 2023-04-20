#include "BSTree.h"

template <typename T>
BSTree<T>::BSTree(ObjectAllocator* oa, bool ShareOA)
    : oa_(oa), share_oa_(ShareOA)
{
    if (oa_ == nullptr)
    {
        OAConfig config(true); // Set UseCPPMemManager to true, default the others
        oa_ = new ObjectAllocator(sizeof(BinTreeNode), config);
        share_oa_ = false; // Do not share this allocator with any other list
    }
    else
    {
        share_oa_ = true; // If a copy of 'this object' is made, share the allocator
    }
}

template <typename T>
BSTree<T>::BSTree(const BSTree& rhs)
{
    if (rhs.share_oa_)
    {
        oa_ = rhs.oa_; // Use rhs' allocator
        share_oa_ = true;            // If a copy of 'this object' is made, share the allocator
    }
    else // No sharing, create our own personal allocator
    {
        OAConfig config(true); // Set UseCPPMemManager to true, default the others
        oa_ = new ObjectAllocator(sizeof(BinTreeNode), config);

        share_oa_ = false; // Do not share this allocator with any other list
    }
    root_ = rhs.get_root();
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
    insert_item(root_, value);
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
    BinTree temp = root_;

    while (temp)
    {
        if (temp->data == value)
            return true;
        else if (temp->data > value)
            temp = temp->left;
        else
            temp = temp->right;
        compares++;
    }
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
    return tree_height(root_);
}

template <typename T>
typename BSTree<T>::BinTree BSTree<T>::root() const
{
}

template <typename T>
bool BSTree<T>::ImplementedIndexing()
{
    return false;
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
    delete node;
}

template <typename T>
int BSTree<T>::tree_height(BinTree tree) const
{
    int left_count = 0, right_count = 0;
    auto left_height = tree->left;
    auto right_height = tree->right;

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
void BSTree<T>::find_predecessor(BinTree tree, BinTree& predecessor) const
{
    predecessor = tree->left;
    while (predecessor->right != 0)
        predecessor = predecessor->right;
}