#ifndef KMP_H
#define KMP_H


class KMP
{
private:
	int paternLen;
	const char *paternStr;
	int *t1Length;//A[i] 表示字符串 T[0 ~ i-1] 求得的 t1 的长度
	int *t1LastLength;//A[i] 表示字符串 T[strlen(T) - i ~ strlen(T)-1] 求得的 t1 的长度
public:
	KMP(const char *pStr);
	~KMP();

	//正向搜索
	int indexOf(const char *sourceStr,int beginPos = 0);

	/************************************************************************/
	/* 
		从后往前(反方向)找.beginPos 是从哪一处开始，往反方向找
	*/
	/************************************************************************/
	int lastIndexOf(const char *sourceStr,int beginPos);




private:
	//求一个字符串的反向字符串，测试用
	static void reverseStr(char *str);

	bool hasInitT1Length;
	//初始化正向的 t1LengthArray
	void initT1Length();

	/************************************************************************/
	/* 
	初始化反向的 t1LengthArray
	< ---------------------
	*/
	/************************************************************************/
	bool hasInitReverseT1Length;
	void initReverseT1Length();

	//字符串 str 正向的 t1LengthArray 应该于 str 的反向字符串的 reverseT1LengthArray
	static bool isT1LengthArrayEqual(char *pStr,int aLength);

public:

	//测试反向的 t1Length 是不是正确的
	static void testT1LengthRigth();

};


#endif