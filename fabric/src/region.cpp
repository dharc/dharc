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
	for (auto i = 0U; i < kSpatialLinks; ++i) {
		units_[ix].slinks[i] = 1;
	}

	for (auto i = 0U; i < kTemporalLinks; ++i) {
		units_[ix].tlinks[i] = 1;
	}

	for (auto i = 0U; i < SMAX; ++i) {
		units_[ix].scontrib[i] = 1.0f / (1.0f * (float)USIZE);
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

	//clearInput();

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
		decaySpatial(i);
		activateSpatial(i);
		//adjustSpatial(i, s);
	}

	clearInput();

	for (auto i = 0U; i < kUnitCount; ++i) {
		decayTemporal(i);
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
		float max = 0.0f;
		size_t maxix = 0;
		for (auto j = 0U; j < SMAX; ++j) {
			if (units_[i].spatial[j] >= max) {
				maxix = j;
				max = units_[i].spatial[j];
			}
		}

		/*if (max < 0.0001) {
			for (auto j = 0U; j < USIZE; ++j) {
				v[i * USIZE + j] = 0.0f;
			}
		} else {*/
			for (auto j = 0U; j < USIZE; ++j) {
				v[i * USIZE + j] = ((float)units_[i].slinks[j * SMAX + maxix] / 255.0f) *
										(max * (1.0f / units_[i].modulation));
			}
		//}
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

	//for (auto j = 0U; j < SMAX; ++j) {
		float contrib = 0.0f;

		//if (j == s) {
			for (auto i = 0U; i < USIZE; ++i) {
				if (inputs_[ibase + i] > depol) {
					if (units_[ix].slinks[i * SMAX + s] < 255) {
						++units_[ix].slinks[i * SMAX + s];
					}
				} else if (inputs_[ibase + i] < (1.0f - depol)) {
					if (units_[ix].slinks[i * SMAX + s] > 1) {
						--units_[ix].slinks[i * SMAX + s];
					}
				}
				contrib += units_[ix].slinks[i * SMAX + s];
			}
		/*} else {
			for (auto i = 0U; i < USIZE; ++i) {
				if (inputs_[ibase + i] > depol) {
					if (units_[ix].slinks[i * SMAX + j] > 1) {
						units_[ix].slinks[i * SMAX + j] -= 2;
					}
				}
				contrib += units_[ix].slinks[i * SMAX + j];
			}
		}*/
		//NOTE: Should never be zero.
		units_[ix].scontrib[s] = 1.0f / contrib;
	//}

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
	const auto ibase = ix * USIZE;
	float depol[SMAX];

	for (auto i = 0U; i < SMAX; ++i) {
		depol[i] = 0.0f;
	}

	for (auto i = 0U; i < USIZE; ++i) {
		for (auto j = 0U; j < SMAX; ++j) {
			depol[j] += inputs_[ibase + i] *
						((float)units_[ix].slinks[i * SMAX + j]);
		}
	}

	float max = 0.0f;
	size_t maxix = 0;
	for (auto i = 0U; i < SMAX; ++i) {
		depol[i] *= units_[ix].scontrib[i];
		if (depol[i] >= max) {
			maxix = i;
			max = depol[i];
		}
	}

	// Must at least reach a threshold value.
	if (units_[ix].modulation * kThresholdScale <= max) {
		// Activation strength inverse of threshold.
		units_[ix].spatial[maxix] = 1.0f - units_[ix].modulation;
		//std::cout << "ACTIVATE: " << max << "\n";

		// Activate any others that are a close enough match
		/*for (auto i = 0U; i < SMAX; ++i) {
			if (i == maxix) continue;
			if (depol[i] >= (max - (1.0f / USIZE))) {
				units_[ix].spatial[i] = 1.0f - units_[ix].modulation;
			}
		}*/

		adjustSpatial(ix, maxix, max);
	}

	return maxix;
}



template<
size_t USIZE,
size_t UNITSX,
size_t UNITSY,
size_t SMAX,
size_t TMAX
>
void Region<USIZE,UNITSX,UNITSY,SMAX,TMAX>::
decaySpatial(size_t ix) {
	for (auto i = 0U; i < SMAX; ++i) {
		units_[ix].spatial[i] *= kSpatialDecay;
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
decayTemporal(size_t ix) {
	for (auto i = 0U; i < TMAX; ++i) {
		units_[ix].temporal[i] *= kTemporalDecay;
	}
}
