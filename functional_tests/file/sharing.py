import requests
import unittest
import json
from subprocess import call
import inspect, os
import datetime
import filecmp

class TestSharingFiles(unittest.TestCase):

	def setUp(self):
		call(["rm", "-rf", os.path.dirname(os.path.abspath(inspect.getfile(inspect.currentframe())))+"/../../testdb"])
	
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
		r = requests.post("http://localhost:8000/files", json = payload)
		self.assertEqual(True, r.json()["result"])
		self.assertIn("fileID", r.json())
		self.assertIsNotNone(r.json()["fileID"])
		self.assertEqual(0, r.json()["version"])
		return r.json()["fileID"]

	def _save_new_version_of_file(self, fileid, token, old_version, filename, overwrite = False):
		payload = {
			"email":		"testemail",
			"token":		token,
			"name":			filename,
			"extension":	".txt2",
			"path":			"root/files",
			"tags":			["palabra1"],
			"size":			2.5,		# En MB.
			"version":		old_version,
			"overwrite": 	overwrite,
			"id" :			fileid
		}
		r = requests.post("http://localhost:8000/files", json = payload)
		return r.json()

	def _share_file(self, token, fileid, email, users):
		payload = {
			"email":		email,
			"token":		token,
			"id":			fileid,
			"users":		users
		}
		r = requests.post("http://localhost:8000/share", json = payload)
		return r.json()


	def test_share_file(self):
		token = self._signup_and_login("user1")
		token2 = self._signup_and_login("user2")
		file_id = self._save_new_file(token, "file", "user1")
		_json = self._share_file(token, file_id, "user1" , ["user2"])
		self.assertTrue(_json["result"])
		payload = {
			"email":		"user2",
			"token":		token2
		}
		r = requests.get("http://localhost:8000/files/"+str(file_id)+"/metadata", params = payload)
		self.assertTrue(r.json()["result"])

	def test_share_inexistent_file(self):
		token1 = self._signup_and_login("user1")
		token2 = self._signup_and_login("user2")
		_json = self._share_file(token2, 0, "user2" , ["user1"])
		self.assertFalse(_json["result"])

	def test_share_file_not_by_owner(self):
		token1 = self._signup_and_login("user1")
		token2 = self._signup_and_login("user2")
		file_id = self._save_new_file(token1, "file", "user1")
		_json = self._share_file(token2, file_id, "user2" , ["user1"])
		self.assertFalse(_json["result"])
		payload = {
			"email":		"user2",
			"token":		token2
		}
		r = requests.get("http://localhost:8000/files/"+str(file_id)+"/metadata", params = payload)
		self.assertFalse(r.json()["result"])

	def test_share_file_twice_to_the_same_user(self):
		token = self._signup_and_login("user1")
		self._signup_and_login("user2")
		file_id = self._save_new_file(token, "file", "user1")
		self._share_file(token, file_id, "user1" , ["user2"])
		_json = self._share_file(token, file_id, "user1" , ["user2"])
		self.assertFalse(_json["result"])

	def test_share_file_to_inexistent_user(self):
		token = self._signup_and_login("user1")
		file_id = self._save_new_file(token, "file", "user1")
		_json = self._share_file(token, file_id, "user1" , ["user2"])
		self.assertFalse(_json["result"])

	def test_share_file_to_many_users(self):
		token = self._signup_and_login("user1")
		token2 = self._signup_and_login("user2")
		token3 = self._signup_and_login("user3")
		token4 = self._signup_and_login("user4")
		file_id = self._save_new_file(token, "file", "user1")
		_json = self._share_file(token, file_id, "user1" , ["user2", "user3", "user4"])
		self.assertTrue(_json["result"])
		payload = {
			"email":		"user2",
			"token":		token2
		}
		r = requests.get("http://localhost:8000/files/"+str(file_id)+"/metadata", params = payload)
		self.assertTrue(r.json()["result"])
		payload = {
			"email":		"user3",
			"token":		token3
		}
		r = requests.get("http://localhost:8000/files/"+str(file_id)+"/metadata", params = payload)
		self.assertTrue(r.json()["result"])
		payload = {
			"email":		"user4",
			"token":		token4
		}
		r = requests.get("http://localhost:8000/files/"+str(file_id)+"/metadata", params = payload)
		self.assertTrue(r.json()["result"])

	def test_share_file_with_some_inexistent_user(self):
		token = self._signup_and_login("user1")
		token2 = self._signup_and_login("user2")
		token3 = self._signup_and_login("user3")
		file_id = self._save_new_file(token, "file", "user1")
		_json = self._share_file(token, file_id, "user1" , ["user2", "user4", "user3"])
		self.assertFalse(_json["result"])
		payload = {
			"email":		"user2",
			"token":		token2
		}
		r = requests.get("http://localhost:8000/files/"+str(file_id)+"/metadata", params = payload)
		self.assertFalse(r.json()["result"])
		payload = {
			"email":		"user3",
			"token":		token3
		}
		r = requests.get("http://localhost:8000/files/"+str(file_id)+"/metadata", params = payload)
		self.assertFalse(r.json()["result"])

	def test_share_file_with_some_already_shared_user(self):
		token = self._signup_and_login("user1")
		self._signup_and_login("user2")
		token3 = self._signup_and_login("user3")
		token4 = self._signup_and_login("user4")
		file_id = self._save_new_file(token, "file", "user1")
		_json = self._share_file(token, file_id, "user1" , ["user2"])
		_json = self._share_file(token, file_id, "user1" , ["user3", "user2", "user4"])
		self.assertFalse(_json["result"])
		payload = {
			"email":		"user3",
			"token":		token3
		}
		r = requests.get("http://localhost:8000/files/"+str(file_id)+"/metadata", params = payload)
		self.assertFalse(r.json()["result"])
		payload = {
			"email":		"user4",
			"token":		token4
		}
		r = requests.get("http://localhost:8000/files/"+str(file_id)+"/metadata", params = payload)
		self.assertFalse(r.json()["result"])

	def test_unshare_file_with_everybody(self):
		token = self._signup_and_login("user1")
		token2 = self._signup_and_login("user2")
		token3 = self._signup_and_login("user3")
		token4 = self._signup_and_login("user4")
		file_id = self._save_new_file(token, "file", "user1")
		self._share_file(token, file_id, "user1" , ["user3", "user2", "user4"])
		payload = {
			"email":		"user1",
			"token":		token,
			"id":			file_id
		}
		r = requests.delete("http://localhost:8000/share", json = payload)
		self.assertTrue(r.json()["result"])
		payload = {
			"email":		"user2",
			"token":		token2
		}
		r = requests.get("http://localhost:8000/files/"+str(file_id)+"/metadata", params = payload)
		self.assertFalse(r.json()["result"])
		payload = {
			"email":		"user3",
			"token":		token3
		}
		r = requests.get("http://localhost:8000/files/"+str(file_id)+"/metadata", params = payload)
		self.assertFalse(r.json()["result"])
		payload = {
			"email":		"user4",
			"token":		token4
		}
		r = requests.get("http://localhost:8000/files/"+str(file_id)+"/metadata", params = payload)
		self.assertFalse(r.json()["result"])

	def test_unshare_file_with_some(self):
		token = self._signup_and_login("user1")
		token2 = self._signup_and_login("user2")
		token3 = self._signup_and_login("user3")
		token4 = self._signup_and_login("user4")
		file_id = self._save_new_file(token, "file", "user1")
		self._share_file(token, file_id, "user1" , ["user3", "user2", "user4"])
		payload = {
			"email":		"user1",
			"token":		token,
			"id":			file_id,
			"users":		["user3", "user2"]
		}
		r = requests.delete("http://localhost:8000/share", json = payload)
		self.assertTrue(r.json()["result"])
		self.assertTrue(r.json()["result"])
		payload = {
			"email":		"user2",
			"token":		token2
		}
		r = requests.get("http://localhost:8000/files/"+str(file_id)+"/metadata", params = payload)
		self.assertFalse(r.json()["result"])
		payload = {
			"email":		"user3",
			"token":		token3
		}
		r = requests.get("http://localhost:8000/files/"+str(file_id)+"/metadata", params = payload)
		self.assertFalse(r.json()["result"])
		payload = {
			"email":		"user4",
			"token":		token4
		}
		r = requests.get("http://localhost:8000/files/"+str(file_id)+"/metadata", params = payload)
		self.assertTrue(r.json()["result"])


if __name__ == '__main__':
    unittest.main()
