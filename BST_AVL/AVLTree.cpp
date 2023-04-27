template <typename T>
AVLTree<T>::AVLTree(ObjectAllocator* oa, bool ShareOA)
	: BSTree<T>(oa, ShareOA)
{
}

template <typename T>
void AVLTree<T>::insert(const T& value)
{
	TreeStack stack;
	avl_insert(BSTree<T>::get_root(), value, stack);
}

template <typename T>
void AVLTree<T>::remove(const T& value)
{
	TreeStack stack;
	avl_remove(BSTree<T>::get_root(), value, stack);
}

template <typename T>
void AVLTree<T>::avl_insert(BinTree& tree, const T& value, TreeStack& stack)
{
	if (tree == nullptr)
	{
		tree = BSTree<T>::make_node(value);
		++this->size_;
		avl_rebalance(stack);
	}
	else if (value < tree->data)
	{
		stack.push(&tree);
		++tree->count;
		avl_insert(tree->left, value, stack);
	}
	else if (value > tree->data)
	{
		stack.push(&tree);
		++tree->count;
		avl_insert(tree->right, value, stack);
	}
}

template <typename T>
void AVLTree<T>::avl_remove(BinTree& tree, const T& value, TreeStack& stack)
{
	if (tree == nullptr)
		return;
	else if (value < tree->data)
	{
		stack.push(&tree);
		--tree->count;
		avl_remove(tree->left, value, stack);
	}
	else if (value > tree->data)
	{
		stack.push(&tree);
		--tree->count;
		avl_remove(tree->right, value, stack);
	}
	else
	{
		--tree->count;
		if (tree->left == nullptr)
		{
			BinTree temp = tree;
			tree = tree->right;
			BSTree<T>::free_node(temp);
			--this->size_;
		}
		else if (tree->right == nullptr)
		{
			BinTree temp = tree;
			tree = tree->left;
			BSTree<T>::free_node(temp);
			--this->size_;
		}
		else
		{
			BinTree predecessor = nullptr;
			BSTree<T>::find_predecessor(tree, predecessor);
			tree->data = predecessor->data;
			avl_remove(tree->left, tree->data, stack);
		}
		avl_rebalance(stack);
	}
}

template <typename T>
int AVLTree<T>::balance_factor(BinTree& tree) const
{
	if (tree == nullptr)
		return 0;
	int left_height = BSTree<T>::tree_height(tree->left);
	int right_height = BSTree<T>::tree_height(tree->right);
	return left_height - right_height;
}

template <typename T>
void AVLTree<T>::avl_rebalance(TreeStack& stack)
{
	while (!stack.empty())
	{
		BinTree* tree = stack.top();
		stack.pop();

		int balance = balance_factor(*tree);

		// Right rotation
		if (balance > 1)
		{
			if (balance_factor((*tree)->left) < 0)
			{
				rotate_left((*tree)->left);
			}
			rotate_right(*tree);
			update_count(*tree);
		}
		// Left rotation
		else if (balance < -1)
		{
			if (balance_factor((*tree)->right) > 0)
			{
				rotate_right((*tree)->right);
			}
			rotate_left(*tree);
			update_count(*tree);
		}
	}
}

template <typename T>
void AVLTree<T>::rotate_left(BinTree& tree)
{
	BinTree temp = tree;
	tree = tree->right;
	temp->right = tree->left;
	tree->left = temp;
}

template <typename T>
void AVLTree<T>::rotate_right(BinTree& tree)
{
	BinTree temp = tree;
	tree = tree->left;
	temp->left = tree->right;
	tree->right = temp;
}

template <typename T>
unsigned int AVLTree<T>::count(BinTree& tree) const
{
	if (tree == nullptr)
		return 0;

	return 1 + count(tree->left) + count(tree->right);
}

template <typename T>
void AVLTree<T>::update_count(BinTree& tree)
{
	if (tree == nullptr)
		return;

	tree->count = count(tree);
	update_count(tree->left);
	update_count(tree->right);
}