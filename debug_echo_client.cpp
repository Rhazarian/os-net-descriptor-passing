#include "debug_echo_client.h"

#include "unix_socket.h"
#include "pipe.h"

#include <vector>
#include <iterator>

void debug_echo_client::run(const std::filesystem::path& p, const char* msg, size_t length, uint16_t requests, std::ostream& log_ostream)
{
	unix_socket socket;
	socket.set_so_timeout(1000);
	socket.connect(p);
	auto[server_in, out] = open_pipe();
	socket.send(std::move(server_in));
	auto[in, server_out] = open_pipe();
	socket.send(std::move(server_out));
	std::vector<char> buffer(length);
	for (auto i = requests; i--; )
	{
		log_ostream << "Processing request " << requests - i << "..." << std::endl;
		try {
			log_ostream << "Sending..." << std::endl;
			out.write(msg, length);
			log_ostream << "Sent: \n";
			std::copy(msg, msg + length, std::ostream_iterator<char>(log_ostream));
			log_ostream << "\nWaiting for an answer..." << std::endl;
			in.readsome(buffer.data(), length);
			log_ostream << "Received answer: \n";
			std::copy(buffer.data(), buffer.data() + in.gcount(), std::ostream_iterator<char>(log_ostream));
			log_ostream << std::endl;
		} catch (const unix_socket::socket_error& ex)
		{
			log_ostream << "Error: " << ex.what() << std::endl;
		}
	}
}