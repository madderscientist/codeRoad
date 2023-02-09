//Version:1.0 普通与或树和α-β剪枝

#include "Board.h"
#include <fstream>
#include "Timer.h"
#include <thread>


extern std::ofstream fout;
extern bool if_log;
using std::endl;

static int sum[30];
static char mode = -1;

void dfs(const BD& prs, const int& depth, const float lst, float* const ret) {
	prs.Refresh();

	sum[prs.ns[0]]++;

	//if (prs.ns[0] > 15) std::cerr << (int)prs.ns[0] << " ";
	if ((depth <= 0 && prs.tts * mode > 0) || prs.ifEnd()) {
		*ret = prs.h0;
		return;
	}

	if (prs.tts > 0) {	//COM,MAX
		float h = -INFINITY;
		for (int i = 1; i <= prs.ns[0]; i++) {
			dfs(BD(prs, i), depth - 1, h, ret);
			if (h < *ret) h = *ret;
			if (h > lst) break;
		}
		*ret = h;
		return;
	}
	else {				//PLY,MIN
		float h = INFINITY;
		for (int i = 1; i <= prs.ns[0]; i++) {
			dfs(BD(prs, i), depth - 1, h, ret);
			if (h > *ret) h = *ret;
			if (h < lst) break;
		}
		*ret = h;
		return;
	}
}

extern int difficulty;


const char BD::autoPut() const {

	if (if_log) {
		timer.clear();
		timer.start();
		for (int i = 0; i < 30; i++) sum[i] = 0;
	}

	Refresh();

	float h;
	char memi = 1;
	if (tts > 0) h = -INFINITY, mode = -1;
	else h = INFINITY, mode = 1;

	int N = ns[0], i;
	float s[35];
	BD* nw[35];
	std::thread* thr[35] = {};

	for (i = 1; i <= N; i++) {
		nw[i] = new BD(*this, i);
		thr[i] = new std::thread(dfs,
			std::ref(*nw[i]), difficulty, std::ref(h), s + i);
	}
	for (i = 1; i <= N; i++) thr[i]->join();	

	for (i = 1; i <= N; i++) {
		if ((h - s[i]) * mode > 0) {
			h = s[i];
			memi = i;
		}
		delete thr[i];
		delete nw[i];
	}

	if (if_log) {
		for (i = 1; i <= N; i++) fout << i << "/" << N << "	s=" << s[i] << std::endl;
		
		fout << "i="; for (int i = 0; i <= 13; i++) fout << '\t' << i; fout << endl;
		fout << "sum[i]="; for (int i = 0; i <= 13; i++) fout << '\t' << sum[i]; fout << endl;
		fout << "i="; for (int i = 14; i <= 27; i++) fout << '\t' << i; fout << endl;
		fout << "sun[i]="; for (int i = 14; i <= 27; i++) fout << '\t' << sum[i]; fout << endl;
		
		timer.stop();
		timer.report(fout);
	}

	return memi;
}
