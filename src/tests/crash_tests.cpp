#include <QtCore/QCoreApplication>

#include <attendant/at_common.h>
#include <attendant/at_log.h>


#include <stdio.h>
#include <execinfo.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>


void make_segfault() {
	int *foo = (int*)-1; // make a bad pointer
	printf("%d\n", *foo);       // causes segfault
}

void on_segfault(int sig) {
  void *array[10];
  size_t size;

  // get void*'s for all entries on the stack
  size = backtrace(array, 10);

  // print out all the frames to stderr
  fprintf(stderr, "Error: signal %d:\n", sig);
  backtrace_symbols_fd(array, size, STDERR_FILENO);
  exit(1);
}

void bar() { make_segfault(); }
void foo() { bar(); }

int main(int argc, char** argv)
{
	signal(SIGSEGV, on_segfault);   // install our handler

	QCoreApplication app(argc, argv);

	AT_LOG << app.applicationFilePath() << " started";
	foo(); // this will call foo, bar, and baz.  baz segfaults.

	return app.exec();
}
