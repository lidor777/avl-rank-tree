#ifndef __AVL_UTILITY__
#define __AVL_UTILITY__

/**
 * @file rank_utility.h
 * @author lidor and jenya
 * @brief utility classes for rank tree
 * @version 0.3
 * @date 2022-05-05
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include <iostream>
#include <cassert>
#include <memory>

/**
 * @brief class for elements in rank tree.
 * 
 */
struct tree_t
{
    int id,salary,grade;
    explicit tree_t(int id_ = 0,int salary_ = 0,int grade_= 0):id(id_),salary(salary_),grade(grade_){}
};

inline bool operator==(const tree_t& a,const tree_t& b)
{
    return a.id == b.id;
}

inline bool operator<(const tree_t& a,const tree_t& b)
{
    if(a.salary == b.salary)
        return a.id < b.id;
    return a.salary < b.salary;
}
inline bool operator>(const tree_t& a,const tree_t& b)
{
    if(a.salary == b.salary)
        return a.id > b.id;
    return a.salary > b.salary;
}
/**
 * @brief node element for rank tree.
 */
class node_
{
    public:

    //type for height of a node
    typedef int t_height;
    typedef std::shared_ptr<node_> node_ptr;
    //type for additional data
    typedef int t_w;

    tree_t value;
    t_height height;
    t_w w1,w2;
    node_* parent;
    node_ptr right;
    node_ptr left;

    explicit node_(const tree_t& value_):
        value(value_),height(0),w1(1),w2(value_.grade),parent(nullptr),right(nullptr),left(nullptr){}
    explicit node_():height(0),w1(1),w2(0),parent(nullptr),right(nullptr),left(nullptr){}
    node_(const node_& other):
        value(other.value),height(other.height),w1(other.w1),w2(other.w2),parent(nullptr),right(nullptr),left(nullptr){}
    ~node_() = default;
    
    /**
     * @brief update the height of a node.
     * 
     */
    void updateheight()
    {
        int l_height = left == nullptr ? -1 : left->height;
        int r_height = right == nullptr ? -1 : right->height;
        height = ((l_height > r_height) ? l_height : r_height) + 1;
    }
    /**
     * @brief update sum of grades in sub tree
     * 
     */
    void updateW1()
    {
        int left_sum = left == nullptr ? 0 : left->w1;
        int right_sum = right == nullptr ? 0 : right->w1;
        w1 = value.grade + left_sum + right_sum;
    }
    /**
     * @brief update number of nodes in sub tree
     * 
     */
    void updateW2()
    {
        int left_sum = left == nullptr ? 0 : left->w2;
        int right_sum = right == nullptr ? 0 : right->w2;
        w2 = 1 + left_sum + right_sum;
    }
    /**
     * @brief check balance factor of a node.
     * 
     */
    int getBf() const
    {
        int l_height = left == nullptr ? -1 : left->height;
        int r_height = right == nullptr ? -1 : right->height;
        return l_height - r_height;
    }
    /**
     * @brief Set the Left of a node
     * 
     * @param to_set a node to be set as left
     */
    void setLeft(node_ptr to_set)
    {
        left = to_set;
        if(to_set == nullptr)
            return;
        to_set->parent = this;
    }
    /**
     * @brief Set the Right of a node
     * 
     * @param to_set a node to be set as right
     */
    void setRight(node_ptr to_set)
    {
        right = to_set;
        if(to_set == nullptr)
            return;
        to_set->parent = this;
    }
    /**
     * @brief replce child of a node with a new node
     * 
     * @param old_node the node to be replaced
     * @param new_node the new child of the node
     */
    void replaceChild(node_ptr old_node, node_ptr new_node)
    {
        assert(left == old_node || right == old_node);
        if(left == old_node)
            setLeft(new_node);
        else
            setRight(new_node);
    }
    /**
     * @brief Get shared_ptr of a node
     * 
     * @return node_ptr shared_ptr of the node
     */
    node_ptr getNodePtr()
    {
        assert(parent != nullptr);
        assert(parent->left.get() == this || parent->right.get() == this);
        if(parent->left.get() == this)
            return parent->left;
        return parent->right;
    }
    /**
     * @brief swap values of two nodes
     * 
     * @param a node
     * @param b node
     */
    static void swapValues(node_ptr a, node_ptr b)
    {
        tree_t temp = a->value;
        a->value = b->value;
        b->value = temp;
    }


};

#ifdef __AVL_PRINTABLE__ 

std::ostream& operator<<(std::ostream& stream,const node_& to_print)
{   
    stream << "id:" << to_print.value.id << ";salary:";
    stream << to_print.value.salary << ";grade:" << to_print.value.grade;
    stream << ";w1:" << to_print.w1 << ";w2:" << to_print.w2 << ";height:" << to_print.height;
    return stream;
}

#endif //__AVL_PRINTABLE__

/**
 * @brief an array class to help with merging arrays
 * 
 */
class myArray_
{   
    public:
    //exeption when bad index is used.
    class badIndex: public std::exception{};

    explicit myArray_(int size_):size(size_),arr(nullptr)
    {
        arr = new tree_t[size];
    }
    myArray_(const myArray_& other):size(other.size),arr(nullptr)
    {
        arr = new tree_t[size];
        for(int i = 0; i < size; i++)
        {
            arr[i] = other.arr[i];
        }
    }
    myArray_& operator=(const myArray_& other)
    {
        tree_t* temp = new tree_t[other.size];
        for(int i = 0; i < other.size; i++)
        {
            temp[i] = other.arr[i];
        }
        delete[] arr;
        arr = temp;
        size = other.size;
        return *this;
    }
    ~myArray_()
    {
        delete[] arr;
    }
    const tree_t& operator[](int i) const
    {
        if(i < 0 || i >= size){
            throw badIndex();
        }
        return arr[i];
    }
    tree_t& operator[](int i)
    {
        if(i < 0 || i >= size){
            throw badIndex();
        }
        return arr[i];
    }
    /**
     * @brief checks if an array is sorted
     * 
     */
    bool isSorted() const
    {
        for(int i = 1; i < size; i++){
            if( arr[i - 1] > arr[i])
                return false;
        }
        return true;
    }
    /**
     * @brief merge 2 arrays into one sorted array 
     * 
     * @param arr1 sorted array
     * @param arr2 sorted array
     * @return myArray sorted array containing all elements of arr1 and arr2
     */
    static myArray_ mergeArrays(const myArray_& arr1,const myArray_& arr2)
    {
        assert(arr1.isSorted() && arr2.isSorted());
        myArray_ merge_array(arr1.size + arr2.size);
        int ia = 0,ib = 0;
        while(ia < arr1.size && ib < arr2.size){
            if(arr2[ib] > arr1[ia]){
                merge_array[ia+ib] = arr1[ia];
                ia++;
                continue;
            }
            merge_array[ia+ib] = arr2[ib];
            ib++;
        }
        while(ib < arr2.size){
            merge_array[ia+ib] = arr2[ib];
            ib++;
        }
        while(ia < arr1.size){
            merge_array[ia+ib] = arr1[ia];
            ia++;
        }
        return merge_array;
    }

    private:

    int size;
    tree_t* arr;
};


#endif //__AVL_UTILITY__