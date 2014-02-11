import glob
import os
import subprocess
import os.path

env = Environment(ENV = {'PATH' : os.environ['PATH']})

env.Replace(CC = "clang")
env.Replace(CXX = "clang++")

if ARGUMENTS.get("verbose", 0):
  env.Append(CXXFLAGS = "-v")

if ARGUMENTS.get('debug', 0):
  env["MODE"] = "debug"
else:
  env["MODE"] = "release"

includedirs = ["/home/dasterin/include"]

for includedir in includedirs:
  env.Append(CXXFLAGS = "-I" + includedir)

libdirs = ["/home/dasterin/lib"]
env.Append(LIBPATH = libdirs)

libs = ["cryptopp", "mysqlcppconn-static", "boost_program_options", "sfml-system", "sfml-network"]
env.Append(LIBS = libs)

mysql_config = subprocess.check_output(["mysql_config", "--libs"])
env.Append(LINKFLAGS = mysql_config)

if env["MODE"] == "debug":
  env.Append(CXXFLAGS = "-ggdb")
  env.Append(CXXFLAGS = "-O0")
elif env["MODE"] == "release":
  env.Append(CXXFLAGS = "-O3")

#server section
serverbuildpath = os.path.join("build", env["MODE"], "tehmud")

serversources = glob.glob("src/serv/*.cpp")

server = env.Program(target=serverbuildpath, source=serversources)

env.Default(server)
