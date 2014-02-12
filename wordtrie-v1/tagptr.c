#include "tagptr.h"

extern inline void* mask_ptr(tagptr_t p);
extern inline void set_tag(tagptr_t *p, uint16_t tag);
extern inline uint16_t get_tag(tagptr_t p);
