#ifndef _SCOTTGS_UTILITY_TEMPLATES_HPP
#define _SCOTTGS_UTILITY_TEMPLATES_HPP

#include <iostream>
#include <sstream>
#include <vector>
#include <string>

namespace scottgs {

/* ***********************************
	DECLARATION
************************************ */

/// Convert a vector of some type to a comma separated values string, bound by [...]
template <typename T>
std::string vectorToCSV(std::vector<T> v);



/* ***********************************
	DEFINITION
************************************ */

template <typename T>
std::string vectorToCSV(std::vector<T> v)
{
	std::stringstream ss;
	typename std::vector<T>::const_iterator i=v.begin();
	ss << "'" << (*i);
	for (++i ; i!=v.end(); ++i)
		ss <<"," << (*i);
	ss << "'";
	return ss.str();
}

}; // END scottgs namespace

#endif // _SCOTTGS_UTILITY_TEMPLATES_HPP