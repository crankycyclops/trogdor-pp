#ifndef LUASTATE_H
#define LUASTATE_H


#include <mutex>
#include <string>

extern "C" {
   #include <lua.h>
   #include <lualib.h>
   #include <lauxlib.h>
}

#include <trogdor/lua/luatype.h>
#include <trogdor/lua/luatable.h>

#include <trogdor/lua/api/luagame.h>

#include <trogdor/lua/api/entities/luaentity.h>
#include <trogdor/lua/api/entities/luaresource.h>
#include <trogdor/lua/api/entities/luatangible.h>
#include <trogdor/lua/api/entities/luaplace.h>
#include <trogdor/lua/api/entities/luaroom.h>
#include <trogdor/lua/api/entities/luathing.h>
#include <trogdor/lua/api/entities/luaobject.h>
#include <trogdor/lua/api/entities/luabeing.h>
#include <trogdor/lua/api/entities/luacreature.h>
#include <trogdor/lua/api/entities/luaplayer.h>

#include <trogdor/exception/luaexception.h>
#include <trogdor/serial/serializable.h>


namespace trogdor {

   class Game;

   namespace entity {
      class Entity;
   }

   /*
      LuaState wraps around an (aptly named) lua state variable and allows us to
      perform basic operations on it, specifically loading scripts and calling
      functions.  A typical workflow might look something like this:

      LuaState L(game); // game here is of type Game *

      try {
         L.loadScriptFromFile("script.lua");
         L.call("functionName");
         L.pushArgument("this is a string");
         L.execute();

         // examples of retrieving return values (0 based from first to last)
         // calling these before L.execute() results in undefined behavior
         double retVal1 = L.getNumber(0); // 1st return value
         std::string retVal2 = L.getString(1); // 2nd return value
      }

      // handle errors
      catch (const LuaException &e) {
         cout << e.what() << std::endl;
      }
   */
   class LuaState {

      private:

         // Lock on this for thread-safety
         std::mutex mutex;

         // Pointer to the Game object this Lua state is a part of
         Game *game;

      protected:

         // number of function arguments pushed onto the Lua stack
         int nArgs;

         // set everytime we run execute(), and used to retrieve return values
         int nReturnValues;

         // Lua state
         lua_State *L;

         // remembers parsed scripts so that we can "copy" them to another state
         std::string parsedScriptData;

         // error message that resulted from the last operation
         std::string lastErrorMsg;

         /*
            Registers the global Game object that Lua will use to interact with
            the state's instance of the C++ class Game.

            Input:
               (none)

            Output:
               (none)
         */
         void registerGlobalGame();

         /*
            Initializes the Lua State. Should only be called by a constructor.

            Input:
               (none)

            Output:
               (none)
         */
         inline void initState() {

            nArgs = 0;
            nReturnValues = 0;
            parsedScriptData = "";
            lastErrorMsg = "";

            L = luaL_newstate();
         }

         /*
            Opens libraries and registers API stuff.

            Input:
               (none)

            Output:
               (none)
         */
         inline void initLibs() {

            // load standard library
            // TODO: only open certain standard libraries. I don't, for example,
            // want to allow things like os.exit(). Hold off on this until I
            // migrate to Lua 5.2+. The answer to this seems to be here:
            // https://stackoverflow.com/questions/4551101/lual-openlibs-and-sandboxing-scripts
            luaL_openlibs(L);

            // register Game type as well as a global instance of it
            // corresponding to the game that spawned the instance of LuaState
            registerGlobalGame();

            // register Entity types
            entity::LuaEntity::registerLuaType(L);
            entity::LuaResource::registerLuaType(L);
            entity::LuaTangible::registerLuaType(L);
            entity::LuaPlace::registerLuaType(L);
            entity::LuaRoom::registerLuaType(L);
            entity::LuaThing::registerLuaType(L);
            entity::LuaObject::registerLuaType(L);
            entity::LuaBeing::registerLuaType(L);
            entity::LuaCreature::registerLuaType(L);
            entity::LuaPlayer::registerLuaType(L);
         }

         /*
            Pushes a Lua Value onto the stack, but doesn't increment nArgs. This
            is used by pushTable and pushArray.

            Input:
               Reference to LuaValue
               Lua state

            Output:
               (none)
         */
         static void pushLuaValue(LuaValue v, lua_State *L);

      public:

         /*
            Returns the version of Lua this class was built against in the
            format "5.x.x".

            Input:
               (none)

            Output:
               const char *
         */
         inline static constexpr const char *getLuaVersion() {

            #ifndef LUA_VERSION_MAJOR
               return LUA_VERSION;
            #else
               return LUA_VERSION_MAJOR "." LUA_VERSION_MINOR "." LUA_VERSION_RELEASE;
            #endif
         }

