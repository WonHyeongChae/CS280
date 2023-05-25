
#include <iostream>  // cout, endl
#include <map>       // map, pair
#include <vector>    // vector
#include <algorithm> // sort
#include <sstream>   // stringstream
#include <cstring>   // strlen, strncpy, memset, 

#include "RBTree.h"  // RBTree implementation

static int SPACING = 3;           // fudge-factor for the printing alignments
static int STRESS_SIZE = 2000000; // can't stress too much with Valgrind or Dr. Memory
static bool VERBOSE = false;      // true will print out the tree after every insert in TestChars

//*********************************************************************
// Printing/Debugging
//*********************************************************************
static int Position;

template <typename T>
void SetTreePositions(const typename RBTree<T>::RBNode* tree,
                      int depth,
                      std::map<const typename RBTree<T>::RBNode*,
                      std::pair<int, int> >& NodePositions)
{
    if (!tree)
        return;

    SetTreePositions<T>(tree->left, depth + 1, NodePositions);
    std::pair<int, int>XY(Position++, depth);
    std::pair<const typename RBTree<T>::RBNode*, std::pair<int, int> > pr(tree, XY);
    NodePositions.insert(pr);
    SetTreePositions<T>(tree->right, depth + 1, NodePositions);
}

template <typename T>
void SetTreePositions(const RBTree<T>& tree,
                      std::map<const typename RBTree<T>::RBNode*,
                      std::pair<int, int> >& NodePositions)
{
    Position = 0;
    NodePositions.clear();
    SetTreePositions<T>(tree.root(), 0, NodePositions);
}

template <typename T>
std::vector<std::pair<const typename RBTree<T>::RBNode*, int> >
GetNodesAtLevel(int level,
                std::map<const typename RBTree<T>::RBNode*,
                std::pair<int, int> >& NodePositions)
{
    std::vector<std::pair<const typename RBTree<T>::RBNode*, int> >nodes;
    typename std::map<const typename RBTree<T>::RBNode*, std::pair<int, int> >::iterator it;
    for (it = NodePositions.begin(); it != NodePositions.end(); ++it)
    {
        std::pair<int, int>XY = it->second;
        if (XY.second == level)
        {
            std::pair<const typename RBTree<T>::RBNode*, int> pr(it->first, XY.first);
            nodes.push_back(pr);
        }
    }
    return nodes;
}

template <typename T>
bool SortNodes(const std::pair<const typename RBTree<T>::RBNode*, int>& a,
               const std::pair<const typename RBTree<T>::RBNode*, int>& b)
{
    if (a.second < b.second)
        return true;
    else
        return false;
}

template <typename T>
class fSortNodes
{
public:
    bool operator()(const std::pair<const typename RBTree<T>::RBNode*, int>& a,
                    const std::pair<const typename RBTree<T>::RBNode*, int>& b)
    {
        if (a.second < b.second)
            return true;
        else
            return false;
    }
};


/*
 Print level order with proper indenting. [] represent black nodes and
 () represent red nodes, e.g. EASYQUTION draws like this:

             [O]
    (E)            (S)
 [A]   [I]      [Q]      [U]
          (N)         (T)   (Y)

*/

template <typename T>
void PrintRBTree(const RBTree<T>& tree)
{
    std::map<const typename RBTree<T>::RBNode*, std::pair<int, int> > NodePositions;

    SetTreePositions(tree, NodePositions);
    int height = tree.height();
    int offset = 0;
    for (int i = 0; i <= height; i++)
    {
        std::vector<std::pair<const typename RBTree<T>::RBNode*, int> >nodes = GetNodesAtLevel<T>(i, NodePositions);
        // Borland doesn't like the function, wants a function object
        //std::sort(nodes.begin(), nodes.end(), SortNodes<T>);
        std::sort(nodes.begin(), nodes.end(), fSortNodes<T>());
        typename std::vector<std::pair<const typename RBTree<T>::RBNode*, int> >::iterator iter;
        char buffer[1024] = { 0 };
        std::memset(buffer, ' ', 1024);

        for (iter = nodes.begin(); iter != nodes.end(); ++iter)
        {
            //char data[60] = {0};
            T value = (*iter).first->data;
            //std::sprintf(data, "%g", value);  // <<**>>

            std::stringstream ss;
            if ((*iter).first->color == RBTree<T>::rbRED)
                ss << "(" << value << ")";
            else
                ss << "[" << value << "]";

            offset = (height / 2) + iter->second * SPACING;  // fudge factor
            //strncpy(buffer + offset, data, strlen(data));
            strncpy(buffer + offset, ss.str().c_str(), ss.str().length());
        }
        buffer[offset + 10] = 0; // 3 digits max
        std::cout << buffer << std::endl; // << std::endl;
    }
}
//*********************************************************************
// End Printing/Debugging
//*********************************************************************



