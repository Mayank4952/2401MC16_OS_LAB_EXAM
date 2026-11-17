#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int global_var = 123;

int
main(void)
{
  printf("freepages at start-up: %d\n", freepages());

  sbrk(10 * 4096);
  printf("freepages after sbrk(10 * 4096): %d\n", freepages());

  uint64 va = (uint64)&global_var;
  uint64 pa = va2pa(va);
  printf("global variable VA: 0x%lx, PA: 0x%lx\n", va, pa);

  long bad_pa = (long)va2pa(0x7FFFFFFF);
  printf("va2pa(0x7FFFFFFF): %ld\n", bad_pa);

  int fp_before = freepages();
  int pid = fork();
  if (pid == 0) {
    uint64 c_pa = va2pa(va);
    printf("child global variable PA: 0x%lx\n", c_pa);
    exit(0);
  }
  uint64 p_pa = va2pa(va);
  printf("parent global variable PA: 0x%lx\n", p_pa);
  printf("freepages after fork: %d\n", freepages());
  wait(0);
  printf("freepages before fork: %d, after wait: %d\n", fp_before, freepages());

  exit(0);
}
