#ifndef INCLUDE_CPP_ASSERTS_H_
#define INCLUDE_CPP_ASSERTS_H_

#include <assert.h>
#include <stdbool.h>

#define ALWAYS(X) (assert(X), (X))
#define NEVER(X) (assert(!(X)), (X))

#endif // INCLUDE_CPP_ASSERTS_H_