         /*
            WARNING: You MUST call LuaState::lock() BEFORE you do ANYTHING with
            a LuaState, because Lua itself is not thread-safe. You MUST remain
            locked on the LuaState's mutex until you've finished using it, and
            then you MUST call LuaState::unlock() to release it.

            Example:

            L.lock();
            L.call(function);
            L.execute(2);
            bool returnVal = L.getBoolean(0);
            L.unlock();

            Input:
               (none)

            Output:
               (none)
         */
         inline void lock() {mutex.lock();}
         inline void unlock() {mutex.unlock();}

         /*
            Returns the Game that the Lua state operates on.

            Input:
               (none)

            Output:
               Game *
         */
         inline Game *getGame() {return game;}

         /*
            Serializes the Lua state.

            Input:
               (none)

            Output:
               Serialized Lua state (std::shared_ptr<Serializable>)
         */
         inline std::shared_ptr<serial::Serializable> serialize() {

            std::shared_ptr<serial::Serializable> data = std::make_shared<serial::Serializable>();

            data->set("scripts", parsedScriptData);
            return data;
         }

         /*
            Wraps around luaL_register and provides equivalent functionality
            for Lua versions 5.2+, which deprecated and later removed this
            function.

            Adapted from this: https://trac.videolan.org/vlc/ticket/14695, which
            references this:
            http://lua.2524044.n2.nabble.com/Why-did-luaL-openlib-luaL-register-go-td7649023.html

            Input:
               Lua state
               Library name
               List of functions to register
         */
         inline static void luaL_register_wrapper(lua_State *L,
         const char *libname, const luaL_Reg *l) {

            if (nullptr == libname) {

               // Lua 5.2+ discourages the use of globals and therefore removed
               // luaL_register. These function calls accomplish the same thing.
               // See: http://www.lua.org/manual/5.2/manual.html#8.3
               #if LUA_VERSION_NUM > 501

                  luaL_setfuncs(L, l, 0);

               #else

                  // Registers methods with metatable at the top of the stack
                  luaL_register(L, 0, l);

               #endif
            }

            else {

               #if LUA_VERSION_NUM > 501

                  // lua_setglobal pops the table from the stack, which is why
                  // we push a copy of it below (so the table remains on the
                  // stack after, just like the now defunct call to luaL_register.
                  lua_newtable(L);
                  luaL_setfuncs(L, l, 0);
                  lua_pushvalue(L, -1);
                  lua_setglobal(L, libname);

               #else

                  // See: https://www.lua.org/manual/5.1/manual.html
                  luaL_register(L, libname, l);

               #endif
            }
         }

         /*
            Like luaL_checkudata, except that it accepts an array of possible
            types instead of just one. Shamelessly stolen from:
            http://lua-users.org/lists/lua-l/2007-04/msg00324.html

            Though I stole this, I *did* have to debug it, so I get partial
            credit ;)

            Input:
               Lua state
               index of user data on the stack
               array of C-style strings naming valid types
         */
         inline static void *luaL_checkudata_ex(lua_State *L, int ud, const char **tnames) {

            int i;
            void *p = lua_touserdata(L, ud);

            // value is a userdata?
            if (p != NULL) {

               // does it have a metatable?
               if (lua_getmetatable(L, ud)) {

                  // compare to each tname given
                  for (i = 0; tnames[i] != NULL; i++) {

                     // get correct mt
                     lua_getfield(L, LUA_REGISTRYINDEX, tnames[i]);

                     // does it have the correct mt?
                     if (lua_rawequal(L, -1, -2)) {
                        lua_pop(L, 2);  // remove both metatables
                        return p;
                     }

                     else {
                        lua_pop(L, 1);
                     }
                  }
               }
            }

            // compose error message that mentions all typenames
            {
               /* separate scope to avoid default stack of > LUAL_BUFFERSIZE,
                  assuming it even makes a difference... */
               luaL_Buffer B;

               luaL_buffinit(L, &B);

               for (i = 0; tnames[i] != NULL; i++) {
                  luaL_addstring(&B, tnames[i]);
                     if (tnames[i+1] != NULL) luaL_addlstring(&B, " or ", 4);
               }

               luaL_pushresult(&B);

               #if LUA_VERSION_NUM > 501

                  // Replaces call to luaL_typerror, which died after Lua 5.1
                  // TODO: luaL_typename() just returns "table," which is not
                  // helpful. Figure out a way to get a better message.
                  const char *errmsg = lua_pushfstring(L, "%s expected, got %s",
                     lua_tostring(L, -1), luaL_typename(L, ud));
                  luaL_argerror(L, ud, errmsg);

               #else

                  luaL_typerror(L, ud, lua_tostring(L, -1));

               #endif

               return NULL; /* to avoid warnings */
            }
         }

         /*
            Pushes an Entity onto a Lua stack. Static access allows for both
            Lua-to-C and C-to-Lua.

            Input:
               Lua state
               Entity *

            Output:
               (none)
         */
         static void pushEntity(lua_State *L, entity::Entity *e);

