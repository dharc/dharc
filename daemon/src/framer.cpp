/*
 * Copyright 2015 Nicolas Pope
 */

#include "fdsb/framer.hpp"

#include <list>
#include <vector>

#include "fdsb/fabric.hpp"

using std::list;
using std::vector;
using fdsb::Nid;
using fdsb::Harc;

list<Harc*> fdsb::Framer::select_partners(const vector<Nid> &p, int max) {
	vector<const list<Harc*>*> parts;
	list<Harc*> res;
	auto *heads = new list<Harc*>::const_iterator[p.size()];

	int j = 0;
	for (auto i : p) {
		parts.push_back(&fdsb::fabric.partners(i));
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