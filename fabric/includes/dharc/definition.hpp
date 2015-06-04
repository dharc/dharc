/*
 * Copyright 2015 Nicolas Pope
 */

#ifndef DHARC_DEFINITION_HPP_
#define DHARC_DEFINITION_HPP_

#include <string>
#include <vector>
#include <sstream>

#include "dharc/node.hpp"

using std::vector;
using std::string;

namespace dharc {

namespace fabric {
class Harc;
};  // namespace fabric

/**
 * Represent a Harc definition and provide a means of converting it to/from
 * various forms as well as doing any internal optimisations for frequently
 * evaluated definitions. Each definition uses a lazy evaluation approach.
 */
class Definition {
	friend class fabric::Harc;

	public:
	/**
	 * Evaluate the definition path and return the result. It may return only
	 * a cached result rather than actually performing an expensive evaluation.
	 * @param harc The hyperarc that this definition is evaluated for.
	 * @return Result of following definition path.
	 */
	Node evaluate(const fabric::Harc *harc) const;


	inline static Definition *makeDefinition(const vector<Node> &);

	private:
	vector<Node> path_;

	Definition() = delete;
	explicit Definition(const vector<Node> &);

	vector<Node> instantiate(const Node &any);

	friend std::ostream &operator<<(std::ostream &os, const Definition &d);
};


std::ostream &operator<<(std::ostream &os, const Definition &d);


inline Definition *Definition::makeDefinition(const vector<Node> &def) {
	return new Definition(def);
}

};  // namespace dharc

#endif  /* FDSB_DEFINITION_H_ */
