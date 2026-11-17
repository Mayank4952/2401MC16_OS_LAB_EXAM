#include "types.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "riscv.h"
#include "defs.h"

#define MAX_SEM 8

struct sem {
  int value;
  int inuse;
};

struct {
  struct spinlock lock;
  struct sem sems[MAX_SEM];
} semtable;

void
seminit(void)
{
  initlock(&semtable.lock, "semtable");
  for (int i = 0; i < MAX_SEM; i++) {
    semtable.sems[i].value = 0;
    semtable.sems[i].inuse = 0;
  }
}

int
sem_init(int id, int value)
{
  if (id < 0 || id >= MAX_SEM || value < 0)
    return -1;
  acquire(&semtable.lock);
  semtable.sems[id].value = value;
  semtable.sems[id].inuse = 1;
  release(&semtable.lock);
  return 0;
}

int
sem_wait(int id)
{
  if (id < 0 || id >= MAX_SEM)
    return -1;
  acquire(&semtable.lock);
  if (!semtable.sems[id].inuse) {
    release(&semtable.lock);
    return -1;
  }
  while (semtable.sems[id].value == 0) {
    sleep_prepare(&semtable.sems[id]);
    release(&semtable.lock);
    sleep();
    acquire(&semtable.lock);
    if (!semtable.sems[id].inuse) {
      release(&semtable.lock);
      return -1;
    }
  }
  semtable.sems[id].value--;
  release(&semtable.lock);
  return 0;
}

int
sem_post(int id)
{
  if (id < 0 || id >= MAX_SEM)
    return -1;
  acquire(&semtable.lock);
  if (!semtable.sems[id].inuse) {
    release(&semtable.lock);
    return -1;
  }
  semtable.sems[id].value++;
  wakeup(&semtable.sems[id]);
  release(&semtable.lock);
  return 0;
}
