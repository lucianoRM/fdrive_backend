import requests
import unittest
import json
from subprocess import call
import inspect, os
import datetime
import filecmp

class TestMetadataVersion(unittest.TestCase):
	
	def setUp(self):
		r = requests.post("http://localhost:8000/cleandb")
	
	def _signup_and_login(self, email="testemail"):	
		payload = {"email": email, "password": "testpassword"}
		r = requests.post("http://localhost:8000/users", params = payload)

		payload = {"email": email, "password": "testpassword"}
		r = requests.get("http://localhost:8000/login", params = payload)
		return r.json()["token"]
	
	def _save_new_file(self, token, filename, email, path = "root"):
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
			"overwrite": 	overwrite
		}
		r = requests.post("http://localhost:8000/files/"+str(fileid)+"/metadata", json = payload)
		return r.json()
		
	def _create_folder(self, email, foldername, token, path = "root"):
		payload = {
			"email":		email,
			"token":		token,
			"name":			foldername,
			"path":			path
		}
		r = requests.post("http://localhost:8000/folders", params = payload)
		self.assertTrue(r.json()["result"])
		return r.json()

	def test_save_new_file_then_get(self):
		email = "testemail"
		token = self._signup_and_login(email)
		fileid = self._save_new_file(token, "somefilename", email)
		payload = {
			"email":		"testemail",
			"token":		token
		}
		r = requests.get("http://localhost:8000/files/"+str(fileid)+"/metadata", params = payload)
		self.assertTrue(r.json()["result"])
		self.assertEqual(".txt", r.json()["file"]["extension"])
		self.assertEqual(fileid, r.json()["file"]["id"])
		modificationTime = datetime.datetime.strptime(r.json()["file"]["lastModified"], "%a %b %d %H:%M:%S %Y")
		self.assertGreater(modificationTime + datetime.timedelta(minutes=1), modificationTime)
		self.assertLess(modificationTime - datetime.timedelta(minutes=1), modificationTime)
		self.assertEqual("testemail", r.json()["file"]["lastUser"])
		self.assertEqual("somefilename", r.json()["file"]["name"])
		self.assertEqual("testemail", r.json()["file"]["owner"])
		self.assertEqual(["palabra1","palabra2"], r.json()["file"]["tags"])
		self.assertEqual(0, r.json()["file"]["lastVersion"])

	def test_save_new_file_in_trash(self):
		email = "testemail"
		token = self._signup_and_login(email)
		payload = {
			"email":		email,
			"token":		token,
			"name":			"name",
			"extension":	".txt",
			"path":			"trash",
			"tags":			["palabra1","palabra2"],
			"size":			2		# En MB.
		}
		r = requests.post("http://localhost:8000/files/metadata", json = payload)
		self.assertFalse(r.json()["result"])

	def test_save_new_file_in_shared(self):
		email = "testemail"
		token = self._signup_and_login(email)
		payload = {
			"email":		email,
			"token":		token,
			"name":			"name",
			"extension":	".txt",
			"path":			"shared",
			"tags":			["palabra1","palabra2"],
			"size":			2		# En MB.
		}
		r = requests.post("http://localhost:8000/files/metadata", json = payload)
		self.assertFalse(r.json()["result"])

	def test_save_new_file_in_inexistent_path(self):
		email = "testemail"
		token = self._signup_and_login(email)
		payload = {
			"email":		email,
			"token":		token,
			"name":			"name",
			"extension":	".txt",
			"path":			"root/folder",
			"tags":			["palabra1","palabra2"],
			"size":			2		# En MB.
		}
		r = requests.post("http://localhost:8000/files/metadata", json = payload)
		self.assertFalse(r.json()["result"])

	def test_save_new_file_inside_root(self):
		token = self._signup_and_login("email")
		self._create_folder("email", "folder", token, "root")
		fileid = self._save_new_file(token, "somefilename", "email", "root/folder")
		payload = {
			"email":		"email",
			"token":		token
		}
		r = requests.get("http://localhost:8000/files/"+str(fileid)+"/metadata", params = payload)
		self.assertTrue(r.json()["result"])
		self.assertEqual(".txt", r.json()["file"]["extension"])
		self.assertEqual(fileid, r.json()["file"]["id"])
		modificationTime = datetime.datetime.strptime(r.json()["file"]["lastModified"], "%a %b %d %H:%M:%S %Y")
		self.assertGreater(modificationTime + datetime.timedelta(minutes=1), modificationTime)
		self.assertLess(modificationTime - datetime.timedelta(minutes=1), modificationTime)
		self.assertEqual("email", r.json()["file"]["lastUser"])
		self.assertEqual("somefilename", r.json()["file"]["name"])
		self.assertEqual("email", r.json()["file"]["owner"])
		self.assertEqual(["palabra1","palabra2"], r.json()["file"]["tags"])
		self.assertEqual(0, r.json()["file"]["lastVersion"])

	def test_save_two_new_files_then_get(self):
		email = "testemail"
		token = self._signup_and_login()
		fileid = self._save_new_file(token, "somefilename", email)
		payload = {
			"email":		"testemail",
			"token":		token
		}
		r = requests.get("http://localhost:8000/files/"+str(fileid)+"/metadata", params = payload)
		self.assertTrue(r.json()["result"])
		self.assertEqual(".txt", r.json()["file"]["extension"])
		self.assertEqual(fileid, r.json()["file"]["id"])
		modificationTime = datetime.datetime.strptime(r.json()["file"]["lastModified"], "%a %b %d %H:%M:%S %Y")
		self.assertGreater(modificationTime + datetime.timedelta(minutes=1), modificationTime)
		self.assertLess(modificationTime - datetime.timedelta(minutes=1), modificationTime)
		self.assertEqual("testemail", r.json()["file"]["lastUser"])
		self.assertEqual("somefilename", r.json()["file"]["name"])
		self.assertEqual("testemail", r.json()["file"]["owner"])
		self.assertEqual(["palabra1","palabra2"], r.json()["file"]["tags"])
		self.assertEqual(0, r.json()["file"]["lastVersion"])
		fileid2 = self._save_new_file(token, "otherfilename", email)
		#print "Voy a pedir el archivo."
		payload = {
			"email":		"testemail",
			"token":		token
		}
		r = requests.get("http://localhost:8000/files/"+str(fileid2)+"/metadata", params = payload)
		self.assertTrue(r.json()["result"])
		self.assertEqual(".txt", r.json()["file"]["extension"])
		self.assertEqual(fileid2, r.json()["file"]["id"])
		modificationTime = datetime.datetime.strptime(r.json()["file"]["lastModified"], "%a %b %d %H:%M:%S %Y")
		self.assertGreater(modificationTime + datetime.timedelta(minutes=1), modificationTime)
		self.assertLess(modificationTime - datetime.timedelta(minutes=1), modificationTime)
		self.assertEqual("testemail", r.json()["file"]["lastUser"])
		self.assertEqual("otherfilename", r.json()["file"]["name"])
		self.assertEqual("testemail", r.json()["file"]["owner"])
		self.assertEqual(["palabra1","palabra2"], r.json()["file"]["tags"])
		self.assertEqual(0, r.json()["file"]["lastVersion"])
		
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
		r = requests.post("http://localhost:8000/files/metadata", json = payload)
		self.assertFalse(r.json()["result"])
		
	def test_get_file_wrong_id(self):
		token = self._signup_and_login()
		fileid = self._save_new_file(token, "somefilename", "testemail")
		payload = {
			"email":		"testemail",
			"token":		token
		}
		r = requests.get("http://localhost:8000/files/"+str(fileid+1)+"/metadata", params = payload)
		self.assertFalse(r.json()["result"])
		self.assertEqual(["File not found"], r.json()["errors"])
		self.assertEqual(2, len(r.json()))
		
	def test_get_file_not_authorized(self):
		token = self._signup_and_login()
		token2 = self._signup_and_login("testemail2")
		fileid = self._save_new_file(token, "somefilename", "testemail")
		payload = {
			"email":		"testemail2",
			"token":		token2
		}
		r = requests.get("http://localhost:8000/files/"+str(fileid)+"/metadata", params = payload)
		self.assertFalse(r.json()["result"])
		self.assertEqual(["The user has no permits for specified file."], r.json()["errors"])
		self.assertEqual(2, len(r.json()))
		
	def test_save_new_version_of_file_then_get(self):
		token = self._signup_and_login()
		fileid = self._save_new_file(token, "somefilename", "testemail")
		_json = self._save_new_version_of_file(fileid, token, 0, "otherfilename")
		self.assertTrue(_json["result"])
		self.assertIn("version", _json)
		self.assertTrue(1, _json["version"])
		payload = {
			"email":		"testemail",
			"token":		token
		}
		r = requests.get("http://localhost:8000/files/"+str(fileid)+"/metadata", params = payload)
		self.assertTrue(r.json()["result"])
		self.assertEqual(".txt2", r.json()["file"]["extension"])
		self.assertEqual(fileid, r.json()["file"]["id"])
		modificationTime = datetime.datetime.strptime(r.json()["file"]["lastModified"], "%a %b %d %H:%M:%S %Y")
		self.assertGreater(modificationTime + datetime.timedelta(minutes=1), modificationTime)
		self.assertLess(modificationTime - datetime.timedelta(minutes=1), modificationTime)
		self.assertEqual("testemail", r.json()["file"]["lastUser"])
		self.assertEqual("otherfilename", r.json()["file"]["name"])
		self.assertEqual("testemail", r.json()["file"]["owner"])
		self.assertEqual(["palabra1"], r.json()["file"]["tags"])
		self.assertEqual(1, r.json()["file"]["lastVersion"])

	def test_save_new_version_of_file_then_get_metadata_of_earlier(self):
		token = self._signup_and_login()
		fileid = self._save_new_file(token, "somefilename", "testemail")
		_json = self._save_new_version_of_file(fileid, token, 0, "otherfilename")
		payload = {
			"email":		"testemail",
			"token":		token
		}
		r = requests.get("http://localhost:8000/files/"+str(fileid)+"/0/metadata", params = payload)
		self.assertEqual("somefilename", r.json()["file"]["name"])
	
	def test_save_new_version_wrong_token(self):
		token = self._signup_and_login()
		fileid = self._save_new_file(token, "somefilename", "testemail")
		_json = self._save_new_version_of_file(fileid, token + "wrong", 0, "somefilename")
		self.assertFalse(_json["result"])
		
	def test_save_new_version_of_inexistent_file(self):
		token = self._signup_and_login()
		fileid = self._save_new_file(token, "somefilename", "testemail")
		_json = self._save_new_version_of_file(fileid + 6, token, 0, "otherFilename")
		self.assertFalse(_json["result"])

	def test_save_new_version_too_soon(self):
		token = self._signup_and_login()
		fileid = self._save_new_file(token, "somefilename", "testemail")
		_json = self._save_new_version_of_file(fileid, token, 5, "otherFilename")
		self.assertEqual(_json["errors"], ["The version specified does not exists."])
		self.assertFalse(_json["result"])

	def test_save_new_version_from_not_last_version_error(self):
		token = self._signup_and_login()
		fileid = self._save_new_file(token, "somefilename", "testemail")
		_json = self._save_new_version_of_file(fileid, token, 0, "otherFilename")	# Correct new version
		_json = self._save_new_version_of_file(fileid, token, 0, "AnotherFilename")
		self.assertFalse(_json["result"])

	def test_save_new_version_from_not_last_version_with_overwrite_then_get(self):
		email = "testemail"
		token = self._signup_and_login()
		fileid = self._save_new_file(token, "somefilename", email)
		_json = self._save_new_version_of_file(fileid, token, 0, "otherFilename")	# Correct new version
		_json = self._save_new_version_of_file(fileid, token, 0, "AnotherFilename", True)
		self.assertTrue(_json["result"])
		self.assertEqual(2, _json["version"])
		payload = {
			"email":		"testemail",
			"token":		token
		}
		r = requests.get("http://localhost:8000/files/"+str(fileid)+"/metadata", params = payload)
		self.assertEqual(".txt2", r.json()["file"]["extension"])
		self.assertEqual(fileid, r.json()["file"]["id"])
		modificationTime = datetime.datetime.strptime(r.json()["file"]["lastModified"], "%a %b %d %H:%M:%S %Y")
		self.assertGreater(modificationTime + datetime.timedelta(minutes=1), modificationTime)
		self.assertLess(modificationTime - datetime.timedelta(minutes=1), modificationTime)
		self.assertEqual("testemail", r.json()["file"]["lastUser"])
		self.assertEqual("AnotherFilename", r.json()["file"]["name"])
		self.assertEqual("testemail", r.json()["file"]["owner"])
		self.assertEqual(["palabra1"], r.json()["file"]["tags"])
		self.assertEqual(2, r.json()["file"]["lastVersion"])
		

if __name__ == '__main__':
    unittest.main()
