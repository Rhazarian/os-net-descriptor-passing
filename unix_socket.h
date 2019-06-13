#ifndef UNIX_SOCKET
#define UNIX_SOCKET

#include "unix_base_socket.h"

struct pipe_input;
struct pipe_output;

struct unix_socket : unix_base_socket
{
	unix_socket();

	void bind(const std::filesystem::path& p);
	void connect(const std::filesystem::path& p);

	void send(pipe_input s);
	void send(pipe_output s);
	void receive(pipe_input& s);
	void receive(pipe_output& s);

private:
	void receive(msghdr& msg);
	void send(const msghdr& msg);

	explicit unix_socket(int fd);

	friend struct unix_server_socket;
};

#endif // UNIX_SOCKET