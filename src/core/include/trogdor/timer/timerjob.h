#ifndef TIMERJOB_H
#define TIMERJOB_H


#include <memory>
#include <trogdor/timer/timer.h>

#include <trogdor/exception/undefinedexception.h>


namespace trogdor {

   /*
      TimerJob is an abstract class that represents a single job in a timer
      queue.  Each time the job is executed, execute() is called.  This class is
      meant to be inherited by a class written to accomplish a specific task.

      In a normal workflow, a typical use would be as follows (we'll assume that
      we're inside a method of Game that has direct access to the timer object
      and that there's a class, Task, which inherits TimerJob and performs some
      specific job):

      // NOTE: if inside a game object, you'll have to cast this to (Game *),
      // because this is a Game * const, not just Game *
      Task *task = new Task(currentGame, 1, 1, 1);

      The job has free reign to change any of its public or protected parameters
      at any time during execution.  For example, a job that was originally
      supposed to execute 1 time might increment executions to make it run one
      additional time.
   */
   class TimerJob {

      /*
         The following values are used by the job queue.
      */
      private:

         // Maps class names to type ids for all registered timer jobs
         static std::unordered_map<std::string_view, std::type_info *> types;

         // Maps type ids to callbacks that can create instances of TimerJob
         // that are the appropriate type. Depending on the data that's passed
         // in, callbacks should either handle copy construction or
         // deserialization.
         static std::unordered_map<
            std::type_index,
            std::function<std::shared_ptr<TimerJob>(std::any)>
         > instantiators;

         // Registers built-in timer job types
   		static void registerBuiltinTypes();

         unsigned long initTime;   // time when the job was registered in the queue
         unsigned long startTime;  // how long to wait before job's first execution
         unsigned long interval;   // timer ticks between each job execution

         // Number of times to execute the job before removing it from the queue.
         // This value is decremented on every tick of the clock, and when it
         // reaches 0, it's removed from the queue. If set to -1, it will
         // continue to execute indefinitely until it's removed manually.
         int executions;

      protected:

         /*
            Registers a new timer job type so that we know how to copy and
            deserialize it later.

            Input:
               Name of derived class (const char *)
               The class's type id (std::type_info *)
               A callback that can handle copy construction and deserialization

            Output:
               (none)
         */
         inline static void registerType(
            const char *name,
            std::type_info *type,
            std::function<std::shared_ptr<TimerJob>(std::any)> instantiator
         ) {
            types[name] = type;
            instantiators[std::type_index(*type)] = instantiator;
         }

         // Pointer to the game in which the timer resides
         Game *game;

      public:

         /*
            Returns the type_info of the given job name.

            Input:
               Type name (const char *)
               Arguments to pass to instantiator callback (std::any)

            Output:
               const std::type_info &

            Throws an instance of UndefinedException if the type hasn't been
            registered.
         */
         inline static const std::type_info &getType(const char *name) {

            if (!types.size()) {
               registerBuiltinTypes();
            }

            if (auto type = types.find(name); type != types.end()) {
               return *type->second;
            } else {
               throw UndefinedException(
                  std::string("TimerJob type '") + name + "' has not been registered"
               );
            }
         }

         /*
            Instantiates a copy constructed or deserialized child of TimerJob.

            Input:
               Type name (const char *)
               Arguments to pass to instantiator callback (std::any)

            Output:
               Copied or deserialized instance of TimerJob (std::shared_ptr<TimerJob>)
         */
         inline static std::shared_ptr<TimerJob> instantiate(
            const char *type,
            std::any args
         ) {

            if (!types.size()) {
               registerBuiltinTypes();
            }

            return instantiators[std::type_index(*types[type])](args);
         }

         /*
            Constructor for the TimerJob abstract class.
         */
         inline TimerJob(Game *g, int i, int e, int s) {

            initTime = 0; // will be set by insertJob()

            // An interval of 0 or less doesn't make sense and would lead to
            // undefined behavior
            // TODO: use unsigned int for this (but if I do, I still have to
            // disallow interval = 0, which can lead to floating point exceptions
            // due to division by 0.)
            if (i < 1) {
               throw UndefinedException("TimerJob::TimerJob(): cannot set interval less than 1");
            }

            game = g;
            interval = i;
            executions = e;
            startTime = s;
         }

         /*
            Deserialization Constructor
         */
         TimerJob(const serial::Serializable &data, Game *g);

         /*
            Apparently, when a class is polymorphic, you need to make the base
            class destructor virtual. Otherwise, if delete is called on an object
            pointed to by a pointer of the base class's type and the derived class
            has a destructor, the derived class's destructor won't be called.

            See:
            https://stackoverflow.com/questions/47702776/how-to-properly-delete-pointers-when-using-abstract-classes

            Interestingly, I got a warning about this from Clang -Wall, but not
            from GCC -Wall.
         */
         virtual ~TimerJob() = 0;

         /*
            Returns the class name of a TimerJob instance.

            Input:
               (none)

            Output:
               Class name (const char *)
         */
         virtual const char *getClassName() = 0;

         /*
            This is the function that will be executed every time the job runs.
            It should be implemented in another class designed for a specific
            job.
         */
         virtual void execute() = 0;

         inline unsigned long getInitTime() const {return initTime;}
         inline unsigned long getStartTime() const {return startTime;}
         inline unsigned long getInterval() const {return interval;}
         inline int getExecutions() const {return executions;}

         inline void setStartTime(unsigned long s) {startTime = s;}
         inline void setInterval(unsigned long i) {interval = i;}
         inline void setExecutions(int e) {executions = e;}

         // decrement executions
         inline void decExecutions() {executions--;}

         // Returns an easily serializable version of a TimerJob instance.
         virtual std::shared_ptr<serial::Serializable> serialize();

         // allows Timer to interact with the TimerJob object
         friend void Timer::insertJob(std::shared_ptr<TimerJob> job);
   };
}


#endif
