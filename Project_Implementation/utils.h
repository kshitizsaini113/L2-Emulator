#ifndef __UTILS__
#define __UTILS__
// Performing header file management to avoid duplicate inclusion of header file.


typedef enum
{
    FALSE,
    TRUE
} boolean_t;
// Define boolean data-type to be used further.


#define IS_BIT_SET(number, position)                                                                 \
    ((number & (1 << (position))) !=0)
// Checks if a particular bit is set.
// Used AND operator with 1 at the specified position to check if the bit is set.


#define TOOGLE_BIT(number, position)                                                                 \
    (number = number ^ (1 << (position)))
// Changes the polarity of a bit
// Uses XOR operator along with the position for the change.


#define COMPLEMENT(number)                                                                      \
    (number = number ^ 0xFFFFFFFF)
// Returns the complement of a number.
// Compares with all 1's and uses XOR for complement.


#define UNSET_BIT(number, position)                                                                  \
    (number = number & ((1 << position) ^ 0xFFFFFFFF))
// Unsets a bit from a number.
// First set the bit and then complement the number to unset the bit.
// Then performing AND operatin with the number.


#define SET_BIT(number, position)                                                                    \
    (number = number | 1 <<position)
// Sets the bit for a number.
// Sets the specified bit by using OR with the 1 at that bit.


#endif
// Ending Header File Management