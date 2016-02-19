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



		namespace impl
		{
			template<typename OUT, typename IN>
			struct type_wrapper
			{
				static const OUT& wrap_type(const IN& t)
				{
					return t;
				}

				static OUT&& wrap_type(IN&& t)
				{
					return std::move(t);
				}


			};

			template<typename IN>
			struct type_wrapper<raw_pointer_unsafe<IN>, IN*>
			{
				static raw_pointer_unsafe<IN> wrap_type(IN* t)
				{
					raw_pointer_unsafe<IN> p;
					p.set((uint64_t)t);
					return p;
				}


			};

			template<typename IN>
			struct type_wrapper<IN*, raw_pointer_unsafe<IN>>
			{
				static IN* wrap_type(raw_pointer_unsafe<IN> p)
				{
					return (IN*)p.get();
				}
			};

			struct transform_strategy_remote_pointer
			{
				template<typename T>
				using transform_return_type = typename remote::TransformType<T>::type;

				template<typename T>
				using transform_argument_type = transform_return_type< std::decay_t<T> >;


				template<typename TO, typename FROM>
				static TO convert(FROM t)
				{
					return type_wrapper<TO, FROM>::wrap_type(t);
				}
			};

		}

		template<typename OUT, typename IN>
		OUT unwrap_type(IN t)
		{
			return impl::type_wrapper<OUT, IN>::wrap_type(t);
		}



		template<typename OUT, typename IN>
		OUT wrap_type(IN t)
		{
			return impl::type_wrapper<OUT, IN>::wrap_type(t);
		}




	}
}

#endif