﻿// license:BSD-3-Clause
// copyright-holders:Aaron Giles, Jürgen Buchmüller
/*********************************************************************

	unicode.c

	Unicode related functions

***************************************************************************/

#include "unicode.h"


/**
 * @brief test for legitimate unicode values
 *
 * return true if a given character is a legitimate unicode character
 *
 * @param uchar value to inspect
 * @return non zero (true) if uchar is valid, 0 otherwise
 */
int uchar_isvalid(unicode_char uchar)
{
	return (uchar < 0x110000) && !((uchar >= 0xd800) && (uchar <= 0xdfff));
}


/**
 * @brief convert an UTF-8 sequence into an unicode character
 * @param uchar pointer to the resulting unicode_char
 * @param utf8char pointer to the source string (may be NULL)
 * @param count number of characters available in utf8char
 * @return the number of characters used
 */
int uchar_from_utf8(unicode_char *uchar, const char *utf8char, size_t count)
{
	unicode_char c, minchar;
	int auxlen, i;
	char auxchar;

	/* validate parameters */
	if (utf8char == NULL || count == 0)
		return 0;

	/* start with the first byte */
	c = (unsigned char) *utf8char;
	count--;
	utf8char++;

	/* based on that, determine how many additional bytes we need */
	if (c < 0x80)
	{
		/* unicode char 0x00000000 - 0x0000007F */
		c &= 0x7f;
		auxlen = 0;
		minchar = 0x00000000;
	}
	else if (c >= 0xc0 && c < 0xe0)
	{
		/* unicode char 0x00000080 - 0x000007FF */
		c &= 0x1f;
		auxlen = 1;
		minchar = 0x00000080;
	}
	else if (c >= 0xe0 && c < 0xf0)
	{
		/* unicode char 0x00000800 - 0x0000FFFF */
		c &= 0x0f;
		auxlen = 2;
		minchar = 0x00000800;
	}
	else if (c >= 0xf0 && c < 0xf8)
	{
		/* unicode char 0x00010000 - 0x001FFFFF */
		c &= 0x07;
		auxlen = 3;
		minchar = 0x00010000;
	}
	else if (c >= 0xf8 && c < 0xfc)
	{
		/* unicode char 0x00200000 - 0x03FFFFFF */
		c &= 0x03;
		auxlen = 4;
		minchar = 0x00200000;
	}
	else if (c >= 0xfc && c < 0xfe)
	{
		/* unicode char 0x04000000 - 0x7FFFFFFF */
		c &= 0x01;
		auxlen = 5;
		minchar = 0x04000000;
	}
	else
	{
		/* invalid */
		return -1;
	}

	/* exceeds the count? */
	if (auxlen > count)
		return -1;

	/* we now know how long the char is, now compute it */
	for (i = 0; i < auxlen; i++)
	{
		auxchar = utf8char[i];

		/* all auxillary chars must be between 0x80-0xbf */
		if ((auxchar & 0xc0) != 0x80)
			return -1;

		c = c << 6;
		c |= auxchar & 0x3f;
	}

	/* make sure that this char is above the minimum */
	if (c < minchar)
		return -1;

	*uchar = c;
	return auxlen + 1;
}


/**
 * @brief convert a UTF-16 sequence	into an unicode character
 * @param uchar pointer to the resulting unicode_char
 * @param utf16char pointer to the source string (may be NULL)
 * @param count number of characters available in utf16char
 * @return the number of characters used
 */
int uchar_from_utf16(unicode_char *uchar, const utf16_char *utf16char, size_t count)
{
	int rc = -1;

	/* validate parameters */
	if (utf16char == NULL || count == 0)
		return 0;

	/* handle the two-byte case */
	if (utf16char[0] >= 0xd800 && utf16char[0] <= 0xdbff)
	{
		if (count > 1 && utf16char[1] >= 0xdc00 && utf16char[1] <= 0xdfff)
		{
			*uchar = 0x10000 + ((utf16char[0] & 0x3ff) * 0x400) + (utf16char[1] & 0x3ff);
			rc = 2;
		}
	}

	/* handle the one-byte case */
	else if (utf16char[0] < 0xdc00 || utf16char[0] > 0xdfff)
	{
		*uchar = utf16char[0];
		rc = 1;
	}

	return rc;
}


/**
 * @brief convert a UTF-16 sequence	into an unicode character from a flipped byte order
 *
 * This flips endianness of the first two utf16_char in a local
 * copy and then calls uchar_from_utf16.
 *
 * @param uchar pointer to the resulting unicode_char
 * @param utf16char pointer to the source string (may be NULL)
 * @param count number of characters available in utf16char
 * @return the number of characters used
 */
int uchar_from_utf16f(unicode_char *uchar, const utf16_char *utf16char, size_t count)
{
	utf16_char buf[2] = {0};
	if (count > 0)
		buf[0] = FLIPENDIAN_INT16(utf16char[0]);
	if (count > 1)
		buf[1] = FLIPENDIAN_INT16(utf16char[1]);
	return uchar_from_utf16(uchar, buf, count);
}


/**
 * @brief convert an unicode character into a UTF-8 sequence
 * @param utf8string pointer to the result char array
 * @param count number of characters that can be written to utf8string
 * @param uchar unciode_char value to convert
 * @return -1 on error, or the number of chars written on success (1 to 6)
 */
int utf8_from_uchar(char *utf8string, size_t count, unicode_char uchar)
{
	int rc = 0;

	/* error on invalid characters */
	if (!uchar_isvalid(uchar))
		return -1;

	/* based on the value, output the appropriate number of bytes */
	if (uchar < 0x80)
	{
		/* unicode char 0x00000000 - 0x0000007F */
		if (count < 1)
			return -1;
		utf8string[rc++] = (char) uchar;
	}
	else if (uchar < 0x800)
	{
		/* unicode char 0x00000080 - 0x000007FF */
		if (count < 2)
			return -1;
		utf8string[rc++] = ((char) (uchar >> 6)) | 0xC0;
		utf8string[rc++] = ((char) ((uchar >> 0) & 0x3F)) | 0x80;
	}
	else if (uchar < 0x10000)
	{
		/* unicode char 0x00000800 - 0x0000FFFF */
		if (count < 3)
			return -1;
		utf8string[rc++] = ((char) (uchar >> 12)) | 0xE0;
		utf8string[rc++] = ((char) ((uchar >> 6) & 0x3F)) | 0x80;
		utf8string[rc++] = ((char) ((uchar >> 0) & 0x3F)) | 0x80;
	}
	else if (uchar < 0x00200000)
	{
		/* unicode char 0x00010000 - 0x001FFFFF */
		if (count < 4)
			return -1;
		utf8string[rc++] = ((char) (uchar >> 18)) | 0xF0;
		utf8string[rc++] = ((char) ((uchar >> 12) & 0x3F)) | 0x80;
		utf8string[rc++] = ((char) ((uchar >> 6) & 0x3F)) | 0x80;
		utf8string[rc++] = ((char) ((uchar >> 0) & 0x3F)) | 0x80;
	}
	else if (uchar < 0x04000000)
	{
		/* unicode char 0x00200000 - 0x03FFFFFF */
		if (count < 5)
			return -1;
		utf8string[rc++] = ((char) (uchar >> 24)) | 0xF8;
		utf8string[rc++] = ((char) ((uchar >> 18) & 0x3F)) | 0x80;
		utf8string[rc++] = ((char) ((uchar >> 12) & 0x3F)) | 0x80;
		utf8string[rc++] = ((char) ((uchar >> 6) & 0x3F)) | 0x80;
		utf8string[rc++] = ((char) ((uchar >> 0) & 0x3F)) | 0x80;
	}
	else if (uchar < 0x80000000)
	{
		/* unicode char 0x04000000 - 0x7FFFFFFF */
		if (count < 6)
			return -1;
		utf8string[rc++] = ((char) (uchar >> 30)) | 0xFC;
		utf8string[rc++] = ((char) ((uchar >> 24) & 0x3F)) | 0x80;
		utf8string[rc++] = ((char) ((uchar >> 18) & 0x3F)) | 0x80;
		utf8string[rc++] = ((char) ((uchar >> 12) & 0x3F)) | 0x80;
		utf8string[rc++] = ((char) ((uchar >> 6) & 0x3F)) | 0x80;
		utf8string[rc++] = ((char) ((uchar >> 0) & 0x3F)) | 0x80;
	}
	else
		rc = -1;

	return rc;
}

/**
 * @brief convert an unicode character into a UTF-16 sequence
 * @param utf16string pointer to the result array of utf16_char
 * @param count number of characters that can be written to utf16string
 * @param uchar unciode_char value to convert
 * @return -1 on error, or the number of utf16_char written on success (1 or 2)
 */
int utf16_from_uchar(utf16_char *utf16string, size_t count, unicode_char uchar)
{
	int rc;

	/* error on invalid characters */
	if (!uchar_isvalid(uchar))
		return -1;

	/* single word case */
	if (uchar < 0x10000)
	{
		if (count < 1)
			return -1;
		utf16string[0] = (utf16_char) uchar;
		rc = 1;
	}

	/* double word case */
	else if (uchar < 0x100000)
	{
		if (count < 2)
			return -1;
		utf16string[0] = ((uchar >> 10) & 0x03ff) | 0xd800;
		utf16string[1] = ((uchar >>  0) & 0x03ff) | 0xdc00;
		rc = 2;
	}
	else
		return -1;
	return rc;
}

