#ifndef ___TIMES___
#define ___TIMES___

#include <iostream>
#include <time.h>
#include <pthread.h>
#include <time_measurements.h>

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
	static TakeTimes phase1_step1;
	static TakeTimes phase1_step2;
	static TakeTimes phase1_step3;

	static TakeTimes phase2_step1;
	static TakeTimes phase2_step2;
	static TakeTimes phase2_step3;

	static pthread_mutex_t mutex;

	static void lock() { pthread_mutex_lock(&mutex); }
	static void unlock() { pthread_mutex_unlock(&mutex); }

	static void print_start(int p, int s) {}
	static void print_end(int p, int s) {}
//	static void print_start(int p, int s) { std::cout << "Starting phase" << p << "_step" << s << std::endl; }
//	static void print_end(int p, int s) { std::cout << "Ending phase" << p << "_step" << s << std::endl; }
public:
	static void start_phase1_step1() { lock(); print_start(1,1); phase1_step1.start(); unlock(); }
	static void start_phase1_step2() { lock(); print_start(1,2); phase1_step2.start(); unlock(); }
	static void start_phase1_step3() { lock(); print_start(1,3); phase1_step3.start(); unlock(); }

	static void start_phase2_step1() { lock(); print_start(2,1); phase2_step1.start(); unlock(); }
	static void start_phase2_step2() { lock(); print_start(2,2); phase2_step2.start(); unlock(); }
	static void start_phase2_step3() { lock(); print_start(2,3); phase2_step3.start(); unlock(); }

	static void end_phase1_step1() { lock(); print_end(1,1); phase1_step1.end(); unlock(); }
	static void end_phase1_step2() { lock(); print_end(1,2); phase1_step2.end(); unlock(); }
	static void end_phase1_step3() { lock(); print_end(1,3); phase1_step3.end(); unlock(); }

	static void end_phase2_step1() { lock(); print_end(2,1); phase2_step1.end(); unlock(); }
	static void end_phase2_step2() { lock(); print_end(2,2); phase2_step2.end(); unlock(); }
	static void end_phase2_step3() { lock(); print_end(2,3); phase2_step3.end(); unlock(); }

	static void print(std::ostream &out) {
		out << "Phase 1 step 1  " << phase1_step1 << std::endl;
		out << "Phase 1 step 2  " << phase1_step2 << std::endl;
		out << "Phase 1 step 3  " << phase1_step3 << std::endl;

		out << "Phase 2 step 1  " << phase2_step1 << std::endl;
		out << "Phase 2 step 2  " << phase2_step2 << std::endl;
		out << "Phase 2 step 3  " << phase2_step3 << std::endl;
	}
};

#endif
