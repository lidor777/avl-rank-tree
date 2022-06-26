#ifndef __AVL_TREE__
#define __AVL_TREE__

/**
 * @file avl_tree.h
 * @brief an avl tree data structure
 * @version 0.3
 * @date 2022-05-05
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include <iostream>
#include <memory>
#include <cassert>
#include "avl_utility.h"


/* NOTE!: new tree base on tree 0.1 from wet1*/

/**
 * @brief class of avl tree.
 * 
 */
class avlTree
{
    typedef node_ node;
    typedef myArray_ myArray;
    typedef std::shared_ptr<node_> node_ptr;
    typedef int t_size;
    typedef int t_w;

    public:

    /**
     * @brief iterator for looping tree elements in inorder traversal -
     *  iterator is invalid after insertion and removal from the tree 
     * 
     */
    class iterator;

    class elementNotExsist: public std::exception{};
    class iteratorIsNullptr: public std::exception{};
    class notEnoughElements: public std::exception{};

    avlTree():root(nullptr),size(0),max(nullptr),min(nullptr){}
    avlTree(const avlTree& other):root(nullptr),size(other.size),max(nullptr),min(nullptr)
    {
        root = recursiveCopyOfSubTree(other.root);
        if(root == nullptr)
            return;
        max = getMaxInSubTree(root);
        min = getMinInSubTree(root);
    }
    ~avlTree() = default;
    avlTree& operator=(const avlTree& other)
    {
        root = recursiveCopyOfSubTree(other.root);
        size = other.size;
        max = min = nullptr;
        if(root == nullptr)
            return *this;
        max = getMaxInSubTree(root);
        min = getMinInSubTree(root);
        return *this;
    }
    void reset(){
        root = max = min = nullptr;
        size = 0;
    }
    /**
     * @brief Get the Size of the tree.
     * 
     * @return t_size - <int> - number of values in the tree.
     */
    t_size getSize() const 
    {
        return size;
    }
    /**
     * @brief Get the Max value in the tree.
     * 
     * @return tree_t - max value in the tree.
     */
    tree_t getMax() const
    {
        return max->value;
    }
    /**
     * @brief Get the Min value in the tree.
     * 
     * @return tree_t - min value in the tree.
     */
    tree_t getMin() const
    {
        return min->value;
    }
    /**
     * @brief checks if key is in the tree.
     * 
     * @param id - int 
     * @param salary - int 
     */
    bool isContain(int id,int salary) const
    {
        tree_t key_e(id,salary);
        node_ptr find_res = findNodeInSubTree(root,key_e);
        return !(find_res == nullptr);
    }
    /**
     * @brief return value from the tree.
     * 
     * @param id - int 
     * @param salary - int
     */
    tree_t find(int id, int salary) const
    {
        tree_t key_e(id,salary);
        node_ptr find_res = findNodeInSubTree(root,key_e);
        if(find_res == nullptr)
            throw elementNotExsist();
        return find_res->value;
    }
    /**
     * @brief insert value to the tree. if key is already in the tree then nothing is change.
     * 
     * @param tree_t - element to stored in the tree.
     */
    void insert(tree_t value)
    {
        if(isContain(value.id,value.salary)){
            return;
        }
        node_ptr new_node = std::make_shared<node>(value);
        if(root == nullptr){
            setRoot(new_node);
        }else{
            insertNodeToSubTree(root,new_node);
        }
        checkBfAndRotateFromNodeToRoot(new_node.get());
        size +=1;
        max = getMaxInSubTree(root);
        min = getMinInSubTree(root);
    }
    /**
     * @brief remove value from the tree.
     * 
     * @param key T reference of the key to be removed
     */
    void remove(int id,int salary)
    {
        if(isContain(id,salary) == false){
            return;
        }
        tree_t key_e(id,salary);
        node* parent_of_remove_node = removeNodeInSubTree(root,key_e);
        checkBfAndRotateFromNodeToRoot(parent_of_remove_node);
        size -= 1;
        max = size == 0 ?nullptr : getMaxInSubTree(root);
        min = size == 0 ?nullptr : getMinInSubTree(root);
    }
    /**
     * @brief replace value in the tree, identical to remove(key) and then insert(value).
     * 
     * @param id id of value to be replaced.
     * @param salary salary of value to be replaced.
     * @param value new value to be inserted.
     */
    void replace(int id,int salary, tree_t value)
    {
        remove(id,salary);
        insert(value);
    }
    /**
     * @brief preform inorder traversal using a functor object. 
     * 
     * @tparam S functor - an object with operator() that get tree_t parameter.
     * @param functor will be called on all values in the tree with inorder traversal.
     */
    template <class S> void inorderTraversalWithFunctor(S& functor) const
    {
        inorderTraversalWithFunctorAux(root,functor);
    }
    /**
     * @brief Get the Sum Of Grades From Start To End 
     * 
     * @param start salary int to start from
     * @param end salary int to end at
     * @return int sum
     */
    int getSumOfGradesFromStartToEnd(int start,int end)
    {
        node_ptr start_node = findLeftBorderInSubTree(root,start);
        node_ptr end_node = findRightBorderInSubTree(root,end);
        if(start_node == nullptr || end_node == nullptr)
            return 0;
        return sumGradesToNode(root,end_node,0) - sumGradesToNode(root,start_node,0) + start_node->value.grade;
    }
    /**
     * @brief Get the Sum Of Nodes From Start To End
     * 
     * @param start salary int to start from
     * @param end salary int to end at
     * @return int number of nodes
     */
    int getSumOfNodesFromStartToEnd(int start,int end)
    {
        node_ptr start_node = findLeftBorderInSubTree(root,start);
        node_ptr end_node = findRightBorderInSubTree(root,end);
        if(start_node == nullptr || end_node == nullptr)
            return 0;
        // std::cout << start_node->value.id << "  #  " << end_node->value.id << "\n";
        return (sumNodesToNode(root,end_node,0) - sumNodesToNode(root,start_node,0) + 1);
    }
    /**
     * @brief Get the Sum Of Top m Nodes 
     * 
     * @param m number of top nodes
     * @return int sum
     */
    int getSumGradesOfTopNodes(int m)
    {
        if(size < m)
            throw notEnoughElements();
        node_ptr n_m_node = findIndexAux(this->root, size - m , 0);
        return root->w1 - sumGradesToNode(root, n_m_node, 0);
    }
    /**
     * @brief return iterator to value matching key.
     * 
     * @param id T id to be searched.
     * @param salary T salary to be searched.
     * @return iterator object. 
     */
    iterator findIterator(int id,int salary) const
    {   
        if(this->isContain(id,salary) == false)
            return iterator(nullptr,this->root);
        tree_t to_find(id,salary);
        return iterator(findNodeInSubTree(this->root,to_find),this->root);
    }
    /**
     * @brief return iterator to the min value of the tree.
     * 
     * @return iterator object.
     */
    iterator begin() const 
    {
        return iterator(min,this->root);
    }
    /**
     * @brief return iterator to the end of a tree.
     * 
     * @return iterator object thats points to nullptr.
     */
    iterator end() const 
    {
        return iterator(nullptr,this->root);
    }
    
