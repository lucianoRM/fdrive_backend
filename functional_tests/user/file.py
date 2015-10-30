import requests
import unittest
import json
from subprocess import call
import inspect, os
import datetime

class TestFile(unittest.TestCase):
	
	def setUp(self):
		call(["rm", "-rf", os.path.dirname(os.path.abspath(inspect.getfile(inspect.currentframe())))+"/../../testdb"])

	def test_save_new_file(self):
		payload = {"email": "testemail", "password": "testpassword"}
		r = requests.get("http://localhost:8000/users", params = payload)

		payload = {"email": "testemail", "password": "testpassword"}
		r = requests.get("http://localhost:8000/login", params = payload)

		token = r.json()["token"]

		payload = {
			"email":		"testemail",
			"token":		token,
			"name":			"somefilename",
			"extension":	".txt",
			"path":			"root",
			"tags":			["palabra1","palabra2"]
		}
		r = requests.post("http://localhost:8000/files", json = payload)

		self.assertEqual(True, r.json()["result"])
		self.assertIn("fileID", r.json())
		self.assertIsNotNone(r.json()["fileID"])
		
		fileid = r.json()["fileID"]
		
		payload = {
			"email":		"testemail",
			"token":		token,
			"id":			fileid
		}
		r = requests.get("http://localhost:8000/files", params = payload)
		
		self.assertEqual(".txt", r.json()["extension"])
		self.assertEqual(fileid, r.json()["id"])
		modificationTime = datetime.datetime.strptime(r.json()["lastModified"], "%a %b %d %H:%M:%S %Y")
		self.assertGreater(modificationTime + datetime.timedelta(minutes=1), modificationTime)
		self.assertLess(modificationTime - datetime.timedelta(minutes=1), modificationTime)
		self.assertEqual("testemail", r.json()["lastUser"])
		self.assertEqual("somefilename", r.json()["name"])
		self.assertEqual("testemail", r.json()["owner"])
		self.assertEqual(["palabra1","palabra2"], r.json()["tags"])
		

if __name__ == '__main__':
    unittest.main()
