// Assignment : Red Black Tree
// Name : Wonhyeong Chae
// Course : CS280
// Term : Spring 2023

template <typename T>
RBTree<T>::RBTree()
    : root_(nullptr), size_(0), height_(-1)
{
}

template <typename T>
RBTree<T>::~RBTree()
{
    clear();
}

template <typename T>
void RBTree<T>::insert(const T& item)
{
    try
    {
        insert_node(root_, item, 0);
    }
    catch (const RBTreeException& except)
    {
        throw except;
    }
}

template <typename T>
void RBTree<T>::remove([[maybe_unused]]const T& item)
{
}

template <typename T>
void RBTree<T>::clear()
{
    if (root_)
    {
        free_node(root_);

        root_ = nullptr;
        size_ = 0;
        height_ = -1;
    }
}

template <typename T>
bool RBTree<T>::find(const T& value, unsigned& compares) const
{
    return find_node(root_, value, compares);
}

template <typename T>
bool RBTree<T>::empty() const
{
    return size_ == 0 ? true : false;
}

template <typename T>
unsigned RBTree<T>::size() const
{
    return size_;
}

template <typename T>
int RBTree<T>::height() const
{
    return tree_height(root_);
}

template <typename T>
const typename RBTree<T>::RBNode* RBTree<T>::root() const
{
    return root_;
}

template <typename T>
bool RBTree<T>::ImplementedRemove()
{
    return false;
}

template <typename T>
typename RBTree<T>::RBNode* RBTree<T>::make_node(const T& item) const
{
    try
    {
        RBNode* node = new RBNode(item);

        return node;
    }
    catch (const RBTreeException& except)
    {
        throw except;
    }
}

template <typename T>
void RBTree<T>::insert_node(RBNode*& node, const T& item, int depth)
{
    try
    {
        if (node == nullptr)
        {
            if (depth > height_)
                ++height_;
            node = make_node(item);
            ++size_;
            return;
        }

        if (item < node->data)
        {
            insert_node(node->left, item, depth + 1);
        }
        else
        {
            insert_node(node->right, item, depth + 1);
        }
    }
    catch (const RBTreeException& except)
    {
        throw except;
    }
}

template <typename T>
void RBTree<T>::free_node(RBNode* node)
{
    if (node == nullptr)
        return;
    free_node(node->left);
    free_node(node->right);

    free_node(node);
}

template <typename T>
bool RBTree<T>::find_node(RBNode* node, const T& value, unsigned& compares) const
{
    ++compares;

    if (node == nullptr)
        return false;
    else if (value == node->data)
        return true;
    else if (value < node->data)
        return find_node(node->left, value, compares);
    else
        return find_node(node->right, value, compares);
}

template <typename T>
int RBTree<T>::tree_height(RBNode * node) const
{
    if (node == nullptr)
        return -1;
    else
    {
        int left_height = tree_height(node->left);
        int right_height = tree_height(node->right);
        return std::max(left_height, right_height) + 1;
    }
}
