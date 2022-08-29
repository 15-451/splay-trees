/*
* splay_tree.c
*
* C implementation of splay trees for 15-451 (Algorithm Design and Analysis).
*
* For more information, consult the lecture notes:
*
* https://www.cs.cmu.edu/~15451-f22/lectures/lec04-splay.pdf
*
* Yoseph Mak <ymak@andrew.cmu.edu>
* Last update: 8/29/22
*/

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

// Struct definitions
struct node {
  struct node *parent;
  struct node *left;
  struct node *right;

  int value;
};

struct tree {
  int size;
  struct node *root;
};

// Tree setting functions
/**
 * Sets parent's left child to left and updates left's parent to be parent if
 * left isn't NULL.
 * 
 * @requires parent cannot be NULL
 */
void set_left(struct node *parent, struct node *left) {
  assert(parent != NULL);
  parent->left = left;
  if (left != NULL) left->parent = parent;
}

/**
 * Sets parent's right child to right and updates right's parent to be parent if
 * right isn't NULL.
 * 
 * @requires parent cannot be NULL
 */
void set_right(struct node *parent, struct node *right) {
  assert(parent != NULL);
  parent->right = right;
  if (right != NULL) right->parent = parent;
}

/**
 * Changes parent's child to be new instead of old. If parent is NULL, this
 * instead assumes old is the root and swaps it out accordingly.
 * 
 * @requires Either old is a non-NULL child of parent, or is the root of the
 * tree if parent is NULL
 */
void swap_child(struct tree *T, struct node *parent, struct node *old, struct node *new) {
  assert(old != NULL);
  new->parent = parent;

  if (parent == NULL) {
    assert(T->root == old);
    T->root = new;
  }
  else if (parent->left == old) parent->left = new;
  else if (parent->right == old) parent->right = new;
  else assert(0); // old is not a child of parent
}

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
void rotate_right(struct tree *T, struct node *y) {
  assert(y != NULL); // Cannot rotate around an empty tree
  struct node *x = y->left;
  assert(x != NULL); // x will replace y's position, so it cannot be NULL
  
  struct node *z = y->parent;
  struct node *A = x->left;
  struct node *B = x->right;
  struct node *C = y->right;

  swap_child(T, z, y, x);
  set_left(x, A);
  set_right(x, y);
  set_left(y, B);
  set_right(y, C);
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
void rotate_left(struct tree *T, struct node *x) {
  assert(x != NULL); // Cannot rotate around an empty tree
  struct node *y = x->right;
  assert(y != NULL); // y will replace x's position, so it cannot be NULL
  
  struct node *z = x->parent;
  struct node *A = x->left;
  struct node *B = y->left;
  struct node *C = y->right;

  swap_child(T, z, x, y);
  set_right(y, C);
  set_left(y, x);
  set_right(x, B);
  set_left(x, A);
}

// Splay implementation
/**
 * Performs a single splay step on x in the tree T.
 * 
 * @requires T and x cannot be NULL
 */
void splay_step(struct tree *T, struct node *x) {
  assert(T != NULL);
  assert(x != NULL);

  struct node *y = x->parent;
  if (y == NULL) return; // root case, do nothing

  struct node *z = y->parent;
  if (z == NULL) { // single-rotation (zig) cases
    if (y->left == x) {
      /*
       *                                  y             x
       *   Zig (y is the tree root):     /     ====>     \
       *                                x                 y
       */
      rotate_right(T, y);
      return;
    }
    else if (y->right == x) {
      /*
       *                                y                 x
       *   Zig (y is the tree root):     \     ====>     /
       *                                  x             y
       */
      rotate_left(T, y);
      return;
    }
  }

  // otherwise, we have one of the complex cases (zig-zig or zig-zag)
  if (z->left != NULL && z->left->right == x) {
    /*
     *                    z              z
     *                   /              /             x
     *   Zig-zag:       y     ====>    x   ====>     / \
     *                   \            /             y   z
     *                    x          y
     */
    rotate_left(T, y);
    rotate_right(T, z);
  }
  else if (z->right != NULL && z->right->left == x) {
    /*
     *                  z            z
     *                   \            \               x
     *   Zig-zag:         y   ====>    x   ====>     / \
     *                   /              \           z   y
     *                  x                y
     */
    rotate_right(T, y);
    rotate_left(T, z);
  }
  else if (z->left != NULL && z->left->left == x) {
    /*
     *                    z                         x
     *                   /            y              \
     *   Zig-zig:       y     ====>  / \   ====>      y
     *                 /            x   z              \
     *                x                                 z
     */
    rotate_right(T, z);
    rotate_right(T, y); // at the start of this stage, y is at the top
  }
  else if (z->right != NULL && z->right->right == x) {
    /*
     *                z                                 x
     *                 \              y                /
     *   Zig-zig:       y     ====>  / \   ====>      y
     *                   \          z   x            /
     *                    x                         z
     */
    rotate_left(T, z);
    rotate_left(T, y);
  }
  else {
    assert(0); // Invalid tree
  }
}

/**
 * Splays node x to the top of tre T.
 * 
 * @requires x cannot be NULL and must be in the non-NULL tree T
 */
void splay_node(struct tree *T, struct node *x) {
  assert(T != NULL);
  assert(x != NULL);
  while (T->root != x) {
    splay_step(T, x);
  }
}

/**
 * Creates a splay tree of size [n] in a linked list structure
 *       n
 *      /
 *    ...
 *    /
 *   1
 *
 */
struct tree* initialize_tree(unsigned int n) {
  if (n == 0) return NULL;
  struct tree *T = calloc(1, sizeof(struct tree));
  T->size = n;
  T->root = calloc(n, sizeof(struct node));
  struct node *current = T->root;
  // chain the nodes together
  for (int i = 1; i < n; i++) {
    current->value = i;
    current->parent = current + 1;
    current->parent->left = current;
    current = current->parent;
  }
  // at the end of the loop, current is the nth node
  current->value = n;
  T->root = current;
  return T;
}

// Frees the tree structure.
void free_tree(struct tree *T) {
  free(T->root - T->root->value + 1);
  free(T);
}

/**
 * Splays the node in tree T with value k to the top.
 * 
 * @requires T is a standard initialized tree, i.e. it has
 * nodes numbered from 1 to n, where n is the size.
 * @requires k is a valid node between 1 and the size, inclusive.
 * @ensures The node with value k is the root of the tree.
 */
void splay(struct tree *T, int k) {
  assert(T != NULL);
  assert(1 <= k && k <= T->size);
  splay_node(T, T->root - T->root->value + k);
  assert(T->root->value == k);
}

int main() {

  struct tree *T = initialize_tree(10);
  splay(T, 1);
  splay(T, 10);
  splay(T, 4);
  splay(T, 7);

  free_tree(T);

  printf("Splay tests passed\n");

  return 0;
}