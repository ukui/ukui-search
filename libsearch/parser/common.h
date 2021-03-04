#ifndef COMMON_H
#define COMMON_H
#include <QtCore>
#include <QtConcurrent/QtConcurrent>

#define SERVER "Everything"

#define LOG(a) \
	//qWarning() << a;

#define REHASH(a) \
	if (sl_minus_1 < (int)sizeof(int) * CHAR_BIT)       \
		hashHaystack -= (a) << sl_minus_1; \
	hashHaystack <<= 1

void* xmalloc(size_t tSize);

void* xcalloc(size_t tNmemb, size_t tSize);

void* xrealloc(void *pvArg, size_t tSize);

void* xfree(void *pvArg);


#endif // COMMON_H
