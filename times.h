#ifndef ___TIMES___
#define ___TIMES___

#include <iostream>
#include <time.h>

// Phase I
//    Step 1: key generation
//    Step 2: local computation. Compute compute outer product x_i x_i^T and encrypt it
//    Step 3: dataset merge. Add all outer products
//
// Phase II
//    Step 1: Data masking
//    Step 2: Masked model computation
//    Step 3: Model reconstruction
class Times {
private:
	static clock_t phase1_step1;
	static clock_t phase1_step2;
	static clock_t phase1_step3;

	static clock_t phase2_step1;
	static clock_t phase2_step2;
	static clock_t phase2_step3;

public:
	static void start_phase1_step1() { phase1_step1 -= clock(); }
	static void start_phase1_step2() { phase1_step2 -= clock(); }
	static void start_phase1_step3() { phase1_step3 -= clock(); }

	static void start_phase2_step1() { phase2_step1 -= clock(); }
	static void start_phase2_step2() { phase2_step2 -= clock(); }
	static void start_phase2_step3() { phase2_step3 -= clock(); }

	static void end_phase1_step1() { phase1_step1 += clock(); }
	static void end_phase1_step2() { phase1_step2 += clock(); }
	static void end_phase1_step3() { phase1_step3 += clock(); }

	static void end_phase2_step1() { phase2_step1 += clock(); }
	static void end_phase2_step2() { phase2_step2 += clock(); }
	static void end_phase2_step3() { phase2_step3 += clock(); }

	static void print(std::ostream &out) {
		out << "Phase 1 step 1 took " << phase1_step1 << " microsec" << std::endl;
		out << "Phase 1 step 2 took " << phase1_step2 << " microsec" << std::endl;
		out << "Phase 1 step 3 took " << phase1_step3 << " microsec" << std::endl;

		out << "Phase 2 step 1 took " << phase2_step1 << " microsec" << std::endl;
		out << "Phase 2 step 2 took " << phase2_step2 << " microsec" << std::endl;
		out << "Phase 2 step 3 took " << phase2_step3 << " microsec" << std::endl;
	}
};

#endif
