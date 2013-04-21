#include <iostream>

#include <vector>
#include <string>

#include <boost/program_options.hpp>

#include "rc/private/RCBuilder.h"

namespace po = boost::program_options;

int main( int argc, char** argv )
{
	std::string output_file;
	std::string config_file;
	// Construct program user_options object
	po::options_description user_options("Options");

	user_options.add_options()
		("help,h", "Show this message")
		("output,o", po::value< std::string >(&output_file), "Output file name (mandatory option)" )
		;
	po::options_description hidden_options("hidden");
	hidden_options.add_options()("input", po::value<std::string>( &config_file ), "hidden");

	po::positional_options_description positional;
	positional.add( "input", 1 );

	po::options_description options("parser");
	options.add( user_options ).add( hidden_options );

	po::variables_map vm;
	po::store(po::command_line_parser(argc, argv).
		options(options).positional(positional).run(), vm);
	po::notify(vm);

	if( (argc == 1) || vm.count("help") || output_file.empty() || config_file.empty() )
	{
		std::cout << "Usage: rc [options] config_file\n" << user_options << std::endl;
		return 1;
	}
	
	try
	{	
		std::cout << "Generating " << output_file << " from " << config_file << std::endl;
		rc::RCBuilder builder;
		builder.parseConfig(config_file);
		builder.writeFile(output_file);
	}
	catch( std::exception& ex )
	{
		std::cout<< ex.what() << std::endl;
		return 1;
	}
	
	return 0;
}