/*
 * Copyright 2015 Nicolas Pope
 */

#include "dharc/region.hpp"

using dharc::fabric::Region;

template<
size_t USIZE,
size_t UNITSX,
size_t UNITSY,
size_t SMAX,
size_t TMAX
>
Region<USIZE,UNITSX,UNITSY,SMAX,TMAX>::Region() {
	for (auto i = 0U; i <kUnitCount; ++i) {
		initUnit(i);
	}

	for (auto i = 0U; i < kInputSize; ++i) {
		inputs_[i] = 0.0f;
	}
}



template<
size_t USIZE,
size_t UNITSX,
size_t UNITSY,
size_t SMAX,
size_t TMAX
>
Region<USIZE,UNITSX,UNITSY,SMAX,TMAX>::~Region() {
}



template<
size_t USIZE,
size_t UNITSX,
size_t UNITSY,
size_t SMAX,
size_t TMAX
>
void Region<USIZE,UNITSX,UNITSY,SMAX,TMAX>::
initUnit(size_t ix) {
	float maxdist = std::sqrt((float)USIZE + (float)USIZE);
	for (auto x = 0U; x < SMAX; ++x) {
		const int xi = (int)(((float)x / (float)SMAX) * (float)USIZE);
		const int xx = xi % kUnitSqrt;
		const int xy = xi / kUnitSqrt;

		units_[ix].counts[x] = 0.0f;

		for (auto y = 0U; y < USIZE; ++y) {
			const int yx = y % kUnitSqrt;
			const int yy = y / kUnitSqrt;
			const int dx = xx - yx;
			const int dy = xy - yy;
			const float dist = std::sqrt(dx * dx + dy * dy) / maxdist;
			
			units_[ix].links[y * SMAX + x].strength = 1.0f - dist;
			units_[ix].links[y * SMAX + x].depol = 0.0f;

			units_[ix].counts[x] += units_[ix].links[y * SMAX + x].strength;
		}
	}

	for (auto i = 0U; i < SMAX; ++i) {
		units_[ix].outputs[i] = 0.0f;
	}

	units_[ix].modulation = 0.5f;
}



template<
size_t USIZE,
size_t UNITSX,
size_t UNITSY,
size_t SMAX,
size_t TMAX
>
void Region<USIZE,UNITSX,UNITSY,SMAX,TMAX>::
write(const vector<float> &v) {
	assert(v.size() == kInputSize);

	clearInput();

	for (auto i = 0U; i < v.size(); ++i) {
		inputs_[i] += (1.0f - inputs_[i]) * v[i];
	}
}



template<
size_t USIZE,
size_t UNITSX,
size_t UNITSY,
size_t SMAX,
size_t TMAX
>
void Region<USIZE,UNITSX,UNITSY,SMAX,TMAX>::
process() {
	adjustModulation();

	#pragma omp parallel for
	for (auto i = 0U; i < kUnitCount; ++i) {
		//decaySpatial(i);
		activate(units_[i], &inputs_[i * USIZE], USIZE);
		//adjustSpatial(i, s);
	}

	//clearInput();

	for (auto i = 0U; i < kUnitCount; ++i) {
		//decayTemporal(i);
		//activateTemporal(i);
		//adjustTemporal(i);
	}

	// Interunit links
	// Generate merged output
}



template<
size_t USIZE,
size_t UNITSX,
size_t UNITSY,
size_t SMAX,
size_t TMAX
>
void Region<USIZE,UNITSX,UNITSY,SMAX,TMAX>::
adjustModulation() {
	for (auto i = 0U; i < kUnitCount; ++i) {
		const auto ibase = i * USIZE;
		const int ux = i % UNITSX;
		const int uy = i / UNITSX;
		float energy = 0.0f;

		for (auto j = 0U; j < USIZE; ++j) {
			energy += inputs_[ibase + j];
		}

		energy /= USIZE;

		energy = 1.0f - energy;
		energy = energy - 0.5f;
		//units_[i].modulation = 0.5f - (0.1f * energy);
		energy =  0.125f * kModChangeRate * energy;

		boostModulation(ux-1, uy-1, energy);
		boostModulation(ux-1, uy, energy);
		boostModulation(ux-1, uy+1, energy);
		boostModulation(ux+1, uy-1, energy);
		boostModulation(ux+1, uy, energy);
		boostModulation(ux+1, uy+1, energy);
		boostModulation(ux, uy-1, energy);
		boostModulation(ux, uy+1, energy);
	}
}



