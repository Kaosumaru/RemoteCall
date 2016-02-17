#ifndef MTL_REMOTE_POINTER
#define MTL_REMOTE_POINTER

#include <string>



namespace mtl
{
	namespace remote
	{
		//this is adequate only for testing AND/OR internal tools, with this client can easily crash server
		template<typename T>
		struct raw_pointer_unsafe
		{
			raw_pointer_unsafe() {}
			raw_pointer_unsafe(uint64_t data) : _data(data) {}

			uint64_t get() const
			{
				return _data;
			}

			void set(uint64_t d)
			{
				_data = d;
			}
		protected:
			uint64_t _data;
		};
		


		template<typename T>
		struct class_traits
		{
			using pointer_type = void;
		};








	}
}

#endif