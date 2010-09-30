#ifndef USER_NAME_EX_H
#define USER_NAME_EX_H

#include <exception>
class UserNotFoundException : public std::exception{
public:
	virtual const char* what(){
		return "User not found for data supplied";
	}
};


#endif
