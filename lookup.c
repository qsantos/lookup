#include "lookup.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <math.h>

#include "md5.h"

#define CHASH(I) (lt->rows + (I)*lt->sizeofRow)
#define  CSTR(I) (lt->rows + (I)*lt->sizeofRow + lt->l_hash)

void LTable_New(LTable* lt, u32 l_string, char* charset)
{
	lt->l_hash    = 16;
	lt->l_string  = l_string;
	lt->charset   = strdup(charset);
	lt->n_charset = strlen(charset);

	lt->n_rows    = pow(lt->n_charset, lt->l_string);
	lt->sizeofRow = lt->l_hash + lt->l_string;
	lt->rows      = malloc(lt->n_rows * lt->sizeofRow);

	lt->curidx    = 0;
	lt->curstr    = malloc(lt->l_string);
	lt->bufrow    = malloc(lt->sizeofRow);

	assert(lt->rows);
	assert(lt->curstr);
	assert(lt->bufrow);

	memset(lt->curstr, charset[0], lt->l_string);
}

void LTable_Delete(LTable* lt)
{
	free(lt->bufrow);
	free(lt->curstr);
	free(lt->rows);
}

char LTable_Next(LTable* lt)
{
	// compute and store the hash of the current string
	MD5((u8*) CHASH(lt->curidx), (u8*) lt->curstr, lt->l_string);

	// compute next string
	char* c = lt->curstr;
	while (*c)
	{
		char* n = strchr(lt->charset, *c);
		if ((*c = n[1]))
			break;
		*c = lt->charset[0];
		c++;
	}
	if (!*c)
		return 0;

	lt->curidx++;
	return 1;
}

static void swap(LTable* lt, u32 a, u32 b)
{
	memcpy(lt->bufrow,                 lt->rows + a*lt->sizeofRow, lt->sizeofRow);
	memcpy(lt->rows + a*lt->sizeofRow, lt->rows + b*lt->sizeofRow, lt->sizeofRow);
	memcpy(lt->rows + b*lt->sizeofRow, lt->bufrow,                 lt->sizeofRow);
}
static void quicksort(LTable* lt, u32 left, u32 right)
{
	if (left >= right)
		return;

	swap(lt, (left+right)/2, right);
	char* pivotValue = CHASH(right);
	u32 storeIndex = left;
	for (u32 i = left; i < right; i++)
		if (bstrncmp(CHASH(i), pivotValue, lt->l_hash) < 0)
			swap(lt, i, storeIndex++);

	swap(lt, storeIndex, right);

	if (storeIndex)
		quicksort(lt, left, storeIndex-1);
	quicksort(lt, storeIndex+1, right);
}
void LTable_Sort(LTable* lt)
{
	quicksort(lt, 0, lt->n_rows-1);
}

void LTable_ToFile(LTable* lt, const char* filename)
{
	FILE* f = fopen(filename, "w");
	if (!f)
	{
		fprintf(stderr, "Could not open '%s'\n", filename);
		exit(1);
	}

	LTF_header h =
	{
		0x3046525C,
		lt->l_string,
		lt->n_charset,
	};
	fwrite(&h,          sizeof(LTF_header), 1,             f);
	fwrite(lt->charset, 1,                  lt->n_charset, f);
	fwrite(lt->curstr,  1,                  lt->l_string,  f);
	fwrite(lt->rows,    lt->sizeofRow,      lt->n_rows,    f);

	fclose(f);
}

char LTable_FromFile(LTable* lt, const char* filename)
{
	FILE* f = fopen(filename, "r");
	if (!f)
		return 0;

	LTF_header h;
	fread(&h, sizeof(LTF_header), 1, f);

	// TODO : avoid allocating and freeing charset
	char* charset = malloc(h.n_charset + 1);
	assert(charset);
	fread(charset, 1, h.n_charset, f);
	charset[h.n_charset] = 0;

	LTable_New(lt, h.l_string, charset);
	free(charset);

	fread(lt->curstr, 1,             lt->l_string, f);
	fread(lt->rows,   lt->sizeofRow, lt->n_rows,   f);

	fclose(f);
	return 1;
}

static s32 binaryFind(LTable* lt, const char* hash)
{
        u32 start = 0;
        u32 end   = lt->n_rows;
        while (start != end)
        {
                u32 middle = (start + end) / 2;
                if (bstrncmp(hash, CHASH(middle), lt->l_hash) <= 0)
                        end = middle;
                else
                        start = middle + 1;
        }
        if (bstrncmp(CHASH(start), hash, lt->l_hash) == 0)
                return start;
        else
                return -1;
}
char LTable_Reverse(LTable* lt, const char* hash, char* dst)
{
	s32 res = binaryFind(lt, hash);
	if (res < 0)
		return 0;

	memcpy(dst, CSTR(res), lt->l_string);
	return 1;
}

char bstrncmp(const char* a, const char* b, u32 n)
{
	for (u32 i = 0; i < n; i++, a++, b++)
		if (*a != *b)
			return *(u8*)a < *(u8*)b ? -1 : 1;
	return 0;
}

void hex2hash(const char* hex, char* hash, u32 l_hash)
{
	for (u32 i = 0; i < l_hash; i++)
	{
		*hash  = *hex - (*hex <= '9' ? '0' : 87);
		hex++;
		*hash *= 16;
		*hash += *hex - (*hex <= '9' ? '0' : 87);
		hex++;
		hash++;
	}
}

void printHash(const char* hash, u32 l_hash)
{
	for (u32 i = 0; i < l_hash; i++, hash++)
		printf("%.2x", (u8) *hash);
}

void printString(const char* str, u32 l_string)
{
	for (u32 j = 0; j < l_string; j++, str++)
		printf("%c", *str);
}
