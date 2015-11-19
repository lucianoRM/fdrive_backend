import multiprocessing
import requests
import unittest
import json
from subprocess import call
import inspect, os
import datetime
import filecmp

def crawl(id, email, token, result_queue):
	for i in range(5):
		tc = unittest.TestCase('__init__')
		
		filename = "somefilename_"+str(id)+"_"+str(i)
		
		
		payload = {
			"email":		email,
			"token":		token,
			"name":			filename,
			"extension":	".txt",
			"path":			"root",
			"tags":			["palabra1","palabra2"],
			"size":			1		# En MB.
		}
		r = requests.post("http://localhost:8000/files/metadata", json = payload)
		if (
		(r.json()["result"] is not True) or
		("fileID" not in r.json()) or
		(r.json()["fileID"] is None) or
		(r.json()["version"] != 0)
		):
			result_queue.put("error")
			raise ValueError()
			
		fileid = r.json()["fileID"]
		r = requests.get("http://localhost:8000/files/"+str(fileid)+"/metadata", params = payload)
		
		modificationTime = datetime.datetime.strptime(r.json()["file"]["lastModified"], "%a %b %d %H:%M:%S %Y")
		if (
		(r.json()["result"] != True) or
		(r.json()["file"]["extension"] != ".txt") or
		(r.json()["file"]["id"] != fileid) or
		(modificationTime + datetime.timedelta(minutes=1) <= modificationTime) or
		(modificationTime - datetime.timedelta(minutes=1) >= modificationTime) or
		(r.json()["file"]["lastUser"] != "testemail") or
		(r.json()["file"]["name"] != filename) or
		(r.json()["file"]["owner"] != "testemail") or
		(r.json()["file"]["tags"] != ["palabra1","palabra2"]) or
		(r.json()["file"]["lastVersion"] != 0)
		):
			result_queue.put("error")
			raise ValueError()
		
	result_queue.put("ok")

r = requests.post("http://localhost:8000/cleandb")

processs = []
result_queue = multiprocessing.Queue()

email = "testemail"
payload = {"email": email, "password": "testpassword"}
r = requests.post("http://localhost:8000/users", params = payload)
payload = {"email": email, "password": "testpassword"}
r = requests.get("http://localhost:8000/login", params = payload)
token = r.json()["token"]

processCount = 75
for n in range(processCount): # start 75 processes crawling for the result
    process = multiprocessing.Process(target=crawl, args=[n, email, token, result_queue])
    process.start()
    processs.append(process)

result = result_queue.get() # waits until any of the proccess have `.put()` a result

for process in processs: # then kill them all off
    process.terminate()

print "Got result:", result
if (result == "ok"):
	exit(0)
exit(1)
