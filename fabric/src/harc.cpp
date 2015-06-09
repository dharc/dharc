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


inline float Harc::lastActive() const {
	float delta = static_cast<float>(Fabric::counter()) -
					static_cast<float>(lastactive_);
	return 1.0f + (delta);
}


Harc::Harc() :
	head_(dharc::null_n),
	activation_(0.0f),
	delta_(0.0f),
	strength_(0.0f),
	lastactive_(1) {}

float Harc::decayedActivation() const {
	// Simple linear decay over time
	return activation_ / lastActive();
}


void Harc::activatePulse(float value) {
	//float decayed = decayedActivation();
	//if (value > decayed) {
		delta_ = value - decayedActivation();
		activation_ = value;
		lastactive_ = Fabric::counter();
	//}
}

void Harc::activateConstant(float value) {
	//float decayed = decayedActivation();
	//if (value > decayed) {
		delta_ = value - activation_;
		activation_ = value;
		lastactive_ = Fabric::counter();
	//}
}


void Harc::define(const Node &n) {
	head_ = n;
	// Build strength
	strength_ += (1.0f - strength_) * 0.01f;
	Fabric::activatePulse(n, strength_);
}


float Harc::significance() const {
	// Simple linear decayed absolute delta
	float decdelta = delta_ / lastActive();
	return (decdelta < 0.0) ? 0.0f - decdelta : decdelta;
}



std::ostream &dharc::fabric::operator<<(std::ostream &os, const Harc &h) {
	return os;
}

