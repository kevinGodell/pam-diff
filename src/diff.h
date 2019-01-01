#ifndef DIFF_H_
#define DIFF_H_

inline uint_fast8_t AbsUint(int_fast16_t n);

inline uint_fast8_t GrayDiff(const uint_fast8_t *buf0, const uint_fast8_t *buf1, const uint_fast32_t i);

uint_fast8_t DiffsPercent(const uint_fast32_t pixDiff, const uint_fast32_t pixCount, const uint_fast8_t *buf0, const uint_fast8_t *buf1);

uint_fast8_t DiffsPercent(const uint_fast32_t pixDiff, const uint_fast32_t pixCount, const uint_fast8_t depth, const uint_fast8_t *buf0, const uint_fast8_t *buf1);

uint_fast8_t DiffsPercent(const uint_fast32_t pixDiff, const uint_fast32_t pixCount, const uint_fast8_t *bitset, const uint_fast32_t bitsetCount, const uint_fast8_t *buf0, const uint_fast8_t *buf1);

uint_fast8_t DiffsPercent(const uint_fast32_t pixDiff, const uint_fast32_t pixCount, const uint_fast8_t depth, const uint_fast8_t *bitset, const uint_fast32_t bitsetCount, const uint_fast8_t *buf0, const uint_fast8_t *buf1);

#endif
