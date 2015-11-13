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
		r = requests.post("http://localhost:8000/users", params = payload)

		payload = {"email": email, "password": "testpassword"}
		r = requests.get("http://localhost:8000/login", params = payload)
		return r.json()["token"]
	
	def _save_new_file(self, token, filename):
		payload = {
			"email":		"testemail",
			"token":		token,
			"name":			filename,
			"extension":	".txt",
			"path":			"root",
			"tags":			["palabra1","palabra2"],
			"size":			2		# En MB.
		}
		r = requests.post("http://localhost:8000/files", json = payload)
		print json.dumps(r.json())
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
		print json.dumps(r.json())
		return r.json()

	def test_save_new_file_then_get(self):
		token = self._signup_and_login()
		#print "Voy a guardar el archivo."
		fileid = self._save_new_file(token, "somefilename")
		#print "Voy a pedir el archivo."
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
		self.assertEqual(0, r.json()["lastVersion"])

	def test_save_two_new_files_then_get(self):
		token = self._signup_and_login()
		#print "Voy a guardar el archivo."
		fileid = self._save_new_file(token, "somefilename")
		#print "Voy a pedir el archivo."
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
		self.assertEqual(0, r.json()["lastVersion"])
		fileid2 = self._save_new_file(token, "otherfilename")
		#print "Voy a pedir el archivo."
		payload = {
			"email":		"testemail",
			"token":		token,
			"id":			fileid2
		}
		r = requests.get("http://localhost:8000/files", params = payload)
		self.assertEqual(".txt", r.json()["extension"])
		self.assertEqual(fileid2, r.json()["id"])
		modificationTime = datetime.datetime.strptime(r.json()["lastModified"], "%a %b %d %H:%M:%S %Y")
		self.assertGreater(modificationTime + datetime.timedelta(minutes=1), modificationTime)
		self.assertLess(modificationTime - datetime.timedelta(minutes=1), modificationTime)
		self.assertEqual("testemail", r.json()["lastUser"])
		self.assertEqual("otherfilename", r.json()["name"])
		self.assertEqual("testemail", r.json()["owner"])
		self.assertEqual(["palabra1","palabra2"], r.json()["tags"])
		self.assertEqual(0, r.json()["lastVersion"])
		
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
		fileid = self._save_new_file(token, "somefilename")
		payload = {
			"email":		"testemail",
			"token":		token,
			"id":			fileid+1
		}
		r = requests.get("http://localhost:8000/files", params = payload)
		self.assertFalse(r.json()["result"])
		self.assertEqual(["File not found"], r.json()["errors"])
		self.assertEqual(2, len(r.json()))
		
	def test_get_file_not_authorized(self):
		token = self._signup_and_login()
		token2 = self._signup_and_login("testemail2")
		fileid = self._save_new_file(token, "somefilename")
		payload = {
			"email":		"testemail2",
			"token":		token2,
			"id":			fileid
		}
		r = requests.get("http://localhost:8000/files", params = payload)
		self.assertFalse(r.json()["result"])
		self.assertEqual(["The user has no permits for specified file."], r.json()["errors"])
		self.assertEqual(2, len(r.json()))
		
	def test_save_new_version_of_file_then_get(self):
		token = self._signup_and_login()
		fileid = self._save_new_file(token, "somefilename")
		json = self._save_new_version_of_file(fileid, token, 0, "otherfilename")
		self.assertTrue(json["result"])
		self.assertIn("version", json)
		self.assertTrue(1, json["version"])
		payload = {
			"email":		"testemail",
			"token":		token,
			"id":			fileid
		}
		r = requests.get("http://localhost:8000/files", params = payload)
		self.assertEqual(".txt2", r.json()["extension"])
		self.assertEqual(fileid, r.json()["id"])
		modificationTime = datetime.datetime.strptime(r.json()["lastModified"], "%a %b %d %H:%M:%S %Y")
		self.assertGreater(modificationTime + datetime.timedelta(minutes=1), modificationTime)
		self.assertLess(modificationTime - datetime.timedelta(minutes=1), modificationTime)
		self.assertEqual("testemail", r.json()["lastUser"])
		self.assertEqual("otherfilename", r.json()["name"])
		self.assertEqual("testemail", r.json()["owner"])
		self.assertEqual(["palabra1"], r.json()["tags"])
		self.assertEqual(1, r.json()["lastVersion"])
	
	def test_save_new_version_wrong_token(self):
		token = self._signup_and_login()
		fileid = self._save_new_file(token, "somefilename")
		json = self._save_new_version_of_file(fileid, token + "wrong", 0, "somefilename")
		self.assertFalse(json["result"])
		
	def test_save_new_version_of_inexistent_file(self):
		token = self._signup_and_login()
		fileid = self._save_new_file(token, "somefilename")
		json = self._save_new_version_of_file(fileid + 6, token, 0, "otherFilename")
		self.assertFalse(json["result"])

	def test_save_new_version_from_not_last_version_error(self):
		token = self._signup_and_login()
		fileid = self._save_new_file(token, "somefilename")
		json = self._save_new_version_of_file(fileid, token, 0, "otherFilename")	# Correct new version
		json = self._save_new_version_of_file(fileid, token, 0, "AnotherFilename")
		self.assertFalse(json["result"])

	def test_save_new_version_from_not_last_version_with_overwrite_then_get(self):
		token = self._signup_and_login()
		fileid = self._save_new_file(token, "somefilename")
		json = self._save_new_version_of_file(fileid, token, 0, "otherFilename")	# Correct new version
		json = self._save_new_version_of_file(fileid, token, 0, "AnotherFilename", True)
		self.assertTrue(json["result"])
		self.assertEqual(2, json["version"])
		payload = {
			"email":		"testemail",
			"token":		token,
			"id":			fileid
		}
		r = requests.get("http://localhost:8000/files", params = payload)
		self.assertEqual(".txt2", r.json()["extension"])
		self.assertEqual(fileid, r.json()["id"])
		modificationTime = datetime.datetime.strptime(r.json()["lastModified"], "%a %b %d %H:%M:%S %Y")
		self.assertGreater(modificationTime + datetime.timedelta(minutes=1), modificationTime)
		self.assertLess(modificationTime - datetime.timedelta(minutes=1), modificationTime)
		self.assertEqual("testemail", r.json()["lastUser"])
		self.assertEqual("AnotherFilename", r.json()["name"])
		self.assertEqual("testemail", r.json()["owner"])
		self.assertEqual(["palabra1"], r.json()["tags"])
		self.assertEqual(2, r.json()["lastVersion"])
		

if __name__ == '__main__':
    unittest.main()
