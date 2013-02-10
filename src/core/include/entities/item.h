#ifndef ITEM_H
#define ITEM_H


#include "thing.h"
#include "being.h"


namespace core { namespace entity {

   class Item: public Thing {

      protected:

         Being *owner;

      public:
   };
}}


#endif

