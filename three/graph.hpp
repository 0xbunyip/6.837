#ifndef GRAPH_H
#define GRAPH_H

#include <vector>

template<typename V, typename E>
class Graph {
  V& v(const int i) { return v_[i]; }
  E& e(const int i) { return e_[i]; }

  int addV(const V& v) {
    v_.push_back(v);
    return v_.size() - 1;
  }

  int addE(const E& e, const int v1, const int v2) {
    e_.push_back(e);
    int eid = e_.size() - 1;
    adj[v1].push_back(eid);
    adj[v2].push_back(eid);
    return eid;
  }

public:
  std::vector<V> v_;
  std::vector<E> e_;
  std::vector<std::vector<int>> adj;
};

#endif