//*********************************************************************
// Tests
//*********************************************************************
void TestChars(const char* str, bool verbose = false)
{
    RBTree<char> tree;
    int len = static_cast<int>(strlen(str));
    for (int i = 0; i < len; i++)
    {
        tree.insert(str[i]);
        if (verbose)
        {
            std::cout << "=============================" << std::endl;
            PrintRBTree<char>(tree);
        }
    }

    std::cout << "\n******* " << str << std::endl;
    PrintRBTree<char>(tree);
    std::cout << "  size:" << tree.size() << std::endl;
    std::cout << "height: " << tree.height() << std::endl;
}

void FindChars(const char* str)
{
    RBTree<char> tree;
    int len = static_cast<int>(strlen(str));
    for (int i = 0; i < len; i++)
        tree.insert(str[i]);

    std::cout << "\n******* " << str << std::endl;
    PrintRBTree<char>(tree);
    std::cout << "  size:" << tree.size() << std::endl;
    std::cout << "height: " << tree.height() << std::endl;
    for (char c = 'A'; c <= 'Z'; c++)
    {
        unsigned compares;
        if (tree.find(c, compares))
            std::cout << c << " found after " << compares << " compares\n";
        else
            std::cout << c << " NOT found after " << compares << " compares\n";
    }
}


//*********************************************************************
// Tests
//*********************************************************************

void TestInsert1()
{
    RBTree<char> tree;
    try
    {
        std::cout << "\n******* E\n"; tree.insert('E'); PrintRBTree<char>(tree);
        std::cout << "\n******* EA\n"; tree.insert('A'); PrintRBTree<char>(tree);
        std::cout << "\n******* EAS\n"; tree.insert('S'); PrintRBTree<char>(tree);
        std::cout << "\n******* EASY\n"; tree.insert('Y'); PrintRBTree<char>(tree);
        std::cout << "\n******* EASYQ\n"; tree.insert('Q'); PrintRBTree<char>(tree);
        std::cout << "\n******* EASYQU\n"; tree.insert('U'); PrintRBTree<char>(tree);
        std::cout << "\n******* EASYQUT\n"; tree.insert('T'); PrintRBTree<char>(tree);
        std::cout << "\n******* EASYQUTI\n"; tree.insert('I'); PrintRBTree<char>(tree);
        std::cout << "\n******* EASYQUTIO\n"; tree.insert('O'); PrintRBTree<char>(tree);
        std::cout << "\n******* EASYQUTION\n"; tree.insert('N'); PrintRBTree<char>(tree);
        std::cout << "\n******* EASYQUTIONZ\n"; tree.insert('Z'); PrintRBTree<char>(tree);
        std::cout << "  size:" << tree.size() << std::endl;
        std::cout << "height: " << tree.height() << std::endl;
    }
    catch (const RBTreeException& e)
    {
        const char* str;
        if (e.code() == RBTreeException::E_NO_MEMORY)
            str = "E_NO_MEMORY";
        else if (e.code() == RBTreeException::E_DUPLICATE)
            str = "E_DUPLICATE";
        else
            str = "Unknown";

        std::cout << "***** Exception: (" << str << ") " << e.what() << std::endl << std::endl;
    }
    catch (...)
    {
        std::cout << "***** Something bad happened in TestInsert1" << std::endl;
    }
}

void TestException()
{
    RBTree<char> tree;
    try
    {
        tree.insert('A');
        tree.insert('A');
    }
    catch (const RBTreeException& e)
    {
        const char* str;
        if (e.code() == RBTreeException::E_NO_MEMORY)
            str = "E_NO_MEMORY";
        else if (e.code() == RBTreeException::E_DUPLICATE)
            str = "E_DUPLICATE";
        else
            str = "Unknown";

        std::cout << "***** Exception: (" << str << ") " << e.what() << std::endl << std::endl;
    }
    catch (...)
    {
        std::cout << "***** Something bad happened in TestException" << std::endl;
    }
}
//*********************************************************************
// End Tests
//*********************************************************************

void TestInsert()
{
    TestInsert1();
    TestChars("EASYQUTIONZ");
}

