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
	lock_.lock();
	delta_ = value;
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


void Harc::define(const Node &n) {
	lock_.lock();
	// Build strength
	if (head_ == n) {
		strength_ += (1.0f - strength_) * 0.01f;
	} else {
		strength_ -= strength_ * 0.01f;
	}
	head_ = n;
	lock_.unlock();
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

