#ifndef AUTO_ENLARGE_BUFFER_H
#define AUTO_ENLARGE_BUFFER_H

#include <stdlib.h>
#include <string.h>  //for memcpy

class AutoEnlargeBuffer
{
   public :

      inline AutoEnlargeBuffer(int size = 128*1024);

      inline ~AutoEnlargeBuffer();

      inline void copy(void *src, int size);

      inline int   size() { return m_size; }
      inline void *data() { return m_buffer; }


   private :

      int    m_SIZE;
      int    m_size;
      void*  m_buffer;
};

inline AutoEnlargeBuffer::AutoEnlargeBuffer(int size)
   : m_SIZE( size ),
     m_size( 0 )
{
   m_buffer = malloc(size);
}

inline AutoEnlargeBuffer::~AutoEnlargeBuffer()
{
   free( m_buffer );
}

inline void AutoEnlargeBuffer::copy(void *src, int size)
{
   if ( size > m_SIZE ) {
      do {
	 m_SIZE *= 2;
      }
      while ( size > m_SIZE );

      free( m_buffer );
      m_buffer = malloc(m_SIZE);
   }

   m_size = size;
   memcpy(m_buffer, src, size);
}

#endif