void TestFind(const char* str)
{
    try
    {
        FindChars(str);
        RBTree<int> tree;
        std::cout << "\nFinding value 5 in empty tree\n";
        unsigned compares;
        if (tree.find(5, compares))
            std::cout << "Found 5, compares=" << compares << std::endl;
        else
            std::cout << "Did not find 5, compares=" << compares << std::endl;
    }
    catch (const RBTreeException& e)
    {
        const char* str;
        if (e.code() == RBTreeException::E_NO_MEMORY)
            str = "E_NO_MEMORY";
        else if (e.code() == RBTreeException::E_DUPLICATE)
            str = "E_DUPLICATE";
        else
            str = "Unknown";

        std::cout << "***** Exception: (" << str << ") " << e.what() << std::endl << std::endl;
    }
    catch (...)
    {
        std::cout << "***** Something bad happened in TestFind" << std::endl;
    }
}

void TestDelete1()
{
    try
    {
        if (!RBTree<char>::ImplementedRemove())
            return;

        RBTree<char> tree;

        const char* str = "abcdef";
        int len = static_cast<int>(strlen(str));
        for (int i = 0; i < len; i++)
            tree.insert(str[i]);

        std::cout << "\n******* " << str << std::endl;
        PrintRBTree<char>(tree);
        std::cout << "  size:" << tree.size() << std::endl;
        std::cout << "height: " << tree.height() << std::endl;

        char val = 'd';

        std::cout << "Delete: " << val << std::endl;
        tree.remove(val);
        PrintRBTree<char>(tree);
        std::cout << "  size:" << tree.size() << std::endl;
        std::cout << "height: " << tree.height() << std::endl;

        val = 'c';
        std::cout << "Delete: " << val << std::endl;
        tree.remove(val);
        PrintRBTree<char>(tree);
        std::cout << "  size:" << tree.size() << std::endl;
        std::cout << "height: " << tree.height() << std::endl;

        val = 'b';
        std::cout << "Delete: " << val << std::endl;
        tree.remove(val);
        PrintRBTree<char>(tree);
        std::cout << "  size:" << tree.size() << std::endl;
        std::cout << "height: " << tree.height() << std::endl;

        val = 'e';
        std::cout << "Delete: " << val << std::endl;
        tree.remove(val);
        PrintRBTree<char>(tree);
        std::cout << "  size:" << tree.size() << std::endl;
        std::cout << "height: " << tree.height() << std::endl;

        val = 'f';
        std::cout << "Delete: " << val << std::endl;
        tree.remove(val);
        PrintRBTree<char>(tree);
        std::cout << "  size:" << tree.size() << std::endl;
        std::cout << "height: " << tree.height() << std::endl;

        val = 'a';
        std::cout << "Delete: " << val << std::endl;
        tree.remove(val);
        PrintRBTree<char>(tree);
        std::cout << "  size:" << tree.size() << std::endl;
        std::cout << "height: " << tree.height() << std::endl;
    }
    catch (const RBTreeException& e)
    {
        const char* str;
        if (e.code() == RBTreeException::E_NO_MEMORY)
            str = "E_NO_MEMORY";
        else if (e.code() == RBTreeException::E_DUPLICATE)
            str = "E_DUPLICATE";
        else
            str = "Unknown";

        std::cout << "***** Exception: (" << str << ") " << e.what() << std::endl << std::endl;
    }
    catch (...)
    {
        std::cout << "***** Something bad happened in TestDelete1" << std::endl;
    }
}

void TestDelete2()
{
    try
    {
        if (!RBTree<char>::ImplementedRemove())
            return;

        RBTree<char> tree;

        const char* str = "abcdefghijklmnopqrstuvwxyz";
        int len = static_cast<int>(strlen(str));
        for (int i = 0; i < len; i++)
            tree.insert(str[i]);

        std::cout << "\n******* " << str << std::endl;
        PrintRBTree<char>(tree);
        std::cout << "  size:" << tree.size() << std::endl;
        std::cout << "height: " << tree.height() << std::endl;




        const char* del = "hponmlkjigfedtsrqvucbxwyaz";
        len = (int)strlen(del);
        for (int i = 0; i < len; i++)
        {
            char c = del[i];
            std::cout << "Delete: " << c << std::endl;
            tree.remove(c);
            PrintRBTree<char>(tree);
            std::cout << "  size:" << tree.size() << std::endl;
            std::cout << "height: " << tree.height() << std::endl;
            std::cout << "------------------------------------------------\n";
        }
    }
    catch (const RBTreeException& e)
    {
        const char* str;
        if (e.code() == RBTreeException::E_NO_MEMORY)
            str = "E_NO_MEMORY";
        else if (e.code() == RBTreeException::E_DUPLICATE)
            str = "E_DUPLICATE";
        else
            str = "Unknown";

        std::cout << "***** Exception: (" << str << ") " << e.what() << std::endl << std::endl;
    }
    catch (...)
    {
        std::cout << "***** Something bad happened in TestDelete2" << std::endl;
    }
}

