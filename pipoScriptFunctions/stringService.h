#ifndef __STRING_SERVICE_H__
#define __STRING_SERVICE_H__

void string_service_init();

void string_service_fin();

/*! @function
  @abstract     Convert int to string with dinamically alocated memory
  @param  i     Number to convert
  @return       Pointer to the string
 */
char *itoa2(int i);


/*! @function
  @abstract     Creates a new string with the concatenation of s1 and s2.
  @param  s1    First string
  @param  s2    Second string
  @return       New dinamically allocated string.
 */
char *joinStrings(char *s1, char *s2);
#endif