/**
 * @brief convert an unicode character into a UTF-16 sequence with flipped endianness
 * @param utf16string pointer to the result array of utf16_char
 * @param count number of characters that can be written to utf16string
 * @param uchar unciode_char value to convert
 * @return -1 on error, or the number of utf16_char written on success (1 or 2)
 */
int utf16f_from_uchar(utf16_char *utf16string, size_t count, unicode_char uchar)
{
	int rc;
	utf16_char buf[2] = { 0, 0 };

	rc = utf16_from_uchar(buf, 2, uchar);

	if (rc >= 1 && count >= 1)
		utf16string[0] = FLIPENDIAN_INT16(buf[0]);
	if (rc >= 2 && count >= 2)
		utf16string[1] = FLIPENDIAN_INT16(buf[1]);
	return rc < count ? rc : count;
}

/**
 * @brief return the number of decoded Unicode values in UTF-8 encoded string
 * @param src pointer to the array of UTF-8 encoded characters
 * @param plen optional pointer to a size_t variable to receive the source string length
 * @return number of unicode_char values decoded from the UTF-8 string
 */
size_t utf8_width(const char* utf8src, size_t * plen)
{
	size_t len = 0;
	size_t total = 0;
	while (*utf8src) {
		unsigned char c = (unsigned char) *utf8src;
		size_t auxlen;

		/* determine how many additional bytes we need */
		if (c < 0x80)
		{
			/* unicode char 0x00000000 - 0x0000007F */
			auxlen = 0;
		}
		else if (c >= 0xc0 && c < 0xe0)
		{
			/* unicode char 0x00000080 - 0x000007FF */
			if (0 == utf8src[1])
				return -1;
			auxlen = 1;
		}
		else if (c >= 0xe0 && c < 0xf0)
		{
			/* unicode char 0x00000800 - 0x0000FFFF */
			if (0 == utf8src[1] || 0 == utf8src[2])
				return -1;
			auxlen = 2;
		}
		else if (c >= 0xf0 && c < 0xf8)
		{
			/* unicode char 0x00010000 - 0x001FFFFF */
			if (0 == utf8src[1] || 0 == utf8src[2] || 0 == utf8src[3])
				return -1;
			auxlen = 3;
		}
		else if (c >= 0xf8 && c < 0xfc)
		{
			/* unicode char 0x00200000 - 0x03FFFFFF */
			if (0 == utf8src[1] || 0 == utf8src[2] || 0 == utf8src[3] || 0 == utf8src[4])
				return -1;
			auxlen = 4;
		}
		else if (c >= 0xfc && c < 0xfe)
		{
			/* unicode char 0x04000000 - 0x7FFFFFFF */
			if (0 == utf8src[1] || 0 == utf8src[2] || 0 == utf8src[3] || 0 == utf8src[4] || 0 == utf8src[5])
				return -1;
			auxlen = 5;
		}
		else
		{
			/* invalid */
			return -1;
		}
		total++;
		len += auxlen + 1;
		utf8src += auxlen + 1;
	}
	if (plen)
		*plen = len;
	return total;
}

/**
 * @brief return the number of decoded Unicode values in UTF-16 encoded string
 * @param src pointer to the array of UTF-16 encoded characters
 * @param plen optional pointer to a size_t variable to receive the source string length
 * @return number of unicode_char values decoded from the UTF-8 string
 */
size_t utf16_width(const utf16_char* utf16src, size_t * plen)
{
	size_t len = 0;
	size_t total = 0;
	while (*utf16src) {
		utf16_char c = (utf16_char) *utf16src;
		size_t auxlen;

		if (c >= 0xd800 && c <= 0xdbff)
		{
			if (0 == utf16src[1])
				return -1;
			auxlen = 1;
		}
		else if (utf16src[0] < 0xdc00 || utf16src[0] > 0xdfff)
		{
			auxlen = 0;
		}
		else
			return -1;
		total++;
		len += auxlen + 1;
		utf16src += auxlen + 1;
	}
	if (plen)
		*plen = len;
	return total;
}

/**
 * @brief return a pointer to the previous character in a string
 * @param utf8string const pointer to the starting position in the string
 * @return pointer to the character which is not an UTF-8 auxiliary character
 */
const char *utf8_previous_char(const char *utf8string)
{
	while ((*--utf8string & 0xc0) == 0x80)
		;
	return utf8string;
}

/**
 * @brief return true if the given string is a properly formed sequence of UTF-8 characters
 * @param utf8string const pointer to the source string
 * @return TRUE if the string is valid, FALSE otherwise
 */
int utf8_is_valid_string(const char *utf8string)
{
	int remaining_length = strlen(utf8string);

	while (*utf8string != 0)
	{
		unicode_char uchar = 0;
		int charlen;

		/* extract the current character and verify it */
		charlen = uchar_from_utf8(&uchar, utf8string, remaining_length);
		if (charlen <= 0 || uchar == 0 || !uchar_isvalid(uchar))
			return FALSE;

		/* advance */
		utf8string += charlen;
		remaining_length -= charlen;
	}

	return TRUE;
}

/**
 * @brief load a lookup table 8 bit codes to Unicode values
 *
 * This opens and reads a file %name which has to be in the
 * unicode.org defined "Format A".
 * That is three columns
 *   column 1: hex encoded 8 bit value of the code
 *   column 2: hex encoded 32 bit (max) unicode value
 *   column 3: a hash (#) and optional comment until the end-of-line
 *
 * @param name name of the (text) file to parse
 * @return pointer to a newly allocated array of 256 unicode_char values
 */
unicode_char * uchar_table_load(const char* name)
{
	FILE* file = fopen(name, "r");
	if (NULL == file)
		return NULL;
	unicode_char* table = (unicode_char*) calloc(sizeof(unicode_char), 256);
	if (NULL == table)
	{
		fclose(file);
		return NULL;
	}
	char *line = (char *)calloc(sizeof(char), 1024);

	while (fgets(line, 1024, file))
	{
		char* src = line;
		while (*src && isspace(*src))
			src++;
		if (*src == '#')
			continue;			// skip comment lines
		// expect Unicode "Table format: Format A"
		UINT32 uchar;
		UINT32 ucode;
		int n = sscanf(line, "%u %u #", &uchar, &ucode);
		if (n != 2)
			continue;			// silently skip of non-conforming lines
		if (uchar > 255)
			continue;			// can't define codes greater than 255
		table[uchar] = ucode;
	}
	free(line);
	return table;
}

/**
 * @brief return the 8 bit code that is mapped to the specified unicode_char
 * @param table table of 256 unicode_char values to use for the reverse lookup
 * @param uchar unicode value to revers lookup
 * @return UINT8 with the 8 bit code, or 255 if uchar wasn't found
 */
UINT8 uchar_table_index(unicode_char* table, unicode_char uchar)
{
	UINT8 index;
	for (index = 0; index < 254; index++)
		if (uchar == table[index])
			return index;
	return index;
}

/**
 * @brief free an unicode lookup table
 * @param table
 */
void uchar_table_free(unicode_char* table)
{
	if (table)
		free(table);
}

/**
 * @brief fill an unicode_char array with values decoded from UTF-8
 * @param ustr destination unicode_char string
 * @param size maximum size ucharstr array in unicode_char
 * @param utf8char source string encoded in UTF-8
 * @return number of unicode_char decoded excluding the final 0, or -1 on error
 */
size_t ustring_from_utf8(unicode_char* ustr, size_t size, const char *utf8src)
{
	size_t avail;
	size_t width = utf8_width(utf8src, &avail);
	if (-1 == width)
		return width;
	size_t length;
	for (length = 0; *utf8src && length < size - 1; length++)
	{
		int len = uchar_from_utf8(&ustr[length], utf8src, avail);
		if (len < 0)
			break;
		utf8src += len;
		avail -= len;
	}
	ustr[length] = 0;
	return length;
}

/**
 * @brief return an unicode_char array allocated while converted from UTF-16
 * @param ustr destination unicode_char string
 * @param size maximum size ucharstr array in unicode_char
 * @param utf16char source string encoded in UTF-16
 * @return number of unicode_char decoded excluding the final 0, or -1 on error
 */
size_t ustring_from_utf16(unicode_char* ustr, size_t size, const utf16_char *utf16src)
{
	size_t avail;
	size_t width = utf16_width(utf16src, &avail);
	if (-1 == width)
		return width;
	size_t length;
	for (length = 0; *utf16src && length < size - 1; length++)
	{
		int len = uchar_from_utf16(&ustr[length], utf16src, avail);
		if (len < 0)
			break;
		utf16src += len;
		avail -= len;
	}
	ustr[length] = 0;
	return length;
}

/**
 * @brief return the unicode_char array length
 * @param src pointer to an array of unicode_char
 * @return length of the array until the first 0
 */
size_t uchar_strlen(const unicode_char* src)
{
	int len = 0;
	while (*src++)
		len++;
	return len;
}

/**
 * @brief compare two unicode_char arrays
 * @param dst pointer to the first array of unicode_char
 * @param src pointer to the second array of unicode_char
 * @return 0 if dst == src, -1 if dst < src or +1 otherwise
 */
int uchar_strcmp(const unicode_char* dst, const unicode_char* src)
{
	while (*src && *dst && *src == *dst)
	{
		src++;
		dst++;
	}
	if (*src != *dst)
		return *src < *dst ? -1 : +1;
	return 0;
}

