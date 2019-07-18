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
	static clock_t phase2_step2a;
	static clock_t phase2_step2b;
	static clock_t phase2_step3;

	static void print_start(int p, int s) {}
	static void print_end(int p, int s) {}
//	static void print_start(int p, int s) { std::cout << "Starting phase" << p << "_step" << s << std::endl; }
//	static void print_end(int p, int s) { std::cout << "Ending phase" << p << "_step" << s << std::endl; }
public:
	static void reset() { phase1_step1 = phase1_step2 = phase1_step3 = phase2_step1 = phase2_step2a = phase2_step2b = phase2_step3 = 0; }
	static void start_phase1_step1() { print_start(1,1); phase1_step1 -= clock(); }
	static void start_phase1_step2() { print_start(1,2); phase1_step2 -= clock(); }
	static void start_phase1_step3() { print_start(1,3); phase1_step3 -= clock(); }

	static void start_phase2_step1() { print_start(2,1); phase2_step1 -= clock(); }
	static void start_phase2_step2a() { print_start(2,2); phase2_step2a -= clock(); }
	static void start_phase2_step2b() { print_start(2,2); phase2_step2b -= clock(); }
	static void start_phase2_step3() { print_start(2,3); phase2_step3 -= clock(); }

	static void end_phase1_step1() { print_end(1,1); phase1_step1 += clock(); }
	static void end_phase1_step2() { print_end(1,2); phase1_step2 += clock(); }
	static void end_phase1_step3() { print_end(1,3); phase1_step3 += clock(); }

	static void end_phase2_step1() { print_end(2,1); phase2_step1 += clock(); }
	static void end_phase2_step2a() { print_end(2,2); phase2_step2a += clock(); }
	static void end_phase2_step2b() { print_end(2,2); phase2_step2b += clock(); }
	static void end_phase2_step3() { print_end(2,3); phase2_step3 += clock(); }

	static void print(std::ostream &out) {
		out << "Phase 1 step 1 took " << phase1_step1 << " microsec" << std::endl;
		out << "Phase 1 step 2 took " << phase1_step2 << " microsec" << std::endl;
		out << "Phase 1 step 3 took " << phase1_step3 << " microsec" << std::endl;

		out << "Phase 2 step 1 took " << phase2_step1 << " microsec" << std::endl;
		out << "Phase 2 step 2 took " << (phase2_step2a + phase2_step2b) << " microsec" << std::endl;
		out << "Phase 2 step 3 took " << phase2_step3 << " microsec" << std::endl;


		out << "Phase 1 step 1 took " << ((float)phase1_step1 / 1000000) << " sec" << std::endl;
		out << "Phase 1 step 2 took " << ((float)phase1_step2 / 1000000) << " sec" << std::endl;
		out << "Phase 1 step 3 took " << ((float)phase1_step3 / 1000000) << " sec" << std::endl;

		out << "Phase 2 step 1 took " << ((float)phase2_step1 / 1000000) << " sec" << std::endl;
		out << "Phase 2 step 2 took " << ((float)(phase2_step2a + phase2_step2b) / 1000000) << " sec" << std::endl;
		out << "Phase 2 step 3 took " << ((float)phase2_step3 / 1000000) << " sec" << std::endl;
	}




	static float time_phase1_step1() { return ((float)phase1_step1 / 1000000); }
	static float time_phase1_step2() { return ((float)phase1_step2 / 1000000); }
	static float time_phase1_step3() { return ((float)phase1_step3 / 1000000); }

	static float time_phase2_step1() { return ((float)phase2_step1 / 1000000); }
	static float time_phase2_step2a() { return ((float)phase2_step2a / 1000000); }
	static float time_phase2_step2b() { return ((float)phase2_step2b / 1000000); }
	static float time_phase2_step3() { return ((float)phase2_step3 / 1000000); }

	static float time_phase2_step2() { return time_phase2_step2a() + time_phase2_step2b(); }

	static float time_server1() { return time_phase1_step3() + time_phase2_step1() + time_phase2_step3(); }
	static float time_server12() { return time_server1() + time_phase2_step2(); }

};

#endif
