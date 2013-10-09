#ifndef VOCABULARY_H
#define VOCABULARY_H


#ifndef VOCABULARY_CPP

using namespace std;


/* list of valid directions */
extern const char *g_directions[];

/* list of English filler words that we should ignore */
extern const char *g_fillerWords[];

/* list of one-word English prepositions recognized by Trogdor */
extern const char *g_prepositions[];

/*
   Returns true if the specified word is a valid direction and false if it's not.

   Input:
      string

   Output:
      bool
*/
extern bool isDirection(string str);

#endif


#endif

