#include "multiboot.h"
#include "types.h"


#define call_INT __asm__ __volatile__ ("int $32");

/* Hardware text mode color constants. */
enum vga_color
{
  COLOR_BLACK = 0,
  COLOR_BLUE = 1,
  COLOR_GREEN = 2,
  COLOR_CYAN = 3,
  COLOR_RED = 4,
  COLOR_MAGENTA = 5,
  COLOR_BROWN = 6,
  COLOR_LIGHT_GREY = 7,
  COLOR_DARK_GREY = 8,
  COLOR_LIGHT_BLUE = 9,
  COLOR_LIGHT_GREEN = 10,
  COLOR_LIGHT_CYAN = 11,
  COLOR_LIGHT_RED = 12,
  COLOR_LIGHT_MAGENTA = 13,
  COLOR_LIGHT_BROWN = 14,
  COLOR_WHITE = 15,
};
 
uint8_t make_color(enum vga_color fg, enum vga_color bg)
{
  return fg | bg << 4;
}
 
uint16_t make_vgaentry(char c, uint8_t color)
{
  uint16_t c16 = c;
  uint16_t color16 = color;
  return c16 | color16 << 8;
}
 
size_t strlen(const char* str)
{
  size_t ret = 0;
  while ( str[ret] != 0 )
    ret++;
  return ret;
}
 
static const size_t VGA_WIDTH = 80;
static const size_t VGA_HEIGHT = 24;
 
size_t terminal_row;
size_t terminal_column;
uint8_t terminal_color;
uint16_t* terminal_buffer;
 
void terminal_initialize()
{
  terminal_row = 0;
  terminal_column = 0;
  terminal_color = make_color(COLOR_LIGHT_GREY, COLOR_BLACK);
  terminal_buffer = (uint16_t*) 0xB8000;
  for ( size_t y = 0; y < VGA_HEIGHT; y++ )
    {
      for ( size_t x = 0; x < VGA_WIDTH; x++ )
	{
	  const size_t index = y * VGA_WIDTH + x;
	  terminal_buffer[index] = make_vgaentry(' ', terminal_color);
	}
    }
}
 
void terminal_setcolor(uint8_t color)
{
  terminal_color = color;
}
 
void terminal_putentryat(char c, uint8_t color, size_t x, size_t y)
{
  const size_t index = y * VGA_WIDTH + x;
  terminal_buffer[index] = make_vgaentry(c, color);
}
 
void terminal_putchar(char c)
{
  terminal_putentryat(c, terminal_color, terminal_column, terminal_row);
  if ( ++terminal_column == VGA_WIDTH )
    {
      terminal_column = 0;
      if ( ++terminal_row == VGA_HEIGHT )
	{
	  terminal_row = 0;
	}
    }
}
 
void terminal_writestring(const char* data)
{
  size_t datalen = strlen(data);
  for ( size_t i = 0; i < datalen; i++ )
    terminal_putchar(data[i]);
}
 
 //Function used for testing purposes
 void write_timer(void){
	terminal_writestring("    timer    "); 
	return;
 }

/* Convert the integer D to a string and save the string in BUF. If
   BASE is equal to 'd', interpret that D is decimal, and if BASE is
   equal to 'x', interpret that D is hexadecimal. */
void itoa (char *buf, int base, int d)
{
  char *p = buf;
  char *p1, *p2;
  unsigned long ud = d;
  int divisor = 10;
     
  /* If %d is specified and D is minus, put `-' in the head. */
  if (base == 'd' && d < 0)
    {
      *p++ = '-';
      buf++;
      ud = -d;
    }
  else if (base == 'x')
    divisor = 16;
     
  /* Divide UD by DIVISOR until UD == 0. */
  do
    {
      int remainder = ud % divisor;
     
      *p++ = (remainder < 10) ? remainder + '0' : remainder + 'a' - 10;
    }
  while (ud /= divisor);
     
  /* Terminate BUF. */
  *p = 0;
     
  /* Reverse BUF. */
  p1 = buf;
  p2 = p - 1;
  while (p1 < p2)
    {
      char tmp = *p1;
      *p1 = *p2;
      *p2 = tmp;
      p1++;
      p2--;
    }
}


void init( multiboot* pmb ) {

   memory_map_t *mmap;
   unsigned int memsz = 0;		/* Memory size in MB */
   static char memstr[10];

  for (mmap = (memory_map_t *) pmb->mmap_addr;
       (unsigned long) mmap < pmb->mmap_addr + pmb->mmap_length;
       mmap = (memory_map_t *) ((unsigned long) mmap
				+ mmap->size + 4 /*sizeof (mmap->size)*/)) {
    
    if (mmap->type == 1)	/* Available RAM -- see 'info multiboot' */
      memsz += mmap->length_low;
  }

  /* Convert memsz to MBs */
  memsz = (memsz >> 20) + 1;	/* The + 1 accounts for rounding
				   errors to the nearest MB that are
				   in the machine, because some of the
				   memory is othrwise allocated to
				   multiboot data structures, the
				   kernel image, or is reserved (e.g.,
				   for the BIOS). This guarantees we
				   see the same memory output as
				   specified to QEMU.
				    */

  itoa(memstr, 'd', memsz);

  terminal_initialize();

  //terminal_writestring("MemOS: Welcome *** System memory is: ");
  //terminal_writestring(memstr);
  //terminal_writestring("MB");

}




