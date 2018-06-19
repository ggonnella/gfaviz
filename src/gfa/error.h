// Taken from http://marknelson.us/2007/11/13/no-exceptions/

//
// fatal_error.h
//
#pragma once
#include <sstream>
#include <stdexcept>
#include <string>
 
//
// This class is designed to make it a little easier
// to throw informative exceptions. It's a little lame,
// but I do like to be able to write code like this
// for fatal errors:
//
// throw fatal_error() <<"Game over, "
//                     <<mHealth
//                     <<" health points!";
//
// It works everywhere I've tested it, let's hope that it holds up.
//
 
class fatal_error : public std::exception
{
public :
    fatal_error() {};
    //
    // Need a copy constructor, because the runtime of the compiler is
    // allowed to insist on it when it throws an object of this type,
    // even if it doesn't actually make a copy. When I make a copy, I
    // need to capture whatever is in the stringstream object. Note:
    // in many cases, attempting to copy an iostream object leads to
    // errors, so the copy constructor here constructs a brand new
    // mstream object.
    //
    fatal_error( const fatal_error &that )
    {
        mWhat += that.mStream.str();
    }
    //
    // Virtual dtor needed? Not really, but here it is anyway.
    //
    virtual ~fatal_error() throw(){};
    //
    // When I finally get this object to an exception handler,
    // (hopefully catching by reference) I want to display the error
    // message that I've inserted. To do that, I just chapture
    // whatever is in the mWhat string object concatenated
    // with anything that might be in the stringstring mStream object,
    // and return it. (Odds are that only one of them will contain
    // anything, depending on whether or not the copy constructor
    // was called.
    //
    virtual const char *what() const throw()
    {
        if ( mStream.str().size() ) {
            mWhat += mStream.str();
            mStream.str( "" );
        }
        return mWhat.c_str();
    }
    //
    // The template function used to create insertion operators for all
    // of the various types of objects one might insert into this guy.
    //
    template<typename T>
    fatal_error& operator<<( const T& t )
    {
        mStream <<t;
        return *this;
     }
private:
   mutable std::stringstream mStream;
   mutable std::string mWhat;
};
