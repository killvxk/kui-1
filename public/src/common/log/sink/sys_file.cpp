#include "sys_file.hpp"

#include <fstream>


namespace logger { namespace sink { 
	
	struct file_t::impl
	{
		std::ofstream os_;

		impl(const fs::path &path)
		{
			fs::create_directories(path.parent_path());

			os_.open(path.string(), std::ios::app | std::ios::in);
		}
	};



	file_t::file_t(const fs::path &file_name)
		: impl_(std::make_unique<impl>(file_name))
	{

	}

	file_t::~file_t()
	{

	}

	void file_t::write(const std::string &content)
	{
		impl_->os_.write(content.c_str(), content.size());
	}

	void file_t::flush()
	{
		impl_->os_.flush();
	}

}

}