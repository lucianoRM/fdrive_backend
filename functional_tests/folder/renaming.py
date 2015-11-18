import requests
import unittest
import json
from subprocess import call
import inspect, os
import datetime
import filecmp

class TestFolderRenaming(unittest.TestCase):
	
	def setUp(self):
		r = requests.post("http://localhost:8000/cleandb")
	
	def _signup_and_login(self, email="testemail"):	
		payload = {"email": email, "password": "testpassword"}
		r = requests.post("http://localhost:8000/users", params = payload)

		payload = {"email": email, "password": "testpassword"}
		r = requests.get("http://localhost:8000/login", params = payload)
		return r.json()["token"]


	def _create_folder(self, email, token, foldername, path):
		payload = {
			"email":		email,
			"token":		token,
			"name":			foldername,
			"path":			path
		}
		r = requests.post("http://localhost:8000/folders", params = payload)
		return r.json()
		
	def test_rename_folder_in_root(self):
		token = self._signup_and_login("email")
		self._create_folder("email", token, "folder", "root")
		payload = {
			"email":		"email",
			"token":		token,
			"nameold":		"folder",
			"namenew":		"newName",
			"path":			"root"
		}
		r = requests.put("http://localhost:8000/folders", params = payload)
		self.assertTrue(r.json()["result"])
		_json = self._create_folder("email", token, "folder", "root")
		self.assertTrue(_json["result"])	#Leaves old name valid
		_json = self._create_folder("email", token, "newName", "root")
		self.assertFalse(_json["result"])	#Leaves new name invalid
		
	def test_rename_folder_inside_root(self):
		token = self._signup_and_login("email")
		self._create_folder("email", token, "folder", "root")
		self._create_folder("email", token, "folder2", "root/folder")
		payload = {
			"email":		"email",
			"token":		token,
			"nameold":		"folder2",
			"namenew":		"newName",
			"path":			"root/folder"
		}
		r = requests.put("http://localhost:8000/folders", params = payload)
		self.assertTrue(r.json()["result"])
		_json = self._create_folder("email", token, "folder2", "root/folder")
		self.assertTrue(_json["result"])	#Leaves old name valid
		_json = self._create_folder("email", token, "newName", "root/folder")
		self.assertFalse(_json["result"])	#Leaves new name invalid
		
	def test_rename_to_already_used_name(self):
		token = self._signup_and_login("email")
		self._create_folder("email", token, "folder", "root")
		self._create_folder("email", token, "folder2", "root")
		payload = {
			"email":		"email",
			"token":		token,
			"nameold":		"folder",
			"namenew":		"folder2",
			"path":			"root"
		}
		r = requests.put("http://localhost:8000/folders", params = payload)
		self.assertFalse(r.json()["result"])
		payload = {
			"email":		"email",
			"token":		token,
			"path":			"root"
		}
		r = requests.get("http://localhost:8000/userfiles", params = payload)
		self.assertTrue(r.json()["result"])
		self.assertEqual(["folder", "folder2"], r.json()["content"]["folders"])

if __name__ == '__main__':
    unittest.main()