     /**
     * @brief return iterator to minimum salary that is greater than key.
     * 
     * @param id int id to the border value
     * @param salary int salary to the border value
     * @return iterator object. could be nullptr if all values in the tree < key
     */
    /* iterator leftBorderBegin(int id,int salary) const 
    {
        node_ptr left_border_res = findLeftBorderInSubTree(root,id,salary);
        if(left_border_res == nullptr)
            return iterator(nullptr,this->root);
        return iterator(left_border_res,this->root);
    } */
    /**
     * @brief return iterator to maximum salary that is smaller than key.
     * 
     * @param id int id to the border value
     * @param salary int salary to the border value
     * @return iterator object. could be nullptr if all values in the tree > key
     */
    /* iterator rightBorderBegin(int id,int salary) const 
    {
        node_ptr right_border_res = findRightBorderInSubTree(root,id,salary);
        if(right_border_res == nullptr)
            return iterator(nullptr,this->root);
        return (iterator(right_border_res,this->root));
    }  */
    
    /**
     * @brief adds all values of other to the tree.
     * 
     * @param other avl tree refrence
     */
    void extend(const avlTree& other)
    {
        assert(IsIntersectionEmpty(*this,other) == true);
        if(other.size == 0){
            return;
        }
        if(getSize() == 0){
            *this = other;
            return;
        }
        myArray this_values = getArrayFromTree(*this);
        myArray other_values = getArrayFromTree(other);
        myArray merged_values = myArray::mergeArrays(this_values,other_values);
        int len = getSize() + other.getSize();
        node_ptr new_merge_tree = createAlmostCompleteBinaryTree(len);
        setNodeValue set_functor(merged_values);
        inorderTraversalWithFunctorAux(new_merge_tree,set_functor);
        max = min = nullptr;
        root = new_merge_tree;
        size = len;
        max = getMaxInSubTree(root);
        min = getMinInSubTree(root);
    }

