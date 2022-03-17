template <typename F, typename... Args>
auto really_async(F&& f, Args&&... args)
-> std::future<typename std::result_of<F(Args...)>::type>
{
	using _Ret = typename std::result_of<F(Args...)>::type;
	auto _func = std::bind(std::forward<F>(f), std::forward<Args>(args)...);
	std::packaged_task<_Ret()> tsk(std::move(_func));
	auto _fut = tsk.get_future();
	std::thread(std::move(tsk)).detach();
	return _fut;
}