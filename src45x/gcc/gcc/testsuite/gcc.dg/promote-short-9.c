/* -fpromote-loop-indices used to ICE on this.  */

/* { dg-do compile } */
/* { dg-options "-O2 -fpromote-loop-indices" } */

char
lookup (char *haystack, char *needle)
{
  char x;

  for (x = haystack[-2]; x < *needle; x++)
    haystack[x] = needle[x];

  return 1;
}