    #ifdef __AVL_PRINTABLE__ 
    friend inline std::ostream& operator<<(std::ostream& stream,const avlTree& to_print);
    #endif //__AVL_PRINTABLE__

    
    class iterator
    {
        public: 

        bool operator==(const iterator& other) const
        {
            if(curr == nullptr && other.curr == nullptr)
                return true;
            if(curr == nullptr || other.curr == nullptr)
                return false;
            return curr->value == other.curr->value;
        }
        bool operator!=(const iterator& other) const
        {
            return !((*this) == other);
        }
        iterator operator++(int)
        {
            if(curr == nullptr)
                return *this;
            if(curr->right != nullptr){
                curr = getNextNodeInSubTree(curr); 
                return *this;
            }
            while (curr != nullptr && curr->parent != nullptr)
            {
                if(curr->parent->left == curr){
                    if(curr->parent->parent == nullptr)
                        curr = this->root;
                    else
                        curr = curr->parent->getNodePtr();
                    return *this;
                }
                if(curr->parent->parent == nullptr)
                        curr = nullptr;
                    else
                        curr = curr->parent->getNodePtr();
            }
            curr = nullptr;
            return *this;
        }
        iterator& operator++()
        {
            (*this)++;
            return *this;
        }
        tree_t& operator*() const
        {   
            if(curr == nullptr)
                throw iteratorIsNullptr();
            return curr->value;
        }
        tree_t* operator->() const
        {
            if(curr == nullptr)
                throw iteratorIsNullptr();
            return &(curr->value);
        }

        private:

        explicit iterator(node_ptr node_,node_ptr root_):curr(node_),root(root_){}
        node_ptr curr;
        node_ptr root;

        friend class avlTree;
        
    };

    #ifdef _TEST_AVL_
    //for debuging of the tree. not for user use.
    bool IsTreeIsValid()
    {
        return IsTreeIsValidAux(root);
    }
    #endif //_TEST_AVL_

    protected:

    node_ptr root;
    t_size size;
    node_ptr max;
    node_ptr min;


    static node_ptr findIndexAux(node_ptr root, int m, int sum)
    {
        if(root == nullptr)
            return nullptr;
        int w2_left = root->left != nullptr ? root->left->w2 : 0;
        if(w2_left + 1 == m - sum)
            return root;
        if(w2_left + 1 < m - sum)
            return findIndexAux(root->right, m, sum + w2_left + 1);
        if(w2_left + 1 > m - sum)
            return findIndexAux(root->left, m, sum);
        return nullptr; // should not get here.
    }

    static int sumGradesToNode(node_ptr root, node_ptr to_find, int sum)
    {
        if(root == nullptr || to_find == nullptr)
            return 0;
        
        int w1_left = root->left != nullptr ? root->left->w1 : 0;
        
        if(root->value == to_find->value)
            return sum + root->value.grade + w1_left;

        if(root->value < to_find->value)
            return sumGradesToNode(root->right,to_find,sum + w1_left + root->value.grade);

        if(root->value > to_find->value)
            return sumGradesToNode(root->left,to_find,sum);

        return 0; // should not get here.
    }
    static int sumNodesToNode(node_ptr root,node_ptr to_find,int sum)
    {
        if(root == nullptr)
            return 0;
        
        int w2_left = root->left != nullptr ? root->left->w2 : 0;
        
        if(root->value == to_find->value)
            return sum + 1 + w2_left;
            
        if(root->value < to_find->value)
            return sumNodesToNode(root->right,to_find,sum + w2_left + 1);

        if(root->value > to_find->value)
            return sumNodesToNode(root->left,to_find,sum);
            
        return 0;//should not get here.
    }


