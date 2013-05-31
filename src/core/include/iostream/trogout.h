#ifndef TROGOUT_H
#define TROGOUT_H


#include <string>
#include <iostream>
#include <sstream>


using namespace std;

namespace core {


   /*
      Base output stream class.  This isn't a true output stream, as it wraps
      around a stringstream and ultimately passes the string on to another real
      output stream (determined by flush()), but behaves like one.
   */
   class Trogout {

      private:

         stringstream buffer;

      protected:

         /*
            Returns the string inside buffer.

            Input:
               (none)

            Output:
               string
         */
         inline string getBufferStr() {return buffer.str();}

         /*
            Clears the "buffer."

            Input:
               (none)

            Output:
               (none)
         */
         inline void clear() {buffer.str("");}

         /*
            Flushes the "buffer" to the real output stream.

            Input:
               (none)

            Output:
               (none)
         */
         virtual void flush() = 0;

      public:

         // string output operators
         inline Trogout& operator<< (string val) {buffer << val;}
         inline Trogout& operator<< (char const *val) {buffer << val;}

         // arithmetic output operators
         inline Trogout& operator<< (bool val) {buffer << val;}
         inline Trogout& operator<< (short val) {buffer << val;}
         inline Trogout& operator<< (unsigned short val) {buffer << val;}
         inline Trogout& operator<< (int val) {buffer << val;}
         inline Trogout& operator<< (unsigned int val) {buffer << val;}
         inline Trogout& operator<< (long val) {buffer << val;}
         inline Trogout& operator<< (unsigned long val) {buffer << val;}
         inline Trogout& operator<< (float val) {buffer << val;}
         inline Trogout& operator<< (double val) {buffer << val;}
         inline Trogout& operator<< (void* val) {buffer << val;}

         // special endl output operator
         inline Trogout& operator<< (ostream& (*pf)(ostream&)) {

            buffer << endl;
            flush();
            clear();
         }
   };
}


#endif

