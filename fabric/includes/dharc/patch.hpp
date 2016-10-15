/*
 * Copyright 2015 Nicolas Pope
 */

#ifndef DHARC_FABRIC_PATCH_HPP_
#define DHARC_FABRIC_PATCH_HPP_

#define IXF(INDEX, SIZE) ((size_t)((float)(INDEX) * (SIZE)))

#include <array>
#include "dharc/element.hpp"
#include <pthread.h>
#include <omp.h>

namespace dharc {
struct ElementState {
	float depol;
	dharc::ElementCore *ele;
	void (*learn)(dharc::ElementCore *ele);
};

struct ElementGenerator {
	size_t sdepth;			// Start depth
	size_t edepth;			// End depth
	float density_mean;		// Mean density in centre of patch
	float density_std;		// Standard deviation of density
	size_t ifield;			// Input field size of these elements
	float idecay;			// Input field decay rate
	size_t ostart;			// Width of initial output field
	float oexpand;			// Rate of output field expansion
	size_t oend;			// min position of output field
}

class PatchBase {
	public:
	virtual ~PatchBase();

	virtual void process();
};

template<size_t LINKS>
class Patch : public PatchBase {
	public:
	Patch();
	~Patch() {};

	void setSize(size_t x, size_t y, size_t maxepi);
	void makeElements(int groupid, ElementGenerator gen);

	void process();
	void getPrimaryLinks(vector<Link*> &links);

	private:

	/*static constexpr float integral(float startval, float decayrate, size_t count) {
		return (count == 0) ? 0 :
			startval + integral(startval * decayrate, decayrate, count - 1);
	}*/

	/*template <size_t I, size_t M>
	struct AnalysisPatch : public AnalysisPatch<I - 1, M> {
		typedef AnalysisPatch<I - 1, M> previous;
		static constexpr size_t kMiddle = M / 2 + ((M % 2) ? 1 : 0);
		static constexpr size_t kDistance = std::abs((int)I - (int)kMiddle);

		static constexpr float kDensity = (kDistance) ? std::pow(kDensityDecay, kDistance) * kDensityMax : kDensityMax;
		static constexpr size_t kFieldSize = (I - 1) * ((MAX_FIELD - MIN_FIELD) / M) + previous::kFieldSize;
		static constexpr size_t kInputs = integral(kInputStart, kInputDecay, kFieldSize);
		static constexpr size_t kSizeX = (size_t)((float)X * kDensity);
		static constexpr size_t kSizeY = (size_t)((float)Y * kDensity);
		static constexpr size_t kSizeZ = EPI / M;
		static constexpr size_t kSize = kSizeX * kSizeY * kSizeZ;
		static constexpr float kToMod = (float)kModSize * kSizeZ / (float)kSize;
		static constexpr float kToBlob = (float)kBlobSize / (float)kSize;
		static constexpr float kToGlobal = (float)kGlobalSize / (float)kSize;

		static_assert(kDensity > 0.0f, "Cannot have 0 density");
		static_assert(kSize != 0, "Analysis patch size must not be 0");

		inline static size_t x(size_t ix) { return ix/(kSizeY*kSizeZ); }
		inline static size_t y(size_t ix) { return (ix%(kSizeY*kSizeZ))/kSizeZ; }
		inline static size_t z(size_t ix) { return (ix%(kSizeY*kSizeZ))%kSizeZ; }
		inline static size_t index(size_t x, size_t y, size_t z) {
			return x*kSizeY*kSizeZ + y*kSizeZ + z;
		}

		std::array<Element<kInputs>, kSize> elements;

		inline void integrate(size_t ix, vector<vector<dharc::ElementState>> &blobs) {
			const float v = elements[ix].integrate(); //modulation_[IXF(ix, kToMod)]);
			if (v >= dharc::ElementCore::kThreshold) {
				blobs[IXF(ix, kToBlob)].push_back(
					dharc::ElementState{v, &elements[ix], dharc::Element<kInputs>::learn});
			} else {
				elements[ix].setFrequency(0);
			}
		}
	};

	template<size_t M>
	struct AnalysisPatch<0, M> {
		static constexpr size_t kFieldSize = MIN_FIELD;
	};

	AnalysisPatch<7,7> analysis_;*/

	vector<Element<LINKS>> elements_;
	vector<ElementCore> inputs_;
};
};



template<size_t X, size_t Y, size_t EPI>
dharc::Patch<X,Y,EPI>::Patch() {
	// Initialise links, perform a field search for every single element.
	// For each element of each field size, randomly generate links with the neighbours.
	// Link strengths decay with distance...
}



