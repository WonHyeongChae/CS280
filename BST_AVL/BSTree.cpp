template <typename T>
BSTree<T>::BSTree(ObjectAllocator* oa, bool ShareOA)
    : oa_(oa), root_(nullptr), share_oa_(ShareOA), size_(0), height_(-1)
{
    if (oa)
    {
        oa_ = oa;
    }
    else
    {
        OAConfig config(true);
        oa_ = new ObjectAllocator(sizeof(BinTreeNode), config);
    }
}

template <typename T>
BSTree<T>::BSTree(const BSTree& rhs)
{
    if (rhs.share_oa_)
    {
        oa_ = rhs.oa_;
        share_oa_ = true;
    }
    else
    {
        OAConfig config(true);
        oa_ = new ObjectAllocator(sizeof(BinTreeNode), config);
        share_oa_ = false;
    }

    copy_tree(rhs.root_, root_);
    size_ = rhs.size_;
    height_ = rhs.height_;
}

template <typename T>
BSTree<T>::~BSTree()
{
    clear();
    delete oa_;
}

template <typename T>
BSTree<T>& BSTree<T>::operator=(const BSTree& rhs)
{
    if (this == &rhs)
        return *this;

    if (rhs.share_oa_)
    {
        oa_ = rhs.oa_;
        share_oa_ = true;
    }

    clear();
    copy_tree(rhs.root_, root_);
    size_ = rhs.size_;
    height_ = rhs.height_;

    return *this;
}

template <typename T>
const typename BSTree<T>::BinTreeNode* BSTree<T>::operator[](int index) const
{
    if (static_cast<unsigned>(index) > size_)
        return nullptr;
    else
        return find_at_index(root_, static_cast<unsigned>(index));
}

template <typename T>
void BSTree<T>::insert(const T& value)
{
    try
    {
        insert_node(root_, value, 0);
    }
    catch (const BSTException& except)
    {
        throw except;
    }
}

template <typename T>
void BSTree<T>::remove(const T& value)
{
    remove_node(root_, const_cast<T&>(value));
    height_ = tree_height(root_);
}

template <typename T>
void BSTree<T>::clear()
{
    if (root_)
    {
        free_tree(root_);

        root_ = nullptr;
        size_ = 0;
        height_ = -1;
    }
}

template <typename T>
bool BSTree<T>::find(const T& value, unsigned& compares) const
{
    return find_node(root_, value, compares);
}

template <typename T>
bool BSTree<T>::empty() const
{
    return size_ == 0 ? true : false;
}

template <typename T>
unsigned BSTree<T>::size() const
{
    return size_;
}

template <typename T>
int BSTree<T>::height() const
{
    return tree_height(root_);
}

template <typename T>
typename BSTree<T>::BinTree BSTree<T>::root() const
{
    return root_;
}

template <typename T>
bool BSTree<T>::ImplementedIndexing()
{
    return true;
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
        BinTree mem = reinterpret_cast<BinTree>(oa_->Allocate());
        BinTree node = new (mem) BinTreeNode(value);

        return node;
    }
    catch (const OAException& e)
    {
        throw(BSTException(BSTException::E_NO_MEMORY, e.what()));
    }
}

template <typename T>
void BSTree<T>::free_node(BinTree node)
{
    node->~BinTreeNode();
    oa_->Free(node);
}

template <typename T>
int BSTree<T>::tree_height(BinTree tree) const
{
    if (tree == nullptr)
        return -1;
    else
    {
        int left_height = tree_height(tree->left);
        int right_height = tree_height(tree->right);
        return std::max(left_height, right_height) + 1;
    }
}

template <typename T>
void BSTree<T>::find_predecessor(BinTree tree, BinTree& predecessor) const
{
    predecessor = tree->left;

    while (predecessor->right != nullptr)
        predecessor = predecessor->right;
}

template <typename T>
void BSTree<T>::copy_tree(const BinTree& source, BinTree& dest)
{
    if (source == nullptr)
        dest = nullptr;
    else
    {
        dest = make_node(source->data);
        dest->count = source->count;
        dest->balance_factor = source->balance_factor;
        copy_tree(source->left, dest->left);
        copy_tree(source->right, dest->right);
    }
}

template <typename T>
void BSTree<T>::free_tree(BinTree tree)
{
    if (tree == nullptr)
        return;

    free_tree(tree->left);
    free_tree(tree->right);

    free_node(tree);
}

template <typename T>
void BSTree<T>::insert_node(BinTree& node, const T& value, int depth)
{
    try
    {
        if (node == nullptr)
        {
            if (depth > height_)
                ++height_;
            node = make_node(value);
            ++size_;
            return;
        }

        if (value < node->data)
        {
            ++node->count;
            insert_node(node->left, value, depth + 1);
        }
        else
        {
            ++node->count;
            insert_node(node->right, value, depth + 1);
        }
    }
    catch (const BSTException& except)
    {
        throw except;
    }
}

template <typename T>
void BSTree<T>::remove_node(BinTree& node, const T& value)
{
    if (node == nullptr)
        return;
    else if (value < node->data)
    {
        --node->count;
        remove_node(node->left, value);
    }
    else if (value > node->data)
    {
        --node->count;
        remove_node(node->right, value);
    }
    else
    {
        --node->count;
        if (node->left == nullptr)
        {
            BinTree temp = node;
            node = node->right;
            free_node(temp);
            --size_;
        }
        else if (node->right == nullptr)
        {
            BinTree temp = node;
            node = node->left;
            free_node(temp);
            --size_;
        }
        else
        {
            BinTree predecessor = nullptr;
            find_predecessor(node, predecessor);
            node->data = predecessor->data;
            remove_node(node->left, node->data);
        }
    }
}

template <typename T>
bool BSTree<T>::find_node(BinTree node, const T& value, unsigned& compares) const
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
typename BSTree<T>::BinTree BSTree<T>::find_at_index(BinTree tree, unsigned index) const
{


    if (tree == nullptr)
        return nullptr;
    else
    {
        unsigned left_count = 0;

        if (tree->left)
            left_count = tree->left->count;

        if (left_count > index)
            return find_at_index(tree->left, index);
        else if (left_count < index)
            return find_at_index(tree->right, index - left_count - 1);
        else
            return tree;
    }
}