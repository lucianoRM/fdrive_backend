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

	def test_owner_delete_root_file_shared(self):
		token1 = self._signup_and_login("email1")
		token2 = self._signup_and_login("email2")
		fileid = self._save_new_file(token1, "file", "email1", "root")
		payload = {
			"email":		"email1",
			"token":		token1
		}
		r = requests.get("http://localhost:8000/files/"+str(fileid)+"/metadata", params = payload)
		self.assertTrue(r.json()["result"])
		self.assertEqual("email1", r.json()["file"]["owner"])
		payload = {
			"email":		"email1",
			"token":		token1,
			"id":			fileid,
			"users":		["email2"]
		}
		r = requests.post("http://localhost:8000/share", json = payload)
		payload = {
			"email":		"email1",
			"token":		token1
		}
		r = requests.get("http://localhost:8000/files/"+str(fileid)+"/metadata", params = payload)
		self.assertTrue(r.json()["result"])
		self.assertEqual("email1", r.json()["file"]["owner"])
		payload = {
			"email":		"email1",
			"token":		token1,
			"path":			"root"
		}
		r = requests.delete("http://localhost:8000/files/" + str(fileid), params = payload)
		self.assertTrue(r.json()["result"])
		r = requests.get("http://localhost:8000/userfiles", params = payload)	# El owner no lo tiene donde estaba, si en trash
		self.assertTrue(r.json()["result"])
		self.assertEqual([], r.json()["content"]["files"])
		payload = {
			"email":		"email1",
			"token":		token1,
			"path":			"trash"
		}
		r = requests.get("http://localhost:8000/userfiles", params = payload)
		self.assertTrue(r.json()["result"])
		self.assertEqual([fileid], r.json()["content"]["files"])
		self.assertEqual(["file.txt"], r.json()["content"]["filesNames"])
		payload = {
			"email":		"email2",
			"token":		token2,
			"path":			"shared"
		}
		r = requests.get("http://localhost:8000/userfiles", params = payload)	# El shared no lo tiene donde estaba, ni en trash
		self.assertTrue(r.json()["result"])
		self.assertEqual([], r.json()["content"]["files"])
		payload = {
			"email":		"email2",
			"token":		token2,
			"path":			"trash"
		}
		r = requests.get("http://localhost:8000/userfiles", params = payload)
		self.assertTrue(r.json()["result"])
		self.assertEqual([], r.json()["content"]["files"])

	def test_recover_file_to_root(self):
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
		self.assertEqual(["file.txt"], r.json()["content"]["filesNames"])

if __name__ == '__main__':
    unittest.main()
