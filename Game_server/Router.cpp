#include "Router.h"

Router& Router::get_instance()
{
	static Router router;
	return router;
}