         /*
            Pushes an array onto a Lua stack. Static access allows for both
            Lua-to-C and C-to-Lua.

            Input:
               Lua state
               Reference to LuaArray

            Output:
               (none)
         */
         static void pushArray(lua_State *L, LuaArray &arg);

         /*
            Pushes a table onto a Lua stack. Static access allows for both
            Lua-to-C and C-to-Lua.

            Input:
               Lua state
               Reference to LuaTable

            Output:
               (none)
         */
         static void pushTable(lua_State *L, LuaTable &arg);

         /*
            Constructor for the LuaState object (requires a pointer to the
            containing Game.)
         */
         LuaState() = delete;
         inline LuaState(Game *g): game(g) {

            initState();
            initLibs();
         }

         /*
            Copy Constructor for the LuaState object.
         */
         inline LuaState(const LuaState &LSrc): game(LSrc.game) {

            initState();
            initLibs();

            if (LSrc.parsedScriptData.length() > 0) {
               loadScriptFromString(LSrc.parsedScriptData);
            }
         }

         /*
            Deserialization constructor.
         */

         inline LuaState(Game *g, const serial::Serializable &data): game(g) {

            initState();
            initLibs();

            loadScriptFromString(std::get<std::string>(*data.get("scripts")));
         }

         /*
            Destructor for LuaState.
         */
         inline ~LuaState() {lua_close(L);}

         /*
            Assignment operator for LuaState.
         */
         inline LuaState &operator=(const LuaState &rhs) {

            if (this != &rhs) {
               lua_close(L);
               game = rhs.game;
               initState();
               initLibs();
               loadScriptFromString(rhs.parsedScriptData);
            }

            return *this;
         }

         /*
            Returns the last error message that occured during an operation.

            Input:
               (none)

            Output:
               Error message (std::string)
         */
         inline std::string getLastErrorMsg() const {return lastErrorMsg;}

         /*
            Loads a script from the specified file.  If there's an error,
            lastErrorMsg will be set and an exception with that same message
            will be thrown.

            Input:
               filename (std::string)

            Output:
               (none)
         */
         void loadScriptFromFile(std::string filename);

         /*
            Loads a script from the specified string.  If there's an error, the
            child-specific implementation is expected to set lastErrorMsg and
            throw an exception with that same message as the argument.

            Input:
               script body (std::string)

            Output:
               (none)
         */
         void loadScriptFromString(std::string script);

         /*
            Push function arguments onto the Lua state's stack.

            Input:
               Value of the appropriate type

            Output:
               (none)
         */
         inline void pushNilArgument() {

            lua_pushnil(L);
            nArgs++;
         }

         inline void pushArgument(const char *arg) {

            lua_pushstring(L, arg);
            nArgs++;
         }

         inline void pushArgument(std::string arg) {

            lua_pushstring(L, arg.c_str());
            nArgs++;
         }

         inline void pushArgument(double arg) {

            lua_pushnumber(L, (lua_Number)arg);
            nArgs++;
         }

         inline void pushArgument(bool arg) {

            lua_pushboolean(L, (int)arg);
            nArgs++;
         }

         inline void pushArgument(LuaArray &arg) {

            nArgs++;
            pushArray(L, arg);
         }

         inline void pushArgument(LuaTable &arg) {

            nArgs++;
            pushTable(L, arg);
         }

         void pushArgument(entity::Entity *e);

         /*
            Get return values at the specified index.  Calling this before
            calling execute() results in undefined behavior.

            Input:
               index (normalized, so that 0 is the index of the first return
               value, 1 is the index of the next, and so on and so forth...)

            Output:
               value of the appropriate type
         */
         inline bool getBoolean(int i) const {

            return lua_toboolean(L, i - nReturnValues);
         }

         inline double getNumber(int i) const {

            return lua_tonumber(L, i - nReturnValues);
         }

         inline std::string getString(int i) const {

            return lua_tostring(L, i - nReturnValues);
         }

         /*
            Primes the Lua state by parsing it so that recently added scripts,
            global variables, etc. will be seen by the interpreter. This gets
            called automatically by loadScriptFromFile() and
            loadScriptFromString(), but may also be called manually if desired.
            Throws an exception if there's an error.

            Input:
               (none)

            Output:
               (none)
         */
         inline void prime() {

            if (lua_pcall(L, 0, 0, 0)) {
               throw LuaException(std::string("error: ") + lua_tostring(L, -1));
            }
         }

         /*
            Gets us ready to execute the specified function.  This should always
            be called BEFORE running passArgument() and friends.  And exception
            with an error message will be thrown if the function doesn't exist
            or if there's an error.

            Input:
               function name (std::string)

            Output:
               (none)
         */
         void call(std::string function);

         /*
            Executes the function set up by LuaState::call().  If there's an
            error, lastErrorMsg will be set and an exception will be thrown.

            Input:
               number of return values (default is 0)

            Output:
               (none)
         */
         void execute(int nReturnVals = 0);
   };
}


#endif