    #ifdef _TEST_AVL_

    static bool IsTreeIsValidAux(node_ptr root)
    {
        if(root == nullptr){
            return true;
        }
        if(!(root->value)){
            return false;
        }
        if(root->getBf() >=2 || root->getBf() <= -2)
        {
            return false; 
        }
        if(root->left != nullptr && root->left->value &&
         (*(root->left->value) > *(root->value) || *(root->left->value) == *(root->value)))
          {
            return false; 
          }  
        if(root->right != nullptr && root->right->value &&
        ( *(root->right->value) < *(root->value) || *(root->right->value) == *(root->value)))
        {
            return false;
        }
        return (IsTreeIsValidAux(root->left) == true) &&
            (IsTreeIsValidAux(root->right) == true);
    }

    #endif //_TEST_AVL_

    node_ptr getRoot(){
        return this->root;
    }
    void setRoot(node_ptr new_root)
    {
        root = new_root;
    }
    template <class S> static void inorderTraversalWithFunctorAux(node_ptr root,S& functor)
    {
        if(root == nullptr)
            return;
        inorderTraversalWithFunctorAux(root->left,functor);
        functor(root->value);
        inorderTraversalWithFunctorAux(root->right,functor);
        root->updateW1();
        root->updateW2();
    }
    static node_ptr getNextNodeInSubTree(node_ptr curr)
    {
        assert(curr != nullptr && curr->right != nullptr);
        if(curr->right->left == nullptr)
            return curr->right;
        return getMinInSubTree(curr->right);

    }
    static node_ptr getMinInSubTree(node_ptr curr)
    {
        assert(curr != nullptr); 
        if(curr->left == nullptr) 
            return curr;
        return getMinInSubTree(curr->left);
    }
    static node_ptr getMaxInSubTree(node_ptr curr)
    {
        assert(curr != nullptr); 
        if(curr->right == nullptr) 
            return curr;
        return getMaxInSubTree(curr->right);
    }
    static node_ptr recursiveCopyOfSubTree(const node_ptr root)
    {
        if(root == nullptr)
            return nullptr;
        node_ptr c_left = recursiveCopyOfSubTree(root->left);
        node_ptr c_right = recursiveCopyOfSubTree(root->right);
        node_ptr c_root = std::make_shared<node>(*root);
        c_root->setLeft(c_left);
        c_root->setRight(c_right);
        return c_root;
    }
    static node_ptr findNodeInSubTree(node_ptr root,tree_t& key)
    {
        if(root == nullptr)
            return nullptr;
        if(root->value == key)
            return root;
        if(root->value > key)
            return findNodeInSubTree(root->left,key);
        return findNodeInSubTree(root->right,key);
    }

    static node_ptr findLeftBorderInSubTree(node_ptr root,int salary)
    {
        if(root == nullptr)
            return nullptr;
        if(root->value.salary == salary){
            node_ptr temp = findLeftBorderInSubTree(root->left,salary);
            if(temp != nullptr)
                return temp;
            return root;
        }
        if(root->value.salary < salary){
            if(root->right == nullptr)
                return nullptr;
            return findLeftBorderInSubTree(root->right,salary);
        }
        if(root->left == nullptr)
            return root;
        if(root->left->value.salary < salary){
            node_ptr rec_res = findLeftBorderInSubTree(root->left->right,salary);
            if(rec_res == nullptr)
                return root;
            return rec_res;
        }
        return findLeftBorderInSubTree(root->left,salary);
    }
    static node_ptr findRightBorderInSubTree(node_ptr root,int salary)
    {
        if(root == nullptr)
            return nullptr;
        if(root->value.salary == salary){
            node_ptr temp = findRightBorderInSubTree(root->right,salary);
            if(temp != nullptr)
                return temp;
            return root;
        }
        if(root->value.salary > salary){
            if(root->left == nullptr)
                return nullptr;
            return findRightBorderInSubTree(root->left,salary);
        }
        if(root->right == nullptr)
            return root;
        if(root->right->value.salary > salary){
            node_ptr rec_res = findRightBorderInSubTree(root->right->left,salary);
            if(rec_res == nullptr)
                return root;
            return rec_res;
        }
        return findRightBorderInSubTree(root->right,salary);
    }

