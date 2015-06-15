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

float Harc::lastActive() const {
	float delta = static_cast<float>(Fabric::counter()) -
					static_cast<float>(lastactive_);
	return 1.0f + (delta);
}


Harc::Harc() :
	head_(dharc::null_n),
	delta_(0.0f),
	strength_(0.0001f),
	lastactive_(0) {}

void Harc::reset() {
	strength_ = 0.0001f;
	lastactive_ = 0;
	head_ = dharc::null_n;
	delta_ = 0.0f;
}

float Harc::decayedActivation() const {
	// Simple linear decay over time
	return activation_ / (0.1 * lastActive());
}


bool Harc::query(const Node &node) {
	if (node == head_) {
		// Boost strength
		strength_ += (1.0f - strength_) * 0.001f;
		if (strength_ > 0.01) {
			//std::cout << "STRONG\n";
		}
		return true;
	} else {
		// Reduce strength
		strength_ -= strength_ * 0.1f;
		head_ = node;
		return false;
	}
}


void Harc::pulse(float s) {
	lock_.lock();
	delta_ = s * strength_;
	strength_ += (1.0f - strength_) * 0.02f;
	// activation_ = value;
	lastactive_ = Fabric::counter();
	lock_.unlock();
}

void Harc::activateConstant(float value) {
	lock_.lock();
	delta_ = value - activation_;
	activation_ = value;
	lastactive_ = Fabric::counter();
	lock_.unlock();
}


float Harc::significance() const {
	// Simple linear decayed absolute delta
	float decdelta = delta_; // / lastActive();
	return (decdelta < 0.0) ? 0.0f - decdelta : decdelta;
}



std::ostream &dharc::fabric::operator<<(std::ostream &os, const Harc &h) {
	return os;
}

