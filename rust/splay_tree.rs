/*
* splay_tree.rs
*
* Rust implementation of a fixed-size splay tree for 15-451 (Algorithm Design and Analysis).
*
* For more information, consult the lecture notes:
*
* https://www.cs.cmu.edu/~15451-f22/lectures/lec04-splay.pdf
*
* Shiva Peri <shivanap@andrew.cmu.edu>
* Last update: 09/18/22
*/

#[derive(Default, Copy, Clone)]
struct Node {
    parent: Option<usize>,  // None, or Some(Index) into Vec<Nodes>
    left: Option<usize>,    // None, or Some(Index) into Vec<Nodes>
    right: Option<usize>    // None, or Some(Index) into Vec<Nodes>
}

#[derive(Clone)]
struct SplayTree {
    root: usize,            // index of root Node in SplayTree::nodes
    nodes: Vec<Node>        // vector of nodes
}

impl SplayTree {

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
    pub fn new(n: usize) -> Self {
        
        let mut nodes = Vec::with_capacity(n);
        for i in 0..n {
            match i {
                0 => nodes.push(Node{ parent: Some(i + 1), left: None, right: None }),
                _ => nodes.push(Node{ parent: Some(i + 1), left: Some(i - 1), right: None })
            }
        }
        nodes[n-1].parent = None;
        
        Self { root: n-1, nodes }
    }

    // for debugging
    pub fn print(&self) {

        println!("\nroot: {} \nnodes:", self.root);

        for i in 0..(self.nodes.len()) {
            let node = &self.nodes[i];

            print!("value: {}, ", i); 
            match node.parent {
                None => print!("parent: none, "),
                Some(x) => print!("parent: {}, ", x)
            }
            match node.left {
                None => print!("left: none, "),
                Some(x) => print!("left: {}, ", x)
            }
            match node.right {
                None => print!("right: none\n"),
                Some(x) => print!("right: {}\n", x)
            }
        }
    }

    // Set the parent of the node at node_idx to the given node
    fn set_parent(&mut self, node_idx: usize, parent_idx: Option<usize>) {
        assert!(node_idx < self.nodes.len());
        let node = &mut self.nodes[node_idx];
        node.parent = parent_idx;
    }

    // Set the left child of the node at node_idx to the given node
    fn set_left(&mut self, node_idx: usize, left_idx: Option<usize>) {

        assert!(node_idx < self.nodes.len());
        let node = &mut self.nodes[node_idx];
        node.left = left_idx;

        match left_idx {
            Some(i) => self.set_parent(i, Some(node_idx)),
            _ => ()
        }
    }

    // Set the right child of the node at node_idx to the given node
    fn set_right(&mut self, node_idx: usize, right_idx: Option<usize>) {

        assert!(node_idx < self.nodes.len());
        let node = &mut self.nodes[node_idx];
        node.right = right_idx;

        match right_idx {
            Some(i) => self.set_parent(i, Some(node_idx)),
            _ => ()
        }
    }

