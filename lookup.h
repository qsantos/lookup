#ifndef LOOKUP_H
#define LOOKUP_H

// a row is made of a hash and a string
// the length of a row is therefore l_hash+l_string (sizeofRow)

typedef unsigned char u8;
typedef unsigned long u32;
typedef signed   long s32;

typedef struct __attribute__((packed))
{
	u32 version; // LTF0 // 0x3046524C
	u32 l_string;
	u32 curidx;
	u32 n_charset;
} LTF_header;

typedef struct
{
	u32   l_hash;      // hash length
	u32   l_string;    // string length
	char* charset;     // character set
	u32   n_charset;   // character count

	u32   n_rows;      // number of rows in the table
	u32   sizeofRow;   // memory size of a row
	char* rows;        // data

	u32   curidx;      // current string index
	char* curstr;      // current string
	char* bufrow;      // temporary row
} LTable;

// generation
void LTable_New   (LTable* lt, u32 l_string, char* charset);
void LTable_Delete(LTable* lt);
char LTable_Next  (LTable* lt);
void LTable_Sort  (LTable* lt);

// loading and storing
void LTable_ToFile  (LTable* lt, const char* filename);
char LTable_FromFile(LTable* lt, const char* filename);

// misc
char LTable_Reverse(LTable* lt, const char* hash, char* dst);

// useful functions
char bstrncmp   (const char* a, const char* b, u32 n);
void hex2hash   (const char* hex, char* hash, u32 l_hash);
void printHash  (const char* hash, u32 l_hash);
void printString(const char* str, u32 l_string);

#endif
