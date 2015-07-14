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
		units_[ix].slinks[i] = 128;
	}

	for (auto i = 0U; i < kTemporalLinks; ++i) {
		units_[ix].tlinks[i] = 128;
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
	for (auto i = 0U; i < kUnitCount; ++i) {
		decaySpatial(i);
		activateSpatial(i);
		//adjustSpatial(i, s);
	}

	//clearInput();

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

		if (max < 0.0001) {
			for (auto j = 0U; j < USIZE; ++j) {
				v[i * USIZE + j] = 0.0f;
			}
		} else {
			for (auto j = 0U; j < USIZE; ++j) {
				v[i * USIZE + j] = ((float)units_[i].slinks[j * SMAX + maxix] / 255.0f);
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
adjustSpatial(size_t ix, size_t s) {
	const auto ibase = ix * USIZE;

	for (auto j = 0U; j < SMAX; ++j) {
		if (j == s) {
			for (auto i = 0U; i < USIZE; ++i) {
				if (inputs_[ibase + i] > units_[ix].modulation * 0.1) {
					if (units_[ix].slinks[i * SMAX + j] < 255) {
						++units_[ix].slinks[i * SMAX + j];
					}
				}/* else {
					if (units_[ix].slinks[i * SMAX + j] > 1) {
						units_[ix].slinks[i * SMAX + j] -= 2;
					}
				}*/
			}
		} else {
			for (auto i = 0U; i < USIZE; ++i) {
				if (inputs_[ibase + i] > units_[ix].modulation * 0.5) {
					if (units_[ix].slinks[i * SMAX + j] > 0) {
						--units_[ix].slinks[i * SMAX + j];
					}
				}
			}
		}
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
	const auto ibase = ix * USIZE;
	const auto contrib = (1.0f / (float)USIZE) * 0.8;
	float depol[SMAX];

	for (auto i = 0U; i < SMAX; ++i) {
		depol[i] = 0.0f;
	}

	for (auto i = 0U; i < USIZE; ++i) {
		for (auto j = 0U; j < SMAX; ++j) {
			depol[j] += inputs_[ibase + i] * contrib *
						((float)units_[ix].slinks[i * SMAX + j] / 255.0f);
		}
	}

	float max = 0.0f;
	size_t maxix = 0;
	for (auto i = 0U; i < SMAX; ++i) {
		if (depol[i] >= max) {
			maxix = i;
			max = depol[i];
		}
	}

	// Must at least reach a threshold value.
	if (max > units_[ix].modulation * 0.1) {
		units_[ix].spatial[maxix] = 1.0f - units_[ix].modulation;

		// Activate any others that are a close enough match
		for (auto i = 0U; i < SMAX; ++i) {
			if (i == maxix) continue;
			if (depol[i] >= (max - contrib)) {
				units_[ix].spatial[i] = 1.0f - units_[ix].modulation;
			}
		}

		adjustSpatial(ix, maxix);
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
