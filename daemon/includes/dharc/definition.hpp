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
class Harc;

/**
 * Represent a Harc definition and provide a means of converting it to/from
 * various forms as well as doing any internal optimisations for frequently
 * evaluated definitions. Each definition uses a lazy evaluation approach.
 */
class Definition {
	friend class Harc;

	public:
	/**
	 * Evaluate the definition path and return the result. It may return only
	 * a cached result rather than actually performing an expensive evaluation.
	 * @param harc The hyperarc that this definition is evaluated for.
	 * @return Result of following definition path.
	 */
	Node evaluate(const Harc *harc) const;


	inline static Definition *makeDefinition(const vector<vector<Node>> &);

	private:
	vector<vector<Node>> path_;

	Definition() = delete;
	explicit Definition(const vector<vector<Node>> &);

	friend std::ostream &operator<<(std::ostream &os, const Definition &d);
};


std::ostream &operator<<(std::ostream &os, const Definition &d);


inline Definition *Definition::makeDefinition(const vector<vector<Node>> &def) {
	return new Definition(def);
}

};  // namespace dharc

#endif  /* FDSB_DEFINITION_H_ */
