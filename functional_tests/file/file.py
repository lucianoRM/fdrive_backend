import requests
import unittest
import json
from subprocess import call
import inspect, os
import datetime
import filecmp

class TestFile(unittest.TestCase):
	
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

	def test_save_new_file_then_get(self):
		token = self._signup_and_login()
		fileid = self._save_new_file(token, "somefilename")
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
		fileid2 = self._save_new_file(token, "otherfilename")
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
		r = requests.post("http://localhost:8000/files", json = payload)
		self.assertFalse(r.json()["result"])
		
	def test_get_file_wrong_id(self):
		token = self._signup_and_login()
		fileid = self._save_new_file(token, "somefilename")
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
		fileid = self._save_new_file(token, "somefilename")
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
		fileid = self._save_new_file(token, "somefilename")
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
	
	def test_save_new_version_wrong_token(self):
		token = self._signup_and_login()
		fileid = self._save_new_file(token, "somefilename")
		_json = self._save_new_version_of_file(fileid, token + "wrong", 0, "somefilename")
		self.assertFalse(_json["result"])
		
	def test_save_new_version_of_inexistent_file(self):
		token = self._signup_and_login()
		fileid = self._save_new_file(token, "somefilename")
		_json = self._save_new_version_of_file(fileid + 6, token, 0, "otherFilename")
		self.assertFalse(_json["result"])

	def test_save_new_version_from_not_last_version_error(self):
		token = self._signup_and_login()
		fileid = self._save_new_file(token, "somefilename")
		_json = self._save_new_version_of_file(fileid, token, 0, "otherFilename")	# Correct new version
		_json = self._save_new_version_of_file(fileid, token, 0, "AnotherFilename")
		self.assertFalse(_json["result"])

	def test_save_new_version_from_not_last_version_with_overwrite_then_get(self):
		token = self._signup_and_login()
		fileid = self._save_new_file(token, "somefilename")
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
		

	def test_file_upload(self):
		python_file_path = os.path.dirname(os.path.abspath(inspect.getfile(inspect.currentframe())))+'/picture.png'
		files = {'upload': open(python_file_path, 'rb')}
		token = self._signup_and_login("testemail")
		fileid = self._save_new_file(token, "somefilename")
		r = requests.post("http://localhost:8000/filesupload?id="+str(fileid)+"&email=testemail&token="+token, files = files)
		self.assertTrue(r.json()["result"])
		server_file_path = os.path.dirname(os.path.abspath(inspect.getfile(inspect.currentframe())))+'/../../files/testemail/root/'+str(fileid)
		self.assertTrue(os.path.isfile(server_file_path))
		self.assertTrue(filecmp.cmp(python_file_path, server_file_path))

	def test_file_download(self):
		python_file_path = os.path.dirname(os.path.abspath(inspect.getfile(inspect.currentframe())))+'/picture.png'
		downloaded_file_path = os.path.dirname(os.path.abspath(inspect.getfile(inspect.currentframe())))+'/picture_download.png'
		files = {'upload': open(python_file_path, 'rb')}
		token = self._signup_and_login("testemail")
		fileid = self._save_new_file(token, "somefilename")
		r = requests.post("http://localhost:8000/filesupload?id="+str(fileid)+"&email=testemail&token="+token, files = files)
		self.assertTrue(r.json()["result"])

		r = requests.get("http://localhost:8000/filesdownload?id="+str(fileid)+"&email=testemail&token="+token)
		call(["rm", "-rf", downloaded_file_path])
		with open(downloaded_file_path, 'wb') as f:
			for chunk in r.iter_content(chunk_size=1024):
				if chunk:
					f.write(chunk)
		self.assertTrue(filecmp.cmp(downloaded_file_path, python_file_path))
		call(["rm", "-rf", downloaded_file_path])

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


if __name__ == '__main__':
    unittest.main()
