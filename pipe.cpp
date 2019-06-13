#include "pipe.h"

#include <cstring>

#include <unistd.h>

namespace
{
	template<typename T>
	T check(T arg)
	{
		if (arg == -1)
		{
			throw pipe_error(std::strerror(errno));
		}
		return arg;
	}
}

pipe_input::pipe_input() noexcept : fd_(-1), count_(0) { }

pipe_input::pipe_input(int fd) noexcept : fd_(fd), count_(0) { }

pipe_input::~pipe_input()
{
	close();
}

pipe_input::pipe_input(pipe_input&& other) noexcept : fd_(other.fd_), count_(other.count_)
{
	other.fd_ = -1;
	other.count_ = 0;
}

pipe_input& pipe_input::operator=(pipe_input&& other) noexcept
{
	close();
	fd_ = other.fd_;
	other.fd_ = -1;
	return *this;
}

void pipe_input::swap(pipe_input& other) noexcept
{
	std::swap(fd_, other.fd_);
}

pipe_input& pipe_input::readsome(char* s, size_t count)
{
	count_ = check(::read(fd_, s, count));
	return *this;
}

size_t pipe_input::gcount() const
{
	return count_;
}

void pipe_input::close() noexcept
{
	::close(fd_);
}

pipe_output::pipe_output() noexcept : fd_(-1) { }

pipe_output::pipe_output(int fd) noexcept : fd_(fd) { }

pipe_output::~pipe_output()
{
	close();
}

pipe_output::pipe_output(pipe_output&& other) noexcept : fd_(other.fd_)
{
	other.fd_ = -1;
}

pipe_output& pipe_output::operator=(pipe_output&& other) noexcept
{
	close();
	fd_ = other.fd_;
	other.fd_ = -1;
	return *this;
}

void pipe_output::swap(pipe_output& other) noexcept
{
	std::swap(fd_, other.fd_);
}

pipe_output& pipe_output::write(const char* s, size_t count)
{
	while (count > 0)
	{
		const auto nwrote = check(::write(fd_, s, count));
		count -= nwrote;
		s += nwrote;
	}
	return *this;
}

void pipe_output::close() noexcept
{
	::close(fd_);
}

std::pair<pipe_input, pipe_output> open_pipe()
{
	int pipefd[2];
	check(pipe(pipefd));
	return { pipe_input(pipefd[0]), pipe_output(pipefd[1]) };
}