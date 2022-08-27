package java;

public class SplayTreeStarter {

    static class SplayTree {

        public Node root;
        public Node[] contents;

        class Node {
            public Node parent, left, right;
            public int value;

            public Node (int value) {
                this.value = value;
            }
        }

        /**
         * Set [parent]'s left child to [left], updating [left]'s parent pointer as necessary.
         * 
         * @requires [parent] is not null
         */
        public void setLeft (Node parent, Node left) {
            if (parent == null) {
                throw new IllegalArgumentException("Cannot set the left child of an empty tree");
            }
            parent.left = left;
            if (left != null) {
                left.parent = parent;
            }
        }

        /**
         * Set [parent]'s right child to [right], updating [right]'s parent pointer as necessary.
         * 
         * @requires [parent] is not null
         */
        public void setRight (Node parent, Node right) {
            if (parent == null) {
                throw new IllegalArgumentException("Cannot set the right child of an empty tree");
            }
            parent.right = right;
            if (right != null) {
                right.parent = parent;
            }
        }

        /**
         * Swap [oldChild] of [parent] for [newChild]
         * 
         * @requires [oldChild] is not null and is a child of [parent]
         */
        public void swapChild (Node parent, Node oldChild, Node newChild) {
            if (oldChild == null) {
                throw new IllegalArgumentException("Cannot swap for an empty child");
            }
            newChild.parent = parent;
            if (parent == null) {
                root = newChild;
            } else if (parent.left == oldChild) {
                parent.left = newChild;
            } else if (parent.right == oldChild) {
                parent.right = newChild;
            } else {
                throw new IllegalArgumentException("Can only swap for an existing child");
            }
        }

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
        public void rotateRight (Node y) {
            if (y == null) {
                throw new IllegalArgumentException("Cannot rotate an empty tree");
            }
            Node x = y.left;
            if (x == null) {
                throw new IllegalArgumentException("Cannot rotate right with no left child");
            }
            Node z = y.parent, A = x.left, B = x.right, C = y.right;
            swapChild(z, y, x);
            setLeft(x, A);
            setRight(x, y);
            setLeft(y, B);
            setRight(y, C);
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
        public void rotateLeft (Node x) {
            if (x == null) {
                throw new IllegalArgumentException("Cannot rotate an empty tree");
            }
            Node y = x.right;
            if (y == null) {
                throw new IllegalArgumentException("Cannot rotate left with no right child");
            }
            Node z = x.parent, A = x.left, B = y.left, C = y.right;
            swapChild(z, x, y);
            setRight(y, C);
            setLeft(y, x);
            setRight(x, B);
            setLeft(x, A);
        }

        /**
         * Performs a single splay step on [x]
         * 
         * @requires [x] is not null or already the root
         */
        public void splayStep (Node x) {
            if (x == null) {
                throw new IllegalArgumentException("Cannot run a splay step on an empty tree");
            }
            Node y = x.parent;
            if (y == null) {
                throw new IllegalArgumentException("Cannot run a splay step on a node already at the root");
            }
            Node z = y.parent;
            if (z == null && y.left == x) {
                /*
                 *                                  y             x
                 *   Zig (y is the tree root):     /     ====>     \
                 *                                x                 y
                 */
                rotateRight(y);
            } else if (z == null && y.right == x) {
                /*
                 *                                y                 x
                 *   Zig (y is the tree root):     \     ====>     /
                 *                                  x             y
                 */
                rotateLeft(y);
            } else if (z.left == y && y.right == x) {
                /*
                 *                    z              z
                 *                   /              /             x
                 *   Zig-zag:       y     ====>    x   ====>     / \
                 *                   \            /             y   z
                 *                    x          y
                 */
                rotateLeft(y);
                rotateRight(z);
            } else if (z.right == y && y.left == x) {
                /*
                 *                  z            z
                 *                   \            \               x
                 *   Zig-zag:         y   ====>    x   ====>     / \
                 *                   /              \           z   y
                 *                  x                y
                 */
                rotateRight(y);
                rotateLeft(z);
            } else if (z.left == y && y.left == x) {
                /*
                 *                    z                         x
                 *                   /            y              \
                 *   Zig-zig:       y     ====>  / \   ====>      y
                 *                 /            x   z              \
                 *                x                                 z
                 */
                rotateRight(z);
                rotateRight(y);
            } else if (z.right == y && y.right == x) {
                /*
                 *                z                                 x
                 *                 \              y                /
                 *   Zig-zig:       y     ====>  / \   ====>      y
                 *                   \          z   x            /
                 *                    x                         z
                 */
                rotateLeft(z);
                rotateLeft(y);
            } else {
                throw new IllegalStateException("Cannot perform splay step due to invalid tree");
            }
        }

        /**
         * Splays [x] to the root
         * 
         * @requires [x] is not null
         */
        public void splay (Node x) {
            if (x == null) {
                throw new IllegalArgumentException("Cannot splay an empty tree");
            }
            while (x.parent != null) {
                splayStep(x);
            }
        }

        /**
         * Creates a splay tree of size [n] in a linked list structure
         *       n
         *      /
         *    ...
         *    /
         *   1
         */
        public SplayTree (int n) {
            Node prev = null;
            for (int i = 0; i < n; i++) {
                Node next = new Node(i);
                setLeft(next, prev);
                prev = next;
            }
            root = prev;
        }
    }
}