/**
 * @brief compare two unicode_char arrays with length limiting
 * @param dst pointer to the first array of unicode_char
 * @param src pointer to the second array of unicode_char
 * @param len maximum number of unicode_char to compare
 * @return 0 if dst == src, -1 if dst < src or +1 otherwise
 */
int uchar_strncmp(const unicode_char* dst, const unicode_char* src, size_t len)
{
	while (*src && *dst && *src == *dst && len > 0)
	{
		src++;
		dst++;
		len--;
	}
	if (*src != *dst)
		return *src < *dst ? -1 : +1;
	return 0;
}

/**
 * @brief print a formatted string of ASCII characters to an unicode_char array
 * @param dst pointer to the array
 * @param format format string followed by optional parameters
 * @return number of unicode_char stored in dst
 */
int uchar_sprintf(unicode_char* ustr, const char* format, ...)
{
	va_list ap;
	char buff[256];
	va_start(ap, format);
	int len = vsnprintf(buff, sizeof(buff), format, ap);
	va_end(ap);
	for (int i = 0; i < len; i++)
		*ustr++ = buff[i];
	*ustr = 0;
	return len;
}

/**
 * @brief print a formatted string of ASCII characters to an unicode_char array
 * @param dst pointer to the array
 * @param format format string followed by optional parameters
 * @param size maximum number of unicode_char to write to dst
 * @return number of unicode_char stored in dst
 */
int uchar_snprintf(unicode_char* ustr, size_t size, const char* format, ...)
{
	va_list ap;
	char buff[256];
	va_start(ap, format);
	int len = vsnprintf(buff, sizeof(buff), format, ap);
	va_end(ap);
	if (len < 0)
		return len;
	for (int i = 0; i < len; i++)
		*ustr++ = buff[i];
	*ustr = 0;
	return len;
}

/**
 * @brief copy an array of unicode_char from source to destination
 *
 * @param dst pointer to destination array
 * @param src const pointer to the source array
 * @return a pointer to the original destination
 */
unicode_char* uchar_strcpy(unicode_char* dst, const unicode_char* src)
{
	unicode_char* str = dst;
	while (*src)
		*dst++ = *src++;
	return str;
}

/**
 * @brief copy a length limited array of unicode_char from source to destination
 *
 * This function always terminates dst with a 0 unicode_char, unlike some
 * classic libc implementations of strncpy(). This means that actually at
 * most len-1 unicode_char are copied from src to leave room for the 0 code.
 *
 * @param dst pointer to destination array
 * @param src const pointer to the source array
 * @param len maximum number of unicode_char to copy
 * @return a pointer to the original destination
 */
unicode_char* uchar_strncpy(unicode_char* dst, const unicode_char* src, size_t len)
{
	unicode_char* str = dst;
	while (*src && len > 1)
	{
		*dst++ = *src++;
		len--;
	}
	if (len > 0)
		*dst = 0;
	return str;
}

/***************************************************************************
 *
 * Parsing and access to the UnicodeData table published at unicode.org
 *
 ***************************************************************************/

//! Information about an unicode_char
typedef struct {
#if	NEED_UNICODE_NAME
	char *name;									//!< name of the character
#endif
#if	NEED_UNICODE_NAME10
	char *name10;								//!< short name of the character
#endif
#if	NEED_UNICODE_GCAT
	unicode_general_category gen_cat;			//!< general category
#endif
#if	NEED_UNICODE_CCOM
	UINT8 canonical_comb;						//!< canonical combining is needed
#endif
#if	NEED_UNICODE_BIDI
	unicode_bidirectional_category bidi;		//!< bidirectional category
#endif
#if	NEED_UNICODE_DECO
	unicode_decomposition_mapping decomp_map;	//!< decomposition mapping
#endif
#if	NEED_UNICODE_DECIMAL
	UINT8 decimal_digit;						//!< decimal digit value
#endif
#if	NEED_UNICODE_DIGIT
	UINT8 digit;								//!< digit value
#endif
#if	NEED_UNICODE_NUMERIC
	UINT8 numeric;								//!< non zero, if this code is numeric
#endif
#if	NEED_UNICODE_MIRRORED
	bool mirrored;								//!< true, if the value is a mirrored form
#endif
#if	NEED_UNICODE_DECN
	UINT8 n_decomp;								//!< number of decomposition codes
	unicode_char *decomp_codes;					//!< array of decomposition codes
#endif
#if	NEED_UNICODE_UCASE
	unicode_char uppercase;						//!< upper case value of this code
#endif
#if	NEED_UNICODE_LCASE
	unicode_char lowercase;						//!< lower case value of this code
#endif
#if	NEED_UNICODE_TCASE
	unicode_char titlecase;						//!< title case value of this code
#endif
#if	NEED_UNICODE_WIDTH
	UINT8 width;								//!< width of the glyph in cells
#endif
}	unicode_data_t;

static unicode_data_t** unicode_data = NULL;

#if	NEED_UNICODE_NAME
const char * unicode_name(unicode_char uchar)
{
	if (!unicode_data || uchar >= UNICODE_PLANESIZE || !unicode_data[uchar])
		return NULL;
	return unicode_data[uchar]->name;
}
#endif

#if	NEED_UNICODE_NAME10
const char * unicode_name10(unicode_char uchar)
{
	if (!unicode_data || uchar >= UNICODE_PLANESIZE || !unicode_data[uchar])
		return NULL;
	return unicode_data[uchar]->name10;
}
#endif

#if	NEED_UNICODE_GCAT
unicode_general_category unicode_gcat(unicode_char uchar)
{
	if (!unicode_data || uchar >= UNICODE_PLANESIZE || !unicode_data[uchar])
		return gcat_0;
	return unicode_data[uchar]->gen_cat;
}

const char * unicode_gcat_name(unicode_char uchar)
{
	if (!unicode_data || uchar >= UNICODE_PLANESIZE || !unicode_data[uchar])
		return "";
	switch (unicode_data[uchar]->gen_cat)
	{
	case gcat_Lu:	return "Lu: Letter, Uppercase";
	case gcat_Ll:	return "Ll: Letter, Lowercase";
	case gcat_Lt:	return "Lt: Letter, Titlecase";
	case gcat_Mn:	return "Mn: Mark, Non-Spacing";
	case gcat_Mc:	return "Mc: Mark, Spacing Combining";
	case gcat_Me:	return "Me: Mark, Enclosing";
	case gcat_Nd:	return "Nd: Number, Decimal Digit";
	case gcat_Nl:	return "Nl: Number, Letter";
	case gcat_No:	return "No: Number, Other";
	case gcat_Zs:	return "Zs: Separator, Space";
	case gcat_Zl:	return "Zl: Separator, Line";
	case gcat_Zp:	return "Zp: Separator, Paragraph";
	case gcat_Cc:	return "Cc: Other, Control";
	case gcat_Cf:	return "Cf: Other, Format";
	case gcat_Cs:	return "Cs: Other, Surrogate";
	case gcat_Co:	return "Co: Other, Private Use";
	case gcat_Cn:	return "Cn: Other, Not Assigned (no characters have this property)";
	case gcat_Lm:	return "Lm: Letter, Modifier";
	case gcat_Lo:	return "Lo: Letter, Other";
	case gcat_Pc:	return "Pc: Punctuation, Connector";
	case gcat_Pd:	return "Pd: Punctuation, Dash";
	case gcat_Ps:	return "Ps: Punctuation, Open";
	case gcat_Pe:	return "Pe: Punctuation, Close";
	case gcat_Pi:	return "Pi: Punctuation, Initial quote (may behave like Ps or Pe depending on usage)";
	case gcat_Pf:	return "Pf: Punctuation, Final quote (may behave like Ps or Pe depending on usage)";
	case gcat_Po:	return "Po: Punctuation, Other";
	case gcat_Sm:	return "Sm: Symbol, Math";
	case gcat_Sc:	return "Sc: Symbol, Currency";
	case gcat_Sk:	return "Sk: Symbol, Modifier";
	case gcat_So:	return "So: Symbol, Other";
	default:
		return "";
	}
}
#endif

#if	NEED_UNICODE_CCOM
UINT8 unicode_ccom(unicode_char uchar)
{
	if (!unicode_data || uchar >= UNICODE_PLANESIZE || !unicode_data[uchar])
		return 255;
	return unicode_data[uchar]->canonical_comb;
}

const char * unicode_ccom_name(unicode_char uchar)
{
	if (!unicode_data || uchar >= UNICODE_PLANESIZE || !unicode_data[uchar])
		return "";
	switch (unicode_data[uchar]->canonical_comb)
	{
	case 0:		return "Spacing, split, enclosing, reordrant, and Tibetan subjoined";
	case 1:		return "Overlays and interior";
	case 7:		return "Nuktas";
	case 8:		return "Hiragana/Katakana voicing marks";
	case 9:		return "Viramas";
	case 10:	return "Start of fixed position classes";
	case 199:	return "End of fixed position classes";
	case 200:	return "Below left attached";
	case 202:	return "Below attached";
	case 204:	return "Below right attached";
	case 208:	return "Left attached (reordrant around single base character)";
	case 210:	return "Right attached";
	case 212:	return "Above left attached";
	case 214:	return "Above attached";
	case 216:	return "Above right attached";
	case 218:	return "Below left";
	case 220:	return "Below";
	case 222:	return "Below right";
	case 224:	return "Left (reordrant around single base character)";
	case 226:	return "Right";
	case 228:	return "Above left";
	case 230:	return "Above";
	case 232:	return "Above right";
	case 233:	return "Double below";
	case 234:	return "Double above";
	case 240:	return "Below (iota subscript)";
	}
	return "INVALID";
}
#endif