    void checkBfAndRotateFromNodeToRoot(node* curr)
    {
        if(curr == nullptr)
            return;
        node_ptr curr_ = nullptr;
        if(curr->parent != nullptr)
            curr_ = curr->getNodePtr();
        else
            curr_ = this->root;
        assert(curr_ != nullptr);
        curr_->updateheight();
        curr_->updateW1();
        curr_->updateW2();
        checkBfAndRotate(curr_);
        checkBfAndRotateFromNodeToRoot(curr_->parent);
    }
    void checkBfAndRotate(node_ptr curr)
    {
        if(curr->getBf() == 2){
            if(curr->left->getBf() >= 0)
                rotationLL(curr);
            else
                rotationLR(curr);
        }
        if(curr->getBf() == -2){
            if(curr->right->getBf() <= 0)
                rotationRR(curr);
            else
                rotationRL(curr);
        }
    }
    void rotationLL(node_ptr to_rotate)
    {
        assert(to_rotate->left != nullptr);
        node_ptr l = to_rotate->left;
        node* parent_ptr = to_rotate->parent;
        to_rotate->setLeft(l->right);
        l->setRight(to_rotate);
        fixParentPtr(parent_ptr, to_rotate, l); 
        to_rotate->updateheight();
        to_rotate->updateW1();
        to_rotate->updateW2();
        l->updateheight();
        l->updateW1();
        l->updateW2();
    }
    void rotationRR(node_ptr to_rotate)
    {
        assert(to_rotate->right != nullptr);
        node_ptr r = to_rotate->right;
        node* parent_ptr = to_rotate->parent;
        to_rotate->setRight(r->left);
        r->setLeft(to_rotate);
        fixParentPtr(parent_ptr, to_rotate, r);
        to_rotate->updateheight();
        to_rotate->updateW1();
        to_rotate->updateW2();
        r->updateheight();
        r->updateW1();
        r->updateW2();
    }
    void rotationLR(node_ptr to_rotate)
    {
        assert(to_rotate->left != nullptr);
        rotationRR(to_rotate->left);
        rotationLL(to_rotate);
    }
    void rotationRL(node_ptr to_rotate)
    {
        assert(to_rotate->right != nullptr);
        rotationLL(to_rotate->right);
        rotationRR(to_rotate);
    }
    void fixParentPtr(node* parent_node,node_ptr old_node,node_ptr new_node)
    {
        if(parent_node == nullptr){
            setRoot(new_node);
            new_node->parent = nullptr;
        }
        else
            parent_node->replaceChild(old_node,new_node);
    }
    static void getHeightAndNumOfNodesInCompleteBinaryTree(const int n,int* height,int* number_of_nodes)
    {   
        assert(n > 0 && height != nullptr && number_of_nodes != nullptr);          
        int h = 0,k = 1,m = 1;
        while(m < n){
            h++;
            k*=2;
            m +=k;
        }
        *height = h;
        *number_of_nodes = m;
    }
    static void removeLeafsFromTheRightInSubTree(node_ptr root,int* leafs_to_remove)
    {
        if(root == nullptr)
            return;
        removeLeafsFromTheRightInSubTree(root->right,leafs_to_remove);
        if(*leafs_to_remove > 0 &&root->right == nullptr &&
         root->left == nullptr && root->parent != nullptr){
            root->parent->replaceChild(root,nullptr);
            (*leafs_to_remove)-=1;
        }
        removeLeafsFromTheRightInSubTree(root->left,leafs_to_remove);
    }
    static node_ptr createCompleteBinaryTree(const int h)
    {
        if(h < 0)
            return nullptr;
        node_ptr left_sub_tree = createCompleteBinaryTree(h-1);
        node_ptr right_sub_tree = createCompleteBinaryTree(h-1);
        node_ptr root = std::make_shared<node>();
        root->setLeft(left_sub_tree);
        root->setRight(right_sub_tree);
        root->updateheight();
        return root;
    }
    static node_ptr createAlmostCompleteBinaryTree(const int n)
    {
        int h,m;
        getHeightAndNumOfNodesInCompleteBinaryTree(n,&h,&m);
        node_ptr root = createCompleteBinaryTree(h);
        int leafs_to_remove = m - n;
        removeLeafsFromTheRightInSubTree(root,&leafs_to_remove);
        return root;
    }
    class setNodeValue
    {
        myArray& arr;
        int i;
        public:
        setNodeValue(myArray& arr_):arr(arr_),i(0){}
        void operator()(tree_t& t_ref)
        {
            t_ref = arr[i++];
        }
    };
    static myArray getArrayFromTree(const avlTree& tree)
    {
        myArray arr_of_values(tree.getSize());
        int i = 0;
        for(auto it = tree.begin(); it != tree.end(); it++){
            assert(i < tree.getSize());
            arr_of_values[i++] = *it;
        }
        return arr_of_values;
    }
    static bool IsIntersectionEmpty(const avlTree& a,const avlTree& b)
    {
        if(a.getSize() == 0 || b.getSize() == 0){
            return true;
        }
        for(auto it = b.begin(); it != b.end(); it++){
            if(a.isContain(it->id,it->salary)){
                return false;
            }
        }
        return true;
    }

