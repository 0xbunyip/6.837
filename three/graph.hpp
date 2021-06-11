#ifndef GRAPH_H
#define GRAPH_H

#include "util.h"

#include <vector>

template<typename V, typename E>
class Graph {
public:
  V& v(const int i) { return v_[i]; }
  E& e(const int i) { return e_[i]; }

  int addV(V&& v) {
    v_.push_back(std::move(v));
    adj.push_back(std::vector<int>());
    return v_.size() - 1;
  }

  int addE(E&& e, const int v1, const int v2) {
    e_.push_back(std::move(e));
    int eid = e_.size() - 1;
    adj[v1].push_back(eid);
    adj[v2].push_back(eid);
    return eid;
  }

private:
  std::vector<V> v_;
  std::vector<E> e_;
  std::vector<std::vector<int>> adj;
};

#endif
