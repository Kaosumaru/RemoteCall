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



		template<typename R>
		struct TransformType
		{
			using type = R;
		};

		template<typename R>
		struct TransformType<R*>
		{
			using type = typename class_traits<R>::pointer_type;
		};

		template<typename T>
		T& unwrap_type(T& t)
		{
			return t;
		}

		template<typename T>
		T unwrap_type(const raw_pointer_unsafe<T>& t)
		{
			return t;
		}



	}
}

#endif