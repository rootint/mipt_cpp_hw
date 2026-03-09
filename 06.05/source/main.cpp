////////////////////////////////////////////////////////////////////////////////

// chapter : Projects and Libraries

////////////////////////////////////////////////////////////////////////////////

// section : Additional Libraries

////////////////////////////////////////////////////////////////////////////////

// content : Additional Libraries
//
// content : Boost Library Collection
//
// content : Static and Dynamic Libraries
//
// content : Library Boost.DLL
//
// content : Wrapper std::function
//
// content : Runtime Library Selection

////////////////////////////////////////////////////////////////////////////////

#include <iostream>
#include <string>

////////////////////////////////////////////////////////////////////////////////

#include <boost/dll.hpp>
#include <boost/function.hpp>
#include <boost/shared_ptr.hpp>

////////////////////////////////////////////////////////////////////////////////

int main()
{
    std::string path;

//  ----------------------------------------------------------------------------

    while (true)
    {
        std::cout << "main : enter library path (or \"exit\"): ";

//      ------------------------------------------------------------------------

        std::cin >> path;

//      ------------------------------------------------------------------------

        if (path == "exit")
        {
            break;
        }

//      ------------------------------------------------------------------------

        try
        {
            auto function =

                boost::dll::import_alias < void() > (path, "test");

//          --------------------------------------------------------------------

            (*function)();
        }
        catch (std::exception const & exception)
        {
            std::cerr << "main : error : " << exception.what() << std::endl;
        }
    }
}

////////////////////////////////////////////////////////////////////////////////