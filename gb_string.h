// gb_string.h - v0.92 - public domain string library - no warranty implied; use at your own risk
// A Simple Dynamic Strings Library for C and C++
//
// Version History:
//     0.92 - Add extern "C" if compiling as C++
//     0.91 - Remove `char* cstr` from String_Header
//     0.90 - Initial Version
//
// LICENSE
//
//     This software is in the public domain. Where that dedication is not
//     recognized, you are granted a perpetual, irrevocable license to copy,
//     distribute, and modify this file as you see fit.
//
// How to use:
//
//     Do this:
//         #define GB_STRING_IMPLEMENTATION
//     before you include this file in *one* C++ file to create the implementation
//
//     i.e. it should look like this:
//     #include ...
//     #include ...
//     #include ...
//     #define GB_STRING_IMPLEMENTATION
//     #include "gb_string.h"
//
//     You can #define GB_ALLOC, and GB_FREE to avoid using malloc,free.
//
//     If you prefer to use C++, you can use all the same functions in a
//     namespace instead, do this:
//         #define GB_STRING_CPP
//     before you include the header file
//
//     i.e it should look like this:
//     #define GB_STRING_CPP
//     #include "gb_string.h"
//
//     The C++ version has the advantage that you do not need to reassign variables
//     i.e.
//
//         /* C version */
//         str = gb_append_cstring(str, "another string");
//         /* C++ version */
//         gb::append_cstring(str, "another string");
//
//         This could be achieved in C by passing a pointer to the string but for
//         simplicity and consistency, reassigning the variable is better.
// Reasoning:
//
//     By default, strings in C are null terminated which means you have to count
//     the number of character up to the null character to calculate the length.
//     Many "better" C string libraries will create a struct for a string.
//     i.e.
//
//         struct String
//         {
//             size_t length;
//             size_t capacity;
//             char*  cstring;
//         };
//
//     This library tries to augment normal C strings in a better way that is still
//     compatible with C-style strings.

//
//     +--------+-----------------------+-----------------+
//     | Header | Binary C-style String | Null Terminator |
//     +--------+-----------------------+-----------------+
//              |
//              +-> Pointer returned by functions
//
//     Due to the meta-data being stored before the string pointer and every gb string
//     having an implicit null terminator, gb strings are full compatible with c-style
//     strings and read-only functions.
//
// Advantages:
//
//     * gb strings can be passed to C-style string functions without accessing a struct
//       member of calling a function, i.e.
//
//           printf("%s\n", gb_str);
//
//       Many other libraries do either of these:
//
//           printf("%s\n", string->cstr);
//           printf("%s\n", get_cstring(string));
//
//     * You can access each character just like a C-style string:
//
//           printf("%c %c\n", str[0], str[13]);
//
//     * gb strings are singular allocated. The meta-data is next to the character array
//       which is better for the cache.
//
// Disadvantages:
//
//     * In the C version of these functions, many return the new string. i.e.
//
//           str = gb_append_cstring(str, "another string");
//
//       In the C++ version, this is made easier with the use of references. i.e.
//
//           gb::append_cstring(str, "another string");
//
//     * Custom allocators must redefine GB_ALLOC and GB_FREE which can be annoying.
//       realloc is not used for compatibility with many custom allocators that do not
//       have a reallocation function.
//
//
// Examples:
//
// C example
#if 0
#define GB_STRING_IMPLEMENTATION
#include "gb_string.h"

#include <stdio.h>

