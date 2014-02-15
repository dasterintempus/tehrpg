import os

if "mysql-rebuild" not in COMMAND_LINE_TARGETS:
	if "LD_LIBRARY_PATH" in os.environ:
		os.environ["LD_LIBRARY_PATH"] = "/home/dasterin/lib:%s" % os.environ["LD_LIBRARY_PATH"]
	else:
		os.environ["LD_LIBRARY_PATH"] = "/home/dasterin/lib"
else:
	if "gdb" in COMMAND_LINE_TARGETS or "run" in COMMAND_LINE_TARGETS:
		print "Cannot rebuild MySQL and also run/gdb due to LD_LIBRARY_PATH"
		Exit(1)

env = Environment(ENV = os.environ)

env.Replace(CC = "clang")
env.Replace(CXX = "clang++")

if ARGUMENTS.get("verbose", 0):
	env.Append(CXXFLAGS = "-v")

if ARGUMENTS.get("debug", 0) or "gdb" in COMMAND_LINE_TARGETS:
	env["MODE"] = "debug"
else:
	env["MODE"] = "release"

#server section
buildpath = os.path.join("build", env["MODE"], "tehrpg")

includedirs = ["/home/dasterin/include"]

for includedir in includedirs:
	env.Append(CXXFLAGS = "-I" + includedir)

libdirs = ["/home/dasterin/lib"]
env.Append(LIBPATH = libdirs)

libs = ["cryptopp", "mysqlcppconn-static", "boost_program_options", "sfml-system", "sfml-network"]
env.Append(LIBS = libs)

env.ParseConfig("mysql_config --libs")

if env["MODE"] == "debug":
	env.Append(CXXFLAGS = "-ggdb")
	env.Append(CXXFLAGS = "-O0")
elif env["MODE"] == "release":
	env.Append(CXXFLAGS = "-O3")

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

mysql = env.Command(source="./tehrpg.sql", target="tehrpg", action="@mysql -u tehrpg -ptur7tle $TARGET < $SOURCE")
env.AlwaysBuild(mysql)
env.Alias("mysql-rebuild", mysql)

env.Default(server)