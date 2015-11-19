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
		fileid = self._save_new_file(token, "file", email, "root")
		payload = {
			"email":		email,
			"token":		token,
			"typeOfSearch": "tag",
			"element":		"palabra1"
		}
		r = requests.get("http://localhost:8000/searches", params = payload)
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
		self.assertEqual([_file], r.json()["files"])
		
	def test_search_from_shared_file(self):
		token1 = self._signup_and_login("email1")
		token2 = self._signup_and_login("email2")
		fileid = self._save_new_file(token1, "file", "email1", "root")
		payload = {
			"email":		"email1",
			"token":		token1,
			"id":			fileid,
			"users":		["email2"]
		}
		r = requests.post("http://localhost:8000/share", json = payload)
		payload = {
			"email":		"email2",
			"token":		token2,
			"typeOfSearch": "name",
			"element":		"file"
		}
		r = requests.get("http://localhost:8000/searches", params = payload)
		self.assertTrue(r.json()["result"])
		_file = {
			"id" : fileid,
			"path" : "shared"
		}
		self.assertEqual([_file], r.json()["files"])
		payload = {
			"email":		"email2",
			"token":		token2,
			"typeOfSearch": "extension",
			"element":		".txt"
		}
		r = requests.get("http://localhost:8000/searches", params = payload)
		self.assertTrue(r.json()["result"])
		self.assertEqual([_file], r.json()["files"])
		payload = {
			"email":		"email2",
			"token":		token2,
			"typeOfSearch": "owner",
			"element":		"email1"
		}
		r = requests.get("http://localhost:8000/searches", params = payload)
		self.assertTrue(r.json()["result"])
		self.assertEqual([_file], r.json()["files"])
		payload = {
			"email":		"email2",
			"token":		token2,
			"typeOfSearch": "tag",
			"element":		"palabra1"
		}
		r = requests.get("http://localhost:8000/searches", params = payload)
		self.assertTrue(r.json()["result"])
		self.assertEqual([_file], r.json()["files"])
		payload = {
			"email":		"email2",
			"token":		token2,
			"typeOfSearch": "tag",
			"element":		"palabra2"
		}
		r = requests.get("http://localhost:8000/searches", params = payload)
		self.assertTrue(r.json()["result"])
		self.assertEqual([_file], r.json()["files"])
		
	def test_changing_file_metadata(self):
		email = "email"
		token = self._signup_and_login(email)
		fileid = self._save_new_file(token, "file", email, "root")
		payload = {
			"email":		email,
			"token":		token,
			"name":			"newName",
			"tag":			"newTag"
		}
		r = requests.put("http://localhost:8000/files/" + str(fileid) + "/metadata", json = payload)
		payload = {
			"email":		email,
			"token":		token,
			"typeOfSearch": "name",
			"element":		"file"
		}
		r = requests.get("http://localhost:8000/searches", params = payload)
		self.assertTrue(r.json()["result"])
		self.assertEqual([], r.json()["files"])
		payload = {
			"email":		email,
			"token":		token,
			"typeOfSearch": "name",
			"element":		"newName"
		}
		r = requests.get("http://localhost:8000/searches", params = payload)
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
			"element":		"newTag"
		}
		r = requests.get("http://localhost:8000/searches", params = payload)
		self.assertTrue(r.json()["result"])
		self.assertEqual([_file], r.json()["files"])
		
	def test_searches_from_deleted_owned_file(self):
		email = "email"
		token = self._signup_and_login(email)
		fileid = self._save_new_file(token, "file", email, "root")
		payload = {
			"email":		email,
			"token":		token,
			"path":			"root"
		}
		r = requests.delete("http://localhost:8000/files/" + str(fileid), params = payload)
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
			"path" : "trash"
		}
		self.assertEqual([_file], r.json()["files"])
		payload = {
			"email":		email,
			"token":		token,
			"typeOfSearch": "extension",
			"element":		".txt"
		}
		r = requests.get("http://localhost:8000/searches", params = payload)
		self.assertTrue(r.json()["result"])
		self.assertEqual([_file], r.json()["files"])
		payload = {
			"email":		email,
			"token":		token,
			"typeOfSearch": "owner",
			"element":		"email"
		}
		r = requests.get("http://localhost:8000/searches", params = payload)
		self.assertTrue(r.json()["result"])
		self.assertEqual([_file], r.json()["files"])
		payload = {
			"email":		email,
			"token":		token,
			"typeOfSearch": "tag",
			"element":		"palabra1"
		}
		r = requests.get("http://localhost:8000/searches", params = payload)
		self.assertTrue(r.json()["result"])
		self.assertEqual([_file], r.json()["files"])
		payload = {
			"email":		email,
			"token":		token,
			"typeOfSearch": "tag",
			"element":		"palabra2"
		}
		r = requests.get("http://localhost:8000/searches", params = payload)
		self.assertTrue(r.json()["result"])
		self.assertEqual([_file], r.json()["files"])

	def test_recovered_file(self):
		email = "email"
		token = self._signup_and_login(email)
		payload = {
			"email":		email,
			"token":		token,
			"name":			"folder",
			"path":			"root"
		}
		r = requests.post("http://localhost:8000/folders", params = payload)
		fileid = self._save_new_file(token, "file", email, "root/folder")
		payload = {
			"email":		email,
			"token":		token,
			"path":			"root/folder"
		}
		r = requests.delete("http://localhost:8000/files/" + str(fileid), params = payload)
		payload = {
			"email":		email,
			"token":		token,
			"id":			fileid
		}
		r = requests.get("http://localhost:8000/recoverfile", params = payload)
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
			"path" : "root/folder"
		}
		self.assertEqual([_file], r.json()["files"])
		payload = {
			"email":		email,
			"token":		token,
			"typeOfSearch": "extension",
			"element":		".txt"
		}
		r = requests.get("http://localhost:8000/searches", params = payload)
		self.assertTrue(r.json()["result"])
		self.assertEqual([_file], r.json()["files"])
		payload = {
			"email":		email,
			"token":		token,
			"typeOfSearch": "owner",
			"element":		"email"
		}
		r = requests.get("http://localhost:8000/searches", params = payload)
		self.assertTrue(r.json()["result"])
		self.assertEqual([_file], r.json()["files"])
		payload = {
			"email":		email,
			"token":		token,
			"typeOfSearch": "tag",
			"element":		"palabra1"
		}
		r = requests.get("http://localhost:8000/searches", params = payload)
		self.assertTrue(r.json()["result"])
		self.assertEqual([_file], r.json()["files"])
		payload = {
			"email":		email,
			"token":		token,
			"typeOfSearch": "tag",
			"element":		"palabra2"
		}
		r = requests.get("http://localhost:8000/searches", params = payload)
		self.assertTrue(r.json()["result"])
		self.assertEqual([_file], r.json()["files"])
			
	def test_searches_shared_and_deleted(self):
		token1 = self._signup_and_login("email1")
		token2 = self._signup_and_login("email2")
		fileid = self._save_new_file(token1, "file", "email1", "root")
		payload = {
			"email":		"email1",
			"token":		token1,
			"id":			fileid,
			"users":		["email2"]
		}
		r = requests.post("http://localhost:8000/share", json = payload)
		print r.json()
		payload = {
			"email":		"email1",
			"token":		token1,
			"path":			"root"
		}
		r = requests.delete("http://localhost:8000/files/" + str(fileid), params = payload)
		print r.json()
		self.assertTrue(r.json()["result"])
		payload = {
			"email":		"email1",
			"token":		token1,
			"typeOfSearch": "name",
			"element":		"file"
		}
		r = requests.get("http://localhost:8000/searches", params = payload)
		self.assertTrue(r.json()["result"])
		_file = {
			"id" : fileid,
			"path" : "trash"
		}
		self.assertEqual([_file], r.json()["files"])
		payload = {
			"email":		"email1",
			"token":		token1,
			"typeOfSearch": "extension",
			"element":		".txt"
		}
		r = requests.get("http://localhost:8000/searches", params = payload)
		self.assertTrue(r.json()["result"])
		self.assertEqual([_file], r.json()["files"])
		payload = {
			"email":		"email1",
			"token":		token1,
			"typeOfSearch": "owner",
			"element":		"email1"
		}
		r = requests.get("http://localhost:8000/searches", params = payload)
		self.assertTrue(r.json()["result"])
		self.assertEqual([_file], r.json()["files"])
		payload = {
			"email":		"email1",
			"token":		token1,
			"typeOfSearch": "tag",
			"element":		"palabra1"
		}
		r = requests.get("http://localhost:8000/searches", params = payload)
		self.assertTrue(r.json()["result"])
		self.assertEqual([_file], r.json()["files"])
		payload = {
			"email":		"email1",
			"token":		token1,
			"typeOfSearch": "tag",
			"element":		"palabra2"
		}
		r = requests.get("http://localhost:8000/searches", params = payload)
		self.assertTrue(r.json()["result"])
		self.assertEqual([_file], r.json()["files"])
		payload = {	#Chequea que al compartido le haya desaparecido
			"email":		"email2",
			"token":		token2,
			"typeOfSearch": "name",
			"element":		"file"
		}
		r = requests.get("http://localhost:8000/searches", params = payload)
		self.assertTrue(r.json()["result"])
		self.assertEqual([], r.json()["files"])
		payload = {
			"email":		"email2",
			"token":		token2,
			"typeOfSearch": "extension",
			"element":		".txt"
		}
		r = requests.get("http://localhost:8000/searches", params = payload)
		self.assertTrue(r.json()["result"])
		self.assertEqual([], r.json()["files"])
		payload = {
			"email":		"email2",
			"token":		token2,
			"typeOfSearch": "owner",
			"element":		"email1"
		}
		r = requests.get("http://localhost:8000/searches", params = payload)
		self.assertTrue(r.json()["result"])
		self.assertEqual([], r.json()["files"])
		payload = {
			"email":		"email2",
			"token":		token2,
			"typeOfSearch": "tag",
			"element":		"palabra1"
		}
		r = requests.get("http://localhost:8000/searches", params = payload)
		self.assertTrue(r.json()["result"])
		self.assertEqual([], r.json()["files"])
		payload = {
			"email":		"email2",
			"token":		token2,
			"typeOfSearch": "tag",
			"element":		"palabra2"
		}
		r = requests.get("http://localhost:8000/searches", params = payload)
		self.assertTrue(r.json()["result"])
		self.assertEqual([], r.json()["files"])
	

if __name__ == '__main__':
    unittest.main()
