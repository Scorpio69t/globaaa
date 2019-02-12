/*
 * Brown University - Department of Computer Science
 * CS033 - Introduction To Computer Systems - Fall 2015
 * Prof. Thomas W. Doeppner
 * Assignment 6 - Performance 2: A CS033 String Library - strings.c
 * Due date: 10/27/15
 */

/*      _        _
 *  ___| |_ _ __(_)_ __   __ _ ___
 * / __| __| '__| | '_ \ / _` / __|
 * \__ \ |_| |  | | | | | (_| \__ \
 * |___/\__|_|  |_|_| |_|\__, |___/
 *                       |___/
 */

/* XXX: Preprocessor instruction to enable basic macros; do not modify. */
#include <stddef.h>

/*
 * strlen() - abbreviated from "string length".
 *
 * - Description: determine the number of characters in a character string.
 *
 * - Arguments: a pointer to a null-terminated character string.
 *
 * - Return value: the number of characters using a special unsigned numerical
 *   type, which is intended specifically for pointer arithmetic, called size_t.
 *
 * Examples:
 * 1. >  strlen("") = 0
 * 2. >  strlen("ilovecs33") = 9
 *
 * - Hint: the length of a C string does _not_ include the null ('\0')
 *   character that terminates it.
 */
size_t strlen(const char *s) {
  int i = 0;

  // O(n); increment i as you search for null terminator
  while(s[i] != '\0')
    i++;

  return i;
}

/*
 * strspn() - abbreviated from "string span".
 *
 * - Description: determine the number of characters in a character string
 *   that consist of characters in accept
 *
 * - Arguments: a pointer to a null-terminated character string,
 *   and a pointer to a null terminated string of characters to accept
 *
 *
 * - Return value: the number of characters at the beginning of s that are in accept
 *
 * Examples:
 * 1. >  strspn("abcde", "ac") = 1
 * 2. >  strspn("123456", "ab") = 0
 * 3. >  strspn("hello", "hell") = 4
 */
size_t strspn(const char *s, const char *accept) {
  size_t counter = 0;
  int i, k;

  // O(ab) | a = length of s; b = length of accept
  for(i = 0; s[i] != '\0'; i++) {
    for(k = 0; accept[k] != '\0'; k++) {
      // if s[i] == accept[k], break and look at next char
      if(s[i] == accept[k]) {
	counter++;
        break;
      }
    }
    // if nothing matched accept chars, break
    if(!accept[k])
      break;
  }
  return counter;
}

/*
 * strcspn() - abbreviated from "string character span".
 *
 * - Description: determine the number of characters in a character string
 *   that consist of characters not in reject
 *
 * - Arguments: a pointer to a null-terminated character string,
 *   and a pointer to a null terminated string of characters to reject
 *
 * - Return value: the number of characters at the beginning of s that are not in reject
 *
 * Examples:
 * 1. >  strcspn("abcde", "de") = 3
 * 2. >  strcspn("123456", "12") = 0
 * 3. >  strcspn("hello", "xyz") = 5

 */
size_t strcspn(const char *s, const char *reject) {
  size_t counter = 0;
  int i, k;

  // O(ab) | a = length of s; b = length of accept
  for(i = 0; s[i] != '\0'; i++) {
    for(k = 0; reject[k] != '\0'; k++) {
      // very similar to above, but now if s[i] == reject[k] break
      if(s[i] == reject[k])
        break;
    }
    // count up chars in 's' that don't match any in 'reject'
    if(!reject[k])
      counter++;
    else
      break;
  }

  return counter;
}

/*
 * strncmp() - abbreviated from "string compare (n bytes)".
 *
 * - Description: compares the first n bytes of two strings, s1 and s2
 *
 * - Arguments: two null terminated strings (s1 and s2) and the maximum number of bytes
 *   to compare (n).
 *
 * - Return value: negative number if s1 < s2, 0 if s1 == s2, positive number if s1 > s2
 *
 * Examples:
 * 1. >  strncmp("abcde", "abdde", 3) = some negative number
 * 2. >  strncmp("abcde", "abdde", 2) = 0
 * 3. >  strncmp("1234567", "123", 3) = some positve number
 */
int strncmp(const char *s1, const char *s2, size_t n) {
  // O(n)
  for(int i = 0; i < n; i++) {
    if(s1[i] == s2[i])
      continue;
    else if(s1[i] < s2[i])
      return -1;
    else
      return 1;
  }
  return 0;
}

