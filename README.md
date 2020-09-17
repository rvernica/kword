This repository implements an operator called `kword`. The operator
takes two named arguments (`foo` and `bar`) of type `string`. The
operator returns an empty string with the `<foo:string NOT NULL>
[bar=0:0:0:1]` schema. The operator exposes three different issues,
listed below.

```
> scidb --version
SciDB Version: 19.11.5
Build Type: RelWithDebInfo
Commit: f8334b6
Copyright (C) 2008-2019 SciDB, Inc.

# Compile the code and install the resulting library file

> iquery --afl --query "load_library('kword')"
Query was executed successfully

> iquery --afl --query "kword()"
{No} foo
> iquery --afl --query "kword(foo:'1')"
{No} foo
> iquery --afl --query "kword(foo:'1', bar:'2')"
{No} foo
> iquery --afl --query "kword(bar:'2')"
{No} foo


# -- - Issue 1 - --

> iquery --afl --query "kword(1, foo:'1', bar:'2')"
SystemException in file: src/network/BaseConnection.h function: receive line: 168
Error id: scidb::SCIDB_SE_NETWORK::SCIDB_LE_CANT_SEND_RECEIVE
Error description: Network error. Cannot receive network message: Read failed: End of file (asio.misc:2).  An instance may be offline..

> iquery --afl --query "kword(1, bar:'2')"
SystemException in file: src/network/BaseConnection.h function: receive line: 168
Error id: scidb::SCIDB_SE_NETWORK::SCIDB_LE_CANT_SEND_RECEIVE
Error description: Network error. Cannot receive network message: Read failed: End of file (asio.misc:2).  An instance may be offline..


# -- - Issue 2 - --

> iquery --afl --query "kword(1)"
UserQueryException in file: src/query/parser/Translator.cpp function: matchParamsByRecognizer line: 1109 instance: s0-i0 (0)
Error id: scidb::SCIDB_SE_SYNTAX::SCIDB_LE_UNEXPECTED_OPERATOR_ARGUMENT
Error description: Query syntax error. Operator 'kword' does not take an argument but was passed '1'.
kword(1)
[ Large number of blank spaces / empty lines! ]


# -- - Issue 3 - --

> iquery --afl --query "kword('1', foo:'1', bar:'2')"
Hangs!
```

`gdb` attached to the `scidb` process shows the following backtrace:
```
> iquery --afl --query "kword(1, foo:'1', bar:'2')"
> gdb scidb 4926
GNU gdb (Ubuntu 7.11.1-0ubuntu1~16.5) 7.11.1
Copyright (C) 2016 Free Software Foundation, Inc.
License GPLv3+: GNU GPL version 3 or later <http://gnu.org/licenses/gpl.html>
This is free software: you are free to change and redistribute it.
There is NO WARRANTY, to the extent permitted by law.  Type "show copying"
and "show warranty" for details.
This GDB was configured as "x86_64-linux-gnu".
Type "show configuration" for configuration details.
For bug reporting instructions, please see:
<http://www.gnu.org/software/gdb/bugs/>.
Find the GDB manual and other documentation resources online at:
<http://www.gnu.org/software/gdb/documentation/>.
For help, type "help".
Type "apropos word" to search for commands related to "word"...
Reading symbols from scidb...(no debugging symbols found)...done.
Attaching to program: /opt/scidb/19.11/bin/scidb, process 4926
[New LWP 4930]
[New LWP 4932]
[New LWP 4933]
[New LWP 4934]
[New LWP 4935]
[New LWP 4936]
[New LWP 4937]
[New LWP 4945]
[New LWP 4946]
[New LWP 4947]
[Thread debugging using libthread_db enabled]
Using host libthread_db library "/lib/x86_64-linux-gnu/libthread_db.so.1".
conti0x00007f00585abad3 in epoll_wait () at ../sysdeps/unix/syscall-template.S:84
84	../sysdeps/unix/syscall-template.S: No such file or directory.
(gdb) continue
Continuing.

Thread 5 "SciDB-0-0-scidb" received signal SIGSEGV, Segmentation fault.
[Switching to Thread 0x7f004c3e0700 (LWP 4934)]
0x0000000000b48892 in ?? ()
(gdb) backtrace
#0  0x0000000000b48892 in ?? ()
#1  0x0000000000b725a4 in scidb::parser::Translator::matchParamsByRecognizer(std::shared_ptr<scidb::LogicalOperator>, scidb::parser::Node const*, std::shared_ptr<std::map<std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> >, scidb::PlistInfo, std::less<std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> > >, std::allocator<std::pair<std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> > const, scidb::PlistInfo> > > const>&, scidb::PointerRange<scidb::parser::Node* const>, std::vector<scidb::ArrayDesc, std::allocator<scidb::ArrayDesc> >&, std::vector<std::shared_ptr<scidb::LogicalQueryPlanNode>, std::allocator<std::shared_ptr<scidb::LogicalQueryPlanNode> > >&, unsigned long) ()
#2  0x0000000000b72977 in scidb::parser::Translator::passAFLOperator(scidb::parser::Node const*, unsigned long) ()
#3  0x0000000000b73515 in scidb::parser::Translator::AstToLogicalPlan(scidb::parser::Node const*, unsigned long, bool) ()
#4  0x0000000000b7a425 in scidb::parser::translatePlan(scidb::parser::Factory&, scidb::parser::Log&, std::shared_ptr<std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> > > const&, scidb::parser::Node*, std::shared_ptr<scidb::Query> const&) ()
#5  0x0000000000b40d35 in scidb::parseStatement(std::shared_ptr<scidb::Query> const&, bool) ()
#6  0x0000000000af8e7e in scidb::QueryProcessorImpl::parseLogical(std::shared_ptr<scidb::Query> const&, bool) ()
#7  0x0000000000b1f1a1 in scidb::SciDBExecutor::prepareQuery(std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> > const&, bool, std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> > const&, scidb::QueryResult&, void*) ()
#8  0x000000000099ad1e in scidb::ClientMessageHandleJob::prepareClientQuery() ()
#9  0x0000000000997da1 in scidb::ClientMessageHandleJob::run() ()
#10 0x00000000009d0bb6 in scidb::Job::executeOnQueue(std::weak_ptr<scidb::WorkQueue>&, std::shared_ptr<scidb::SerializationCtx>&) ()
#11 0x00000000009d0dc4 in scidb::Job::execute() ()
#12 0x00000000009dea04 in scidb::Thread::_threadFunction() ()
#13 0x00000000009df649 in scidb::Thread::threadFunction(void*) ()
#14 0x00007f005a71e6ba in start_thread (arg=0x7f004c3e0700) at pthread_create.c:333
#15 0x00007f00585ab4dd in clone () at ../sysdeps/unix/sysv/linux/x86_64/clone.S:109
```
