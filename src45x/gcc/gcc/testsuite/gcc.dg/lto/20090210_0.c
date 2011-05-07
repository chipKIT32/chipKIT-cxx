/* { dg-lto-do run }  */
/* { dg-suppress-ld-options {-fPIC} }  */
/* { dg-require-effective-target tls_runtime } */
int foo (int x)
{
  return x;
}
