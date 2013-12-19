
/* ICE splitting DImode compare */
/* { dg-do compile } */
/* { dg-options "-m4 -O2" } */

long long *lookup_extent_mapping (long long start, long long len);
long long *alloc_extent_map (void);
void free_extent_map (long long *);

void Foo (long long start, long long len, int testend)
{
  long long *em;
  long long *split;
  
  for (;;)
    {
      split = alloc_extent_map ();
      
      em = lookup_extent_mapping (start, len);
      
      if (*em < -4)
	*split = *em;
      
      if (*em < -4 && testend)
	*split = start + len;
      free_exent_map (split);
    }
}