void string_test(void)
{
	gb_String str = gb_make_string("Hello");
	gb_String other_str = gb_make_string_length(", ", 2);
	str = gb_append_string(str, other_str);
	str = gb_append_cstring(str, "world!");

	printf("%s\n", str); // Hello, world!

	printf("str length = %d\n", gb_string_length(str));

	str = gb_set_string(str, "Potato soup");
	printf("%s\n", str); // Potato soup

	str = gb_set_string(str, "Hello");
	other_str = gb_set_string(other_str, "Pizza");
	if (gb_strings_are_equal(str, other_str))
		printf("Not called\n");
	else
		printf("Called\n");

	str = gb_set_string(str, "Ab.;!...AHello World       ??");
	str = gb_trim_string(str, "Ab.;!. ?");
	printf("%s\n", str); // "Hello World"

	gb_free_string(str);
	gb_free_string(other_str);

}
#endif
//
// C++ example
#if 0
#define GB_STRING_CPP
#define GB_STRING_IMPLEMENTATION
#include "gb_string.h"

#include <stdio.h>

void string_test()
{
	using namespace gb;

	String str = make_string("Hello");
	String other_str = make_string(", ", 2);
	append_string(str, other_str);
	append_cstring(str, "world!");

	printf("%s\n", str); // Hello, world!

	printf("str length = %d\n", string_length(str));

	set_string(str, "Potato soup");
	printf("%s\n", str); // Potato soup

	set_string(str, "Hello");
	set_string(other_str, "Pizza");
	if (strings_are_equal(str, other_str))
		printf("Not called\n");
	else
		printf("Called\n");

	set_string(str, "Ab.;!...AHello World       ??");
	trim_string(str, "Ab.;!. ?");
	printf("%s\n", str); // "Hello World"

	free_string(str);
	free_string(other_str);
}
#endif

#ifndef GB_STRING_INCLUDE_GB_STRING_H
#define GB_STRING_INCLUDE_GB_STRING_H

#ifndef GB_ALLOC
#define GB_ALLOC(sz) malloc(sz)
#define GB_FREE(sz)  free(sz)
#endif

#ifndef _MSC_VER
	#ifdef __cplusplus
	#define gb_inline inline
	#else
	#define gb_inline
	#endif
#else
#define gb_inline __forceinline
#endif

#include <string.h> // Needed for memcpy and cstring functions

#ifdef __cplusplus
extern "C" {
#endif

typedef char* gb_String;

typedef signed int gb_bool;
#if !defined(GB_TRUE) || !defined(GB_FALSE)
#define GB_TRUE  1
#define GB_FALSE 0
#endif

#ifndef GB_SIZE_TYPE
#define GB_SIZE_TYPE
typedef size_t gb_usize;
#endif

#ifndef GB_NULLPTR
	#if __cplusplus
		#if __cplusplus >= 201103L
		#define GB_NULLPTR nullptr
		#else
		#define GB_NULLPTR 0
		#endif
	#else
	#define GB_NULLPTR (void*)0
	#endif
#endif

struct gb_String_Header
{
	gb_usize len;
	gb_usize cap;
};

#define GB_STRING_HEADER(s) ((struct gb_String_Header*)((s) - sizeof(struct gb_String_Header)))

gb_String gb_make_string(const char* str);
gb_String gb_make_string_length(const void* str, gb_usize len);
void gb_free_string(gb_String str);

gb_String gb_duplicate_string(const gb_String str);

gb_usize gb_string_length(const gb_String str);
gb_usize gb_string_capacity(const gb_String str);
gb_usize gb_string_available_space(const gb_String str);

void gb_clear_string(gb_String str);

gb_String gb_append_string_length(gb_String str, const void* other, gb_usize len);
gb_String gb_append_string(gb_String str, const gb_String other);
gb_String gb_append_cstring(gb_String str, const char* other);

gb_String gb_set_string(gb_String str, const char* cstr);

gb_String gb_string_make_space_for(gb_String str, gb_usize add_len);
gb_usize gb_string_allocation_size(const gb_String str);

gb_bool gb_strings_are_equal(const gb_String lhs, const gb_String rhs);

gb_String gb_trim_string(gb_String str, const char* cut_set);


#ifdef __cplusplus
}
#endif