#if	NEED_UNICODE_BIDI
unicode_bidirectional_category unicode_bidi(unicode_char uchar)
{
	if (!unicode_data || uchar >= UNICODE_PLANESIZE || !unicode_data[uchar])
		return bidi_0;
	return unicode_data[uchar]->bidi;
}

const char * unicode_bidi_name(unicode_char uchar)
{
	if (!unicode_data || uchar >= UNICODE_PLANESIZE || !unicode_data[uchar])
		return "";
	switch (unicode_data[uchar]->bidi)
	{
	case bidi_L:	return "L: Left-to-Right";
	case bidi_LRE:	return "LRE: Left-to-Right Embedding";
	case bidi_LRO:	return "LRO: Left-to-Right Override";
	case bidi_R:	return "R: Right-to-Left";
	case bidi_AL:	return "AL: Right-to-Left Arabic";
	case bidi_RLE:	return "RLE: Right-to-Left Embedding";
	case bidi_RLO:	return "RLO: Right-to-Left Override";
	case bidi_PDF:	return "PDF: Pop Directional Format";
	case bidi_EN:	return "EN: European Number";
	case bidi_ES:	return "ES: European Number Separator";
	case bidi_ET:	return "ET: European Number Terminator";
	case bidi_AN:	return "AN: Arabic Number";
	case bidi_CS:	return "CS: Common Number Separator";
	case bidi_NSM:	return "NSM: Non-Spacing Mark";
	case bidi_BN:	return "BN: Boundary Neutral";
	case bidi_B:	return "B: Paragraph Separator";
	case bidi_S:	return "S: Segment Separator";
	case bidi_WS:	return "WS: Whitespace";
	case bidi_ON:	return "ON: Other Neutrals";
	default:
		return "";
	}
}
#endif

#if	NEED_UNICODE_DECO
unicode_decomposition_mapping unicode_deco(unicode_char uchar)
{
	if (!unicode_data || uchar >= UNICODE_PLANESIZE || !unicode_data[uchar])
		return deco_0;
	return unicode_data[uchar]->decomp_map;
}

const char * unicode_deco_name(unicode_char uchar)
{
	if (!unicode_data || uchar >= UNICODE_PLANESIZE || !unicode_data[uchar])
		return "";
	switch (unicode_data[uchar]->decomp_map)
	{
	case deco_canonical:	return "Canonical mapping";
	case deco_font:			return "A font variant (e.g. a blackletter form)";
	case deco_noBreak:		return "A no-break version of a space or hyphen";
	case deco_initial:		return "An initial presentation form (Arabic)";
	case deco_medial:		return "A medial presentation form (Arabic)";
	case deco_final:		return "A final presentation form (Arabic)";
	case deco_isolated:		return "An isolated presentation form (Arabic)";
	case deco_circle:		return "An encircled form";
	case deco_super:		return "A superscript form";
	case deco_sub:			return "A subscript form";
	case deco_vertical:		return "A vertical layout presentation form";
	case deco_wide:			return "A wide (or zenkaku) compatibility character";
	case deco_narrow:		return "A narrow (or hankaku) compatibility character";
	case deco_small:		return "A small variant form (CNS compatibility)";
	case deco_square:		return "A CJK squared font variant";
	case deco_fraction:		return "A vulgar fraction form";
	case deco_compat:		return "Otherwise unspecified compatibility character";
	default:
		return "";
	}
}

#if	NEED_UNICODE_DECN
unicode_char unicode_deco_n(unicode_char uchar, int n)
{
	if (!unicode_data || uchar >= UNICODE_PLANESIZE || !unicode_data[uchar])
		return 0;
	if (n == -1)
		return unicode_data[uchar]->n_decomp;
	if (n < unicode_data[uchar]->n_decomp)
		return unicode_data[uchar]->decomp_codes[n];
	return 0;
}
#endif
#endif

#if	NEED_UNICODE_DIGIT
UINT8 unicode_decimal(unicode_char uchar)
{
	if (!unicode_data || uchar >= UNICODE_PLANESIZE || !unicode_data[uchar])
		return UNICODE_NOT_DECIMAL;
	return unicode_data[uchar]->decimal_digit;
}

UINT8 unicode_digit(unicode_char uchar)
{
	if (!unicode_data || uchar >= UNICODE_PLANESIZE || !unicode_data[uchar])
		return UNICODE_NOT_DIGIT;
	return unicode_data[uchar]->digit;
}
#endif

#if	NEED_UNICODE_NUMERIC
UINT8 unicode_numeric(unicode_char uchar)
{
	if (!unicode_data || uchar >= UNICODE_PLANESIZE || !unicode_data[uchar])
		return UNICODE_NOT_NUMERIC;
	return unicode_data[uchar]->numeric;
}
#endif

#if	NEED_UNICODE_MIRRORED
bool unicode_mirrored(unicode_char uchar)
{
	if (!unicode_data || uchar >= UNICODE_PLANESIZE || !unicode_data[uchar])
		return 0;
	return unicode_data[uchar]->mirrored;
}
#endif

#if	NEED_UNICODE_UCASE
unicode_char unicode_ucase(unicode_char uchar)
{
	if (!unicode_data || uchar >= UNICODE_PLANESIZE || !unicode_data[uchar])
		return uchar;
	if (unicode_data[uchar]->uppercase)
		return unicode_data[uchar]->uppercase;
	return uchar;
}
#endif

#if	NEED_UNICODE_LCASE
unicode_char unicode_lcase(unicode_char uchar)
{
	if (!unicode_data || uchar >= UNICODE_PLANESIZE || !unicode_data[uchar])
		return uchar;
	if (unicode_data[uchar]->lowercase)
		return unicode_data[uchar]->lowercase;
	return uchar;
}
#endif

#if	NEED_UNICODE_TCASE
unicode_char unicode_tcase(unicode_char uchar)
{
	if (!unicode_data || uchar >= UNICODE_PLANESIZE || !unicode_data[uchar])
		return uchar;
	if (unicode_data[uchar]->titlecase)
		return unicode_data[uchar]->titlecase;
	return uchar;
}
#endif

