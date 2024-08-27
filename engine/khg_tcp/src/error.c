#include "khg_tcp/error.h"

#if defined(_WIN32) || defined(_WIN64)
#include <winsock2.h>
#endif

#include <stddef.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>

static tcp_error_callback_fn tcp_error_callback = NULL;
static void *tcp_user_data = NULL;

void tcp_set_error_callback(tcp_error_callback_fn error_callback, void *user_data) {
	tcp_error_callback = error_callback;
	tcp_user_data = user_data;
}

tcp_error tcp_get_last_error() {
#if defined(_WIN32) || defined(_WIN64)
	return (stcp_error) WSAGetLastError();
#else
	return (tcp_error) errno;
#endif
}

void tcp_raise_error(tcp_error err) {
	if (tcp_error_callback) {
		tcp_error_callback(err, tcp_user_data);
  }
}

const char *tcp_error_to_string(tcp_error err) {
	switch(err) {
    case TCP_NO_ERROR:
      return "No error";
    case TCP_EINTR:
      return strerror(EINTR);
    case TCP_EBADF:
      return strerror(EBADF);
    case TCP_EACCES:
      return strerror(EACCES);
    case TCP_EFAULT:
      return strerror(EFAULT);
    case TCP_EINVAL:
      return strerror(EINVAL);
    case TCP_EMFILE:
      return strerror(EMFILE);
    case TCP_EWOULDBLOCK:
      return strerror(EWOULDBLOCK);
    case TCP_EINPROGRESS:
      return strerror(EINPROGRESS);
    case TCP_EALREADY:
      return strerror(EALREADY);
    case TCP_ENOTSOCK:
      return strerror(ENOTSOCK);
    case TCP_EDESTADDRREQ:
      return strerror(EDESTADDRREQ);
    case TCP_EMSGSIZE:
      return strerror(EMSGSIZE);
    case TCP_EPROTOTYPE:
      return strerror(EPROTOTYPE);
    case TCP_ENOPROTOOPT:
      return strerror(ENOPROTOOPT);
    case TCP_EPROTONOSUPPORT:
      return strerror(EPROTONOSUPPORT);
    case TCP_ESOCKTNOSUPPORT:
      return "Socket not supported";
    case TCP_EOPNOTSUPP:
      return strerror(EOPNOTSUPP);
    case TCP_EPFNOSUPPORT:
      return "Protocol family not supported";
    case TCP_EAFNOSUPPORT:
      return strerror(EAFNOSUPPORT);
    case TCP_EADDRINUSE:
      return strerror(EADDRINUSE);
    case TCP_EADDRNOTAVAIL:
      return strerror(EADDRNOTAVAIL);
    case TCP_ENETDOWN:
      return strerror(ENETDOWN);
    case TCP_ENETUNREACH:
      return strerror(ENETUNREACH);
    case TCP_ENETRESET:
      return strerror(ENETRESET);
    case TCP_ECONNABORTED:
      return strerror(ECONNABORTED);
    case TCP_ECONNRESET:
      return strerror(ECONNRESET);
    case TCP_ENOBUFS:
      return strerror(ENOBUFS);
    case TCP_EISCONN:
      return strerror(EISCONN);
    case TCP_ENOTCONN:
      return strerror(ENOTCONN);
    case TCP_ESHUTDOWN:
      return "ESHUTDOWN";
    case TCP_ETOOMANYREFS:
      return "ETOOMANYREFS";
    case TCP_ETIMEDOUT:
      return "Connection timed out";
    case TCP_ECONNREFUSED:
      return strerror(ECONNREFUSED);
    case TCP_ELOOP:
      return strerror(ELOOP);
    case TCP_ENAMETOOLONG:
      return strerror(ENAMETOOLONG);
    case TCP_EHOSTDOWN:
      return "Host is down";
    case TCP_EHOSTUNREACH:
      return strerror(EHOSTUNREACH);
    case TCP_ENOTEMPTY:
      return strerror(ENOTEMPTY);
    case TCP_EPROCLIM:
      return "Too many processes";
    case TCP_EUSERS:
      return "Too many users";
    case TCP_EDQUOT:
      return "EDQUOT";
    case TCP_ESTALE:
      return "ESTALE";
    case TCP_EREMOTE:
      return "EREMOTE";
    default:
      return "Unknown TCP error";
	}
}

void tcp_print_error(tcp_error e) {
	fprintf(stderr, "TCP error: %s\n", tcp_error_to_string(e));
}

void tcp_fail(tcp_error err, const char *file, int line) {
	tcp_print_error(err);
	fprintf(stderr, "File: %s\nLine: %d\n", file, line);
	fflush(stderr);
	exit(-1);
}

