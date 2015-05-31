/*
 * Copyright 2015 Nicolas Pope
 */

#include "dharc/framer.hpp"

#include <list>
#include <vector>

#include "dharc/fabric.hpp"

using std::list;
using std::vector;
using dharc::Node;
using dharc::Harc;

list<Harc*> dharc::Framer::select_partners(const vector<Node> &p, int max) {
	vector<const list<Harc*>*> parts;
	list<Harc*> res;
	auto *heads = new list<Harc*>::const_iterator[p.size()];

	int j = 0;
	for (auto i : p) {
		parts.push_back(&dharc::fabric.partners(i));
		heads[j++] = parts.back()->begin();
	}

	while (max--) {
		Harc *largest = nullptr;
		int ix = -1;

		for (int i = 0; i < static_cast<int>(p.size()); ++i) {
			if (heads[i] == parts[i]->end()) continue;

			if (largest == nullptr) {
				largest = *heads[i];
				ix = i;
			} else {
				if ((*heads[i])->significance() >
						largest->significance()) {
					largest = *heads[i];
					ix = i;
				}
			}
		}

		if (ix == -1) break;
		heads[ix]++;
		res.push_back(largest);
	}

	delete [] heads;

	return res;
}
