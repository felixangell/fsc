#ifndef SIP_HASH_H_INCLUDED
#define SIP_HASH_H_INCLUDED

#include "type.h"

int siphash(const u8 *in, const u32 inlen, const u8 *k,
            u8 *out, const u32 outlen);

int halfsiphash(const u8 *in, const u32 inlen, const u8 *k,
                u8 *out, const u32 outlen);


#endif // SIP_HASH_H_INCLUDED
