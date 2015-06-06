#ifndef HEATMAP_H
#define HEATMAP_H

#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "group.h"

#define INITIAL_PENALTY (-35)

#define BASEGROUP_BONUS 34
#define WORDSTART_BONUS 85

#define GROUP_NUMBER_PENALTY_MULTIPLIER 2
#define GROUP_POSITION_PENALTY 2
#define WORD_IN_GROUP_POSITION_PENALTY_MULTIPLIER 3

static const char wordDelimiters[] = " -_:./\\";

/**
 * Increase every value of heatmap array between from and to by delta.
 *
 * The passed array is modified by side effect.
 *
 * @param Array
 * @param From (inclusive)
 * @param To (exclusive)
 * @param Delta (can be positive or negative)
 */
void incHeatmap(int* heatmap, int from, int to, int delta);

/**
 * Create heatmap for given input
 *
 * The passed array is modified by side effect.
 *
 * @param Input string
 * @param Group delimiters.  A string where each character is a
 *        possible string delimiter.
 */
int* makeHeatmap(const char* input, const char* groupDelimiters);

#endif /* HEATMAP_H */
