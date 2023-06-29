// Assignment : Red Black Tree
// Name : Wonhyeong Chae
// Course : CS280
// Term : Spring 2023

//---------------------------------------------------------------------------
#ifndef RBTREEH
#define RBTREEH
//---------------------------------------------------------------------------

#include <string>

/*!
  Exception class
*/
class RBTreeException
{
public:
    /*!
      Constructor

      \param ErrCode
        The type of exception.

      \param Message
        The text message related to the exception.
    */
    RBTreeException(int ErrCode, const std::string& Message) :
        error_code_(ErrCode), message_(Message)
    {
    };

    /*!
      Retrieves the error code

      \return
        The actual error code
    */
    int code() const
    {
        return error_code_;
    }

    /*!
      Retrieves the text message associated with the error.

      \return
        The text message.
    */
    const char* what() const
    {
        return message_.c_str();
    }

    /*!
      Destructor
    */
    ~RBTreeException() {}
    /*!
      Possible exceptions
    */
    enum RBTREE_EXCEPTION { E_DUPLICATE, E_NO_MEMORY };

private:
    int error_code_;      //!< The error code
    std::string message_; //!< The text message associated with the error
};

/*!
  Red-Black tree class
*/
template <typename T>
class RBTree
{
public:
    /*!
      Allowable colors
    */
    enum RBCOLOR { rbRED, rbBLACK };

    /*!
      A red-black tree node structure
    */
    struct RBNode
    {
        RBNode* left;   //!< left child
        RBNode* right;  //!< right child
        RBNode* parent; //!< parent node ptr
        RBCOLOR color;  //!< color (red or black)
        T data;         //!< The actual data
    };

    RBTree();
    ~RBTree();
    void insert(const T& item);
    void remove(const T& item);
    void clear(void);
    bool find(const T& value, unsigned& compares) const;
    bool empty(void) const;
    unsigned size(void) const;
    int height(void) const;
    const RBNode* root(void) const;
    static bool ImplementedRemove(void);

private:
    // Private fields and methods ...

    RBNode* root_;
    unsigned int size_;
    int height_;
};

#include "RBTree.cpp"

#endif