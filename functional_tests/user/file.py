import requests
import unittest
import json
from subprocess import call
import inspect, os
import datetime

class TestFile(unittest.TestCase):
	
	def setUp(self):
		call(["rm", "-rf", os.path.dirname(os.path.abspath(inspect.getfile(inspect.currentframe())))+"/../../testdb"])
	
	def _signup_and_login(self, email="testemail"):	
		payload = {"email": email, "password": "testpassword"}
		r = requests.get("http://localhost:8000/users", params = payload)

		payload = {"email": email, "password": "testpassword"}
		r = requests.get("http://localhost:8000/login", params = payload)
		return r.json()["token"]
	
	def _save_new_file(self, token):
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
		return r.json()["fileID"]

	def test_save_new_file_then_get(self):
		token = self._signup_and_login()

		fileid = self._save_new_file(token)
		
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
		
	def test_save_new_file_wrong_token(self):
		token = self._signup_and_login()

		payload = {
			"email":		"testemail",
			"token":		"asd",
			"name":			"somefilename",
			"extension":	".txt",
			"path":			"root",
			"tags":			["palabra1","palabra2"]
		}
		r = requests.post("http://localhost:8000/files", json = payload)
		
		self.assertFalse(r.json()["result"])
		
	def test_get_file_wrong_id(self):
		token = self._signup_and_login()

		fileid = self._save_new_file(token)
		
		payload = {
			"email":		"testemail",
			"token":		token,
			"id":			fileid+1
		}
		r = requests.get("http://localhost:8000/files", params = payload)
		self.assertFalse(r.json()["result"])
		self.assertEqual(["The user has no permits for specified file."], r.json()["errors"])
		self.assertEqual(2, len(r.json()))
		
	def test_get_file_not_authorized(self):
		token = self._signup_and_login()
		token2 = self._signup_and_login("testemail2")

		fileid = self._save_new_file(token)
		
		payload = {
			"email":		"testemail2",
			"token":		token2,
			"id":			fileid
		}
		r = requests.get("http://localhost:8000/files", params = payload)
		self.assertFalse(r.json()["result"])
		self.assertEqual(["The user has no permits for specified file."], r.json()["errors"])
		self.assertEqual(2, len(r.json()))

if __name__ == '__main__':
    unittest.main()