#if	NEED_UNICODE_WIDTH
int unicode_width(unicode_char uchar)
{
	/* sorted list of non-overlapping intervals of non-spacing characters */
	static const struct interval {
		unicode_char first, last;
	} combining[] = {
		{ 0x0300, 0x034E }, { 0x0360, 0x0362 }, { 0x0483, 0x0486 },
		{ 0x0488, 0x0489 }, { 0x0591, 0x05A1 }, { 0x05A3, 0x05B9 },
		{ 0x05BB, 0x05BD }, { 0x05BF, 0x05BF }, { 0x05C1, 0x05C2 },
		{ 0x05C4, 0x05C4 }, { 0x064B, 0x0655 }, { 0x0670, 0x0670 },
		{ 0x06D6, 0x06E4 }, { 0x06E7, 0x06E8 }, { 0x06EA, 0x06ED },
		{ 0x0711, 0x0711 }, { 0x0730, 0x074A }, { 0x07A6, 0x07B0 },
		{ 0x0901, 0x0902 }, { 0x093C, 0x093C }, { 0x0941, 0x0948 },
		{ 0x094D, 0x094D }, { 0x0951, 0x0954 }, { 0x0962, 0x0963 },
		{ 0x0981, 0x0981 }, { 0x09BC, 0x09BC }, { 0x09C1, 0x09C4 },
		{ 0x09CD, 0x09CD }, { 0x09E2, 0x09E3 }, { 0x0A02, 0x0A02 },
		{ 0x0A3C, 0x0A3C }, { 0x0A41, 0x0A42 }, { 0x0A47, 0x0A48 },
		{ 0x0A4B, 0x0A4D }, { 0x0A70, 0x0A71 }, { 0x0A81, 0x0A82 },
		{ 0x0ABC, 0x0ABC }, { 0x0AC1, 0x0AC5 }, { 0x0AC7, 0x0AC8 },
		{ 0x0ACD, 0x0ACD }, { 0x0B01, 0x0B01 }, { 0x0B3C, 0x0B3C },
		{ 0x0B3F, 0x0B3F }, { 0x0B41, 0x0B43 }, { 0x0B4D, 0x0B4D },
		{ 0x0B56, 0x0B56 }, { 0x0B82, 0x0B82 }, { 0x0BC0, 0x0BC0 },
		{ 0x0BCD, 0x0BCD }, { 0x0C3E, 0x0C40 }, { 0x0C46, 0x0C48 },
		{ 0x0C4A, 0x0C4D }, { 0x0C55, 0x0C56 }, { 0x0CBF, 0x0CBF },
		{ 0x0CC6, 0x0CC6 }, { 0x0CCC, 0x0CCD }, { 0x0D41, 0x0D43 },
		{ 0x0D4D, 0x0D4D }, { 0x0DCA, 0x0DCA }, { 0x0DD2, 0x0DD4 },
		{ 0x0DD6, 0x0DD6 }, { 0x0E31, 0x0E31 }, { 0x0E34, 0x0E3A },
		{ 0x0E47, 0x0E4E }, { 0x0EB1, 0x0EB1 }, { 0x0EB4, 0x0EB9 },
		{ 0x0EBB, 0x0EBC }, { 0x0EC8, 0x0ECD }, { 0x0F18, 0x0F19 },
		{ 0x0F35, 0x0F35 }, { 0x0F37, 0x0F37 }, { 0x0F39, 0x0F39 },
		{ 0x0F71, 0x0F7E }, { 0x0F80, 0x0F84 }, { 0x0F86, 0x0F87 },
		{ 0x0F90, 0x0F97 }, { 0x0F99, 0x0FBC }, { 0x0FC6, 0x0FC6 },
		{ 0x102D, 0x1030 }, { 0x1032, 0x1032 }, { 0x1036, 0x1037 },
		{ 0x1039, 0x1039 }, { 0x1058, 0x1059 }, { 0x17B7, 0x17BD },
		{ 0x17C6, 0x17C6 }, { 0x17C9, 0x17D3 }, { 0x18A9, 0x18A9 },
		{ 0x20D0, 0x20E3 }, { 0x302A, 0x302F }, { 0x3099, 0x309A },
		{ 0xFB1E, 0xFB1E }, { 0xFE20, 0xFE23 }
	};
	int _min = 0;
	int _max = sizeof(combining) / sizeof(combining[0]) - 1;
	int _mid;

	/* test for 8-bit control characters */
	if (uchar == 0)
		return 0;
	if (uchar < 32 || (uchar >= 0x7f && uchar < 0xa0))
		return -1;

	/* first quick check for Latin-1 etc. characters */
	if (uchar < combining[0].first)
		return 1;

	/* binary search in table of non-spacing characters */
	while (_max >= _min)
	{
		_mid = (_min + _max) / 2;
		if (combining[_mid].last < uchar)
			_min = _mid + 1;
		else if (combining[_mid].first > uchar)
			_max = _mid - 1;
		else if (combining[_mid].first <= uchar && combining[_mid].last >= uchar)
			return 0;
	}

	/* if we arrive here, uchar is not a combining or C0/C1 control character */

	/* fast test for majority of non-wide scripts */
	if (uchar < 0x1100)
		return 1;

	return 1 +
		((uchar >= 0x1100 && uchar <= 0x115f) || /* Hangul Jamo */
		 (uchar >= 0x2e80 && uchar <= 0xa4cf && (uchar & ~0x0011) != 0x300a && uchar != 0x303f) || /* CJK ... Yi */
		 (uchar >= 0xac00 && uchar <= 0xd7a3) || /* Hangul Syllables */
		 (uchar >= 0xf900 && uchar <= 0xfaff) || /* CJK Compatibility Ideographs */
		 (uchar >= 0xfe30 && uchar <= 0xfe6f) || /* CJK Compatibility Forms */
		 (uchar >= 0xff00 && uchar <= 0xff5f) || /* Fullwidth Forms */
		 (uchar >= 0xffe0 && uchar <= 0xffe6));
}
#endif

#if	NEED_UNICODE_RANGES
typedef struct {
	unicode_char first, last;
	const char *name;
}	unicode_range_t;

