/*
 * Copyright 2015 Nicolas Pope
 */

#include "dharc/harc.hpp"

#include <iostream>

#include "dharc/node.hpp"
#include "dharc/fabric.hpp"

using dharc::fabric::Harc;
using dharc::Node;
using dharc::Fabric;


Harc::Harc() :
	head_(dharc::null_n),
	activation_(0.0f),
	delta_(0.0f),
	strength_(0.0f),
	lastactive_(0) {}


void Harc::activate(float value) {
	delta_ = value - activation_;
	activation_ = value;
	lastactive_ = Fabric::counter();
}


void Harc::define(const Node &n) {
	head_ = n;
	lastactive_ = Fabric::counter();
	// Do some more activation??
	// Build strength
	strength_ = (1.0f - strength_) * 0.1f;
}


float Harc::significance() const {
	return 0.0f;
}



std::ostream &dharc::fabric::operator<<(std::ostream &os, const Harc &h) {
	return os;
}