#define MAX_THREADS 10

typedef struct runqueue {
  struct TCB * thread;
  struct runqueue *next;
  struct runqueue *prev;
} rq;

static rq head;
static int done[MAX_THREADS];

//do I need an array of threads that are done

struct TCB {
  void (*task)(struct TCB *);
  int tid;
  void * stack_b;
  void * stack_top;
  void * isp;
  int busy;
  int preempt;
  int priority;
};


struct TCB * TCBs[MAX_THREADS];
//struct TCB * TCBs;

// this is an array of function pointers that don't take any parameters
int (*f[MAX_THREADS])(int);



extern void yield(struct TCB * tcb);


void threadFunction(struct TCB * tcb){


  char tidStr[10];
  int tid = tcb->tid;
  itoa(tidStr, 'd', tid);
  terminal_writestring("<");
  terminal_writestring("Running");
  
  
  //tcb->preempt = 1;
  //call_INT;
  //yield(tcb);

  for (int i = 0; i < 10000000; i++)
  {

  }

  terminal_writestring("Thread");
  terminal_writestring(tidStr);
  terminal_writestring(">");

  terminal_writestring("Done <");
  terminal_writestring(tidStr);
  terminal_writestring(">");
  done[tid] = 1;
  tcb->busy = 0;

  return;
}


/*
On first call to addToQueue, obviously the queue is empty,
so head contains {thread = 0x0, next = 0, prev = 0} and doesn't 
enter the while loop. So then it sets the current (head) to contain
a pointer to the TCB[0] structure
  {thread = 0x105500, next = 0x0, prev= 0x0}

*/
void addToQueue(int tid)
{

  rq *ptr, *pptr;
  rq * current = &head;
  int i = 1;
  while (current->thread != 0)
  {

    ptr = (rq *) (&head + (i*(sizeof(rq))));
    if (i == 1)
    {
        current->next = ptr;
        pptr = &head;
    }
    else {
      pptr = pptr->next;
      pptr->next = ptr;
    }

    ptr->prev = pptr;
    ptr->next = &head; // Wraparound
    head.prev = ptr;
    current = ptr;
    i++;
  }

  //current->thread = TCBs[tid];
  current->thread = TCBs[tid];
  return;
}




/**********************************
 * Only creates one thread
 * 
 *  - need to make it so that it takes in a stack pointer
 * 
 *********************************/
int thread_create(void (*func)(struct TCB *), void * stackStart)
{
  for (int i = 0; i < MAX_THREADS; i++)
  {   
      if (TCBs[i]->busy == 0)
      {
          //looks good
          TCBs[i]->task = func;
          TCBs[i]->busy = 1;
          TCBs[i]->stack_b = stackStart;
          TCBs[i]->stack_top = stackStart;

          addToQueue(i);
          return i;
      }
  }
  return 1000;
}

extern void switch_context(struct TCB * tcb);
extern void restore_context(void);
extern void resume_thread(struct TCB * tcb);
extern void runThread(struct TCB * tcb);



//doesn't take care of stacks
void initializeThreads(void){
  int startAddress = 0x10a500;
  for (int i = 0; i < MAX_THREADS; i++)
  {
    TCBs[i] = (struct TCB *) (startAddress + (i * sizeof(struct TCB)));
    TCBs[i]->tid = i;
    TCBs[i]->busy = 0;
    TCBs[i]->preempt = 0;
    TCBs[i]->priority = 0;
  }
}



void schedule(void)
{
  rq * current;
  rq *finished;

  int threads = MAX_THREADS;

  current = &head;

  // current is set to an address, the address of head
  while(current)
  {
    int tid = current->thread->tid;   //works correctly




    runThread(current->thread);


    if(done[tid])
    {
      if (threads == 1)
      {
        return;
      }
      finished = current;
      finished->prev->next = current->next;
      current = current->next;
      current->prev = finished->prev;

      if (current->next == finished) 
      { // Down to last thread
	        current->next = finished->next;
      }
      threads--;
    }
    else
    {
      current = current->next;
    } 
  }
}




int main(int argc, char * argv[])
{

//looks like it worked correctly
initializeThreads();


//this only creates one thread
/*
if ((thread_create(threadFunction) == 1000))
{
  terminal_writestring("All the threads were busy");
  return 0;

}
*/

//assigns threads a task, sets busy flag, adds them to queue
int threadStackStart = 0x110000; 
for (int i = 0; i < MAX_THREADS; i++)
{
  thread_create(threadFunction, (void *) threadStackStart);
  threadStackStart += 0x100;
}

//passes pointer to TCB structure
//the stack variable isn't initialized yet
/*
TCBs[0]->stack_b = (void *) 0x11110;
TCBs[1]->stack_b = (void *) 0x11210;
*/

/*
before schedule is called, we have:
TCBs[0] = {task = 0x10142f, tid = 0, stack = 0x11110, 
          isp=0x0, busy=1, priority=0}
head = {thread 0x105500, next=0x0, prev=0x0} (thread points to TCBs[0])

*/

schedule();


return 0;
}