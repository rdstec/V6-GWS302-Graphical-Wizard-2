/***********************************************************************/
/*                                                                     */
/*  SYSCALLS.C:  System Calls                                          */
/*  most of this is from newlib-lpc and a Keil-demo                    */
/*                                                                     */
/*  These are "reentrant functions" as needed by                       */
/*  the WinARM-newlib-config, see newlib-manual.                       */
/*  Collected and modified by Martin Thomas                            */
/*                                                                     */
/***********************************************************************/

/* adapted for the SAM7 "serial.h" mthomas 10/2005 */

// Include Standard files
#include <stdlib.h>
#include <reent.h>
#include <sys/stat.h>

#include "global.h"

_ssize_t _read_r(
    struct _reent *r, 
    int file, 
    void *ptr, 
    size_t len)
{
	signed portCHAR cByteRxed;
	int  i;
	unsigned char *p;

	p = (unsigned char*)ptr;

	for( i = 0; i < len; i++ )
	{
		if( xQueueReceive( RxCOM0, &cByteRxed, comNO_BLOCK )== pdFALSE )
		{
			break;
		}
		*p++ = cByteRxed;
	}
	return i;
}


_ssize_t _write_r (
    struct _reent *r, 
    int file, 
    const void *ptr, 
    size_t len)
{
	int i;
	const unsigned char *p;

	p = (const unsigned char*)ptr;

	for( i = 0; i < len; i++ )
	{
		if( xQueueSend( TxCOM0, p++, comTX_BLOCK_TIME ) != pdPASS )
		{
			break;
		}
	}
	return i;
}


int _close_r(
    struct _reent *r, 
    int file)
{
	return 0;
}


int _isatty( int file )
{
	return -1; // return error
}


_off_t _lseek_r(
    struct _reent *r, 
    int file, 
    _off_t ptr, 
    int dir)
{
	return (_off_t)0;	/*  Always indicate we are at file beginning.  */
}


int _fstat_r(
    struct _reent *r, 
    int file, 
    struct stat *st)
{
	/*  Always set as character device.				*/
	st->st_mode = S_IFCHR;
	/* assigned to strong type with implicit 	*/
	/* signed/unsigned conversion.  Required by 	*/
	/* newlib.					*/

	return 0;
}


void abort( void )
{

	/* endless loop */
	while( 1 )
	  {
	  }
}


#if 0
static void _exit( int n )
{

/* endless loop */	 
while( 1 )
  {
  }
}
#endif 


/* "malloc clue function" from newlib-lpc/Keil-Demo/"generic" */
/**** Locally used variables. ****/
// mt: "cleaner": extern char* end;
/* end is set in the linker command file and is the end of statically
   allocated data (thus start of heap).	*/
extern char end[];
static char *heap_ptr; /* Points to current end of the heap.	*/

/************************** _sbrk_r *************************************
 * Support function. Adjusts end of heap to provide more memory to
 * memory allocator. Simple and dumb with no sanity checks.

 *  struct _reent *r -- re-entrancy structure, used by newlib to
 *                      support multiple threads of operation.
 *  ptrdiff_t nbytes -- number of bytes to add.
 *                      Returns pointer to start of new heap area.
 *
 *  Note:  This implementation is not thread safe (despite taking a
 *         _reent structure as a parameter).
 *         Since _s_r is not used in the current implementation, 
 *         the following messages must be suppressed.
 */
void * _sbrk_r(
    struct _reent *_s_r, 
    ptrdiff_t nbytes)
{
	char  *base;		/*  errno should be set to  ENOMEM on error  */

	if( !heap_ptr )
      {	/*  Initialize if first time through.  */
	  heap_ptr = end;
	  }
	base = heap_ptr;	/*  Point to end of heap.  */
	heap_ptr += nbytes;	/*  Increase heap.  */
	
	return base;		/*  Return pointer to start of new heap area.  */
}
