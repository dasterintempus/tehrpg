#!/usr/bin/env python

import json
import os.path
import os
import sys
import traceback

lineparsefuncs = {}

def parse_set(line):
	path = line.split(" ")[1].lower()
	try:
		size = int(line.split(" ")[2])
	except ValueError:
		return "err", ("Invalid size specified on SET",)
	return (path, size)
lineparsefuncs["set"] = parse_set

def parse_get(line):
	path = line.split(" ")[1].lower()
	return (path,)
lineparsefuncs["get"] = parse_get

def parse_list(line):
	path = line.split(" ")[1].lower()
	return (path,)
lineparsefuncs["list"] = parse_list

def parse_line(line):
	verb = line.split(" ")[0].lower()
	try:
		func = lineparsefuncs[word]
		params = func(line)
		return verb, params
	except KeyError:
		return "err", ("Invalid verb",)

def sanitize_path(path):
	if path[0] == "/":
		path = path[1:]
	path = path.replace("../", "")
	return path

def ensure_directory(path):
	if path[-1] == "/":
		path = path[:-1]
	leading, trailing = os.path.split(path)
	if leading == "":
		return
	ensure_directory(leading)
	if !os.path.exists(path):
		os.mkdir(path)

linehandlefuncs = {}

def handle_set(pipe, params):
	path = params[0]
	path = sanitize_path(path)
	path = os.path.join(datapath, path)
	size = params[1]
	rawdata = pipe.read(size)
	try:
		data = json.loads(rawdata)
	except ValueError:
		return "err", ("Invalid JSON",)
	ensure_directory(path)
	with open(path, "w") as outf:
		outf.write(rawdata)
linehandlefuncs["set"] = handle_set

def handle_get(pipe, params):
	path = params[0]
	path = sanitize_path(path)
	path = os.path.join(datapath, path)
	rawdata = ""
	with open(path, "r") as inf:
		rawdata = inf.read()
	pipe.write("%s\n%s\n" % (len(rawdata), rawdata))
linehandlefuncs["get"] = handle_get

def handle_list(pipe, params):
	path = params[0]
	path = sanitize_path(path)
	path = os.path.join(datapath, path)
	rawdata = json.dumps(os.list(path))
	pipe.write("%s\n%s\n" % (len(rawdata), rawdata))
linehandlefuncs["list"] = handle_list

if __name__ == "__main__":
	pipepath = sys.argv[1]
	global datapath
	datapath = sys.argv[2]
	if !os.path.exists(pipepath):
		os.mkfifo(pipepath)
	try:
		with open(pipepath, "rw") as pipe:
			for line in pipe:
				verb, params = parse_line(line)
				if verb == "err":
					pipe.write("ERROR: %s\n" % params[0])
				else:
					try:
						func = linehandlefuncs[verb]
						func(pipe, params)
					except KeyError:
						pipe.write("ERROR: Unknown verb for handlers\n")
	except:
		sys.stderr.write(traceback.format_exc())
	finally:
		os.unlink(pipepath)