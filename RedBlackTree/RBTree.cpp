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
    RBNode* new_node = make_node(item);
    RBNode* parent_node = nullptr;
    RBNode* traversing_node = root_;

    // Searching for a place to put a new node and finding the parent
    while (traversing_node != nullptr)
    {
        parent_node = traversing_node;
        if (item < traversing_node->data)
        {
            traversing_node = traversing_node->left;
        }
        else if (item > traversing_node->data)
        {
            traversing_node = traversing_node->right;
        }
        else
        {
            throw RBTreeException(RBTreeException::E_DUPLICATE, "Duplicate item inserted");
        }
    }

    new_node->parent = parent_node;

    // if new node is a root node
    if (parent_node == nullptr)
    {
        root_ = new_node;
    }
    // if new node is not a root node
    else if (item < parent_node->data)
    {
        parent_node->left = new_node;
    }
    else
    {
        parent_node->right = new_node;
    }

    // Increment the size here
    ++size_;

    // if new node is a root node
    if (new_node->parent == nullptr)
    {
        new_node->color = rbBLACK;
        return;
    }

    // if the parent of new node is a root node
    if (new_node->parent->parent == nullptr)
    {
        return;
    }

    // Fixing tree properties after insertion
    insert_fix_up(new_node);
}

template <typename T>
void RBTree<T>::insert_fix_up(RBNode* new_node)
{
    while (new_node != root_ && new_node->parent->color == rbRED)
    {
        RBNode* grandparent_node = new_node->parent->parent;
        bool is_left_child = new_node->parent == grandparent_node->left;
        RBNode* uncle_node = is_left_child ? grandparent_node->right : grandparent_node->left;

        if (uncle_node != nullptr && uncle_node->color == rbRED)
        {
            new_node->parent->color = rbBLACK;
            uncle_node->color = rbBLACK;
            grandparent_node->color = rbRED;
            new_node = grandparent_node;
            grandparent_node = new_node->parent->parent;
        }
        else
        {
            if (new_node == (is_left_child ? new_node->parent->right : new_node->parent->left))
            {
                new_node = new_node->parent;
                is_left_child ? rotate_left(new_node) : rotate_right(new_node);
            }
            new_node->parent->color = rbBLACK;
            grandparent_node->color = rbRED;

            is_left_child ? rotate_right(grandparent_node) : rotate_left(grandparent_node);
        }
    }
    root_->color = rbBLACK;
}

template <typename T>
void RBTree<T>::remove(const T& item)
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
    compares = 0;
    if (root_ != nullptr)
        return find_node(root_, value, compares);
    else
        return false;
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
        RBNode* node = new RBNode;
        node->left = nullptr;
        node->right = nullptr;
        node->parent = nullptr;
        node->color = rbRED; // new nodes are red by default
        node->data = item;

        return node;
    }
    catch (const std::bad_alloc&)
    {
        throw RBTreeException(RBTreeException::E_NO_MEMORY, "Out of memory");
    }
}

template <typename T>
void RBTree<T>::rotate_left(RBNode* node)
{
    RBNode* right_child = node->right;
    node->right = right_child->left;

    if (node->right != nullptr)
        node->right->parent = node;

    right_child->parent = node->parent;

    if (node->parent == nullptr)
        root_ = right_child;
    else if (node == node->parent->left)
        node->parent->left = right_child;
    else
        node->parent->right = right_child;

    right_child->left = node;
    node->parent = right_child;
}

template <typename T>
void RBTree<T>::rotate_right(RBNode* node)
{
    RBNode* left_child = node->left;
    node->left = left_child->right;

    if (node->left != nullptr)
        node->left->parent = node;

    left_child->parent = node->parent;

    if (node->parent == nullptr)
        root_ = left_child;
    else if (node == node->parent->right)
        node->parent->right = left_child;
    else
        node->parent->left = left_child;

    left_child->right = node;
    node->parent = left_child;
}

template <typename T>
void RBTree<T>::free_node(RBNode* node)
{
    if (node == nullptr)
        return;
    free_node(node->left);
    free_node(node->right);

    delete node;
}

template <typename T>
bool RBTree<T>::find_node(RBNode* node, const T& value, unsigned& compares) const
{
    compares++;
    
    if (node == nullptr)
        return false;

    if (value < node->data)
    {
        return find_node(node->left, value, compares);
    }
    else if (value > node->data)
    {
        return find_node(node->right, value, compares);
    }
    else
    {
        return true;
    }
}

template <typename T>
int RBTree<T>::tree_height(RBNode* node) const
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