
#include <iostream>
#include <map>
#include <vector>
#include <algorithm>
#include <sstream>
#include <cstring>

#include "RBTree.h"

//*********************************************************************
// Printing/Debugging
//*********************************************************************
static int Position;


void SetTreePositions(const RBTree::RBNode* tree,
                      int depth,
                      std::map<const RBTree::RBNode*,
                      std::pair<int, int> >& NodePositions)
{
    if (!tree)
        return;

    SetTreePositions(tree->left, depth + 1, NodePositions);
    std::pair<int, int>XY(Position++, depth);
    std::pair<const RBTree::RBNode*, std::pair<int, int> > pr(tree, XY);
    NodePositions.insert(pr);
    SetTreePositions(tree->right, depth + 1, NodePositions);
}


void SetTreePositions(const RBTree& tree,
                      std::map<const RBTree::RBNode*,
                      std::pair<int, int> >& NodePositions)
{
    Position = 0;
    NodePositions.clear();
    SetTreePositions(tree.root(), 0, NodePositions);
}


std::vector<std::pair<const RBTree::RBNode*, int> >
GetNodesAtLevel(int level,
                std::map<const RBTree::RBNode*,
                std::pair<int, int> >& NodePositions)
{
    std::vector<std::pair<const RBTree::RBNode*, int> >nodes;
    std::map<const RBTree::RBNode*, std::pair<int, int> >::iterator it;
    for (it = NodePositions.begin(); it != NodePositions.end(); ++it)
    {
        std::pair<int, int>XY = it->second;
        if (XY.second == level)
        {
            std::pair<const RBTree::RBNode*, int> pr(it->first, XY.first);
            nodes.push_back(pr);
        }
    }
    return nodes;
}


bool SortNodes(const std::pair<const RBTree::RBNode*, int>& a,
               const std::pair<const RBTree::RBNode*, int>& b)
{
    if (a.second < b.second)
        return true;
    else
        return false;
}


