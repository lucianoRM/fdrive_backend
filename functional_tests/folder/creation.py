import requests
import unittest
import json
from subprocess import call
import inspect, os
import datetime
import filecmp

class TestFolderCreation(unittest.TestCase):
	
	def setUp(self):
		r = requests.post("http://localhost:8000/cleandb")
	
	def _signup_and_login(self, email="testemail"):	
		payload = {"email": email, "password": "testpassword"}
		r = requests.post("http://localhost:8000/users", params = payload)

		payload = {"email": email, "password": "testpassword"}
		r = requests.get("http://localhost:8000/login", params = payload)
		return r.json()["token"]


	def test_create_folder_in_root(self):
		email = "email"
		foldername = "folder"
		token = self._signup_and_login(email)
		payload = {
			"email":		email,
			"token":		token,
			"name":			foldername,
			"path":			"root"
		}
		r = requests.post("http://localhost:8000/folders", params = payload)
		self.assertTrue(r.json()["result"])
		
	def test_create_folder_inside_root(self):
		email = "email"
		foldername = "folder"
		token = self._signup_and_login(email)
		payload = {
			"email":		email,
			"token":		token,
			"name":			foldername,
			"path":			"root"
		}
		r = requests.post("http://localhost:8000/folders", params = payload)
		payload = {
			"email":		email,
			"token":		token,
			"name":			foldername,
			"path":			"root/" + foldername
		}
		r = requests.post("http://localhost:8000/folders", params = payload)
		self.assertTrue(r.json()["result"])
		payload = {
			"email":		email,
			"token":		token,
			"name":			foldername,
			"path":			"root/" + foldername + "/" + foldername
		}
		r = requests.post("http://localhost:8000/folders", params = payload)
		self.assertTrue(r.json()["result"])

	def test_create_folder_in_shared(self):
		email = "email"
		foldername = "folder"
		token = self._signup_and_login(email)
		payload = {
			"email":		email,
			"token":		token,
			"name":			foldername,
			"path":			"shared"
		}
		r = requests.post("http://localhost:8000/folders", params = payload)
		self.assertFalse(r.json()["result"])
		
	def test_create_folder_in_trash(self):
		email = "email"
		foldername = "folder"
		token = self._signup_and_login(email)
		payload = {
			"email":		email,
			"token":		token,
			"name":			foldername,
			"path":			"trash"
		}
		r = requests.post("http://localhost:8000/folders", params = payload)
		self.assertFalse(r.json()["result"])
		
		

if __name__ == '__main__':
    unittest.main()
