#include "window.h"
#include <sys/wait.h>
#include <pty.h>
#include <signal.h>
#ifdef XRAFT_UTMP
#include <cstring>
#include <pwd.h>
#include <utmp.h>
#endif

namespace
{

void f_terminate(int a_signal)
{
	try {
		f_application()->f_exit();
	} catch (std::runtime_error&) {
	}
}

void f_wait(int a_signal)
{
	wait(NULL);
	try {
		f_application()->f_exit();
	} catch (std::runtime_error&) {
	}
}

#ifdef XRAFT_UTMP

uid_t v_euid = geteuid();
gid_t v_egid = getegid();

void f_log(struct utmp& a_utmp)
{
	seteuid(v_euid);
	setegid(v_egid);
	setutent();
	pututline(&a_utmp);
	endutent();
	updwtmp("/var/log/wtmp", &a_utmp);
	seteuid(getuid());
	setegid(getgid());
}

void f_login(t_application& a_application, struct utmp& a_utmp, int a_master)
{
	a_utmp.ut_type = UT_UNKNOWN;
	a_utmp.ut_pid = getpid();
	const char* line = ptsname(a_master);
	if (!line || std::strncmp(line, "/dev/", 5) != 0) return;
	line += 5;
	std::strncpy(a_utmp.ut_line, line, UT_LINESIZE);
	if (std::strncmp(line, "pts/", 4) == 0) {
		a_utmp.ut_id[0] = 'p';
		std::strncpy(a_utmp.ut_id + 1, line + 4, 3);
	} else if (std::strncmp(line, "tty", 3) == 0) {
		std::strncpy(a_utmp.ut_id, line + 3, 4);
	} else {
		return;
	}
	uid_t uid = getuid();
	struct passwd* passwd = getpwuid(uid);
	if (passwd)
		std::strncpy(a_utmp.ut_user, passwd->pw_name, UT_NAMESIZE);
	else
		std::snprintf(a_utmp.ut_user, UT_NAMESIZE, "%d", uid);
	std::strncpy(a_utmp.ut_host, DisplayString(a_application.f_x11_display()), UT_HOSTSIZE);
	a_utmp.ut_time = time(NULL);
	a_utmp.ut_addr = 0;
	a_utmp.ut_type = USER_PROCESS;
	f_log(a_utmp);
}

void f_logout(struct utmp& a_utmp)
{
	if (a_utmp.ut_type != USER_PROCESS) return;
	a_utmp.ut_type = DEAD_PROCESS;
	std::memset(a_utmp.ut_line, 0, UT_LINESIZE);
	std::memset(a_utmp.ut_user, 0, UT_NAMESIZE);
	std::memset(a_utmp.ut_host, 0, UT_HOSTSIZE);
	a_utmp.ut_time = 0;
	f_log(a_utmp);
}

#endif

int f_main(int argc, char* argv[], int a_master)
{
	struct sigaction action;
	action.sa_flags = 0;
	action.sa_handler = f_terminate;
	sigaction(SIGTERM, &action, NULL);
	sigaction(SIGINT, &action, NULL);
	sigaction(SIGQUIT, &action, NULL);
	sigaction(SIGHUP, &action, NULL);
	action.sa_handler = f_wait;
	sigaction(SIGCHLD, &action, NULL);
	std::setlocale(LC_ALL, "");
	std::vector<std::string> arguments(argv, argv + argc);
	t_application application(arguments);
#ifdef XRAFT_UTMP
	struct utmp utmp;
	f_login(application, utmp, a_master);
#endif
	t_pointer<t_content> content = new t_content(192, 80, 24, a_master);
	t_pointer<t_pane> pane = new t_pane(content);
	application.f_add(pane);
	pane->f_hints(argv[0]);
	pane->f_show();
	application.f_run();
#ifdef XRAFT_UTMP
	f_logout(utmp);
#endif
	return 0;
}

}

int main(int argc, char* argv[])
{
#ifdef XRAFT_UTMP
	seteuid(getuid());
	setegid(getgid());
#endif
	int master;
	switch (forkpty(&master, NULL, NULL, NULL)) {
	case -1:
		return 1;
	case 0:
		{
			struct sigaction action;
			action.sa_flags = 0;
			action.sa_handler = SIG_DFL;
			sigaction(SIGINT, &action, NULL);
			sigaction(SIGQUIT, &action, NULL);
			const char* shell = std::getenv("SHELL");
			if (shell == NULL) shell = "/bin/sh";
			setenv("TERM", "xterm", 1);
			return execlp(shell, shell, NULL);
		}
	default:
		{
			int n = f_main(argc, argv, master);
			close(master);
			return n;
		}
	}
}
