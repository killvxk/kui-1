// test.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <memory.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "iconv.h"


//代码转换:从一种编码转为另一种编码
int code_convert(char *from_charset, char *to_charset, const char *inbuf, size_t inlen, char *outbuf, size_t outlen)
{
	iconv_t cd;
	const char **pin = &inbuf;
	char **pout = &outbuf;

	cd = iconv_open(to_charset, from_charset);
	if (cd == 0) 
		return -1;
	memset(outbuf, 0, outlen);
	if (iconv(cd, pin, &inlen, pout, &outlen) == -1)
		return -1;
	iconv_close(cd);
	return 0;
}
//UNICODE码转为GB2312码
int u2g(const char *inbuf, int inlen, char *outbuf, int outlen)
{
	return code_convert("utf-8", "gb2312", inbuf, inlen, outbuf, outlen);
}
//GB2312码转为UNICODE码
int g2u(char *inbuf, size_t inlen, char *outbuf, size_t outlen)
{
	return code_convert("gb2312", "utf-8", inbuf, inlen, outbuf, outlen);
}
#define  OUTLEN 1024
int _tmain(int argc, _TCHAR* argv[])
{
	char *in_utf8 = "姝ｅ湪瀹夎";
	char *in_gb2312 = "正在安装";
	char out[OUTLEN] = { 0 };

	int rc = 0;
	//unicode码转为gb2312码
	rc = u2g(in_utf8, (int)strlen(in_utf8), out, OUTLEN);
	printf("utf-8-->gb2312 out=%s\n", out);
	//gb2312码转为unicode码
	rc = g2u(in_gb2312, (int)strlen(in_gb2312), out, OUTLEN);
	printf("gb2312-->utf-8 out=%s\n", out);
	system("pause");
	return 0;
}
