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

// Struct definitions
struct node {
  struct node* parent;
  struct node* left;
  struct node* right;

  int value;
};

struct tree {
  int size;
  struct node* root;
};

// Tree setting functions
/**
 * Sets P's left child to L and updates L's parent to be P if L isn't NULL.
 * 
 * @requires P cannot be NULL
 */
void set_left(struct node *parent, struct node *left) {
  if (parent == NULL) printf("missing node\n");
  parent->left = left;
  if (left != NULL) left->parent = parent;
}

/**
 * Sets P's left child to R and updates R's parent to be P if R isn't null.
 * 
 * @requires P cannot be NULL
 */
void set_right(struct node *parent, struct node *right) {
  if (parent == NULL) printf("missing node\n");
  parent->right = right;
  if (right != NULL) right->parent = parent;
}

/**
 * Changes P's child to be new instead of old. If P is NULL, this instead
 * assumes old is the root and swaps it out accordingly.
 * 
 * @requires old is the root if P is NULL, or else old is a non-NULL child of P
 */
void swap_child(struct tree *T, struct node *parent, struct node *old, struct node *new) {
  if (old == NULL) printf("Can't swap out empty node\n");
  new->parent = parent;

  if (parent == NULL) {
    if (T->root != old) printf("Can't swap out a non-root node at the root\n");
    T->root = new;
  }
  else if (parent->left == old) parent->left = new;
  else if (parent->right == old) parent->right = new;
  else printf("Failed to find %d as a child of %d\n", old->value, parent->value);
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
  if (y == NULL) printf("Cannot rotate around NULL\n");
  struct node *x = y->left;
  if (x == NULL) printf("No left node found to replace the root\n");
  
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
  if (x == NULL) printf("Cannot rotate around NULL\n");
  struct node *y = x->right;
  if (y == NULL) printf("No right node found to replace the root\n");
  
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
 * Performs a single splay step on x.
 * 
 * @requires x cannot be NULL
 */
void splay_step(struct tree *T, struct node *x) {
  if (T == NULL) printf("Tree not found\n");
  if (x == NULL) printf("Can't splay NULL\n");

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
    printf("Invalid tree\n");
  }
}

/**
 * Splays node x to the top of tre T.
 * 
 * @requires x cannot be NULL and must be in the tree
 */
void splay_node(struct tree *T, struct node *x) {
  if (T == NULL) printf("No tree found\n");
  if (x == NULL) printf("Can't splay NULL\n");
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
 * @requires n is nonnegative
 */
struct tree* initialize_tree(int n) {
  if (n < 0) printf("Invalid size\n");
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

/**
 * Splays the node in tree T with value k to the top.
 * 
 * @requires T is a standard initialized tree, i.e. it has
 * nodes numbered from 1 to n, where n is the size.
 */
void splay(struct tree *T, int k) {
  if (T == NULL) printf("Tree cannot be NULL\n");
  if (k <= 0 || k > T->size) printf("Invalid node request\n");
  splay_node(T, T->root + (k - T->root->value));
  if (T->root->value != k) {
    printf("Splay failed\n");
  }
}

int main() {

  struct tree *T = initialize_tree(10);
  splay(T, 1);
  splay(T, 10);
  splay(T, 4);
  splay(T, 7);

  printf("Splay tests passed\n");

  return 0;
}