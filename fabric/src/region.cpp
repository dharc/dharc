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
			
			units_[ix].slinks[y * SMAX + x] = (uint8_t)(255.0f - (dist * 255.0f));

			units_[ix].scount[x] += units_[ix].slinks[y * SMAX + x] / 255.0f;
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

	for (auto i = 0U; i < kUnitCount; ++i) {
		//decaySpatial(i);
		activateSpatial(i);
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
		//energy = 1.0f - energy;
		energy = energy - 0.5f;
		//units_[i].modulation = 0.5f - (0.1f * energy);
		energy =  -0.125f * kModChangeRate * energy;

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
			if (units_[i].spatial[j] > 0.01) {
				for (auto k = 0U; k < USIZE; ++k) {
					v[i * USIZE + k] += ((float)units_[i].slinks[k * SMAX + j] / 255.0f) *
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
void Region<USIZE,UNITSX,UNITSY,SMAX,TMAX>::
adjustSpatial(size_t ix, size_t s, float depol) {
	const auto ibase = ix * USIZE;
	auto &unit = units_[ix];

	for (auto j = 0U; j < SMAX; ++j) {
		unit.scount[j] = 0.0f;

		// Did this pattern just get activated by the inputs?
		if (j == s) {
			for (auto i = 0U; i < USIZE; ++i) {
				auto &link = unit.slinks[i * SMAX + j];
				if (inputs_[ibase + i] * (1.0f - ((float)link / 255.0f)) > (1.0f - unit.spatial[j])) {
					if (link < 255) {
						++link;
					}
				}

				// Update weighted link count for pattern
				unit.scount[j] += link / 255.0f;
			}
		// Otherwise decay those strong links of others if they are activated.
		} else {
			for (auto i = 0U; i < USIZE; ++i) {
				auto &link = unit.slinks[i * SMAX + j];
				if (inputs_[ibase + i] * (((float)link / 255.0f)) > (1.0f - unit.spatial[j])) {
					if (link > 0) {
						--link;
					}
				}

				unit.scount[j] += link / 255.0f;
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
size_t Region<USIZE,UNITSX,UNITSY,SMAX,TMAX>::
activateSpatial(size_t ix) {
	auto &unit = units_[ix];
	const auto ibase = ix * USIZE;
	float depol[SMAX];

	for (auto i = 0U; i < SMAX; ++i) {
		depol[i] = 0.0f;
	}

	// Generate depolarisation values from inputs and link strengths.
	for (auto i = 0U; i < USIZE; ++i) {
		for (auto j = 0U; j < SMAX; ++j) {
			auto &link = unit.slinks[i * SMAX + j];
			/*if ((inputs_[ibase + i] > 0.5f) && (link < 255)) {
				if (link == 0) ++unit.scount[j];
				++link;
			}*/
			auto localdepol = inputs_[ibase + i] * ((float)link / 255.0f);
			depol[j] += localdepol;
		}
	}

	float depolmax = 0.0f;
	size_t depolix = 0;
	float energy = 0.0f;

	// Find max depol and max currently active pattern
	// This is role of interneurons, calculating unit energy levels.
	for (auto i = 0U; i < SMAX; ++i) {
		depol[i] /= unit.scount[i];
		if (depol[i] >= depolmax) {
			depolix = i;
			depolmax = depol[i];
		}
		unit.spatial[i] *= kDecayRate;
		energy += unit.spatial[i];
	}

	//energy -= unit.spatial[depolix];

	// Depolarisation must reach a minimum level
	//if (depolmax > (unit.modulation * kThresholdScale)) {
		//const auto maxactive = 1.0f - unit.modulation;
		auto activation = (1.0f - kModFactor) * depolmax;
		activation += kModFactor * depolmax * unit.modulation;
		activation = (1.0f - energy) * activation;
		auto delta = activation - unit.spatial[depolix];
		//activation = unit.spatial[depolix] + (delta * 0.8f);
		//activation = 0.5f * activation + (0.5f * delta);

		// If off then boost to full level.
		//if (isSpatialOff(ix, depolix)) {
			if (delta > 0.0f) {
				unit.spatial[depolix] = activation;
				adjustSpatial(ix, depolix, delta);
			}
		//} else {
		//	unit.spatial[depolix] /= kDecayRate;
		//	unit.spatial[depolix] *= 0.99f;
		//	depolix = SMAX;
		//}

		//for (auto i = 0U; i < USIZE; ++i) {
		//	auto &link = unit.slinks[i * SMAX + depolix];
			//auto localdepol = inputs_[ibase + i] * ((float)link / 255.0f);
		//	if ((link < 255)) ++link;
		//}


			
		//} else {
			// otherwise decay to percentage of activation
			//if (unit.spatial[depolix] < activation * kBaseLimit) {
			//	unit.spatial[depolix] = activation * kBaseLimit;
			//} else {
				// Slowly decay it
				//unit.spatial[depolix] *= kActiveDecayRate;
			//}
		//}
	//} else {
		// Nothing activated to set this beyond end
	//	depolix = SMAX;
	//}

	// Generate depolarisation values from inputs and link strengths.
	/*for (auto i = 0U; i < USIZE; ++i) {
		for (auto j = 0U; j < SMAX; ++j) {
			if (j == depolix) continue;
			auto &link = unit.slinks[i * SMAX + j];
			if ((inputs_[ibase + i] > 0.5f) && (link > 0)) {
				--link;
				if (link == 0) --unit.scount[j];
			}
			//if ((inputs_[ibase + i] > 0.5f) && (link > 0)) ++link;
			//auto localdepol = inputs_[ibase + i] * ((float)link / 255.0f);
			//depol[j] += localdepol;
		}
	}*/

	return 0;
}

