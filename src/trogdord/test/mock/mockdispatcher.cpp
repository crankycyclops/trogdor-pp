#include "mockdispatcher.h"


MockDispatcher::MockDispatcher(std::unordered_map<std::string, ScopeController *> s) {

	scopes = s;
}
