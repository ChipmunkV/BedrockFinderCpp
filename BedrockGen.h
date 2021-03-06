
#include <cstdint>
#include "PrecomputedRandAdvance.h"

extern std::array<int64_t, 256 * 4> A_OW_112;
extern std::array<int64_t, 256 * 4> B_OW_112;
extern std::array<int64_t, 289 * 4> A_OW_113_114;
extern std::array<int64_t, 289 * 4> B_OW_113_114;
extern std::array<int64_t, 289 * 8> A_N_113_114;
extern std::array<int64_t, 289 * 8> B_N_113_114;

inline int64_t rawSeedFromChunk(int x, int z) //chunk coordinates
{
	return (((int64_t)x * (int64_t)341873128712 + (int64_t)z * (int64_t)132897987541) ^ (int64_t)0x5DEECE66D) & ((((int64_t)1 << 48) - 1));
}

inline int rand5(int64_t raw_seed, int64_t a, int64_t b)
{
	return (int)((((raw_seed * a + b) & (((int64_t)1 << 48) - 1)) >> 17) % ((int64_t)5));
}

inline int precompChunkIndCalcNormal(int x, int y, int z, bool nether)// y 123->126 must be 4->7
{
	return ((z * 16 + x) * (nether ? 8 : 4) + ((nether ? 7 : 3) - y));
}

inline void precompChunkIndCalc113(int* inds, int& count, int x, int y, int z, bool nether)// y 123->126 must be 4->7
{
	inds[count] = ((z * 17 + x) * (nether ? 8 : 4) + ((nether ? 7 : 3) - y));
	count++;
	if (x == 0)
	{
		inds[count] = ((z * 17 + 16) * (nether ? 8 : 4) + ((nether ? 7 : 3) - y));
		count++;
	}
	if (z == 0)
	{
		inds[count] = ((16 * 17 + x) * (nether ? 8 : 4) + ((nether ? 7 : 3) - y));
		count++;
	}
	if (x == 0 && z == 0)
	{
		inds[count] = ((16 * 17 + 16) * (nether ? 8 : 4) + ((nether ? 7 : 3) - y));
		count++;
	}
}

bool bedrockOverworld112(int x, int y, int z)
{
	if (y == 0) return true;
	if (y < 0 || y > 4) return false;

	int precomp_ind = precompChunkIndCalcNormal(x & 15, y - 1, z & 15, false);

	return rand5(rawSeedFromChunk(x >> 4, z >> 4), A_OW_112[precomp_ind], B_OW_112[precomp_ind]) >= y;
}

bool bedrockOverworld113(int x, int y, int z)
{
	if (y == 0) return true;
	if (y < 0 || y > 4) return false;

	int inds[4];
	int count = 0;
	precompChunkIndCalc113(inds, count, x & 15, y - 1, z & 15, false);

	bool found = false;

	for (int i = 0; i < count; i++)
		found = found || rand5(rawSeedFromChunk(x >> 4, z >> 4), A_OW_113_114[inds[i]], B_OW_113_114[inds[i]]) >= y;
	return found;
}

bool bedrockOverworld114(int x, int y, int z)
{
	if (y == 0) return true;
	if (y < 0 || y > 4) return false;

	int precomp_ind = precompChunkIndCalcNormal(x & 15, y - 1, z & 15, false);

	return rand5(rawSeedFromChunk(x >> 4, z >> 4), A_OW_113_114[precomp_ind], B_OW_113_114[precomp_ind]) >= y;
}

bool bedrockNether113(int x, int y, int z)
{
	if (y == 0 || y == 127) return true;
	if (y < 0 || (y > 4 && y < 123) || y > 127) return false;

	int inds[4];
	int count = 0;
	precompChunkIndCalc113(inds, count, x & 15, y > 4 ? y - 119 : y - 1, z & 15, true);

	bool found = false;

	for (int i = 0; i < count; i++)
		found = found || (y > 4 ? (4 - rand5(rawSeedFromChunk(x >> 4, z >> 4), A_N_113_114[inds[i]], B_N_113_114[inds[i]]) <= y - 123) :
		(rand5(rawSeedFromChunk(x >> 4, z >> 4), A_N_113_114[inds[i]], B_N_113_114[inds[i]]) >= y));

	return found;
}

bool bedrockNether114(int x, int y, int z)
{
	if (y == 0 || y == 127) return true;
	if (y < 0 || (y > 4 && y < 123) || y > 127) return false;

	int precomp_ind = precompChunkIndCalcNormal(x & 15, y > 4 ? y - 119 : y - 1, z & 15, true);

	return y > 4 ? 4 - rand5(rawSeedFromChunk(x >> 4, z >> 4), A_N_113_114[precomp_ind], B_N_113_114[precomp_ind]) <= y - 123 :
		rand5(rawSeedFromChunk(x >> 4, z >> 4), A_N_113_114[precomp_ind], B_N_113_114[precomp_ind]) >= y;
}