    void insertNodeToSubTree(node_ptr root,node_ptr new_node)
    {
        assert(root != nullptr || new_node != nullptr);
        if(root->value > new_node->value){
            if(root->left == nullptr){
                root->setLeft(new_node);
                return;
            }
            insertNodeToSubTree(root->left, new_node);
            return;
        }
        if(root->right == nullptr){
            root->setRight(new_node);
            return;
        }
        insertNodeToSubTree(root->right, new_node);
    }
    node* removeNodeInSubTree(node_ptr root,tree_t& key)
    {
        node_ptr to_remove = findNodeInSubTree(root,key);
        assert(to_remove != nullptr);
        if(to_remove->right == nullptr && to_remove->left == nullptr){
            return removeLeaf(to_remove);
        }
        if(to_remove->right == nullptr || to_remove->left == nullptr){
            return removeNodeWithOneChild(to_remove);
        }
        return removeNodeWithTwoChilds(to_remove);
    }
    node* removeLeaf(node_ptr to_remove)
    {
        node* perant_of_removed = to_remove->parent;
        if(perant_of_removed == nullptr){
            setRoot(nullptr);
            return nullptr;
        }
        perant_of_removed->replaceChild(to_remove,nullptr);
        return perant_of_removed;
    }
    node* removeNodeWithOneChild(node_ptr to_remove)
    {
        node* perant_of_removed = to_remove->parent;
        node_ptr child_of_removed = to_remove->left != nullptr ? to_remove->left : to_remove->right;
        if(perant_of_removed == nullptr){
            setRoot(child_of_removed);
            child_of_removed->parent = nullptr;
        }else{
            perant_of_removed->replaceChild(to_remove,child_of_removed);
        }
        return perant_of_removed;
    }
    node* removeNodeWithTwoChilds(node_ptr to_remove)
    {
        node_ptr next_node_in_inorder_traversal = getNextNodeInSubTree(to_remove);
        node::swapValues(to_remove,next_node_in_inorder_traversal);
        if(next_node_in_inorder_traversal->right == nullptr){
            return removeLeaf(next_node_in_inorder_traversal);
        }
        return removeNodeWithOneChild(next_node_in_inorder_traversal);
    }


    #ifdef __AVL_PRINTABLE__ 

    static void print(node_ptr root){
        if(root == nullptr)
            std::cout << "tree is empty!" << std::endl;
        else
            printAux("",root,true);
    }
    static void printAux(const std::string& prefix,node_ptr root,bool isLeft){
        if(root != nullptr){
            std::cout << prefix;
            std::cout << (!isLeft ? "├─r─" : "└─l─" );

            // print the value of the node
            std::cout << *root << std::endl;

            // enter the next tree level - left and right branch
            printAux( prefix + (!isLeft ? "│   " : "    "), root->right, false);
            printAux( prefix + (!isLeft ? "│   " : "    "), root->left, true);
        }
    }
    #endif //__AVL_PRINTABLE__
};

#ifdef __AVL_PRINTABLE__ 
inline std::ostream& operator<<(std::ostream& stream,const avlTree& to_print)
{   
    std::cout << "size: " << to_print.size << "\n";
    avlTree::print(to_print.root);
    std::cout << std::endl;
    return stream;
}
#endif //__AVL_PRINTABLE__


#endif //__AVL_TREE__