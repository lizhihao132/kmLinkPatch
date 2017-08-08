#include "encoding.h"
#include <memory>

void EncodeFixed32(char* buf, uint32_t value) {
#if __BYTE_ORDER == __LITTLE_ENDIAN
	memcpy(buf, &value, sizeof(value));
#else
	buf[0] = value & 0xff;
	buf[1] = (value >> 8) & 0xff;
	buf[2] = (value >> 16) & 0xff;
	buf[3] = (value >> 24) & 0xff;
#endif
}

void EncodeFixed64(char* buf, uint64_t value) {
#if __BYTE_ORDER == __LITTLE_ENDIAN
	memcpy(buf, &value, sizeof(value));
#else
	buf[0] = value & 0xff;
	buf[1] = (value >> 8) & 0xff;
	buf[2] = (value >> 16) & 0xff;
	buf[3] = (value >> 24) & 0xff;
	buf[4] = (value >> 32) & 0xff;
	buf[5] = (value >> 40) & 0xff;
	buf[6] = (value >> 48) & 0xff;
	buf[7] = (value >> 56) & 0xff;
#endif
}



char* EncodeVarint32(char* dst, uint32_t v) {
	// Operate on characters as unsigneds
	unsigned char* ptr = reinterpret_cast<unsigned char*>(dst);
	static const int B = 128;
	if (v < (1<<7)) {
		*(ptr++) = v;
	} else if (v < (1<<14)) {
		*(ptr++) = v | B;
		*(ptr++) = v>>7;
	} else if (v < (1<<21)) {
		*(ptr++) = v | B;
		*(ptr++) = (v>>7) | B;
		*(ptr++) = v>>14;
	} else if (v < (1<<28)) {
		*(ptr++) = v | B;
		*(ptr++) = (v>>7) | B;
		*(ptr++) = (v>>14) | B;
		*(ptr++) = v>>21;
	} else {
		*(ptr++) = v | B;
		*(ptr++) = (v>>7) | B;
		*(ptr++) = (v>>14) | B;
		*(ptr++) = (v>>21) | B;
		*(ptr++) = v>>28;
	}
	return reinterpret_cast<char*>(ptr);
}


char* EncodeVarint64(char* dst, uint64_t v) {
	static const int B = 128;
	unsigned char* ptr = reinterpret_cast<unsigned char*>(dst);
	while (v >= B) {
		*(ptr++) = (v & (B-1)) | B;
		v >>= 7;
	}
	*(ptr++) = static_cast<unsigned char>(v);
	return reinterpret_cast<char*>(ptr);
}


inline uint32_t DecodeFixed32(const char* ptr,bool kLittleEndian) 
{
	if (kLittleEndian) {
		// Load the raw bytes
		uint32_t result;
		memcpy(&result, ptr, sizeof(result));  // gcc optimizes this to a plain load
		return result;
	} else {
		return ((static_cast<uint32_t>(ptr[0]))
			| (static_cast<uint32_t>(ptr[1]) << 8)
			| (static_cast<uint32_t>(ptr[2]) << 16)
			| (static_cast<uint32_t>(ptr[3]) << 24));
	}
}

inline uint64_t DecodeFixed64(const char* ptr,bool kLittleEndian) {
	if (kLittleEndian) {
		// Load the raw bytes
		uint64_t result;
		memcpy(&result, ptr, sizeof(result));  // gcc optimizes this to a plain load
		return result;
	} else {
		uint64_t lo = DecodeFixed32(ptr,kLittleEndian);
		uint64_t hi = DecodeFixed32(ptr + 4,kLittleEndian);
		return (hi << 32) | lo;
	}
}

const char* GetVarint64Ptr(const char* p, const char* limit, uint64_t* value) 
{
	uint64_t result = 0;
	for (uint32_t shift = 0; shift <= 63 && p < limit; shift += 7) {
		uint64_t byte = *(reinterpret_cast<const unsigned char*>(p));
		p++;
		if (byte & 128) {
			// More bytes are present
			result |= ((byte & 127) << shift);
		} else {
			result |= (byte << shift);
			*value = result;
			return reinterpret_cast<const char*>(p);
		}
	}
	return NULL;
}

inline const char* GetVarint32Ptr(const char* p,
								  const char* limit,
								  uint32_t* value) 
{
	if (p < limit) {
		uint32_t result = *(reinterpret_cast<const unsigned char*>(p));
		if ((result & 128) == 0) {
			*value = result;
			return p + 1;
		}
	}
	return GetVarint32PtrFallback(p, limit, value);
}

const char* GetVarint32PtrFallback(const char* p,
								   const char* limit,
								   uint32_t* value) 
{
	uint32_t result = 0;
	for (uint32_t shift = 0; shift <= 28 && p < limit; shift += 7) {
		uint32_t byte = *(reinterpret_cast<const unsigned char*>(p));
		p++;
		if (byte & 128) {
			// More bytes are present
			result |= ((byte & 127) << shift);
		} else {
			result |= (byte << shift);
			*value = result;
			return reinterpret_cast<const char*>(p);
		}
	}
	return NULL;
}