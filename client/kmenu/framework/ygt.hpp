#ifndef __YGT_HPP
#define __YGT_HPP

#include <memory>
#include <string>


	struct ygt_t
	{
		struct impl;
		std::unique_ptr<impl> impl_;

		ygt_t();
		~ygt_t();
		
		bool write( const std::string & );
		std::string read();

		bool del();
	};



#endif