template<size_t X, size_t Y, size_t EPI, size_t MIN_FIELD, size_t MAX_FIELD, size_t ISTART>
void dharc::Patch<X,Y,EPI,MIN_FIELD,MAX_FIELD,ISTART>::process() {
	vector<vector<ElementState>> blobs(kBlobSize);
	vector<vector<ElementState>*> allLocal(omp_get_max_threads());

	#pragma omp parallel
	{
		int np = omp_get_num_threads();
		vector<vector<ElementState>> localBlobs(kBlobSize);
		allLocal[omp_get_thread_num()] = localBlobs.data();

		size_t n = AnalysisPatch<1,7>::kSize;
		#pragma omp for nowait
		for (auto i = 0U; i < n; ++i) {
			auto &patch = static_cast<AnalysisPatch<1,7>&>(analysis_);
			patch.integrate(i, localBlobs);
		}

		n = AnalysisPatch<2,7>::kSize;
		#pragma omp for nowait
		for (auto i = 0U; i < n; ++i) {
			auto &patch = static_cast<AnalysisPatch<2,7>&>(analysis_);
			patch.integrate(i, localBlobs);
		}

		n = AnalysisPatch<3,7>::kSize;
		#pragma omp for nowait
		for (auto i = 0U; i < n; ++i) {
			auto &patch = static_cast<AnalysisPatch<3,7>&>(analysis_);
			patch.integrate(i, localBlobs);
		}

		n = AnalysisPatch<4,7>::kSize;
		#pragma omp for nowait
		for (auto i = 0U; i < n; ++i) {
			auto &patch = static_cast<AnalysisPatch<4,7>&>(analysis_);
			patch.integrate(i, localBlobs);
		}

		n = AnalysisPatch<5,7>::kSize;
		#pragma omp for nowait
		for (auto i = 0U; i < n; ++i) {
			auto &patch = static_cast<AnalysisPatch<5,7>&>(analysis_);
			patch.integrate(i, localBlobs);
		}

		n = AnalysisPatch<6,7>::kSize;
		#pragma omp for nowait
		for (auto i = 0U; i < n; ++i) {
			auto &patch = static_cast<AnalysisPatch<6,7>&>(analysis_);
			patch.integrate(i, localBlobs);
		}

		n = AnalysisPatch<7,7>::kSize;
		#pragma omp for
		for (auto i = 0U; i < n; ++i) {
			auto &patch = static_cast<AnalysisPatch<7,7>&>(analysis_);
			patch.integrate(i, localBlobs);
		}

		// Now perform task of interneurons
		//   Generate blobs and within each, sort thresholds and adjust them
		//   About 20% of total size, with blob covering all space between
		#pragma omp for
		for (auto b = 0U; b < kBlobSize; ++b) {
			for (auto t = 0; t < np; ++t) {
				auto &blob = allLocal[t][b];
				blobs[b].insert(blobs[b].end(), blob.begin(), blob.end());
			}
			std::sort(blobs[b].begin(), blobs[b].end(), [](auto a, auto b) {
				return a.depol > b.depol;
			});

			float threshold = dharc::ElementCore::kThreshold;
			float factor = 1.0f;

			for (auto e : blobs[b]) {
				// If it matched enough then
				if (e.depol * factor >= threshold) {
					float newoutput = (e.depol - threshold) * (1.0f / (1.0f - threshold)) * factor;
					factor *= (1.0f - newoutput);
					e.ele->setFrequency((uint8_t)(newoutput * 255.0f));
					e.learn(e.ele);
				} else {
					e.ele->setFrequency(0);
				}
			}
		}

		n = AnalysisPatch<1,7>::kSize;
		#pragma omp for nowait
		for (auto i = 0U; i < n; ++i) {
			auto &patch = static_cast<AnalysisPatch<1,7>&>(analysis_);
			patch.elements[i].forward();
		}

		n = AnalysisPatch<2,7>::kSize;
		#pragma omp for nowait
		for (auto i = 0U; i < n; ++i) {
			auto &patch = static_cast<AnalysisPatch<2,7>&>(analysis_);
			patch.elements[i].forward();
		}

		n = AnalysisPatch<3,7>::kSize;
		#pragma omp for nowait
		for (auto i = 0U; i < n; ++i) {
			auto &patch = static_cast<AnalysisPatch<3,7>&>(analysis_);
			patch.elements[i].forward();
		}

		n = AnalysisPatch<4,7>::kSize;
		#pragma omp for nowait
		for (auto i = 0U; i < n; ++i) {
			auto &patch = static_cast<AnalysisPatch<4,7>&>(analysis_);
			patch.elements[i].forward();
		}

		n = AnalysisPatch<5,7>::kSize;
		#pragma omp for nowait
		for (auto i = 0U; i < n; ++i) {
			auto &patch = static_cast<AnalysisPatch<5,7>&>(analysis_);
			patch.elements[i].forward();
		}

		n = AnalysisPatch<6,7>::kSize;
		#pragma omp for nowait
		for (auto i = 0U; i < n; ++i) {
			auto &patch = static_cast<AnalysisPatch<6,7>&>(analysis_);
			patch.elements[i].forward();
		}

		n = AnalysisPatch<7,7>::kSize;
		#pragma omp for
		for (auto i = 0U; i < n; ++i) {
			auto &patch = static_cast<AnalysisPatch<7,7>&>(analysis_);
			patch.elements[i].forward();
		}
	}
}

#endif  // DHARC_FABRIC_PATCH_HPP_

