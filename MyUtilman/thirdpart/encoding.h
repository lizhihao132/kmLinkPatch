#ifndef TEST_ENCODING
#define TEST_ENCODING

typedef unsigned int uint32_t;
typedef unsigned long long uint64_t;

/************************************************************************/
/* 
下面所有的编码的函数，如果有返回值，则是输入字节指针的偏移
这样做的好处是不用使用额外的变量存储编码后的内容占几个字节
*/
/************************************************************************/
//编码固定长度的 32 位整数和 64 位整数
extern void EncodeFixed32(char* dst, uint32_t value);
extern void EncodeFixed64(char* dst, uint64_t value);

//解码固定长度的 32 位整数和 64 位整数
extern uint32_t DecodeFixed32(const char* ptr,bool kLittleEndian);
extern uint64_t DecodeFixed64(const char* ptr,bool kLittleEndian);

//编码变长的 32 位整数和 64 位整数
extern char* EncodeVarint32(char* dst, uint32_t value);
extern char* EncodeVarint64(char* dst, uint64_t value);

//解码变长的 32 位整数和 64 位整数
extern const char* GetVarint32Ptr(const char* p,const char* limit,uint32_t* value) ;
extern const char* GetVarint64Ptr(const char* p, const char* limit, uint64_t* value);
// Internal routine for use by fallback path of GetVarint32Ptr
extern const char* GetVarint32PtrFallback(const char* p,
										  const char* limit,
										  uint32_t* value);

#endif