static const unicode_range_t unicode_ranges[] =
{
	{  0x0000,  0x007F, "Basic Latin"},
	{  0x0080,  0x00FF, "Latin-1 Supplement"},
	{  0x0100,  0x017F, "Latin Extended-A"},
	{  0x0180,  0x024F, "Latin Extended-B"},
	{  0x0250,  0x02AF, "IPA Extensions"},
	{  0x02B0,  0x02FF, "Spacing Modifier Letters"},
	{  0x0300,  0x036F, "Combining Diacritical Marks"},
	{  0x0370,  0x03FF, "Greek and Coptic"},
	{  0x0400,  0x04FF, "Cyrillic"},
	{  0x0500,  0x052F, "Cyrillic Supplement"},
	{  0x0530,  0x058F, "Armenian"},
	{  0x0590,  0x05FF, "Hebrew"},
	{  0x0600,  0x06FF, "Arabic"},
	{  0x0700,  0x074F, "Syriac"},
	{  0x0750,  0x077F, "Arabic Supplement"},
	{  0x0780,  0x07BF, "Thaana"},
	{  0x07C0,  0x07FF, "NKo"},
	{  0x0800,  0x083F, "Samaritan"},
	{  0x0840,  0x085F, "Mandaic"},
	{  0x08A0,  0x08FF, "Arabic Extended-A"},
	{  0x0900,  0x097F, "Devanagari"},
	{  0x0980,  0x09FF, "Bengali"},
	{  0x0A00,  0x0A7F, "Gurmukhi"},
	{  0x0A80,  0x0AFF, "Gujarati"},
	{  0x0B00,  0x0B7F, "Oriya"},
	{  0x0B80,  0x0BFF, "Tamil"},
	{  0x0C00,  0x0C7F, "Telugu"},
	{  0x0C80,  0x0CFF, "Kannada"},
	{  0x0D00,  0x0D7F, "Malayalam"},
	{  0x0D80,  0x0DFF, "Sinhala"},
	{  0x0E00,  0x0E7F, "Thai"},
	{  0x0E80,  0x0EFF, "Lao"},
	{  0x0F00,  0x0FFF, "Tibetan"},
	{  0x1000,  0x109F, "Myanmar"},
	{  0x10A0,  0x10FF, "Georgian"},
	{  0x1100,  0x11FF, "Hangul Jamo"},
	{  0x1200,  0x137F, "Ethiopic"},
	{  0x1380,  0x139F, "Ethiopic Supplement"},
	{  0x13A0,  0x13FF, "Cherokee"},
	{  0x1400,  0x167F, "Unified Canadian Aboriginal Syllabics"},
	{  0x1680,  0x169F, "Ogham"},
	{  0x16A0,  0x16FF, "Runic"},
	{  0x1700,  0x171F, "Tagalog"},
	{  0x1720,  0x173F, "Hanunoo"},
	{  0x1740,  0x175F, "Buhid"},
	{  0x1760,  0x177F, "Tagbanwa"},
	{  0x1780,  0x17FF, "Khmer"},
	{  0x1800,  0x18AF, "Mongolian"},
	{  0x18B0,  0x18FF, "Unified Canadian Aboriginal Syllabics Extended"},
	{  0x1900,  0x194F, "Limbu"},
	{  0x1950,  0x197F, "Tai Le"},
	{  0x1980,  0x19DF, "New Tai Lue"},
	{  0x19E0,  0x19FF, "Khmer Symbols"},
	{  0x1A00,  0x1A1F, "Buginese"},
	{  0x1A20,  0x1AAF, "Tai Tham"},
	{  0x1B00,  0x1B7F, "Balinese"},
	{  0x1B80,  0x1BBF, "Sundanese"},
	{  0x1BC0,  0x1BFF, "Batak"},
	{  0x1C00,  0x1C4F, "Lepcha"},
	{  0x1C50,  0x1C7F, "Ol Chiki"},
	{  0x1CC0,  0x1CCF, "Sundanese Supplement"},
	{  0x1CD0,  0x1CFF, "Vedic Extensions"},
	{  0x1D00,  0x1D7F, "Phonetic Extensions"},
	{  0x1D80,  0x1DBF, "Phonetic Extensions Supplement"},
	{  0x1DC0,  0x1DFF, "Combining Diacritical Marks Supplement"},
	{  0x1E00,  0x1EFF, "Latin Extended Additional"},
	{  0x1F00,  0x1FFF, "Greek Extended"},
	{  0x2000,  0x206F, "General Punctuation"},
	{  0x2070,  0x209F, "Superscripts and Subscripts"},
	{  0x20A0,  0x20CF, "Currency Symbols"},
	{  0x20D0,  0x20FF, "Combining Diacritical Marks for Symbols"},
	{  0x2100,  0x214F, "Letterlike Symbols"},
	{  0x2150,  0x218F, "Number Forms"},
	{  0x2190,  0x21FF, "Arrows"},
	{  0x2200,  0x22FF, "Mathematical Operators"},
	{  0x2300,  0x23FF, "Miscellaneous Technical"},
	{  0x2400,  0x243F, "Control Pictures"},
	{  0x2440,  0x245F, "Optical Character Recognition"},
	{  0x2460,  0x24FF, "Enclosed Alphanumerics"},
	{  0x2500,  0x257F, "Box Drawing"},
	{  0x2580,  0x259F, "Block Elements"},
	{  0x25A0,  0x25FF, "Geometric Shapes"},
	{  0x2600,  0x26FF, "Miscellaneous Symbols"},
	{  0x2700,  0x27BF, "Dingbats"},
	{  0x27C0,  0x27EF, "Miscellaneous Mathematical Symbols-A"},
	{  0x27F0,  0x27FF, "Supplemental Arrows-A"},
	{  0x2800,  0x28FF, "Braille Patterns"},
	{  0x2900,  0x297F, "Supplemental Arrows-B"},
	{  0x2980,  0x29FF, "Miscellaneous Mathematical Symbols-B"},
	{  0x2A00,  0x2AFF, "Supplemental Mathematical Operators"},
	{  0x2B00,  0x2BFF, "Miscellaneous Symbols and Arrows"},
	{  0x2C00,  0x2C5F, "Glagolitic"},
	{  0x2C60,  0x2C7F, "Latin Extended-C"},
	{  0x2C80,  0x2CFF, "Coptic"},
	{  0x2D00,  0x2D2F, "Georgian Supplement"},
	{  0x2D30,  0x2D7F, "Tifinagh"},
	{  0x2D80,  0x2DDF, "Ethiopic Extended"},
	{  0x2DE0,  0x2DFF, "Cyrillic Extended-A"},
	{  0x2E00,  0x2E7F, "Supplemental Punctuation"},
	{  0x2E80,  0x2EFF, "CJK Radicals Supplement"},
	{  0x2F00,  0x2FDF, "Kangxi Radicals"},
	{  0x2FF0,  0x2FFF, "Ideographic Description Characters"},
	{  0x3000,  0x303F, "CJK Symbols and Punctuation"},
	{  0x3040,  0x309F, "Hiragana"},
	{  0x30A0,  0x30FF, "Katakana"},
	{  0x3100,  0x312F, "Bopomofo"},
	{  0x3130,  0x318F, "Hangul Compatibility Jamo"},
	{  0x3190,  0x319F, "Kanbun"},
	{  0x31A0,  0x31BF, "Bopomofo Extended"},
	{  0x31C0,  0x31EF, "CJK Strokes"},
	{  0x31F0,  0x31FF, "Katakana Phonetic Extensions"},
	{  0x3200,  0x32FF, "Enclosed CJK Letters and Months"},
	{  0x3300,  0x33FF, "CJK Compatibility"},
	{  0x3400,  0x4DBF, "CJK Unified Ideographs Extension A"},
	{  0x4DC0,  0x4DFF, "Yijing Hexagram Symbols"},
	{  0x4E00,  0x9FFF, "CJK Unified Ideographs"},
	{  0xA000,  0xA48F, "Yi Syllables"},
	{  0xA490,  0xA4CF, "Yi Radicals"},
	{  0xA4D0,  0xA4FF, "Lisu"},
	{  0xA500,  0xA63F, "Vai"},
	{  0xA640,  0xA69F, "Cyrillic Extended-B"},
	{  0xA6A0,  0xA6FF, "Bamum"},
	{  0xA700,  0xA71F, "Modifier Tone Letters"},
	{  0xA720,  0xA7FF, "Latin Extended-D"},
	{  0xA800,  0xA82F, "Syloti Nagri"},
	{  0xA830,  0xA83F, "Common Indic Number Forms"},
	{  0xA840,  0xA87F, "Phags-pa"},
	{  0xA880,  0xA8DF, "Saurashtra"},
	{  0xA8E0,  0xA8FF, "Devanagari Extended"},
	{  0xA900,  0xA92F, "Kayah Li"},
	{  0xA930,  0xA95F, "Rejang"},
	{  0xA960,  0xA97F, "Hangul Jamo Extended-A"},
	{  0xA980,  0xA9DF, "Javanese"},
	{  0xAA00,  0xAA5F, "Cham"},
	{  0xAA60,  0xAA7F, "Myanmar Extended-A"},
	{  0xAA80,  0xAADF, "Tai Viet"},
	{  0xAAE0,  0xAAFF, "Meetei Mayek Extensions"},
	{  0xAB00,  0xAB2F, "Ethiopic Extended-A"},
	{  0xABC0,  0xABFF, "Meetei Mayek"},
	{  0xAC00,  0xD7AF, "Hangul Syllables"},
	{  0xD7B0,  0xD7FF, "Hangul Jamo Extended-B"},
	{  0xD800,  0xDB7F, "High Surrogates"},
	{  0xDB80,  0xDBFF, "High Private Use Surrogates"},
	{  0xDC00,  0xDFFF, "Low Surrogates"},
	{  0xE000,  0xF8FF, "Private Use Area"},
	{  0xF900,  0xFAFF, "CJK Compatibility Ideographs"},
	{  0xFB00,  0xFB4F, "Alphabetic Presentation Forms"},
	{  0xFB50,  0xFDFF, "Arabic Presentation Forms-A"},
	{  0xFE00,  0xFE0F, "Variation Selectors"},
	{  0xFE10,  0xFE1F, "Vertical Forms"},
	{  0xFE20,  0xFE2F, "Combining Half Marks"},
	{  0xFE30,  0xFE4F, "CJK Compatibility Forms"},
	{  0xFE50,  0xFE6F, "Small Form Variants"},
	{  0xFE70,  0xFEFF, "Arabic Presentation Forms-B"},
	{  0xFF00,  0xFFEF, "Halfwidth and Fullwidth Forms"},
	{  0xFFF0,  0xFFFF, "Specials"},
	{ 0x10000, 0x1007F, "Linear B Syllabary"},
	{ 0x10080, 0x100FF, "Linear B Ideograms"},
	{ 0x10100, 0x1013F, "Aegean Numbers"},
	{ 0x10140, 0x1018F, "Ancient Greek Numbers"},
	{ 0x10190, 0x101CF, "Ancient Symbols"},
	{ 0x101D0, 0x101FF, "Phaistos Disc"},
	{ 0x10280, 0x1029F, "Lycian"},
	{ 0x102A0, 0x102DF, "Carian"},
	{ 0x10300, 0x1032F, "Old Italic"},
	{ 0x10330, 0x1034F, "Gothic"},
	{ 0x10380, 0x1039F, "Ugaritic"},
	{ 0x103A0, 0x103DF, "Old Persian"},
	{ 0x10400, 0x1044F, "Deseret"},
	{ 0x10450, 0x1047F, "Shavian"},
	{ 0x10480, 0x104AF, "Osmanya"},
	{ 0x10800, 0x1083F, "Cypriot Syllabary"},
	{ 0x10840, 0x1085F, "Imperial Aramaic"},
	{ 0x10900, 0x1091F, "Phoenician"},
	{ 0x10920, 0x1093F, "Lydian"},
	{ 0x10980, 0x1099F, "Meroitic Hieroglyphs"},
	{ 0x109A0, 0x109FF, "Meroitic Cursive"},
	{ 0x10A00, 0x10A5F, "Kharoshthi"},
	{ 0x10A60, 0x10A7F, "Old South Arabian"},
	{ 0x10B00, 0x10B3F, "Avestan"},
	{ 0x10B40, 0x10B5F, "Inscriptional Parthian"},
	{ 0x10B60, 0x10B7F, "Inscriptional Pahlavi"},
	{ 0x10C00, 0x10C4F, "Old Turkic"},
	{ 0x10E60, 0x10E7F, "Rumi Numeral Symbols"},
	{ 0x11000, 0x1107F, "Brahmi"},
	{ 0x11080, 0x110CF, "Kaithi"},
	{ 0x110D0, 0x110FF, "Sora Sompeng"},
	{ 0x11100, 0x1114F, "Chakma"},
	{ 0x11180, 0x111DF, "Sharada"},
	{ 0x11680, 0x116CF, "Takri"},
	{ 0x12000, 0x123FF, "Cuneiform"},
	{ 0x12400, 0x1247F, "Cuneiform Numbers and Punctuation"},
	{ 0x13000, 0x1342F, "Egyptian Hieroglyphs"},
	{ 0x16800, 0x16A3F, "Bamum Supplement"},
	{ 0x16F00, 0x16F9F, "Miao"},
	{ 0x1B000, 0x1B0FF, "Kana Supplement"},
	{ 0x1D000, 0x1D0FF, "Byzantine Musical Symbols"},
	{ 0x1D100, 0x1D1FF, "Musical Symbols"},
	{ 0x1D200, 0x1D24F, "Ancient Greek Musical Notation"},
	{ 0x1D300, 0x1D35F, "Tai Xuan Jing Symbols"},
	{ 0x1D360, 0x1D37F, "Counting Rod Numerals"},
	{ 0x1D400, 0x1D7FF, "Mathematical Alphanumeric Symbols"},
	{ 0x1EE00, 0x1EEFF, "Arabic Mathematical Alphabetic Symbols"},
	{ 0x1F000, 0x1F02F, "Mahjong Tiles"},
	{ 0x1F030, 0x1F09F, "Domino Tiles"},
	{ 0x1F0A0, 0x1F0FF, "Playing Cards"},
	{ 0x1F100, 0x1F1FF, "Enclosed Alphanumeric Supplement"},
	{ 0x1F200, 0x1F2FF, "Enclosed Ideographic Supplement"},
	{ 0x1F300, 0x1F5FF, "Miscellaneous Symbols And Pictographs"},
	{ 0x1F600, 0x1F64F, "Emoticons"},
	{ 0x1F680, 0x1F6FF, "Transport And Map Symbols"},
	{ 0x1F700, 0x1F77F, "Alchemical Symbols"},
	{ 0x20000, 0x2A6DF, "CJK Unified Ideographs Extension B"},
	{ 0x2A700, 0x2B73F, "CJK Unified Ideographs Extension C"},
	{ 0x2B740, 0x2B81F, "CJK Unified Ideographs Extension D"},
	{ 0x2F800, 0x2FA1F, "CJK Compatibility Ideographs Supplement"},
	{ 0xE0000, 0xE007F, "Tags"},
	{ 0xE0100, 0xE01EF, "Variation Selectors Supplement"},
	{ 0xF0000, 0xFFFFF, "Supplementary Private Use Area-A"},
	{0x100000,0x10FFFF, "Supplementary Private Use Area-B"}
};

