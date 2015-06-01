/*
 * Copyright 2015 Nicolas Pope
 */

#include "dharc/harc.hpp"

#include <vector>
#include <iostream>
#include <list>
#include <utility>
#include <string>
#include <future>
#include <sstream>

#include "dharc/node.hpp"
#include "dharc/fabric.hpp"
#include "dharc/definition.hpp"

using dharc::fabric::Harc;
using dharc::Fabric;
using dharc::Node;
using dharc::Definition;
using std::stringstream;



Harc::Harc(const pair<Node, Node> &t) :
	tail_(t),
	head_(null_n),
	flags_(static_cast<unsigned char>(Flag::none)),
	lastquery_(Fabric::counter()),
	strength_(0.0),
	dependants_(nullptr) {}



void Harc::addDependant(const Harc &h) {
	lock_.lock();
	if (!dependants_) dependants_ = new list<const Harc*>();
	dependants_->push_back(&h);
	lock_.unlock();
}



float Harc::significance() const {
	float delta = static_cast<float>(Fabric::counter()) -
					static_cast<float>(lastquery_);
	if (delta == 0) delta = 1.0;
	return 1.0 / delta;
}



float Harc::lastQuery() const {
	float delta = static_cast<float>(Fabric::counter()) -
					static_cast<float>(lastquery_);
	return (delta * Fabric::counterResolution()) / 1000.0f;
}



const Node &Harc::query() const {
	if (lastquery_ < Fabric::counter()) {
		lastquery_ = Fabric::counter();
		Fabric::updatePartners(this);
	}

	lock_.lock();
	if (checkFlag(Flag::defined) && checkFlag(Flag::outofdate)) {
		head_ = def_->evaluate(this);
		clearFlag(Flag::outofdate);
	}
	lock_.unlock();

	return head_;
}



void Harc::dirty() const {
	if (checkFlag(Flag::defined)) setFlag(Flag::outofdate);

	lock_.lock();
	if (!dependants_) {
		lock_.unlock();
		return;
	}
	auto dependants = dependants_;
	dependants_ = nullptr;
	lock_.unlock();

	for (auto i : *dependants) {
		i->dirty();
	}
	delete dependants;
}



void Harc::define(const Node &n) {
	if (checkFlag(Flag::log)) Fabric::logChange(this);

	lock_.lock();
	if (checkFlag(Flag::defined)) {
		delete def_;
		clearFlag(Flag::defined);
	}
	head_ = n;
	lock_.unlock();

	dirty();
}



void Harc::define(const vector<vector<Node>> &definition) {
	lock_.lock();
	if (checkFlag(Flag::defined)) {
		delete def_;
	} else {
		setFlag(Flag::defined);
	}
	def_ = new Definition(definition);
	setFlag(Flag::outofdate);
	lock_.unlock();

	if (checkFlag(Flag::log)) Fabric::logChange(this);

	dirty();
}

std::string Harc::definition() const {
	if (checkFlag(Flag::defined)) {
		stringstream ss;
		ss << *def_;
		return ss.str();
	}
	return (std::string)head_;
}

void Harc::definition(std::ostream &os) const {
	if (checkFlag(Flag::defined)) {
		os << *def_;
	} else {
		os << head_;
	}
}


Harc *Harc::instantiate(const Node &any) {
	/*if (checkFlag(Flag::defined)) {
		Node partner = tailPartner(dharc::any_n);
		Harc *newharc = new Harc(
			(any < partner) ?
			pair<Node, Node>(any, partner) :
			pair<Node, Node>(partner, any));
		newharc->define(def_->instantiate(any));
		return newharc;
	} else {*/
		return this;
	//}
}



std::ostream &dharc::fabric::operator<<(std::ostream &os, const Harc &h) {
	os << '[' << h.tail().first << ',' << h.tail().second
		<< "->" << '?' << ']';
	return os;
}

