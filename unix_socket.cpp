#include "unix_socket.h"

#include "pipe.h"

#include <cstring>

namespace
{
	msghdr prepare_msghdr()
	{
		static char buffer[sizeof(int)];
		static char control[CMSG_SPACE(sizeof(int))];
		static iovec iov[1] = { iovec{ buffer, sizeof(buffer) } };
		return msghdr{ nullptr, 0, iov, 1, control, sizeof(control) };
	}

	msghdr prepare_fd_msghdr(int fd)
	{
		auto msg = prepare_msghdr();
		auto cmsg = CMSG_FIRSTHDR(&msg);
		cmsg->cmsg_len = CMSG_LEN(sizeof(int));
		cmsg->cmsg_level = SOL_SOCKET;
		cmsg->cmsg_type = SCM_RIGHTS;
		*reinterpret_cast<int*>(CMSG_DATA(cmsg)) = fd;
		return msg;
	}
}

unix_socket::unix_socket() = default;

void unix_socket::bind(const std::filesystem::path& p)
{
	const auto addr = prepare_sockaddr(p);
	check(::bind(sfd_, reinterpret_cast<const sockaddr*>(&addr), sizeof(decltype(addr))));
}

void unix_socket::connect(const std::filesystem::path& p)
{
	const auto addr = prepare_sockaddr(p);
	check(::connect(sfd_, reinterpret_cast<const sockaddr*>(&addr), sizeof(decltype(addr))));
}

void unix_socket::send(pipe_input s)
{
	send(prepare_fd_msghdr(s.fd_));
}

void unix_socket::send(pipe_output s)
{
	send(prepare_fd_msghdr(s.fd_));
}

void unix_socket::receive(pipe_input& s)
{
	auto msg = prepare_msghdr();
	receive(msg);
	s.fd_ = *reinterpret_cast<int*>(CMSG_DATA(CMSG_FIRSTHDR(&msg)));
}

void unix_socket::receive(pipe_output& s)
{
	auto msg = prepare_msghdr();
	receive(msg);
	s.fd_ = *reinterpret_cast<int*>(CMSG_DATA(CMSG_FIRSTHDR(&msg)));
}

void unix_socket::receive(msghdr& msg)
{
	check(recvmsg(sfd_, &msg, 0));
}

void unix_socket::send(const msghdr& msg)
{
	check(sendmsg(sfd_, &msg, 0));
}

unix_socket::unix_socket(int fd) : unix_base_socket(fd) { }
