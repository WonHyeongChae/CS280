//---------------------------------------------------------------------------
#ifndef AVLTREEH
#define AVLTREEH
//---------------------------------------------------------------------------
#include <stack>
#include "BSTree.h"

/*!
  Definition for the AVL Tree
*/
template <typename T>
class AVLTree : public BSTree<T>
{
public:
    AVLTree(ObjectAllocator* oa = 0, bool ShareOA = false);
    virtual ~AVLTree() = default; // DO NOT IMPLEMENT
    virtual void insert(const T& value) override;
    virtual void remove(const T& value) override;

private:
    // private stuff...
    using BinTree = typename  BSTree<T>::BinTree;
    using TreeStack = std::stack<BinTree*>;
    void avl_insert(BinTree& tree, const T& value, TreeStack& stack);
    void avl_remove(BinTree& tree, const T& value, TreeStack& stack);
    void avl_rebalance(TreeStack& stack);

    void rotate_left(BinTree& tree);
    void rotate_right(BinTree& tree);
    unsigned int count(BinTree& tree) const;
    void update_count(BinTree& tree);
    int balance_factor(BinTree& tree) const;
};

#include "AVLTree.cpp"
#endif
//---------------------------------------------------------------------------