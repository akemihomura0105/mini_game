#include <boost/asio.hpp>
#include "System.h"
#include <memory>
#include <string>
#include <iostream>
#include <sstream>

using namespace boost::asio;

struct Test
{
	friend class boost::serialization::access;
	template<typename Archive>
	void serialize(Archive& ar, const unsigned int version)
	{
		ar& a;
		ar& b;
		ar& str;
	}
	int a, b;
	std::string str;
};

int main(int argc, char* argv[])
{
	io_context io;
	//ip::tcp::endpoint ep(ip::address::from_string(argv[1]), atoi(argv[2]));
	Test t;
	t.a = 0;
	t.b = 1;
	t.str = "aabbccdd";
	std::stringstream oss;
	boost::archive::text_oarchive oa(oss);
	oa << t;
	auto str = oss.str();
	std::cout << str << std::endl;
	std::stringstream iss(oss.str());
	boost::archive::text_iarchive ia(iss);
	Test ans;
	ia >> ans;
	std::cout << ans.a << " " << ans.b << " " << ans.str << std::endl;
	//System system(io, ep);
	//system.run();
}