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
		self.assertEqual(True, r.json()["result"])
		self.assertIn("fileID", r.json())
		self.assertIsNotNone(r.json()["fileID"])
		return r.json()["fileID"]

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

if __name__ == '__main__':
    unittest.main()
