#include "AVLTree.h"

template <typename T>
AVLTree<T>::AVLTree(ObjectAllocator* oa, bool ShareOA)
    : BSTree<T>(oa, ShareOA)
{
}

template <typename T>
void AVLTree<T>::insert(const T& value)
{
    BSTree<T>::insert(value);
}

template <typename T>
void AVLTree<T>::remove(const T& value)
{
    BSTree<T>::remove(value);
}