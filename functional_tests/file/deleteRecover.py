import requests
import unittest
import json
from subprocess import call
import inspect, os
import datetime
import filecmp

class TestDeleteRecoverFile(unittest.TestCase):
	
	def setUp(self):
		r = requests.post("http://localhost:8000/cleandb")
	
	def _signup_and_login(self, email="testemail"):	
		payload = {"email": email, "password": "testpassword"}
		r = requests.post("http://localhost:8000/users", params = payload)

		payload = {"email": email, "password": "testpassword"}
		r = requests.get("http://localhost:8000/login", params = payload)
		return r.json()["token"]
	
	def _save_new_file(self, token, filename, email = "email", path = "root"):
		payload = {
			"email":		email,
			"token":		token,
			"name":			filename,
			"extension":	".txt",
			"path":			path,
			"tags":			["palabra1","palabra2"],
			"size":			2		# En MB.
		}
		r = requests.post("http://localhost:8000/files/metadata", json = payload)
		self.assertEqual(True, r.json()["result"])
		self.assertIn("fileID", r.json())
		self.assertIsNotNone(r.json()["fileID"])
		self.assertEqual(0, r.json()["version"])
		return r.json()["fileID"]


	def test_owner_delete_root_file_unshared(self):
		token = self._signup_and_login("email")
		fileid = self._save_new_file(token, "file", "email", "root")
		payload = {
			"email":		"email",
			"token":		token,
			"path":			"root"
		}
		r = requests.delete("http://localhost:8000/files/" + str(fileid), params = payload)
		self.assertTrue(r.json()["result"])
		payload = {
			"email":		"email",
			"token":		token
		}
		r = requests.get("http://localhost:8000/files/"+str(fileid)+"/metadata", params = payload)
		self.assertTrue(r.json()["result"])		# Mantiene el archivo existiendo
		payload = {
			"email":		"email",
			"token":		token,
			"path":			"root"
		}
		r = requests.get("http://localhost:8000/userfiles", params = payload)
		self.assertTrue(r.json()["result"])
		self.assertEqual([], r.json()["content"]["files"])
		payload = {
			"email":		"email",
			"token":		token,
			"path":			"trash"
		}
		r = requests.get("http://localhost:8000/userfiles", params = payload)
		self.assertTrue(r.json()["result"])
		self.assertEqual([fileid], r.json()["content"]["files"])
		self.assertEqual(["file.txt"], r.json()["content"]["filesNames"])

	def recover_file_to_root(self):
		token = self._signup_and_login("email")
		fileid = self._save_new_file(token, "file", "email", "root")
		payload = {
			"email":		"email",
			"token":		token,
			"path":			"root"
		}
		requests.delete("http://localhost:8000/files/" + str(fileid), params = payload)
		payload = {
			"email":		"email",
			"token":		token,
			"id":			fileid
		}
		r = requests.get("http://localhost:8000/recoverfile", params = payload)
		self.assertTrue(r.json()["result"])
		self.assertEqual("root", r.json()["path"])
		payload = {
			"email":		"email",
			"token":		token,
			"path":			"root"
		}
		r = requests.get("http://localhost:8000/userfiles", params = payload)
		self.assertTrue(r.json()["result"])
		self.assertEqual([fileid], r.json()["content"]["files"])
		self.assertEqual(["file"], r.json()["content"]["filesNames"])

if __name__ == '__main__':
    unittest.main()
