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
	for (auto x = 0U; x < SMAX; ++x) {
		const int xi = (int)(((float)x / (float)SMAX) * (float)USIZE);
		const int xx = xi % kUnitSqrt;
		const int xy = xi / kUnitSqrt;

		units_[ix].scount[x] = 0.0f;

		for (auto y = 0U; y < USIZE; ++y) {
			const int yx = y % kUnitSqrt;
			const int yy = y / kUnitSqrt;
			const int dx = xx - yx;
			const int dy = xy - yy;
			const float dist = std::sqrt(dx * dx + dy * dy) / (float)kUnitSqrt;
			
			units_[ix].slinks[y * SMAX + x] = 1.0f - dist;

			units_[ix].scount[x] += units_[ix].slinks[y * SMAX + x];
		}
	}

	/*for (auto i = 0U; i < kSpatialLinks; ++i) {
		units_[ix].slinks[i] = (std::rand() % 255) + 1;
	}*/

	for (auto i = 0U; i < kTemporalLinks; ++i) {
		units_[ix].tlinks[i] = 0;
	}

	for (auto i = 0U; i < SMAX; ++i) {
		units_[ix].spatial[i] = 0.0f;
	}

	for (auto i = 0U; i < TMAX; ++i) {
		units_[ix].temporal[i] = 0.0f;
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
		activate(units_[i], &inputs_[i * USIZE], USIZE, units_[i].slinks, units_[i].scount, units_[i].spatial, SMAX);
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
			if (units_[i].spatial[j] > 0.01) {
				for (auto k = 0U; k < USIZE; ++k) {
					v[i * USIZE + k] += (units_[i].slinks[k * SMAX + j]) *
											(units_[i].spatial[j]);
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
activate(Unit &unit, float *inputs, size_t insize, float *links, float *counts, float *outputs, size_t outsize) {
	//auto &unit = units_[ix];
	//const auto ibase = ix * USIZE;
	float depol[outsize];
	float energy = 0.0f;
	float factor = 1.0f;

	// Reset depols and outputs.
	for (auto i = 0U; i < outsize; ++i) {
		depol[i] = 0.0f;
		outputs[i] = 0.0f;
	}

	// Sort the inputs
	vector<std::pair<float,size_t>> inputs_sorted;
	for (auto i = 0U; i < insize; ++i) {
		energy += inputs[i];
		inputs_sorted.push_back({inputs[i],i});
	}
	std::sort(inputs_sorted.begin(), inputs_sorted.end(), [](auto a, auto b) {
		return a.first > b.first;
	});

	// Generate depolarisation values until first is fired.
	for (auto i : inputs_sorted) {
		for (auto j = 0U; j < outsize; ++j) {
			auto &link = links[i.second * outsize + j];
			// Use factor for soma suppression
			depol[j] += i.first * link * factor;

			// STDP Learning adjustments
			// Has this pattern already fired?
			counts[j] -= link;
			link -= i.first * link * outputs[j] * kLearnRate;
			counts[j] += link;

			// TRIGGER if 20% activated. (% controlled by modulation)
			if (depol[j] > counts[j] * (energy / insize) * 0.5f) {
				// Suppress existing depolarisations and reduce axon output
				for (auto k = 0U; k < outsize; ++k) {
					// Dendrite suppression
					depol[k] *= 0.8f;
					// Output axon suppression
					outputs[k] *= (1.0f - factor);
				}
				// I can't go again...
				depol[j] = -0.5f;

				// Generate new depolarisation
				outputs[j] = factor * (energy / insize);
				factor *= 0.5f;

				// Boost all links that caused this firing
				for (auto k : inputs_sorted) {
					auto &link2 = links[k.second * outsize + j];
					counts[j] -= link2;
					link2 += k.first * (1.0f - link2) * outputs[j] * kLearnRate;
					counts[j] += link2;
					if (k.second == i.second) break;
				}
			}
		}
	}

	// Generate action potentials
	energy /= insize;
	for (auto i = 0U; i < outsize; ++i) {
		//outputs[i] *= energy;
		/*if (outputs[i] >= 0.5f) {
			outputs[i] = energy + ((outputs[i] - 0.5f) * (1.0f - energy) * 0.2f);
		} else {
			outputs[i] = energy + ((outputs[i] - 0.5f) * (energy) * 0.2f);
		}*/
	}

	/*

	float depolmax = 0.0f;
	float depolmin = 1000.0f;
	size_t depolix = 0;
	float energy = 0.0f;

	// Find min and max depolarisations for leveling
	for (auto i = 0U; i < outsize; ++i) {
		// Level based on number+strength of synapses
		// Each synapse is less significant if there are lots of them...
		depol[i] /= counts[i];

		if (depol[i] >= depolmax) {
			depolmax = depol[i];
		}
		if (depol[i] <= depolmin) {
			depolmin = depol[i];
		}
		//outputs[i] = 0.0f;
		outputs[i] *= kDecayRate;
	}

	

	// Adjust depolarisation levels.
	float depoldiff = depolmax - depolmin;
	if (depoldiff > 0.0001f) {
		depoldiff = 1.0f / depoldiff;
		for (auto i = 0U; i < outsize; ++i) {
			// Level the depolarisation
			depol[i] -= depolmin;
			depol[i] *= depoldiff;
			energy += depol[i];
		}

		energy = 1.0f / energy;

		for (auto i = 0U; i < outsize; ++i) {
			depol[i] *= energy;
			// Depolarisation must reach a minimum level
			if (depol[i] > (energyin * kThresholdScale)) {
				// Normalise activation (PV soma inhib effect)
				auto activation = depol[i] * (depolmax - depolmin);
				auto delta = activation - outputs[i];

				if (delta > 0.0f) {
					outputs[i] = activation;
				}

				for (auto j = 0U; j < insize; ++j) {
					auto &link = links[j * outsize + i];
					if (inputs[j] * (1.0f - ((float)link / 255.0f)) > (1.0f - depol[i])) {
						if (link < 255) {
							++link;
						}
					}
				}
			} else {
				
			}
		}
	}

	adjust(depolix, unit, inputs, insize, links, counts, depol, outsize);
*/
	return 0;
}