/*
 * strncpy() - abbreviated from "string copy (n bytes)".
 *
 * - Description: copies the first n bytes of the contents of the source string
 *   into the location of the destination string. As with strcpy(), we are
 *   assuming the destimation string is large enough to hold the entire
 *   contents of the source string. If the number of bytes to be copied is less
 *   than the number of bytes in the src string, the null character will
 *   not be copied. If the number of bytes to be copied is longer than the
 *   length of src, the difference will be padded with null characters appended
 *   onto the end of dest to fill out the requested in bytes.
 *
 * - Arguments: a pointer (char *) to the destination string, a pointer
 *   (char *) to the source string, and the number of bytes (size_t) to copy.
 *
 * - Return value: a pointer (char *) to the destination string.
 *
 * - Usage:
 *
 *      char abbreviation[5];
 *
 *      strncpy(abbreviation, "professor", 4);
 *
 *      (The variable abbreviation now contains the four bytes of "prof", but
 *      it is not null-terimated.)
 *
 *      char buffer[10];
 *
 *      strncpy(buffer, "brown", 10);
 *
 *      (The variable buffer now contains the five bytes of "brown" followed by
 *      five null characters.)
 *
 *      char good_fit[5];
 *
 *      strncpy(good_fit, "okay", 5);
 *
 *      (The variable good_fit now contains the four bytes of "okay" and is
 *      correctly null-terminated.)
 *
 * - Hint: make sure not to modify the source string. Please pay attention to the
 *   function description to see what is expected of your implementation.
 */
char *strncpy(char *dest, const char *src, size_t n) {
  int k;
  // O(n)
  for(k = 0; k < n && src[k] != '\0'; k++)
    dest[k] = src[k];

  // if extra space, fill with '\0'
  while(k != n) {
    dest[k] = '\0';
    k++;
  }

  return dest;
}

/*
 * strstr() - abbreviated from "string (find) string".
 *
 * - Description: Find the string "needle" in the string "haystack". This is
 *   not some joke from the TAs but the actual terminology used in the Linux
 *   version of strstr(). Don't believe us? Type "man strstr" into a terminal,
 *   and see for yourself! For another example of systems programming humor,
 *   you may also appreciate "man strfry".
 *
 * - Arguments: a pointer to a null-terminated character string to find (needle), and
 *   a pointer to anull-terminated character string in which to find the string (haystack)
 *
 * - Return value: a pointer to the beginning of the needle string within the haystack string
 *
 * Examples:
 * 1. >  strstr("abcde", "bcd"); should return pointer to string starting @ b
 * 2. >  strstr("123456", "4"); should return pointer to string starting @ 4
 * 3. >  strstr("hello", "xyz") = Null
 *
 * - Hint: Use strncmp().
 */
char *strstr(const char *haystack, const char *needle) {
  char* ptr = NULL;

  for(int i = 0; haystack[i] != '\0'; i++) {
    if (haystack[i] == needle[0]) {
      // set pointer to beginning of substring in haystack
      ptr = (char*) (haystack + i);

      // make sure complete needle is in haystack
      int k;
      for(k = 0; needle[k] != '\0'; k++) {
        if(needle[k] != haystack[i + k])
          break;
      }

      if(!needle[k])
        return ptr;
      else
        ptr = NULL;
    }
  }
  return ptr;
}

/*
 * strtok() - abbreviated from "string tokenize".
 *
 * - Description: break string str into a series of tokens using the delimiter delim.
 *
 * - Input: a pointer to a null-terminated character string to break up and a pointer to a
 *   null-terminated character string containing the delimeter
 *
 * - Return value: a pointer to the last token found in the string. A null pointer is returned if there are no tokens left to retrieve.
 *
 *
 * Examples:
 * 1. > char *str = "1,2,,,3,4";
 *    > char *s1 = strtok(str, ",");  // s1 should point to string starting @ 1
 *    > char *s2 = strtok(NULL, ","); // s2 should point to string starting @ 2
 *    > char *s3 = strtok(NULL, ","); // s2 should point to string starting @ 3

 * - Hint: This function must save its state between calls, how might you
 *   accomplish this? Note that this version of the function is destructive on
 *   the string str. Do not expect it to come out the other end in one piece...
 *   literally.
 */

// save state with global variable
char* strtok_saved_state = NULL;

char *strtok(char *str, const char *delim) {
  int dlen = strlen(delim);

  if(!str)
    str = strtok_saved_state;

  // skip over any characters in the delim
  int i, k;
  for(i = 0; str[i] != '\0'; i++) {
    for(k = 0; delim[k] != '\0'; k++) {
      if(str[i] == delim[k])
        break;
    }
    if(k == dlen) {
      str = str + i;
      break;
    }
  }

  // if str is made entirely of chars in delim
  if(!str[i])
    return NULL;

  // find end of string
  for(i = 0; str[i] != '\0'; i++) {
    for(k = 0; delim[k] != '\0'; k++) {
      if(str[i] == delim[k])
        break;
    }
    if(k != dlen) {
      str[i] = '\0';
      break;
    }
  }

  // update static variable and return str
  int slen = strlen(str);
  strtok_saved_state = str + slen + 1;
  if(!slen)
    return NULL;
  else
    return str;
}
