#ifndef NEWPLAYERACTION_H
#define NEWPLAYERACTION_H


#include "../networkaction.h"


// Adds a new player to the game.
class NEWPLAYERAction: public NetworkAction {

	private:

		virtual void execute(TCPConnection::ptr connection);
};


#endif

