import requests
import unittest
import json
from subprocess import call
import inspect, os
import datetime
import filecmp

class TestSearches(unittest.TestCase):
	
	def setUp(self):
		r = requests.post("http://localhost:8000/cleandb")
	
	def _signup_and_login(self, email="testemail"):	
		payload = {"email": email, "password": "testpassword"}
		r = requests.post("http://localhost:8000/users", params = payload)

		payload = {"email": email, "password": "testpassword"}
		r = requests.get("http://localhost:8000/login", params = payload)
		return r.json()["token"]
	
	def _save_new_file(self, token, filename, email = "testemail", path = "root"):
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

	def test_search_name_from_one_owned(self):
		email = "email"
		token = self._signup_and_login(email)
		fileid = self._save_new_file(token, "file", email, "root")
		payload = {
			"email":		email,
			"token":		token,
			"typeOfSearch": "name",
			"element":		"file"
		}
		r = requests.get("http://localhost:8000/searches", params = payload)
		self.assertTrue(r.json()["result"])
		_file = {
			"id" : fileid,
			"path" : "root"
		}
		self.assertEqual([_file], r.json()["files"])
		
	def test_search_extension_from_one_owned(self):
		email = "email"
		token = self._signup_and_login(email)
		fileid = self._save_new_file(token, "file", email, "root")
		payload = {
			"email":		email,
			"token":		token,
			"typeOfSearch": "extension",
			"element":		".txt"
		}
		r = requests.get("http://localhost:8000/searches", params = payload)
		self.assertTrue(r.json()["result"])
		_file = {
			"id" : fileid,
			"path" : "root"
		}
		self.assertEqual([_file], r.json()["files"])
		
	def test_search_owner_from_one_owned(self):
		email = "email"
		token = self._signup_and_login(email)
		fileid = self._save_new_file(token, "file", email, "root")
		payload = {
			"email":		email,
			"token":		token,
			"typeOfSearch": "owner",
			"element":		"email"
		}
		r = requests.get("http://localhost:8000/searches", params = payload)
		self.assertTrue(r.json()["result"])
		_file = {
			"id" : fileid,
			"path" : "root"
		}
		self.assertEqual([_file], r.json()["files"])
		
	def test_search_tag_from_one_owned(self):
		email = "email"
		token = self._signup_and_login(email)
		fileid = self._save_new_file(token, "files", email, "root")
		payload = {
			"email":		email,
			"token":		token,
			"typeOfSearch": "tag",
			"element":		"palabra1"
		}
		r = requests.get("http://localhost:8000/searches", params = payload)
		print r.json()
		self.assertTrue(r.json()["result"])
		_file = {
			"id" : fileid,
			"path" : "root"
		}
		self.assertEqual([_file], r.json()["files"])
		payload = {
			"email":		email,
			"token":		token,
			"typeOfSearch": "tag",
			"element":		"palabra2"
		}
		r = requests.get("http://localhost:8000/searches", params = payload)
		self.assertTrue(r.json()["result"])
		_file = {
			"id" : fileid,
			"path" : "root"
		}
		self.assertEqual([_file], r.json()["files"])
		


if __name__ == '__main__':
    unittest.main()
