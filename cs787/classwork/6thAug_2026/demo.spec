# Test spec for tool.cpp: covers 0, 1 and 2 arguments, strings and void.
service Demo
port 9191
method calculate int int
method add int int int
method greet std::string std::string
method ping int
method reset void
