/*
 * Copyright 2015 Nicolas Pope
 */

#ifndef FDSB_FRAMER_H_
#define FDSB_FRAMER_H_

#include <list>
#include <vector>

#include "dharc/harc.hpp"

namespace dharc {
class Framer {
	public:
	static std::list<fabric::Harc*> select_partners(
				const std::vector<Node> &p, int max = 10);

	private:
	/*
	 * Re-generate the harc list from the existing list plus recent changes
	 * plus significant path searches plus external significance suggestions.
	 */
	void repopulate();

	/*
	 * For the updated harc list, recalculate significance data.
	 */
	void update_significance();

	/*
	 * For the updated harc list and significance data, sort the harcs by
	 * their significance.
	 */
	void sort();

	/*
	 * Remove harcs from the harc list that have low significance to
	 * maintain a maximum size limit for the list.
	 */
	void cull();
};
};  // namespace dharc

#endif  /* FDSB_FRAMER_H_ */
