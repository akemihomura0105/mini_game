#include "User_info.h"

User_info::User_info()
{
}

User_info::User_info(int _id, std::string _name, bool _ready) :session_id(_id), name(_name), ready(_ready)
{
}