#ifdef GB_STRING_IMPLEMENTATION
static void gb_set_string_length(gb_String str, gb_usize len)
{
	GB_STRING_HEADER(str)->len = len;
}

static void gb_set_string_capacity(gb_String str, gb_usize cap)
{
	GB_STRING_HEADER(str)->cap = cap;
}


gb_String gb_make_string_length(const void* init_str, gb_usize len)
{
	gb_usize header_size = sizeof(struct gb_String_Header);
	void* ptr = GB_ALLOC(header_size + len + 1);
	if (!init_str)
		memset(ptr, 0, header_size + len + 1);

	if (ptr == GB_NULLPTR)
		return GB_NULLPTR;

	gb_String str = (char*)ptr + header_size;
	struct gb_String_Header* header = GB_STRING_HEADER(str);
	header->len = len;
	header->cap = len;
	if (len && init_str)
		memcpy(str, init_str, len);
	str[len] = '\0';

	return str;
}

gb_String gb_make_string(const char* str)
{
	gb_usize len = str ? strlen(str) : 0;
	return gb_make_string_length(str, len);
}

void gb_free_string(gb_String str)
{
	if (str == GB_NULLPTR)
		return;

	GB_FREE((char*)str - sizeof(struct gb_String_Header));
}

gb_String gb_duplicate_string(const gb_String str)
{
	return gb_make_string_length(str, gb_string_length(str));
}

gb_usize gb_string_length(const gb_String str)
{
	return GB_STRING_HEADER(str)->len;
}

gb_usize gb_string_capacity(const gb_String str)
{
	return GB_STRING_HEADER(str)->cap;
}

gb_usize gb_string_available_space(const gb_String str)
{
	struct gb_String_Header* h = GB_STRING_HEADER(str);
	if (h->cap > h->len)
		return h->cap - h->len;
	return 0;
}

void gb_clear_string(gb_String str)
{
	gb_set_string_length(str, 0);
	str[0] = '\0';
}

gb_String gb_append_string_length(gb_String str, const void* other, gb_usize other_len)
{
	gb_usize curr_len = gb_string_length(str);

	str = gb_string_make_space_for(str, other_len);
	if (str == GB_NULLPTR)
		return GB_NULLPTR;

	memcpy(str + curr_len, other, other_len);
	str[curr_len + other_len] = '\0';
	gb_set_string_length(str, curr_len + other_len);

	return str;
}

gb_String gb_append_string(gb_String str, const gb_String other)
{
	return gb_append_string_length(str, other, gb_string_length(other));
}

gb_String gb_append_cstring(gb_String str, const char* other)
{
	return gb_append_string_length(str, other, strlen(other));
}

gb_String gb_set_string(gb_String str, const char* cstr)
{
	gb_usize len = strlen(cstr);
	if (gb_string_capacity(str) < len)
	{
		str = gb_string_make_space_for(str, len - gb_string_length(str));
		if (str == GB_NULLPTR)
			return GB_NULLPTR;
	}

	memcpy(str, cstr, len);
	str[len] = '\0';
	gb_set_string_length(str, len);

	return str;
}

static gb_inline void* gb__string_realloc(void* ptr, gb_usize old_size, gb_usize new_size)
{
	if (!ptr)
		return GB_ALLOC(new_size);

	if (new_size < old_size)
		new_size = old_size;

	if (old_size == new_size)
		return ptr;

	void* new_ptr = GB_ALLOC(new_size);
	if (!new_ptr)
		return GB_NULLPTR;

	memcpy(new_ptr, ptr, old_size);

	GB_FREE(ptr);

	return new_ptr;
}

