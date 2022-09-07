/*
* splay_tree.cpp
*
* C++ implementation of a fixed-size splay tree for 15-451 (Algorithm Design and Analysis).
*
* For more information, consult the lecture notes:
*
* https://www.cs.cmu.edu/~15451-f22/lectures/lec04-splay.pdf
*
* Daniel Anderson <dlanders@andrew.cmu.edu>
* Last update: 09/07/22
*/

#include <cassert>

#include <utility>
#include <vector>


class SplayTree {
 public:
  /**
   * Creates a splay tree of size n whose inorder traversal is
   *
   *  1, 2, 3, ..., n
   *
   * The nodes are arranged in a linked-list-like structure rooted at n:
   *
   *       n
   *      /
   *    ...
   *    /
   *   1
   *
   * The number of nodes in the tree is fixed and can not be modified
   */
  explicit SplayTree(unsigned int n) : nodes{} {
    nodes.reserve(n);
    nodes.emplace_back(1);
    for (unsigned int i = 1; i < n; i++) {
      nodes.emplace_back(i+1);
      nodes[i].set_left(std::addressof(nodes[i-1]));
    }
    set_root(std::addressof(nodes.back()));
  }
  
  // Splays the node with the value equal to key
  void splay_by_key(unsigned int key) {
    assert(1 <= key && key <= nodes.size());
    splay(std::addressof(nodes[key-1]));
  }
  
 protected:
  struct Node {
    unsigned int value;
    unsigned int size;
    Node* parent{nullptr};
    Node* left{nullptr};
    Node* right{nullptr};
    
    // Create a node with the given value as its key
    Node(unsigned int value_) : value(value_), size(1) { }
    
    // Set the left child of the node to the given node
    void set_left(Node* node) {
      left = node;
      if (left != nullptr)
        left->parent = this;
    }
    
    // Set the right child of the node to the given node
    void set_right(Node* node) {
      right = node;
      if (right != nullptr)
        right->parent = this;
    }
    
    // Replace whichever child is currently equal to old with new_child
    void replace_child(Node* old, Node* new_child) {
      assert(left == old || right == old);
      if (left == old)
        set_left(new_child);
      else
        set_right(new_child);
    }
  };
  
  // Tree rotations
  /**
   * 
   *         z                                        z
   *        /       right rotation about y           /
   *       y      ===========================>      x
   *      / \                                      / \
   *     x   C                                    A   y
   *    / \                                          / \
   *   A   B                                        B   C
   *
   */
  void rotate_right(Node* y) {
    assert(y != nullptr);  // Cannot rotate around an empty tree
    Node* x = y->left;
    assert(x != nullptr);  // x will replace y's position, so it cannot be null
    
    Node* z = y->parent;
    Node* A = x->left;
    Node* B = x->right;
    Node* C = y->right;

    // y is the root
    if (z == nullptr)
      set_root(x);
    else
      z->replace_child(y, x);
    
    x->set_left(A);
    x->set_right(y);
    y->set_left(B);
    y->set_right(C);
  }

  /**
   * 
   *         z                                        z
   *        /                                        /
   *       y                                        x
   *      / \                                      / \
   *     x   C       left rotation about x        A   y
   *    / \       <===========================       / \
   *   A   B                                        B   C
   *
   */
  void rotate_left(Node* x) {
    assert(x != nullptr);  // Cannot rotate around an empty tree
    Node* y = x->right;
    assert(y != nullptr);  // y will replace x's position, so it cannot be null
    
    Node* z = x->parent;
    Node* A = x->left;
    Node* B = y->left;
    Node* C = y->right;

    // x is the root
    if (z == nullptr)
      set_root(y);
    else
      z->replace_child(x, y);
    
    y->set_right(C);
    y->set_left(x);
    x->set_right(B);
    x->set_left(A);
  }
  
  void splay_step(Node* x) {
    assert(x != nullptr);

    Node* y = x->parent;
    if (y == nullptr) return; // root case, do nothing
    
    Node* z = y->parent;
    
    // single-rotation (zig) cases
    if (z == nullptr) {
      assert(y->left == x || y->right == x);
      
      if (y->left == x) {
        /*
         *                                  y             x
         *   Zig (y is the tree root):     /     ====>     \
         *                                x                 y
         */
        rotate_right(y);
      }
      else {
        /*
         *                                y                 x
         *   Zig (y is the tree root):     \     ====>     /
         *                                  x             y
         */
        rotate_left(y);
      }
    }
    else {
      assert((z->left != nullptr && z->left->right == x) ||
             (z->right != nullptr && z->right->left == x) ||
             (z->left != nullptr && z->left->left == x) ||
             (z->right != nullptr && z->right->right == x));

      // otherwise, we have one of the complex cases (zig-zig or zig-zag)
      if (z->left != nullptr && z->left->right == x) {
        /*
         *                    z              z
         *                   /              /             x
         *   Zig-zag:       y     ====>    x   ====>     / \
         *                   \            /             y   z
         *                    x          y
         */
        rotate_left(y);
        rotate_right(z);
      }
      else if (z->right != nullptr && z->right->left == x) {
        /*
         *                  z            z
         *                   \            \               x
         *   Zig-zag:         y   ====>    x   ====>     / \
         *                   /              \           z   y
         *                  x                y
         */
        rotate_right(y);
        rotate_left(z);
      }
      else if (z->left != nullptr && z->left->left == x) {
        /*
         *                    z                         x
         *                   /            y              \
         *   Zig-zig:       y     ====>  / \   ====>      y
         *                 /            x   z              \
         *                x                                 z
         */
        rotate_right(z);
        rotate_right(y); // at the start of this stage, y is at the top
      }
      else {
        /*
         *                z                                 x
         *                 \              y                /
         *   Zig-zig:       y     ====>  / \   ====>      y
         *                   \          z   x            /
         *                    x                         z
         */
        rotate_left(z);
        rotate_left(y);
      }
    }
  }
  
  void splay(Node* x) {
    assert(x != nullptr);
    while (root != x)
      splay_step(x);
  }
  
  void set_root(Node* x) {
    assert(x != nullptr);
    root = x;
    x->parent = nullptr;
  }
  
  Node* root;
  std::vector<Node> nodes;
};


int main() {
  SplayTree tree(10);
  
  for (unsigned k = 1; k <= 10; k++) {
    tree.splay_by_key(k);
  }
}