/**
 * @brief return the name of the Unicode range which %uchar lies in
 * @param uchar Unicode value to search the range for
 * @return descriptive text for the range, NULL if not found
 */
const char * unicode_range_name(unicode_char uchar)
{
	int _min = 0;
	int _max = sizeof(unicode_ranges) / sizeof(unicode_ranges[0]) - 1;
	int _mid;

	/* binary search in table of unicode ranges */
	while (_max >= _min)
	{
		_mid = (_min + _max) / 2;
		if (unicode_ranges[_mid].last < uchar)
			_min = _mid + 1;
		else if (unicode_ranges[_mid].first > uchar)
			_max = _mid - 1;
		else if (unicode_ranges[_mid].first <= uchar && unicode_ranges[_mid].last >= uchar)
			return unicode_ranges[_mid].name;
	}
	return NULL;
}

/**
 * @brief return the first value of the Unicode range which %uchar lies in
 * @param uchar Unicode value to search the range for
 * @return range first code
 */
unicode_char unicode_range_first(unicode_char uchar)
{
	int _min = 0;
	int _max = sizeof(unicode_ranges) / sizeof(unicode_ranges[0]) - 1;
	int _mid;

	/* binary search in table of unicode ranges */
	while (_max >= _min)
	{
		_mid = (_min + _max) / 2;
		if (unicode_ranges[_mid].last < uchar)
			_min = _mid + 1;
		else if (unicode_ranges[_mid].first > uchar)
			_max = _mid - 1;
		else if (unicode_ranges[_mid].first <= uchar && unicode_ranges[_mid].last >= uchar)
			return unicode_ranges[_mid].first;
	}
	return uchar;
}

/**
 * @brief return the last value of the Unicode range which %uchar lies in
 * @param uchar Unicode value to search the range for
 * @return range last code
 */
unicode_char unicode_range_last(unicode_char uchar)
{
	int _min = 0;
	int _max = sizeof(unicode_ranges) / sizeof(unicode_ranges[0]) - 1;
	int _mid;

	/* binary search in table of unicode ranges */
	while (_max >= _min)
	{
		_mid = (_min + _max) / 2;
		if (unicode_ranges[_mid].last < uchar)
			_min = _mid + 1;
		else if (unicode_ranges[_mid].first > uchar)
			_max = _mid - 1;
		else if (unicode_ranges[_mid].first <= uchar && unicode_ranges[_mid].last >= uchar)
			return unicode_ranges[_mid].last;
	}
	return uchar;
}
#endif

//! parse a string just as strtok would do - local static tok
static char *parse_strtok(char *src, const char *delim)
{
	static char *token = NULL;
	char *start;

	if (src)
		token = src;
	start = token;
	while (token && *token)
	{
		const char *d = delim;
		while (*d)
		{
			if (*token == *d)
			{
				*token++ = '\0';
				return start;
			}
			d++;
		}
		token++;
	}
	return NULL;
}

