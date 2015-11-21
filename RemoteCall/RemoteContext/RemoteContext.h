
class RemoteTask
{
public:
	binarystream ss;

};


class RemoteContext : public enable_shared_from_this
{
public:
	using pointer = std::shared_ptr<RemoteContext>;

	bool ready(); //connected to other side

	void post(binarystream &&stream); //post task to the other side

	void run(); //run all tasks from other side

	void close();

	rc::signal<void(void)> onClosed;
	rc::signal<void(void)> onReady;
	rc::signal<void(RemoteTask&)> gotTask;

};




class RemoteContext_Local : public RemoteContext
{
public:
	pointer CreateMirrorContext();

};
