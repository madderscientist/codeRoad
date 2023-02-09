#ifndef TIMER_H_
#define TIMER_H_

#include <ctime>
#include <iostream>
#include <string>
using std::ostream;
using std::endl;

class Timer_class
{
private:
	clock_t totalTime;
	clock_t beginTime;
	int Counts;
	bool On;
	std::string Name;

	void report_num(ostream& os , const double& d) const {
		if (d >=1) {
			os.width(5);
			os << d << "s\n";
		}
		else {
			os.width(4);
			os << (int)(d * CLOCKS_PER_SEC);
			os << "ms\n";
		}
	}

public:
	Timer_class(const std::string& name = "Timer") {
		Counts = 0;
		totalTime = 0;
		On = false;
		Name = name;
	}
	operator double() const{
		return total();
	}

	bool start() {
		if (On) return false;
		On = true;
		beginTime = clock();
		return true;
	}
	bool stop() {
		if (!On) return false;
		Counts++, On = false;
		totalTime += clock() - beginTime;
		return true;
	}
	void clear() {
		*this = Timer_class(Name);
	}
	void reName(const std::string& name) {
		Name = name;
	}
	void report(ostream& os = std::cout) {
		os << *this;
	}

	bool is_on() const {
		return On;
	}
	double total() const {
		if (On) return 1.0 * (totalTime + clock() - beginTime) / CLOCKS_PER_SEC;
		else return 1.0 * totalTime / CLOCKS_PER_SEC;
	}
	double average() const {
		if (On || Counts == 0) return NAN;
		else return 1.0 * totalTime / Counts / CLOCKS_PER_SEC;
	}

	friend ostream& operator<<(ostream& os, const Timer_class& tc);
};

ostream& operator<<(ostream& os, const Timer_class& tc) {
	os.fill(' ');
	os.precision(4);
	os.setf(std::ios_base::showpoint);
	os << std::dec;
	if (tc.Name.size() >= 17) os << "--" << tc.Name << "--" << endl;
	else {
		int num = (21 - tc.Name.size()) / 2;
		os << std::string(num, '-') << tc.Name << std::string(21 - num - tc.Name.size(), '-') << endl;
	}
	if (tc.On) {
		os << "Timing..." << endl;
		os << "Total  time  : ";
		tc.report_num(os, tc.total());
	}
	else if (tc.Counts) {
		os << "Total  time  : ";
		tc.report_num(os, tc.total());
		if (tc.Counts > 1) {
			os << "Average time : ";
			tc.report_num(os, tc.average());
			os << "Repeat times : ";
			os.width(6);
			os << tc.Counts << endl;
		}
	}
	else {
		os << "The timer hasn't been been started." << endl;
	}
	os << "---------------------" << endl;
	return os;
};

Timer_class timer;

#endif // !TIMER_H_
