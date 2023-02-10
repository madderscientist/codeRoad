#include "Cryptology.h"
using namespace Cryptology_;

void Round(UI& x) {
	x = ((x & 0xFF000000u) >> 24) | ((x & 0x00FF0000u) >> 8) |
		((x & 0x0000FF00u) << 8)  | ((x & 0x000000FFu) << 24);
}
UI S(const UI& x, const int& m) {
	return (x << m) | (x >> (32 - m));
}
UI P0(const UI& x) {
	return x ^ S(x, 9) ^ S(x, 17);
}
UI P1(const UI& x) {
	return x ^ S(x, 15) ^ S(x, 23);
}
UI T(const int& m) {
	return S((m < 16 ? 0x79CC4519u : 0x7A879D8Au), m);
}

static UI W1[68] = {};
static UI V[8] = {};

void blockHash() {
	for (int i = 16; i < 68; i++)
		W1[i] = P1(W1[i - 16] ^ W1[i - 9] ^ S(W1[i - 3], 15))
		^ S(W1[i - 13], 7) ^ W1[i - 6];

	UI W2[64] = {};
	for (int i = 0; i < 64; i++)
		W2[i] = W1[i] ^ W1[i + 4];

	UI	a = V[0], b = V[1], c = V[2], d = V[3],
		e = V[4], f = V[5], g = V[6], h = V[7],
		ss1, ss2, tt1, tt2;
	for (int i = 0; i < 64; i++) {
		ss1 = S(S(a, 12) + e + T(i), 7);
		ss2 = ss1 ^ S(a, 12);
		tt1 = (i < 16 ? a ^ b ^ c : (a & b) | (a & c) | (b & c)) + d + ss2 + W2[i];
		tt2 = (i < 16 ? e ^ f ^ g : (e & f) | (~e & g)) + h + ss1 + W1[i];
		d = c, c = S(b, 9), b = a, a = tt1;
		h = g, g = S(f, 19), f = e, e = P0(tt2);
	}

	V[0] ^= a, V[1] ^= b, V[2] ^= c, V[3] ^= d, V[4] ^= e, V[5] ^= f, V[6] ^= g, V[7] ^= h;
	return;
}

inline void blockRound() {
	for (int i = 0; i < 16; i++) Round(W1[i]);
}

static const UI V0[8] = {
	0x7380166F, 0x4914B2B9, 0x172442D7, 0xDA8A0600,
	0xA96F30BC, 0x163138AA, 0xE38DEE4D, 0xB0FB0E4E };

void hash_(istream& fin, ostream& fout){
	ULL ttsize = getLen(fin);
	int size = ttsize % 64;
	ttsize *= 8;

	for (int j = 0; j < 8; j++) V[j] = V0[j];

	while (memset(W1, 0, 64),fin.read((char*)&W1, 64).good()) {
		blockRound();
		blockHash();
	}

	blockRound();
	if (size >= 56) {
		W1[size / 4] |= 0x80000000u >> ((size % 4) * 8);
		blockHash();
		for (int j = 0; j < 14; j++) W1[j] = 0;
		W1[15] = ttsize;
		W1[14] = ttsize >> 32;
		blockHash();
	}
	else {
		W1[size / 4] |= 0x80000000u >> ((size % 4) * 8);
		W1[15] = ttsize;
		W1[14] = ttsize >> 32;
		blockHash();
	}

	for (int i = 0; i < 8; i++) Round(V[i]);
	fout.write((char*)&V, sizeof V);

}


char* SM3::HashFile(const char* const iFile) {
	ifstream fin;
	fin.open(iFile, std::ios_base::in | std::ios_base::binary);
	if (!fin.is_open()) return nullptr;

	std::ostringstream osstr;
	hash_(fin, osstr);
	return sstrToCstr(osstr);
}
char* SM3::HashStr(const char* const str) {
	if (str == nullptr) return nullptr;
	std::istringstream isstr(str);
	std::ostringstream osstr;
	hash_(isstr, osstr);
	return sstrToCstr(osstr);
}