//////////////
//
// from https://dmoj.ca/problem/noi05p2
// a BBST a day (preferably before brushing teeth) keeps the WAs away...
//
//

const int MAXN = 500100;
const int NOVALUE = 1000000101;
const int VERYBIG = 1000000100;

struct NodeType{
  int pa, c[2];
  char flipped;
  int val_set;
  int val;
  int siz, sum, max_l, max_r, max_s;
  NodeType() {
    pa = c[0] = c[1] = 0;
    val_set = NOVALUE;
    flipped = 0;
    siz = 0;
    sum = 0;
    max_l = max_r = 0;
    max_s = -VERYBIG;
  }
} node[MAXN];

const int nil = 0;

int free_nodes[MAXN], head, tail;

int NewNode() {
  int x;
  scanf("%d", &x);
//printf("::::%d\n", x); fflush(stdout);
  int p = free_nodes[head];
  head = (head + 1) % MAXN;
  node[p] = NodeType();
//printf("%d\n", node[p].val_set);
  node[p].val = node[p].sum = node[p].max_s = x;
  node[p].max_l = node[p].max_r = max(0, x);
  return p;
}

void DeleteTree(int p) {
  if(p) {
    FOR(c, 2) DeleteTree(node[p].c[c]);
    free_nodes[tail] = p;
    tail = (tail + 1) % MAXN;
  }
}

inline void SetValue(int p, int x) {
  if(p) {
    node[p].val_set = x;
    node[p].val = x;
    node[p].sum = x * node[p].siz;
    node[p].max_l = node[p].max_r = max(0, node[p].sum);
    node[p].max_s = max(x, x * node[p].siz);
  }
}

inline void Flip(int p) {
  if(p) {
    node[p].flipped ^= 1;
    swap(node[p].max_l, node[p].max_r);
  }
}

inline void Down(int p) {
  if(p) {
    if(node[p].val_set != NOVALUE) {
      FOR(c, 2) SetValue(node[p].c[c], node[p].val_set);
      node[p].val_set = NOVALUE;
    }
    if(node[p].flipped) {
      swap(node[p].c[0], node[p].c[1]);
      FOR(c, 2) Flip(node[p].c[c]);
      node[p].flipped = 0;
    }
  }
}

inline void Up(int p) {
  FOR(c, 2) Down(node[p].c[c]);
  node[p].siz = node[node[p].c[0]].siz + 1 + node[node[p].c[1]].siz;
  node[p].sum = node[node[p].c[0]].sum + node[p].val  + node[node[p].c[1]].sum;
  node[p].max_l = max(node[node[p].c[0]].max_l, node[node[p].c[0]].sum + node[p].val + node[node[p].c[1]].max_l);
  node[p].max_r = max(node[node[p].c[0]].max_r + node[p].val + node[node[p].c[1]].sum, node[node[p].c[1]].max_r);
  node[p].max_s = node[node[p].c[0]].max_r + node[p].val + node[node[p].c[1]].max_l;
  FOR(c, 2) SetMax<int>(node[p].max_s, node[node[p].c[c]].max_s);
}

int root;

void SetC(int p, int p1, int d) {
  node[p].c[d] = p1;
  if(p1) node[p1].pa = p;
}

int GetDir(int p) {
  return p == node[node[p].pa].c[1];
}

void Rot(int p) {
  int p1 = node[p].pa;
  int d = GetDir(p);
  if(p1 == root) {
    root = p;
    node[p].pa = nil;
  } else {
    SetC(node[p1].pa, p, GetDir(p1));
  }
  SetC(p1, node[p].c[!d], d);
  SetC(p, p1, !d);
  Up(p1);
  Up(p);
}

void Splay(int p) {
  while(p != root) {
    int p1 = node[p].pa;
    if(p1 != root) {
      if(GetDir(p) == GetDir(p1)) {
        Rot(p1);
      } else {
        Rot(p);
      }
    }
    Rot(p);
  }
}