#ifndef MTL_BINARY_STREAM
#define MTL_BINARY_STREAM

#include <sstream>
#include <type_traits>

namespace mtl
{
	class binary_stream
	{
	public:

		void seekp(std::streampos p)
		{
			_ss.seekp(p);
		}

		std::streampos tellp()
		{
			return _ss.tellp();
		}

		void seekg(std::streampos p)
		{
			_ss.seekg(p);
		}

		std::streampos tellg()
		{
			return _ss.tellg();
		}

		binary_stream& write(const char* s, std::streamsize n)
		{
			_ss.write(s, n);
			return *this;
		}

		binary_stream& read(char* s, std::streamsize n)
		{
			_ss.read(s, n);
			return *this;
		}
	private:
		std::stringstream _ss;
	};

}

//arithmethic (TODO endianess)
template<typename T>
typename std::enable_if< std::is_arithmetic<T>::value, mtl::binary_stream>::type&
operator >> (mtl::binary_stream& ss, T& t)
{
	ss.read((char*)&t, sizeof(T));
	return ss;
}

template<typename T>
typename std::enable_if< std::is_arithmetic<T>::value, mtl::binary_stream>::type&
operator << (mtl::binary_stream& ss, const T& t)
{
	ss.write((char*)&t, sizeof(T));
	return ss;
}


//std::string
mtl::binary_stream& operator >> (mtl::binary_stream& ss, std::string& t)
{
	std::size_t size;
	ss >> size;
	t.resize(size);

	for (auto &c : t)
		ss >> c;
	return ss;
}

mtl::binary_stream& operator << (mtl::binary_stream& ss, std::string& t)
{
	std::size_t size = t.size();
	ss << size;
	ss.write((char*)t.c_str(), size);
	return ss;
}

//const char * (C style string)
mtl::binary_stream& operator << (mtl::binary_stream& ss, const char* str)
{
	std::size_t size = strlen(str);
	ss << size;
	ss.write(str, size);
	return ss;
}


#endif