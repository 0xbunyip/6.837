#ifndef GRAPH_H
#define GRAPH_H

#include "util.h"

#include <vector>

struct Adj {
  int eid;
  int v;
};

template<typename V, typename E>
class Graph {
public:
  V& v(const int i) { return v_[i]; }
  E& e(const int i) { return e_[i]; }
  std::vector<Adj>& adj(const int i) { return adj_[i]; }

  int addV(V&& v) {
    v_.push_back(std::move(v));
    adj_.push_back(std::vector<Adj>());
    return v_.size() - 1;
  }

  int addE(E&& e, const int v1, const int v2) {
    e_.push_back(std::move(e));
    int eid = e_.size() - 1;
    adj_[v1].push_back(Adj{eid, v2});
    adj_[v2].push_back(Adj{eid, v1});
    return eid;
  }

private:
  std::vector<V> v_;
  std::vector<E> e_;
  std::vector<std::vector<Adj>> adj_;
};

#endif