gb_String gb_string_make_space_for(gb_String str, gb_usize add_len)
{
	gb_usize len = gb_string_length(str);
	gb_usize new_len = len + add_len;

	gb_usize available = gb_string_available_space(str);
	if (available >= add_len) // Return if there is enough space left
		return str;


	void* ptr = (char*)str - sizeof(struct gb_String_Header);
	gb_usize old_size = sizeof(struct gb_String_Header) + gb_string_length(str) + 1;
	gb_usize new_size = sizeof(struct gb_String_Header) + new_len + 1;

	void* new_ptr = gb__string_realloc(ptr, old_size, new_size);
	if (new_ptr == GB_NULLPTR)
		return GB_NULLPTR;
	str = (char*)new_ptr + sizeof(struct gb_String_Header);

	gb_set_string_capacity(str, new_len);

	return str;
}

gb_usize gb_string_allocation_size(const gb_String s)
{
	gb_usize cap = gb_string_capacity(s);
	return sizeof(struct gb_String_Header) + cap;
}

gb_bool gb_strings_are_equal(const gb_String lhs, const gb_String rhs)
{
	gb_usize lhs_len = gb_string_length(lhs);
	gb_usize rhs_len = gb_string_length(rhs);
	if (lhs_len != rhs_len)
		return GB_FALSE;

	for (gb_usize i = 0; i < lhs_len; i++)
	{
		if (lhs[i] != rhs[i])
			return GB_FALSE;
	}

	return GB_TRUE;
}

gb_String gb_trim_string(gb_String str, const char* cut_set)
{
	char* start;
	char* end;
	char* start_pos;
	char* end_pos;

	start_pos = start = str;
	end_pos   = end   = str + gb_string_length(str) - 1;

	while (start_pos <= end && strchr(cut_set, *start_pos))
		start_pos++;
	while (end_pos > start_pos && strchr(cut_set, *end_pos))
		end_pos--;

	gb_usize len = (start_pos > end_pos) ? 0 : ((end_pos - start_pos)+1);

	if (str != start_pos)
		memmove(str, start_pos, len);
	str[len] = '\0';

	gb_set_string_length(str, len);

	return str;
}



#endif // GB_STRING_IMPLEMENTATION

#if defined(GB_STRING_CPP)

#if !defined(__cplusplus)
#error You need to compile as C++ for the C++ version of gb_string.h to work
#endif

namespace gb
{
typedef gb_String String;
typedef gb_usize usize;

gb_inline String make_string(const char* str = "")
{
	return gb_make_string(str);
}

gb_inline String make_string(const void* str, usize len)
{
	return gb_make_string_length(str, len);
}

gb_inline void free_string(String& str)
{
	gb_make_string(str);
	str = GB_NULLPTR;
}

gb_inline String duplicate_string(const String str)
{
	return gb_duplicate_string(str);
}

gb_inline usize string_length(const String str)
{
	return gb_string_length(str);
}

gb_inline usize string_capacity(const String str)
{
	return gb_string_capacity(str);
}

gb_inline usize string_available_space(const String str)
{
	return gb_string_available_space(str);
}

gb_inline void clear_string(String str)
{
	gb_clear_string(str);
}

gb_inline void append_string_length(String& str, const void* other, usize len)
{
	str = gb_append_string_length(str, other, len);
}

gb_inline void append_string(String& str, const String other)
{
	str = gb_append_string(str, other);
}

gb_inline void append_cstring(String& str, const char* other)
{
	str = gb_append_cstring(str, other);
}

gb_inline void set_string(String& str, const char* cstr)
{
	str = gb_set_string(str, cstr);
}

gb_inline void string_make_space_for(String& str, usize add_len)
{
	str = gb_string_make_space_for(str, add_len);
}

gb_inline usize string_allocation_size(const String str)
{
	return gb_string_allocation_size(str);
}

gb_inline bool strings_are_equal(const String lhs, const String rhs)
{
	return gb_strings_are_equal(lhs, rhs) == GB_TRUE;
}

gb_inline void trim_string(String& str, const char* cut_set)
{
	str = gb_trim_string(str, cut_set);
}

} // namespace gb
#endif // GB_STRING_CPP

#endif // GB_STRING_HP
