#ifndef MOCK_TIMER_JOB_H
#define MOCK_TIMER_JOB_H


#include <trogdor/timer/timerjob.h>


class MockTimerJob: public trogdor::TimerJob {

	private:

		// This gets called by execute().
		std::function<void()> executeCallback;

	public:

	/*
		Constructor for the MockTimerJob class.
	*/
	inline MockTimerJob(trogdor::Game *g, int i, int e, int s,
	std::function<void()> callback): TimerJob(g, i, e, s),
	executeCallback(callback) {}

	/*
		Execute the timer job (calls executeCallback)

		Input:
		(none)

		Output:
		(none)
	*/
	virtual void execute();
};


#endif
