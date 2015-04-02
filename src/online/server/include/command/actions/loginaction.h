#ifndef LOGINACTION_H
#define LOGINACTION_H


#include "../networkaction.h"


class LOGINAction: public NetworkAction {

	private:

		virtual void execute(TCPConnection::ptr connection);
};


#endif

