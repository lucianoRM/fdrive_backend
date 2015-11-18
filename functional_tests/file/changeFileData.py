import requests
import unittest
import json
from subprocess import call
import inspect, os
import datetime
import filecmp

class TestChangeFileData(unittest.TestCase):
	
	def setUp(self):
		r = requests.post("http://localhost:8000/cleandb")
	
	def _signup_and_login(self, email="testemail"):	
		payload = {"email": email, "password": "testpassword"}
		r = requests.post("http://localhost:8000/users", params = payload)

		payload = {"email": email, "password": "testpassword"}
		r = requests.get("http://localhost:8000/login", params = payload)
		return r.json()["token"]
	
	def _save_new_file(self, token, filename, email = "testemail"):
		payload = {
			"email":		email,
			"token":		token,
			"name":			filename,
			"extension":	".txt",
			"path":			"root",
			"tags":			["palabra1","palabra2"],
			"size":			2		# En MB.
		}
		r = requests.post("http://localhost:8000/files/metadata", json = payload)
		self.assertEqual(True, r.json()["result"])
		self.assertIn("fileID", r.json())
		self.assertIsNotNone(r.json()["fileID"])
		self.assertEqual(0, r.json()["version"])
		return r.json()["fileID"]

	def test_change_name(self):
		token = self._signup_and_login("email")
		file_id = self._save_new_file(token, "file", "email")
		payload = {
			"email":		"email",
			"token":		token,
			"name":			"nuevoNombre"
		}
		r = requests.put("http://localhost:8000/files/"+str(file_id)+"/metadata", json = payload)
		self.assertEqual(True, r.json()["result"])
		payload = {
			"email":		"email",
			"token":		token
		}
		r = requests.get("http://localhost:8000/files/"+str(file_id)+"/metadata", params = payload)
		self.assertEqual(True, r.json()["result"])
		self.assertEqual("nuevoNombre", r.json()["file"]["name"])
		payload = {
			"email":		"email",
			"token":		token,
			"name":			"file",
			"extension":	".txt",
			"path":			"root",
			"tags":			["palabra1","palabra2"],
			"size":			2		# En MB.
		}
		r = requests.post("http://localhost:8000/files/metadata", json = payload)
		self.assertTrue(r.json()["result"])	# Leaves old name available
		payload = {
			"email":		"email",
			"token":		token,
			"name":			"nuevoNombre",
			"extension":	".txt",
			"path":			"root",
			"tags":			["palabra1","palabra2"],
			"size":			2		# En MB.
		}
		r = requests.post("http://localhost:8000/files/metadata", json = payload)
		self.assertFalse(r.json()["result"])	# Leaves new name unavailable
		
	def test_add_tag(self):
		token = self._signup_and_login("email")
		file_id = self._save_new_file(token, "file", "email")
		payload = {
			"email":		"email",
			"token":		token,
			"tag":			"nuevoTag"
		}
		r = requests.put("http://localhost:8000/files/"+str(file_id)+"/metadata", json = payload)
		self.assertEqual(True, r.json()["result"])
		payload = {
			"email":		"email",
			"token":		token
		}
		r = requests.get("http://localhost:8000/files/"+str(file_id)+"/metadata", params = payload)
		self.assertEqual(True, r.json()["result"])
		self.assertEqual(["palabra1", "palabra2", "nuevoTag"], r.json()["file"]["tags"])
		
	def test_change_name_and_tag(self):
		token = self._signup_and_login("email")
		file_id = self._save_new_file(token, "file", "email")
		payload = {
			"email":		"email",
			"token":		token,
			"id":			file_id,
			"tag":			"nuevoTag",
			"name":			"nuevoNombre"
		}
		r = requests.put("http://localhost:8000/files/"+str(file_id)+"/metadata", json = payload)
		self.assertEqual(True, r.json()["result"])
		payload = {
			"email":		"email",
			"token":		token
		}
		r = requests.get("http://localhost:8000/files/"+str(file_id)+"/metadata", params = payload)
		self.assertEqual(True, r.json()["result"])
		self.assertEqual("nuevoNombre", r.json()["file"]["name"])
		self.assertEqual(["palabra1", "palabra2", "nuevoTag"], r.json()["file"]["tags"])
		payload = {
			"email":		"email",
			"token":		token,
			"name":			"file",
			"extension":	".txt",
			"path":			"root",
			"tags":			["palabra1","palabra2"],
			"size":			2		# En MB.
		}
		r = requests.post("http://localhost:8000/files/metadata", json = payload)
		self.assertTrue(r.json()["result"])	# Leaves old name available
		payload = {
			"email":		"email",
			"token":		token,
			"name":			"nuevoNombre",
			"extension":	".txt",
			"path":			"root",
			"tags":			["palabra1","palabra2"],
			"size":			2		# En MB.
		}
		r = requests.post("http://localhost:8000/files/metadata", json = payload)
		self.assertFalse(r.json()["result"])	# Leaves new name unavailable
	

if __name__ == '__main__':
    unittest.main()