class fSortNodes
{
public:
    bool operator()(const std::pair<const RBTree::RBNode*, int>& a,
                    const std::pair<const RBTree::RBNode*, int>& b)
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


void PrintRBTree(const RBTree& tree)
{
    std::map<const RBTree::RBNode*, std::pair<int, int> > NodePositions;

    SetTreePositions(tree, NodePositions);
    int height = tree.height();
    int offset = 0;
    for (int i = 0; i <= height; i++)
    {
        std::vector<std::pair<const RBTree::RBNode*, int> >nodes = GetNodesAtLevel(i, NodePositions);
        // Borland doesn't like the function, wants a function object
        //std::sort(nodes.begin(), nodes.end(), SortNodes);
        std::sort(nodes.begin(), nodes.end(), fSortNodes());
        std::vector<std::pair<const RBTree::RBNode*, int> >::iterator iter;
        char buffer[1024] = { 0 };
        memset(buffer, ' ', 1024);

        for (iter = nodes.begin(); iter != nodes.end(); ++iter)
        {
            //char data[60] = {0};
            TYPE value = (*iter).first->data;
            //std::sprintf(data, "%g", value);  // <<**>>

            std::stringstream ss;
            if ((*iter).first->color == RBTree::rbRED)
                ss << "(" << value << ")";
            else
                ss << "[" << value << "]";

            offset = (height / 2) + iter->second * 3;  // fudge factor
            //strncpy(buffer + offset, data, strlen(data));
            strncpy(buffer + offset, ss.str().c_str(), ss.str().length());
        }
        buffer[offset + 3] = 0; // 3 digits max
        std::cout << buffer << std::endl; // << std::endl;
    }
}
//*********************************************************************
// End Printing/Debugging
//*********************************************************************



//*********************************************************************
// Tests
//*********************************************************************
void TestChars(const char* str)
{
    RBTree tree;
    int len = static_cast<int>(strlen(str));
    for (int i = 0; i < len; i++)
    {
        tree.insert(str[i]);
        std::cout << "==============================" << std::endl;
        PrintRBTree(tree);
    }

    std::cout << "\n******* " << str << std::endl;
    PrintRBTree(tree);
    std::cout << "  size:" << tree.size() << std::endl;
    std::cout << "height: " << tree.height() << std::endl;
}

void FindChars(const char* str)
{
    RBTree tree;
    int len = static_cast<int>(strlen(str));
    for (int i = 0; i < len; i++)
        tree.insert(str[i]);

    std::cout << "\n******* " << str << std::endl;
    PrintRBTree(tree);
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

void TestInsert1(void)
{
    RBTree tree;
    try
    {
        std::cout << "\n******* E\n"; tree.insert('E'); PrintRBTree(tree);
        std::cout << "\n******* EA\n"; tree.insert('A'); PrintRBTree(tree);
        std::cout << "\n******* EAS\n"; tree.insert('S'); PrintRBTree(tree);
        std::cout << "\n******* EASY\n"; tree.insert('Y'); PrintRBTree(tree);
        std::cout << "\n******* EASYQ\n"; tree.insert('Q'); PrintRBTree(tree);
        std::cout << "\n******* EASYQU\n"; tree.insert('U'); PrintRBTree(tree);
        std::cout << "\n******* EASYQUT\n"; tree.insert('T'); PrintRBTree(tree);
        std::cout << "\n******* EASYQUTI\n"; tree.insert('I'); PrintRBTree(tree);
        std::cout << "\n******* EASYQUTIO\n"; tree.insert('O'); PrintRBTree(tree);
        std::cout << "\n******* EASYQUTION\n"; tree.insert('N'); PrintRBTree(tree);
        std::cout << "\n******* EASYQUTIONZ\n"; tree.insert('Z'); PrintRBTree(tree);
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

void TestException(void)
{
    RBTree tree;
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

void TestInsert(void)
{
    TestInsert1();
    TestChars("EASYQUTIONZ");
}

void TestFind(const char* str)
{
    try
    {
        FindChars(str);
        RBTree tree;
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
void TestDelete1(void)
{
    try
    {
        RBTree tree;

        if (!tree.ImplementedRemove())
            return;

        const char* str = "abcdef";
        int len = static_cast<int>(strlen(str));
        for (int i = 0; i < len; i++)
            tree.insert(str[i]);

        std::cout << "\n******* " << str << std::endl;
        PrintRBTree(tree);
        std::cout << "  size:" << tree.size() << std::endl;
        std::cout << "height: " << tree.height() << std::endl;

        char val = 'd';

        std::cout << "Delete: " << val << std::endl;
        tree.remove(val);
        PrintRBTree(tree);
        std::cout << "  size:" << tree.size() << std::endl;
        std::cout << "height: " << tree.height() << std::endl;

        val = 'c';
        std::cout << "Delete: " << val << std::endl;
        tree.remove(val);
        PrintRBTree(tree);
        std::cout << "  size:" << tree.size() << std::endl;
        std::cout << "height: " << tree.height() << std::endl;

        val = 'b';
        std::cout << "Delete: " << val << std::endl;
        tree.remove(val);
        PrintRBTree(tree);
        std::cout << "  size:" << tree.size() << std::endl;
        std::cout << "height: " << tree.height() << std::endl;

        val = 'e';
        std::cout << "Delete: " << val << std::endl;
        tree.remove(val);
        PrintRBTree(tree);
        std::cout << "  size:" << tree.size() << std::endl;
        std::cout << "height: " << tree.height() << std::endl;

        val = 'f';
        std::cout << "Delete: " << val << std::endl;
        tree.remove(val);
        PrintRBTree(tree);
        std::cout << "  size:" << tree.size() << std::endl;
        std::cout << "height: " << tree.height() << std::endl;

        val = 'a';
        std::cout << "Delete: " << val << std::endl;
        tree.remove(val);
        PrintRBTree(tree);
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

void TestDelete2(void)
{
    try
    {
        RBTree tree;

        if (!tree.ImplementedRemove())
            return;

        const char* str = "abcdefghijklmnopqrstuvwxyz";
        int len = static_cast<int>(strlen(str));
        for (int i = 0; i < len; i++)
            tree.insert(str[i]);

        std::cout << "\n******* " << str << std::endl;
        PrintRBTree(tree);
        std::cout << "  size:" << tree.size() << std::endl;
        std::cout << "height: " << tree.height() << std::endl;

        const char* del = "hponmlkjigfedtsrqvucbxwyaz";
        len = (int)strlen(del);
        for (int i = 0; i < len; i++)
        {
            char c = del[i];
            std::cout << "Delete: " << c << std::endl;
            tree.remove(c);
            PrintRBTree(tree);
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

void TestDelete(void)
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

//*********************************************************************
// main
//*********************************************************************
int main(int argc, char** argv)
{
    int test = 0;

    if (argc > 1)
        test = std::atoi(argv[1]);

    switch (test)
    {
        case 1:
        TestChars("ABCDEFG");
        break;
        case 2:
        TestChars("abBcCdDefFgGhHijJkKlLmMnNopPqQrSsTtuVvWwXxYyZz");
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
        default:
        TestChars("ABCDEFG");
        TestChars("abBcCdDefFgGhHijJkKlLmMnNopPqQrSsTtuVvWwXxYyZz");
        TestInsert1();
        TestFind("EASYQUTIONZ");
        TestFind("abBcCdDefFgGhHijJkKlLmMnNopPqQrSsTtuVvWwXxYyZz");
        TestException();
        //TestDelete1();
        //TestDelete2();
        break;
    }
    return 0;
}
//*********************************************************************
// End main
//*********************************************************************