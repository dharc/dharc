/*
 * Copyright 2015 Nicolas Pope
 */

#ifndef DHARC_FABRIC_ELEMENT_HPP_
#define DHARC_FABRIC_ELEMENT_HPP_

#include <vector>
#include <array>
#include <algorithm>
#include <math.h>

using std::vector;
using std::array;

namespace dharc {
struct Link {
	Link() : energy(0.0f), strength(0.0f) {}

	float energy;
	float strength;

	inline void set(float e) { energy = e * strength; }
	inline float get() const { return energy; }
};

class ElementCore {
	public:
	static constexpr float kThreshold = 0.1f;
	static constexpr float kLearnRate = 0.2f;

	ElementCore() : oldfreq_(0), newfreq_(0) {}
	void forward();

	inline void setFrequency(uint8_t freq) { newfreq_ = freq; }
	void reset() {
		oldfreq_ = 0;
	}

	protected:
	uint8_t oldfreq_;
	uint8_t newfreq_;
	vector<Link*> outputs_;
};

template<size_t I>
class Element : public ElementCore {
	public:
	Element() {};
	~Element() {};

	static constexpr float kEnergyLimit = 0.2f * (float)I;

	inline float integrate() const;
	static void learn(ElementCore *ele);

	vector<Link*> deadLinks(size_t count);
	inline void addLink(Link *link) { outputs_.push_back(link); }

	private:
	array<Link, I> inputs_;
};
};



template<size_t I>
inline float dharc::Element<I>::integrate() const {
	float energy = 0.0f;
	for (auto i = 0U; i < I; ++i) {
		energy += inputs_[i].energy;
	}

	energy /= kEnergyLimit;

	// Limit to 1 and decay otherwise
	if (energy > 1.0f) {
		energy = 1.0f - (energy - 1.0f);
	}

	return energy;
}



template<size_t I>
void dharc::Element<I>::learn(dharc::ElementCore *ele) {
	auto &this_ = *static_cast<dharc::Element<I>*>(ele);
	float energy = (float)this_.newfreq_ / 255.0f;
	
	if (this_.oldfreq_ == 0 && this_.newfreq_ > 0) {
		vector<Link*> sorted(I);
		for (auto i = 0U; i < I; ++i) sorted[i] = &this_.inputs_[i];
		std::sort(sorted.begin(), sorted.end(), [](auto a, auto b) {
			return a->energy > b->energy;
		});

		float energysum = 0.0f;

		for (auto i = 0U; i < I; ++i) {
			if (energysum >= kThreshold) {
				sorted[i]->strength -=	sorted[i]->energy *
										energy * kLearnRate;
			} else {
				float original = sorted[i]->energy / sorted[i]->strength;
				sorted[i]->strength +=	original *
										(1.0f - sorted[i]->strength) *
										energy * kLearnRate;
			}

			energysum += sorted[i]->energy;
		}
	}

	this_.oldfreq_ = this_.newfreq_;
}



void dharc::ElementCore::forward() {
	if ((newfreq_ == oldfreq_) == 0) return;
	float energy = (float)newfreq_ / 255.0f;
	for (auto o : outputs_) {
		if (o) o->set(energy);
	}
}



template<size_t I>
vector<dharc::Link*> dharc::Element<I>::deadLinks(size_t count) {
	vector<dharc::Link*> deads;
	for (auto i = 0U; i < I; ++i) {
		if (inputs_[i].strength == 0.0f) {
			deads.push_back(&inputs_[i]);
			--count;
			if (count == 0) return deads;
		}
	}

	return deads;
}


#endif  // DHARC_FABRIC_ELEMENT_HPP_