void TestDelete()
{
    try
    {
        TestDelete1();
        TestDelete2();
    }
    catch (const RBTreeException& e)
    {
        const char* str;
        if (e.code() == RBTreeException::E_NO_MEMORY)
            str = "E_NO_MEMORY";
        else if (e.code() == RBTreeException::E_DUPLICATE)
            str = "E_DUPLICATE";
        else
            str = "Unknown";

        std::cout << "***** Exception: (" << str << ") " << e.what() << std::endl << std::endl;
    }
    catch (...)
    {
        std::cout << "***** Something bad happened in TestDelete" << std::endl;
    }
}

int RandomInt(int low, int high)
{
    return std::rand() % (high - low + 1) + low;
}

template <typename T>
void SwapInt(T& a, T& b)
{
    T temp = a;
    a = b;
    b = temp;
}

template <typename T>
void Shuffle(T* array, int count, int stride = 1)
{
    for (int i = 0; i < count; i += stride)
    {
        int r = RandomInt(0, count - 1);
        SwapInt(array[i], array[r]);
    }
}

template <typename T>
void Print(T* array, int count)
{
    for (int i = 0; i < count; i++)
        std::cout << array[i] << " ";
    std::cout << std::endl;
}

void TestStressWithDelete()
{
    int old = SPACING;
    SPACING = 6;
    int* ia = 0;
    try
    {
        RBTree<int> tree;

        int size = STRESS_SIZE;

        ia = new int[static_cast<unsigned>(size)];
        for (int i = 0; i < size; i++)
            ia[i] = i;

        Shuffle(ia, size, 1);
        //Print(ia, size);

        for (int i = 0; i < size; i++)
            tree.insert(ia[i]);

        //PrintRBTree<int>(tree);

        Shuffle(ia, size, 1);
        for (int i = 0; i < size - 10; i++)
        {
            tree.remove(ia[i]);
            //PrintRBTree<int>(tree);
        }
        PrintRBTree<int>(tree);
    }
    catch (const RBTreeException& e)
    {
        std::cout << "Exception caught: " << e.what() << std::endl;
    }
    catch (...)
    {
        std::cout << "Unknown exception." << std::endl;
    }
    delete[] ia;
    SPACING = old;
}

void TestStressWithoutDelete()
{
    int old = SPACING;
    SPACING = 6;
    int* ia = 0;
    try
    {
        RBTree<int> tree;

        int size = STRESS_SIZE;

        ia = new int[static_cast<unsigned>(size)];
        for (int i = 0; i < size; i++)
            ia[i] = i;

        Shuffle(ia, size, 1);
        //Print(ia, size);

        for (int i = 0; i < size; i++)
            tree.insert(ia[i]);

        //PrintRBTree<int>(tree);

    }
    catch (const RBTreeException& e)
    {
        std::cout << "Exception caught: " << e.what() << std::endl;
    }
    catch (...)
    {
        std::cout << "Unknown exception." << std::endl;
    }
    delete[] ia;
    SPACING = old;
}

//*********************************************************************
// main
//*********************************************************************
int main(int argc, char** argv)
{

    int test = 0;

    if (argc > 1)
        test = std::atoi(argv[1]);
    if (argc > 2)
        STRESS_SIZE = std::atoi(argv[2]);
    if (argc > 3)
        VERBOSE = (bool)std::atoi(argv[3]);

    switch (test)
    {
        case 1:
        TestChars("ABCDEFG", VERBOSE);
        break;
        case 2:
        TestChars("abBcCdDefFgGhHijJkKlLmMnNopPqQrSsTtuVvWwXxYyZz", VERBOSE);
        break;
        case 3:
        TestInsert1();
        break;
        case 4:
        TestFind("EASYQUTIONZ");
        TestFind("abBcCdDefFgGhHijJkKlLmMnNopPqQrSsTtuVvWwXxYyZz");
        break;
        case 5:
        TestException();
        break;
        case 6:
        TestDelete1();
        break;
        case 7:
        TestDelete2();
        break;
        case 8:
        TestStressWithoutDelete();
        break;
        case 9:
        TestStressWithDelete();
        break;
        default:
        TestChars("ABCDEFG", VERBOSE);
        TestChars("abBcCdDefFgGhHijJkKlLmMnNopPqQrSsTtuVvWwXxYyZz", VERBOSE);
        TestInsert1();
        TestFind("EASYQUTIONZ");
        TestFind("abBcCdDefFgGhHijJkKlLmMnNopPqQrSsTtuVvWwXxYyZz");
        TestException();
        //TestDelete1();
        //TestDelete2();
        //TestStressWithoutDelete();
        //TestStressWithDelete();
        break;
    }

    return 0;
}
//*********************************************************************
// End main
//*********************************************************************