//  This file is part of Empirical, https://github.com/devosoft/Empirical
//  Copyright (C) Michigan State University, 2015-2016.
//  Released under the MIT Software license; see doc/LICENSE
//
//
// A simple class to weight items differently within a container.
//
// Constructor:
// WeightedSet(int num_items)
//     num_items is the maximum number of items that can be placed into the data structure.
//
// void Adjust(int id, double weight)
//     id is the identification number of the item whose weight is being adjusted.
//     weight is the new weight for that entry.
//
//
// Development NOTES:
//   * We should probably change the name to something like WeightedRandom since it does not
//     have to be used just for scheduling.
//   * We could easily convert this structure to a template that acts as a glorified vector
//     giving the ability to perform a weighted random choice.
//   * We should allow the structure to be resized, either dynamically or through a Resize()
//     method.

#ifndef EMP_WEIGHTED_SET_H
#define EMP_WEIGHTED_SET_H

#include "vector.h"

namespace emp {

  class WeightedSet {
  private:
    struct WeightInfo { double item=0.0; double tree=0.0; };
    emp::vector<WeightInfo> weight;

    int ParentID(int id) const { return (id-1) / 2; }
    int LeftID(int id) const { return 2*id + 1; }
    int RightID(int id) const { return 2*id + 2; }
    bool IsLeaf(int id) const { return 2*id >= (int) weight.size(); }

  public:
    WeightedSet(int num_items) : weight(num_items) {;}
    WeightedSet(const WeightedSet &) = default;
    WeightedSet(WeightedSet &&) = default;
    ~WeightedSet() = default;
    WeightedSet & operator=(const WeightedSet &) = default;
    WeightedSet & operator=(WeightedSet &&) = default;

    int GetSize() const { return (int) weight.size(); }
    double GetWeight(int id) const { return weight[id].item; }

    // Standard library compatibility
    size_t size() const { return weight.size(); }

    void Adjust(int id, const double new_weight) {
      // Update this node.
      double weight_diff = new_weight - weight[id].item;
      weight[id].item = new_weight;       // Update item weight
      weight[id].tree += weight_diff;

      // Update tree to root.
      while (id > 0) {
        id = ParentID(id);
        weight[id].tree += weight_diff;
      }
    }

    int Index(double index, int cur_id=0) {
      // If our target is in the current node, return it!
      const double cur_weight = weight[cur_id].item;
      if (index < cur_weight) return cur_id;

      // Otherwise determine if we need to recurse left or right.
      index -= cur_weight;
      const int left_id = LeftID(cur_id);
      const double left_weight = weight[left_id].tree;

      return (index < left_weight) ? Index(index, left_id) : Index(index-left_weight, left_id+1);
    }

    int operator[](double index) { return Index(index,0); }

  };
};

#endif