//! load the UnicodeData-x.y.txt file and parse it
int unicode_data_load(const char* name)
{
	// already loaded?
	if (unicode_data)
		return -1;

	FILE* file = fopen(name, "r");
	if (NULL == file)
		return -1;

	unicode_data = (unicode_data_t **) calloc(sizeof(unicode_data_t*), UNICODE_PLANESIZE);
	char *line = (char*) calloc(sizeof(char), 1024);
	int linenum = 0;
	unicode_char first = 0;
	unicode_char last = 0;

	while (fgets(line, 1024, file))
	{
		unicode_data_t u;
		unicode_char code;
		int tokennum = 1;
		char* parse = strdup(line);
		char* token;
		memset(&u, 0, sizeof(u));

		++linenum;
		if (NULL == (token = parse_strtok(parse, ";\r\n")))
			fprintf(stderr, "%s: token #%d failed on line %d\n%s", __FUNCTION__, tokennum, linenum, line);
		code = UNICODE_PLANESIZE;
		if (NULL != token)
			code = strtoul(token, NULL, 16);

		tokennum++;
		if (NULL == (token = parse_strtok(NULL, ";\r\n")))
			fprintf(stderr, "%s: token #%d failed on line %d\n%s", __FUNCTION__, tokennum, linenum, line);
		if (NULL != token)
		{
			// check for a range description
			if (token[0] == '<')
			{
				// strip a trailing ", First>" string fragment
				if (0 == strcmp(token + strlen(token) - 8,", First>"))
				{
					strcpy(token, token + 1);
					token[strlen(token) - 8] = '\0';
					first = code;
				}
				// strip a trailing ", Last>" string fragment
				if (0 == strcmp(token + strlen(token) - 7,", Last>"))
				{
					strcpy(token, token + 1);
					token[strlen(token) - 7] = '\0';
					last = code;
				}
			}
#if	NEED_UNICODE_NAME
			// allocate a string for the name
			u.name = strdup(token);
#endif
		}

		tokennum++;
		if (NULL == (token = parse_strtok(NULL, ";\r\n")))
			fprintf(stderr, "%s: token #%d failed on line %d\n%s", __FUNCTION__, tokennum, linenum, line);
#if	NEED_UNICODE_GCAT
		// parse general category
		u.gen_cat = gcat_0;
		if (NULL != token)
		{
			if (0 == strcmp(token, "Lu"))
				u.gen_cat = gcat_Lu;
			if (0 == strcmp(token, "Ll"))
				u.gen_cat = gcat_Ll;
			if (0 == strcmp(token, "Lt"))
				u.gen_cat = gcat_Lt;
			if (0 == strcmp(token, "Mn"))
				u.gen_cat = gcat_Mn;
			if (0 == strcmp(token, "Mc"))
				u.gen_cat = gcat_Mc;
			if (0 == strcmp(token, "Me"))
				u.gen_cat = gcat_Me;
			if (0 == strcmp(token, "Nd"))
				u.gen_cat = gcat_Nd;
			if (0 == strcmp(token, "Nl"))
				u.gen_cat = gcat_Nl;
			if (0 == strcmp(token, "No"))
				u.gen_cat = gcat_No;
			if (0 == strcmp(token, "Zs"))
				u.gen_cat = gcat_Zs;
			if (0 == strcmp(token, "Zl"))
				u.gen_cat = gcat_Zl;
			if (0 == strcmp(token, "Zp"))
				u.gen_cat = gcat_Zp;
			if (0 == strcmp(token, "Cc"))
				u.gen_cat = gcat_Cc;
			if (0 == strcmp(token, "Cf"))
				u.gen_cat = gcat_Cf;
			if (0 == strcmp(token, "Cs"))
				u.gen_cat = gcat_Cs;
			if (0 == strcmp(token, "Co"))
				u.gen_cat = gcat_Co;
			if (0 == strcmp(token, "Cn"))
				u.gen_cat = gcat_Cn;
			if (0 == strcmp(token, "Lm"))
				u.gen_cat = gcat_Lm;
			if (0 == strcmp(token, "Lo"))
				u.gen_cat = gcat_Lo;
			if (0 == strcmp(token, "Pc"))
				u.gen_cat = gcat_Pc;
			if (0 == strcmp(token, "Pd"))
				u.gen_cat = gcat_Pd;
			if (0 == strcmp(token, "Ps"))
				u.gen_cat = gcat_Ps;
			if (0 == strcmp(token, "Pe"))
				u.gen_cat = gcat_Pe;
			if (0 == strcmp(token, "Pi"))
				u.gen_cat = gcat_Pi;
			if (0 == strcmp(token, "Pf"))
				u.gen_cat = gcat_Pf;
			if (0 == strcmp(token, "Po"))
				u.gen_cat = gcat_Po;
			if (0 == strcmp(token, "Sm"))
				u.gen_cat = gcat_Sm;
			if (0 == strcmp(token, "Sc"))
				u.gen_cat = gcat_Sc;
			if (0 == strcmp(token, "Sk"))
				u.gen_cat = gcat_Sk;
			if (0 == strcmp(token, "So"))
				u.gen_cat = gcat_So;
		}
#endif

		tokennum++;
		if (NULL == (token = parse_strtok(NULL, ";\r\n")))
			fprintf(stderr, "%s: token #%d failed on line %d\n%s", __FUNCTION__, tokennum, linenum, line);
#if	NEED_UNICODE_CCOM
		if (NULL != token)
			u.canonical_comb = (UINT8)strtoul(token, NULL, 10);
#endif

		tokennum++;
		if (NULL == (token = parse_strtok(NULL, ";\r\n")))
			fprintf(stderr, "%s: token #%d failed on line %d\n%s", __FUNCTION__, tokennum, linenum, line);
#if	NEED_UNICODE_BIDI
		// parse bidirectional category
		u.bidi = bidi_0;
		if (NULL != token)
		{
			if (0 == strcmp(token, "L"))
				u.bidi = bidi_L;	// Left-to-Right
			if (0 == strcmp(token, "LRE"))
				u.bidi = bidi_LRE;	// Left-to-Right Embedding
			if (0 == strcmp(token, "LRO"))
				u.bidi = bidi_LRO;	// Left-to-Right Override
			if (0 == strcmp(token, "R"))
				u.bidi = bidi_R;	// Right-to-Left
			if (0 == strcmp(token, "AL"))
				u.bidi = bidi_AL;	// Right-to-Left Arabic
			if (0 == strcmp(token, "RLE"))
				u.bidi = bidi_RLE;	// Right-to-Left Embedding
			if (0 == strcmp(token, "RLO"))
				u.bidi = bidi_RLO;	// Right-to-Left Override
			if (0 == strcmp(token, "PDF"))
				u.bidi = bidi_PDF;	// Pop Directional Format
			if (0 == strcmp(token, "EN"))
				u.bidi = bidi_EN;	// European Number
			if (0 == strcmp(token, "ES"))
				u.bidi = bidi_ES;	// European Number Separator
			if (0 == strcmp(token, "ET"))
				u.bidi = bidi_ET;	// European Number Terminator
			if (0 == strcmp(token, "AN"))
				u.bidi = bidi_AN;	// Arabic Number
			if (0 == strcmp(token, "CS"))
				u.bidi = bidi_CS;	// Common Number Separator
			if (0 == strcmp(token, "NSM"))
				u.bidi = bidi_NSM;	// Non-Spacing Mark
			if (0 == strcmp(token, "BN"))
				u.bidi = bidi_BN;	// Boundary Neutral
			if (0 == strcmp(token, "B"))
				u.bidi = bidi_B;	// Paragraph Separator
			if (0 == strcmp(token, "S"))
				u.bidi = bidi_S;	// Segment Separator
			if (0 == strcmp(token, "WS"))
				u.bidi = bidi_WS;	// Whitespace
			if (0 == strcmp(token, "ON"))
				u.bidi = bidi_ON;	// Other Neutrals
		}
#endif

		tokennum++;
		if (NULL == (token = parse_strtok(NULL, ";\r\n")))
			fprintf(stderr, "%s: token #%d failed on line %d\n%s", __FUNCTION__, tokennum, linenum, line);
#if	NEED_UNICODE_DECO
		// parse decomposition mapping
		if (NULL != token)
		{
			unicode_char decomposed[256];
			UINT8 n = 0;
			char *p = token;

			if (0 == strncmp(token, "<font>", 6))
				u.decomp_map = deco_font;
			if (0 == strncmp(token, "<noBreak>", 9))
				u.decomp_map = deco_noBreak;
			if (0 == strncmp(token, "<initial>", 9))
				u.decomp_map = deco_initial;
			if (0 == strncmp(token, "<medial>", 8))
				u.decomp_map = deco_medial;
			if (0 == strncmp(token, "<final>", 7))
				u.decomp_map = deco_final;
			if (0 == strncmp(token, "<isolated>", 10))
				u.decomp_map = deco_initial;
			if (0 == strncmp(token, "<circle>", 8))
				u.decomp_map = deco_circle;
			if (0 == strncmp(token, "<super>", 7))
				u.decomp_map = deco_super;
			if (0 == strncmp(token, "<sub>", 5))
				u.decomp_map = deco_sub;
			if (0 == strncmp(token, "<vertical>", 10))
				u.decomp_map = deco_vertical;
			if (0 == strncmp(token, "<wide>", 6))
				u.decomp_map = deco_wide;
			if (0 == strncmp(token, "<narrow>", 8))
				u.decomp_map = deco_narrow;
			if (0 == strncmp(token, "<small>", 7))
				u.decomp_map = deco_small;
			if (0 == strncmp(token, "<square>", 8))
				u.decomp_map = deco_square;
			if (0 == strncmp(token, "<fraction>", 10))
				u.decomp_map = deco_fraction;
			if (0 == strncmp(token, "<compat>", 8))
				u.decomp_map = deco_compat;
			// skip over whitespace, if a decomposition type was found
			if (u.decomp_map != deco_0)
				while (isspace(*p))
					p++;
			// parse decomposition codes
			while (*p)
			{
				// skip initial whitespace
				while (isspace(*p))
					p++;
				// convert next code
				decomposed[n++] = (unicode_char)strtoul(p, NULL, 16);
				// skip over hex digits
				while (isxdigit(*p))
					p++;
				// break if too many codes
				if (n >= 255)
					break;
			}
			if (n > 0)
			{
				u.n_decomp = n;
				u.decomp_codes = (unicode_char*)malloc(sizeof(unicode_char) * n);
				memcpy(u.decomp_codes, decomposed, sizeof(unicode_char) * n);
			}
		}
#endif

		tokennum++;
		if (NULL == (token = parse_strtok(NULL, ";\r\n")))
			fprintf(stderr, "%s: token #%d failed on line %d\n%s", __FUNCTION__, tokennum, linenum, line);
#if	NEED_UNICODE_DECIMAL
		u.decimal_digit = UNICODE_NOT_DECIMAL;
		if (NULL != token && *token != '\0')
			u.decimal_digit = (UINT8)strtoul(token, NULL, 10);
#endif

		tokennum++;
		if (NULL == (token = parse_strtok(NULL, ";\r\n")))
			fprintf(stderr, "%s: token #%d failed on line %d\n%s", __FUNCTION__, tokennum, linenum, line);
#if	NEED_UNICODE_DIGIT
		u.digit = UNICODE_NOT_DIGIT;
		if (NULL != token && *token != '\0')
			u.digit = (UINT8)strtoul(token, NULL, 10);
#endif

		tokennum++;
		if (NULL == (token = parse_strtok(NULL, ";\r\n")))
			fprintf(stderr, "%s: token #%d failed on line %d\n%s", __FUNCTION__, tokennum, linenum, line);
#if	NEED_UNICODE_NUMERIC
		u.numeric = UNICODE_NOT_NUMERIC;
		if (NULL != token && *token)
			u.numeric = (UINT8)strtoul(token, NULL, 10);
#endif

		tokennum++;
		if (NULL == (token = parse_strtok(NULL, ";\r\n")))
			fprintf(stderr, "%s: token #%d failed on line %d\n%s", __FUNCTION__, tokennum, linenum, line);
#if	NEED_UNICODE_MIRRORED
		if (NULL != token && *token)
			u.mirrored = token[0] == 'Y';
#endif

		tokennum++;
		if (NULL == (token = parse_strtok(NULL, ";\r\n")))
			fprintf(stderr, "%s: token #%d failed on line %d\n%s", __FUNCTION__, tokennum, linenum, line);
#if	NEED_UNICODE_NAME10
		if (NULL != token && *token)
			u.name10 = strdup(token);
#endif

		tokennum++;
		if (NULL == (token = parse_strtok(NULL, ";\r\n")))
			fprintf(stderr, "%s: token #%d failed on line %d\n%s", __FUNCTION__, tokennum, linenum, line);
		if (NULL != token && *token)
		{
			/* FIXME: hmm ... don't know what this token means */
		}

		tokennum++;
		if (NULL == (token = parse_strtok(NULL, ";\r\n")))
			fprintf(stderr, "%s: token #%d failed on line %d\n%s", __FUNCTION__, tokennum, linenum, line);
#if	NEED_UNICODE_UCASE
		if (NULL != token && *token)
			u.uppercase = strtoul(token, NULL, 16);
#endif

		tokennum++;
		if (NULL == (token = parse_strtok(NULL, ";\r\n")))
			fprintf(stderr, "%s: token #%d failed on line %d\n%s", __FUNCTION__, tokennum, linenum, line);
#if	NEED_UNICODE_LCASE
		if (NULL != token && *token)
			u.lowercase = strtoul(token, NULL, 16);
#endif

		tokennum++;
		if (NULL == (token = parse_strtok(NULL, ";\r\n")))
			fprintf(stderr, "%s: token #%d failed on line %d\n%s", __FUNCTION__, tokennum, linenum, line);
#if	NEED_UNICODE_TCASE
		if (NULL != token && *token)
			u.titlecase = strtoul(token, NULL, 16);
#endif

		if (first > 0 && last > 0)
		{
			if (first + 1 >= UNICODE_PLANESIZE)
			{
				fprintf(stderr, "%s: range %#07x-%#07x outside planes\n", __FUNCTION__, first + 1, last);
			}
			else
			{
				for (code = first + 1; code <= last && code < UNICODE_PLANESIZE; code++)
					unicode_data[code] = unicode_data[first];
			}
			first = 0;
			last = 0;
			code = UNICODE_PLANESIZE;
		}
		if (code < UNICODE_PLANESIZE)
		{
			unicode_data[code] = (unicode_data_t *)malloc(sizeof(unicode_data_t));
			memcpy(unicode_data[code], &u, sizeof(u));
		}
		free(parse);
	}
	fclose(file);
	return 0;
}

void unicode_data_free()
{
	if (NULL == unicode_data)
		return;
	for (unicode_char uchar = 0; uchar < UNICODE_PLANESIZE; uchar++)
	{
		if (NULL == unicode_data[uchar])
			continue;		// undefined code
		unicode_data_t * uc = unicode_data[uchar];
		// find and zero dupes
		for (unicode_char dupes = uchar + 1; dupes  < UNICODE_PLANESIZE; dupes++)
			if (unicode_data[dupes] == uc)
				unicode_data[dupes] = NULL;
			else
				break;
#if	NEED_UNICODE_NAME
		if (uc->name)
			free(uc->name);
#endif
#if	NEED_UNICODE_NAME10
		if (uc->name10)
			free(uc->name10);
#endif
#if	NEED_UNICODE_DECN
		if (uc->decomp_codes)
			free(uc->decomp_codes);
#endif
		free(uc);
		unicode_data[uchar] = NULL;
	}
	free(unicode_data);
	unicode_data = NULL;
}
