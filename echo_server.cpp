#include "echo_server.h"

#include "unix_server_socket.h"
#include "pipe.h"

#include <array>
#include <thread>

void echo_server::start(const std::filesystem::path& p)
{
	static constexpr size_t buffer_size = 1024 * 4;
	unix_server_socket socket;
	socket.bind(p);
	try {
		while (true)
		{
			std::thread([client = socket.accept()]() mutable
			{
				try {
					std::array<char, buffer_size> buffer;
					pipe_input input;
					client.receive(input);
					pipe_output output;
					client.receive(output);
					do {
						input.readsome(buffer.data(), buffer.size());
						output.write(buffer.data(), input.gcount());
					} while (input.gcount());
				} catch (...)
				{
					// finishing
				}
			}).detach();
		}
	} catch (...)
	{
		// finishing
	}
}
