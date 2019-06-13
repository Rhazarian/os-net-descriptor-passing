#ifndef PIPE_H
#define PIPE_H

#include <stdexcept>
#include <iostream>

struct pipe_error : std::runtime_error
{
	using std::runtime_error::runtime_error;
};

struct pipe_output;

struct pipe_input
{
	friend std::pair<pipe_input, pipe_output> open_pipe();

	pipe_input() noexcept;
	~pipe_input();

	pipe_input(const pipe_input& other) = delete;
	pipe_input& operator=(const pipe_input& other) = delete;

	pipe_input(pipe_input&& other) noexcept;
	pipe_input& operator=(pipe_input&& other) noexcept;

	void swap(pipe_input& other) noexcept;

	pipe_input& readsome(char* s, size_t count);
	size_t gcount() const;

	void close() noexcept;

	friend struct unix_socket;

//private:
	explicit pipe_input(int fd) noexcept;

	int fd_;
	size_t count_;
};

struct pipe_output
{
	friend std::pair<pipe_input, pipe_output> open_pipe();

	pipe_output() noexcept;
	~pipe_output();

	pipe_output(const pipe_output& other) = delete;
	pipe_output& operator=(const pipe_output& other) = delete;

	pipe_output(pipe_output&& other) noexcept;
	pipe_output& operator=(pipe_output&& other) noexcept;

	void swap(pipe_output& other) noexcept;

	pipe_output& write(const char* s, size_t count);

	void close() noexcept;

	friend struct unix_socket;

//private:
	explicit pipe_output(int fd) noexcept;

	int fd_;
};

std::pair<pipe_input, pipe_output> open_pipe();

#endif // PIPE_H