    // Replace whichever child is currently equal to old with new_child
    fn replace_child(&mut self, node_idx: usize, old_child: Option<usize>, new_child: Option<usize>) {

        assert!(old_child != None);
        assert!(new_child != None);
        assert!(node_idx < self.nodes.len());
        let node = &mut self.nodes[node_idx];

        assert!(node.left == old_child || node.right == old_child);
        if node.left == old_child { node.left = new_child; }
        else { node.right = new_child; }
        self.set_parent(new_child.unwrap(), Some(node_idx));
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
    fn rotate_right(&mut self, y_idx: Option<usize>) {

        let y = &self.nodes[y_idx.unwrap()];     // Cannot rotate around an empty tree
        let x_idx = y.left;
        let x = &self.nodes[x_idx.unwrap()];     // x will replace y's position, so it cannot be null
        
        let z_idx = y.parent;
        let a_idx = x.left;
        let b_idx = x.right;
        let c_idx = y.right;

        self.set_left(x_idx.unwrap(), a_idx);       // x->set_left(A);
        self.set_right(x_idx.unwrap(), y_idx);      // x->set_right(y);
        self.set_left(y_idx.unwrap(), b_idx);       // y->set_left(B);
        self.set_right(y_idx.unwrap(), c_idx);      // y->set_right(C);

        match z_idx {
            None => self.set_root(x_idx),           // y is the root
            Some(i) => self.replace_child(i, y_idx, x_idx)
        }
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
    fn rotate_left(&mut self, x_idx: Option<usize>) {

        let x = &self.nodes[x_idx.unwrap()];    // Cannot rotate around an empty tree
        let y_idx = x.right;
        let y = &self.nodes[y_idx.unwrap()];    // y will replace x's position, so it cannot be null
        
        let z_idx = x.parent;
        let a_idx = x.left;
        let b_idx = y.left;
        let c_idx = y.right;

        self.set_right(y_idx.unwrap(), c_idx);      // y->set_right(C);
        self.set_left(y_idx.unwrap(), x_idx);       // y->set_left(x);
        self.set_right(x_idx.unwrap(), b_idx);      // x->set_right(B);
        self.set_left(x_idx.unwrap(), a_idx);       // x->set_left(A);

        match z_idx {
            None => self.set_root(y_idx),           // x is the root
            Some(i) => self.replace_child(i, x_idx, y_idx)
        }
    }

    fn splay_step(&mut self, x_idx: Option<usize>) {
        assert!(x_idx != None);

        let x = &self.nodes[x_idx.unwrap()];
        let y_idx = x.parent;

        if y_idx == None {()}   // root case, do nothing
        let y = &self.nodes[y_idx.unwrap()];
        let z_idx = y.parent;
        
        match z_idx {
            None => {   // single-rotation (zig) cases
                assert!(y.left == x_idx || y.right == x_idx);

                if y.left == x_idx {
                    /*
                    *                                  y             x
                    *   Zig (y is the tree root):     /     ====>     \
                    *                                x                 y
                    */
                    self.rotate_right(y_idx)
                }
                else if y.right == x_idx {
                    /*
                    *                                y                 x
                    *   Zig (y is the tree root):     \     ====>     /
                    *                                  x             y
                    */
                    self.rotate_left(y_idx)
                }
                else {
                    panic!("No case");
                }
            },
            Some(i) => {    // otherwise, we have one of the complex cases (zig-zig or zig-zag)

                let z = &self.nodes[i];

                let z_left = z.left;
                let z_left_left = match z.left {
                    Some(idx) => self.nodes[idx].left,
                    None => None };
                let z_left_right = match z.left {
                    Some(idx) => self.nodes[idx].right,
                    None => None };

                let z_right = z.right;
                let z_right_left = match z.right {
                    Some(idx) => self.nodes[idx].left,
                    None => None };
                let z_right_right = match z.right {
                    Some(idx) => self.nodes[idx].right,
                    None => None };

                assert!((z_left != None && z_left_right == x_idx) ||
                        (z_right != None && z_right_left == x_idx) ||
                        (z_left != None && z_left_left == x_idx) ||
                        (z_right != None && z_right_right == x_idx));
                
                if z_left != None && z_left_right == x_idx {
                    /*
                        *                    z              z
                        *                   /              /             x
                        *   Zig-zag:       y     ====>    x   ====>     / \
                        *                   \            /             y   z
                        *                    x          y
                        */
                    self.rotate_left(y_idx);
                    self.rotate_right(z_idx)
                }
                else if z_left != None && z_left_left == x_idx {
                    /*
                        *                    z                         x
                        *                   /            y              \
                        *   Zig-zig:       y     ====>  / \   ====>      y
                        *                 /            x   z              \
                        *                x                                 z
                        */
                    self.rotate_right(z_idx);
                    self.rotate_right(y_idx)     // at the start of this stage, y is at the top
                } 
                else if z_right != None && z_right_left == x_idx {
                    /*
                        *                  z            z
                        *                   \            \               x
                        *   Zig-zag:         y   ====>    x   ====>     / \
                        *                   /              \           z   y
                        *                  x                y
                        */
                    self.rotate_right(y_idx);
                    self.rotate_left(z_idx)
                }
                else if z_right != None && z_right_right == x_idx {
                    /*
                        *                z                                 x
                        *                 \              y                /
                        *   Zig-zig:       y     ====>  / \   ====>      y
                        *                   \          z   x            /
                        *                    x                         z
                        */
                    self.rotate_left(z_idx);
                    self.rotate_left(y_idx)
                }
                else {
                    panic!("No case");
                }
            }
        }
    }

    // main interface function
    pub fn splay(&mut self, x_idx: Option<usize>) {
        assert!(x_idx != None);
        while self.root != x_idx.unwrap() {
            self.splay_step(x_idx);
        }
    }

    fn set_root(&mut self, x_idx: Option<usize>) {
        assert!(x_idx != None);

        self.root = x_idx.unwrap();
        let x = &mut self.nodes[x_idx.unwrap()];
        x.parent = None;
    }
}

fn main() {
    let mut tree: SplayTree = SplayTree::new(10);
    tree.print();

    println!("splaying 5 ----------");
    tree.splay(Some(5));
    tree.print();
}