template<
size_t USIZE,
size_t UNITSX,
size_t UNITSY,
size_t SMAX,
size_t TMAX
>
void Region<USIZE,UNITSX,UNITSY,SMAX,TMAX>::
reform(vector<float> &v) {
	/*for (auto i = 0U; i < kInputSize; ++i) {
		v[i] = inputs_[i];
	}*/
	for (auto i = 0U; i < kUnitCount; ++i) {
		//float max = 0.0f;
		//size_t maxix = 0;
		for (auto j = 0U; j < SMAX; ++j) {
			//v[i * USIZE + j] = units_[i].spatial[j];
			if (units_[i].outputs[j] > 0.01) {
				for (auto k = 0U; k < USIZE; ++k) {
					v[i * USIZE + k] += (units_[i].links[k * SMAX + j].strength) *
											(units_[i].outputs[j]);
					if (v[i * USIZE + k] > 1.0f) v[i * USIZE + k] = 1.0f;
				}
			}
		}
	}
}



template<
size_t USIZE,
size_t UNITSX,
size_t UNITSY,
size_t SMAX,
size_t TMAX
>
void Region<USIZE,UNITSX,UNITSY,SMAX,TMAX>::
activityMap(vector<uint8_t> &v) {

}



template<
size_t USIZE,
size_t UNITSX,
size_t UNITSY,
size_t SMAX,
size_t TMAX
>
void Region<USIZE,UNITSX,UNITSY,SMAX,TMAX>::
clearInput() {
	for (auto i = 0U; i < kInputSize; ++i) {
		inputs_[i] = 0.0f;
	}
}



template<
size_t USIZE,
size_t UNITSX,
size_t UNITSY,
size_t SMAX,
size_t TMAX
>
size_t Region<USIZE,UNITSX,UNITSY,SMAX,TMAX>::
activate(Unit &unit, float *inputs, size_t insize) {
	float energy = 0.0f;
	vector<float> total_depol(SMAX, 0.0f);
	vector<float> newoutputs(SMAX, 0.0f);

	// Calculate total depolarisation
	for (auto i = 0U; i < insize; ++i) {
		energy += inputs[i];
		for (auto j = 0U; j < SMAX; ++j) {
			auto &link = unit.links[i * SMAX + j];
			total_depol[j] += inputs[i] * link.strength;
		}
	}

	float modulation = 0.2f;
	//float threshold = (energy / insize) * modulation;
	float maxdepol = 0.0f;

	// Sum the depols
	for (auto i = 0U; i < SMAX; ++i) {
		total_depol[i] /= unit.counts[i];

		if (total_depol[i] > maxdepol) {
			maxdepol = total_depol[i];
		}
	}

	// Calculate new outputs.
	for (auto i = 0U; i < SMAX; ++i) {
		// Scale depol by total
		if (total_depol[i] < maxdepol) {
			total_depol[i] *= maxdepol - total_depol[i];
		}

		if (total_depol[i] > modulation) {
			newoutputs[i] = total_depol[i];
		} else {
			newoutputs[i] = 0.0f;
		}

		// Update link strengths
		for (auto j = 0U; j < insize; ++j) {
			auto &link = unit.links[j * SMAX + i];
			float newdepol = inputs[i] * link.strength;
			float delta = newdepol - link.depol;
			// If the link became active (or more active)
			if (delta > 0.2f) {
				// If remains active
				if ((unit.outputs[j] > 0.0001f) && (newoutputs[j] > 0.0001f)) {
					// Weaken the link
					unit.counts[i] -= link.strength;
					link.strength -= newdepol * newoutputs[j] * kLearnRate; // * (newdepol - link.depol);
					unit.counts[i] += link.strength;
				}
				// If became active
				if ((unit.outputs[j] < 0.0001f) && (newoutputs[j] > 0.0001f)) {
					// Strengthen the link
					unit.counts[i] -= link.strength;
					link.strength += inputs[i] * (1.0f - link.strength) * newoutputs[j] * kLearnRate; // * (newdepol - link.depol);
					unit.counts[i] += link.strength;
				}
			}

			link.depol = newdepol;
		}
	}

	// Copy outputs
	for (auto i = 0U; i < SMAX; ++i) {
		unit.outputs[i] = newoutputs[i];
	}

	return 0;
}

