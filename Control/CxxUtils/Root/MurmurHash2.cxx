/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
/*
 * Public domain; see below.
 */
/**
 * @file CxxUtils/Root/MurmurHash2.cxx
 * @author Austin Appleby, adapted for ATLAS by scott snyder <snyder@bnl.gov>
 * @date Feb, 2018
 * @brief Implementation of MurmurHash2.
 *
 * From <https://github.com/aappleby/smhasher>
 */


//-----------------------------------------------------------------------------
// MurmurHash2 was written by Austin Appleby, and is placed in the public

// Note - This code makes a few assumptions about how your machine behaves -

// 1. We can read a 4-byte value from any address without crashing
// 2. sizeof(int) == 4

// And it has a few limitations -

// 1. It will not work incrementally.
// 2. It will not produce the same results on little-endian and big-endian
//    machines.

#include "CxxUtils/MurmurHash2.h"

//-----------------------------------------------------------------------------
// Platform-specific functions and macros

#define BIG_CONSTANT(x) (x##LLU)

//-----------------------------------------------------------------------------


namespace CxxUtils {


uint32_t MurmurHash2 ( const void * key, int len, uint32_t seed )
{
  // 'm' and 'r' are mixing constants generated offline.
  // They're not really 'magic', they just happen to work well.

  const uint32_t m = 0x5bd1e995;
  const int r = 24;

  // Initialize the hash to a 'random' value

  uint32_t h = seed ^ len;

  // Mix 4 bytes at a time into the hash

  const unsigned char * data = (const unsigned char *)key;

  while(len >= 4)
  {
    uint32_t k = *(uint32_t*)data;

    k *= m;
    k ^= k >> r;
    k *= m;

    h *= m;
    h ^= k;

    data += 4;
    len -= 4;
  }

  // Handle the last few bytes of the input array

  switch(len)
  {
  case 3: h ^= data[2] << 16;
  // FALLTHROUGH
  case 2: h ^= data[1] << 8;
  // FALLTHROUGH
  case 1: h ^= data[0];
      h *= m;
  };

  // Do a few final mixes of the hash to ensure the last few
  // bytes are well-incorporated.

  h ^= h >> 13;
  h *= m;
  h ^= h >> 15;

  return h;
} 

//-----------------------------------------------------------------------------
// MurmurHash2, 64-bit versions, by Austin Appleby

// The same caveats as 32-bit MurmurHash2 apply here - beware of alignment 
// and endian-ness issues if used across multiple platforms.

// 64-bit hash for 64-bit platforms

uint64_t MurmurHash64A ( const void * key, int len, uint64_t seed )
{
  const uint64_t m = BIG_CONSTANT(0xc6a4a7935bd1e995);
  const int r = 47;

  uint64_t h = seed ^ (len * m);

  const uint64_t * data = (const uint64_t *)key;
  const uint64_t * end = data + (len/8);

  while(data != end)
  {
    uint64_t k = *data++;

    k *= m; 
    k ^= k >> r; 
    k *= m; 
    
    h ^= k;
    h *= m; 
  }

  const unsigned char * data2 = (const unsigned char*)data;

  switch(len & 7)
  {
  case 7: h ^= uint64_t(data2[6]) << 48;
  // FALLTHROUGH
  case 6: h ^= uint64_t(data2[5]) << 40;
  // FALLTHROUGH
  case 5: h ^= uint64_t(data2[4]) << 32;
  // FALLTHROUGH
  case 4: h ^= uint64_t(data2[3]) << 24;
  // FALLTHROUGH
  case 3: h ^= uint64_t(data2[2]) << 16;
  // FALLTHROUGH
  case 2: h ^= uint64_t(data2[1]) << 8;
  // FALLTHROUGH
  case 1: h ^= uint64_t(data2[0]);
          h *= m;
  };
 
  h ^= h >> r;
  h *= m;
  h ^= h >> r;

  return h;
} 


// 64-bit hash for 32-bit platforms

uint64_t MurmurHash64B ( const void * key, int len, uint64_t seed )
{
  const uint32_t m = 0x5bd1e995;
  const int r = 24;

  uint32_t h1 = uint32_t(seed) ^ len;
  uint32_t h2 = uint32_t(seed >> 32);

  const uint32_t * data = (const uint32_t *)key;

  while(len >= 8)
  {
    uint32_t k1 = *data++;
    k1 *= m; k1 ^= k1 >> r; k1 *= m;
    h1 *= m; h1 ^= k1;
    len -= 4;

    uint32_t k2 = *data++;
    k2 *= m; k2 ^= k2 >> r; k2 *= m;
    h2 *= m; h2 ^= k2;
    len -= 4;
  }

  if(len >= 4)
  {
    uint32_t k1 = *data++;
    k1 *= m; k1 ^= k1 >> r; k1 *= m;
    h1 *= m; h1 ^= k1;
    len -= 4;
  }

  switch(len)
  {
  case 3: h2 ^= ((unsigned const char*)data)[2] << 16;
  // FALLTHROUGH
  case 2: h2 ^= ((unsigned const char*)data)[1] << 8;
  // FALLTHROUGH
  case 1: h2 ^= ((unsigned const char*)data)[0];
      h2 *= m;
  };

  h1 ^= h2 >> 18; h1 *= m;
  h2 ^= h1 >> 22; h2 *= m;
  h1 ^= h2 >> 17; h1 *= m;
  h2 ^= h1 >> 19; h2 *= m;

  uint64_t h = h1;

  h = (h << 32) | h2;

  return h;
} 

//-----------------------------------------------------------------------------
// MurmurHash2A, by Austin Appleby

// This is a variant of MurmurHash2 modified to use the Merkle-Damgard 
// construction. Bulk speed should be identical to Murmur2, small-key speed 
// will be 10%-20% slower due to the added overhead at the end of the hash.

// This variant fixes a minor issue where null keys were more likely to
// collide with each other than expected, and also makes the function
// more amenable to incremental implementations.


uint32_t MurmurHash2A ( const void * key, int len, uint32_t seed )
{
  const uint32_t M = 0x5bd1e995;
  const int R = 24;
  uint32_t l = len;

  const unsigned char * data = (const unsigned char *)key;

  uint32_t h = seed;

  while(len >= 4)
  {
    uint32_t k = *(uint32_t*)data;

    MurmurHash_mmix(h,k);

    data += 4;
    len -= 4;
  }

  uint32_t t = 0;

  switch(len)
  {
  case 3: t ^= data[2] << 16;
  // FALLTHROUGH
  case 2: t ^= data[1] << 8;
  // FALLTHROUGH
  case 1: t ^= data[0];
  };

  MurmurHash_mmix(h,t);
  MurmurHash_mmix(h,l);

  h ^= h >> 13;
  h *= M;
  h ^= h >> 15;

  return h;
}

//-----------------------------------------------------------------------------
// MurmurHashNeutral2, by Austin Appleby

// Same as MurmurHash2, but endian- and alignment-neutral.
// Half the speed though, alas.

uint32_t MurmurHashNeutral2 ( const void * key, int len, uint32_t seed )
{
  const uint32_t m = 0x5bd1e995;
  const int r = 24;

  uint32_t h = seed ^ len;

  const unsigned char * data = (const unsigned char *)key;

  while(len >= 4)
  {
    uint32_t k;

    k  = data[0];
    k |= data[1] << 8;
    k |= data[2] << 16;
    k |= data[3] << 24;

    k *= m; 
    k ^= k >> r; 
    k *= m;

    h *= m;
    h ^= k;

    data += 4;
    len -= 4;
  }
  
  switch(len)
  {
  case 3: h ^= data[2] << 16;
  // FALLTHROUGH
  case 2: h ^= data[1] << 8;
  // FALLTHROUGH
  case 1: h ^= data[0];
          h *= m;
  };

  h ^= h >> 13;
  h *= m;
  h ^= h >> 15;

  return h;
} 

//-----------------------------------------------------------------------------
// MurmurHashAligned2, by Austin Appleby

// Same algorithm as MurmurHash2, but only does aligned reads - should be safer
// on certain platforms. 

// Performance will be lower than MurmurHash2

#define MIX(h,k,m) { k *= m; k ^= k >> r; k *= m; h *= m; h ^= k; }


uint32_t MurmurHashAligned2 ( const void * key, int len, uint32_t seed )
{
  const uint32_t m = 0x5bd1e995;
  const int r = 24;

  const unsigned char * data = (const unsigned char *)key;

  uint32_t h = seed ^ len;

  int align = (uint64_t)data & 3;

  if(align && (len >= 4))
  {
    // Pre-load the temp registers

    uint32_t t = 0, d = 0;

    switch(align)
    {
      case 1: t |= data[2] << 16;
      // FALLTHROUGH
      case 2: t |= data[1] << 8;
      // FALLTHROUGH
      case 3: t |= data[0];
    }

    t <<= (8 * align);

    data += 4-align;
    len -= 4-align;

    int sl = 8 * (4-align);
    int sr = 8 * align;

    // Mix

    while(len >= 4)
    {
      d = *(uint32_t *)data;
      t = (t >> sr) | (d << sl);

      uint32_t k = t;

      MIX(h,k,m);

      t = d;

      data += 4;
      len -= 4;
    }

    // Handle leftover data in temp registers

    d = 0;

    if(len >= align)
    {
      switch(align)
      {
      case 3: d |= data[2] << 16;
      // FALLTHROUGH
      case 2: d |= data[1] << 8;
      // FALLTHROUGH
      case 1: d |= data[0];
      }

      uint32_t k = (t >> sr) | (d << sl);
      MIX(h,k,m);

      // At this point, we know that len < 4 and align > 0.
      data += align;
      len -= align;
      // So here, we must have 0 >= len < 3.

      //----------
      // Handle tail bytes

      switch(len)
      {
      // can't happen --- see above.
      //case 3: h ^= data[2] << 16;
      // FALLTHROUGH
      case 2: h ^= data[1] << 8;
      // FALLTHROUGH
      case 1: h ^= data[0];
          h *= m;
      };
    }
    else
    {
      switch(len)
      {
      // len cannot be 3, because align is at most 3.
      //case 3: d |= data[2] << 16;
      // FALLTHROUGH
      case 2: d |= data[1] << 8;
      // FALLTHROUGH
      case 1: d |= data[0];
      // FALLTHROUGH
      case 0: h ^= (t >> sr) | (d << sl);
          h *= m;
      }
    }

    h ^= h >> 13;
    h *= m;
    h ^= h >> 15;

    return h;
  }
  else
  {
    while(len >= 4)
    {
      uint32_t k = *(uint32_t *)data;

      MIX(h,k,m);

      data += 4;
      len -= 4;
    }

    //----------
    // Handle tail bytes

    switch(len)
    {
    case 3: h ^= data[2] << 16;
    // FALLTHROUGH
    case 2: h ^= data[1] << 8;
    // FALLTHROUGH
    case 1: h ^= data[0];
        h *= m;
    };

    h ^= h >> 13;
    h *= m;
    h ^= h >> 15;

    return h;
  }
}

//-----------------------------------------------------------------------------


} // namespace CxxUtils
