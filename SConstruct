import os
import os.path

if "mysql-rebuild" not in COMMAND_LINE_TARGETS:
	if "LD_LIBRARY_PATH" in os.environ:
		os.environ["LD_LIBRARY_PATH"] = "/home/dasterin/lib:%s" % os.environ["LD_LIBRARY_PATH"]
	else:
		os.environ["LD_LIBRARY_PATH"] = "/home/dasterin/lib"
else:
	if "gdb" in COMMAND_LINE_TARGETS or "run" in COMMAND_LINE_TARGETS:
		print "Cannot rebuild MySQL database and also run/gdb due to LD_LIBRARY_PATH"
		Exit(1)

env = Environment(ENV = os.environ)

env.Replace(CC = "clang")
env.Replace(CXX = "clang++")

env.Append(CXXFLAGS = "-std=c++11")

if ARGUMENTS.get("verbose", 0):
	env.Append(CXXFLAGS = "-v")

if ARGUMENTS.get("debug", 0) or "gdb" in COMMAND_LINE_TARGETS:
	env["MODE"] = "debug"
else:
	env["MODE"] = "release"

buildpath = os.path.join("build", env["MODE"], "tehrpg")

includedirs = []
includedirs = [os.path.expanduser("~/include"), os.path.expanduser("~/Qt/include")]
env.Append(CXXFLAGS="-fPIC")
env.Append(CPPPATH = includedirs)

libdirs = []
libdirs = [os.path.expanduser("~/lib"), os.path.expanduser("~/Qt/lib")]
env.Append(LIBPATH = libdirs)

if env["MODE"] == "debug":
	env.Append(CXXFLAGS = "-ggdb")
	if "memcheck" in COMMAND_LINE_TARGETS:
		env.Append(CXXFLAGS = "-O1")
	else:
		env.Append(CXXFLAGS = "-O0")
	env.Append(CXXFLAGS = "-DTEHDEBUG")
elif env["MODE"] == "release":
	env.Append(CXXFLAGS = "-O3")

env.ParseConfig("mysql_config --libs")

libs = ["cryptopp", "mysqlcppconn-static", "sfml-system", "sfml-network", "boost_regex", "boost_program_options", "lua"]
env.Append(LIBS = libs)

sources = Glob("src/*.cpp")
server = env.Program(target=buildpath, source=sources)

run = env.Command(source=buildpath, target="cerr.log", action="$SOURCE 2> $TARGET")
#env.Depends(server, run)
env.AlwaysBuild(run)
env.Alias("run", run)

gdb = env.Command(source=buildpath, target="gdbcerr.log", action="gdb -ex 'run 2> $TARGET' $SOURCE")
#env.Depends(server, gdb)
env.AlwaysBuild(gdb)
env.Alias("gdb", gdb)

memcheck = env.Command(source=buildpath, target="memcheck.log", action="valgrind --leak-check=full --show-reachable=yes --num-callers=20 $SOURCE 1>$TARGET 2>&1")
env.AlwaysBuild(memcheck)
env.Alias("memcheck", memcheck)

mysql = env.Command(source="./tehrpg.sql", target="tehrpg", action="@mysql -u tehrpg -ptur7tle $TARGET < $SOURCE")
env.AlwaysBuild(mysql)
env.Alias("mysql-rebuild", mysql)

env.Default(server)
