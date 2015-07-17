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

		for (auto y = 0U; y < USIZE; ++y) {
			const int yx = y % kUnitSqrt;
			const int yy = y / kUnitSqrt;
			const int dx = xx - yx;
			const int dy = xy - yy;
			const float dist = std::sqrt(dx * dx + dy * dy) / (float)kUnitSqrt;
			
			units_[ix].slinks[y * SMAX + x] = (uint8_t)(255.0f - (dist * 255.0f));
		}
	}

	/*for (auto i = 0U; i < kSpatialLinks; ++i) {
		units_[ix].slinks[i] = (std::rand() % 255) + 1;
	}*/

	for (auto i = 0U; i < kTemporalLinks; ++i) {
		units_[ix].tlinks[i] = 0;
	}

	for (auto i = 0U; i < SMAX; ++i) {
		units_[ix].scount[i] = 0;
		units_[ix].spatial[i] = 0.0f;
	}

	for (auto i = 0U; i < TMAX; ++i) {
		units_[ix].temporal[i] = 0.0f;
	}

	units_[ix].modulation = 0.3f;
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
											(units_[i].spatial[j] / (1.0f - units_[i].modulation));
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

	for (auto j = 0U; j < SMAX; ++j) {
		//float contrib = 0.0f;

		// Did this pattern just get activated by the inputs?
		if (j == s) {
			for (auto i = 0U; i < USIZE; ++i) {
				auto &link = units_[ix].slinks[i * SMAX + j];
				if (inputs_[ibase + i] * (1.0f - ((float)link / 255.0f)) > (1.0f - depol)) {
					if (link < 255) {
						++link;
					}
				}/* else {
					if (units_[ix].slinks[i * SMAX + j] > 0) {
						--units_[ix].slinks[i * SMAX + j];
					}
				}*/
				//contrib += link;
			}
		} else {
			// If pattern not already active then don't bother.
			//if (isSpatialOff(ix, j)) continue;

			for (auto i = 0U; i < USIZE; ++i) {
				auto &link = units_[ix].slinks[i * SMAX + j];
				if (inputs_[ibase + i] * (1.0f - ((float)link / 255.0f)) > (1.0f - depol)) {
					if (link > 0) {
						--link;
					}
				}
				//contrib += link;
			}
		}
		//NOTE: Should never be zero.
		//units_[ix].scontrib[j] = 1.0f / contrib;
	}

	/*for (auto i = 0U; i < USIZE; ++i) {
		for (auto j = 0U; j < SMAX; ++j) {
			// If the input provided was significant enough
			if (inputs_[ibase + i] > units_[ix].modulation * 0.5) {
				if (j == s) {
					if (units_[ix].slinks[j * SMAX + i] < 255) {
						++units_[ix].slinks[j * SMAX + i];
					}
				} else {
					if (units_[ix].slinks[j * SMAX + i] > 0) {
						--units_[ix].slinks[j * SMAX + i];
					}
				}
			} else {
				if (units_[ix].slinks[j * SMAX + i] > 0) {
						--units_[ix].slinks[j * SMAX + i];
					}
			}
		}
	}*/
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
		if (depol[i] >= depolmax) {
			depolix = i;
			depolmax = depol[i];
		}
		unit.spatial[i] *= kDecayRate;
		energy += unit.spatial[i];
	}

	energy -= unit.spatial[depolix];

	// Depolarisation must reach a minimum level
	//if (depolmax > (unit.modulation * kThresholdScale)) {
		//const auto maxactive = 1.0f - unit.modulation;
		auto activation = depolmax / (float)USIZE;
		activation = (1.0f - energy) * activation;
		auto delta = activation - unit.spatial[depolix];
		//activation = activation + (0.2f * (activation - energy));

		

		// If off then boost to full level.
		//if (isSpatialOff(ix, depolix)) {
		if (delta > 0.0f) {
			adjustSpatial(ix, depolix, delta);
			unit.spatial[depolix] = activation;
		}
		//} else {
		//	unit.spatial[depolix] *= kActiveDecayRate;
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

